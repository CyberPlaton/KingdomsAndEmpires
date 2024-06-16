#include "io_zip_file.hpp"
#include <plugin_logging.h>

namespace io
{
	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		czip::czip(stringview_t path) :
			m_filename(path.data())
		{
			if (m_zip_archive = (miniz::mz_zip_archive*)CORE_MALLOC(sizeof(struct miniz::mz_zip_archive_tag)); m_zip_archive)
			{
				std::memset(m_zip_archive, 0, sizeof(struct miniz::mz_zip_archive_tag));

				if (const auto status = mz_zip_reader_init_file(m_zip_archive, filepath().data(), 0); status)
				{
					for (auto i = 0u; i < mz_zip_reader_get_num_files(m_zip_archive); ++i)
					{
						miniz::mz_zip_archive_file_stat file_stat;

						if (!mz_zip_reader_file_stat(m_zip_archive, i, &file_stat))
						{
							//- could not read an entry
							logging::log_warn(fmt::format("Failed reading a ZIP file entry at '{}'", path.data()));
						}

						m_entries[file_stat.m_filename] = std::make_pair(file_stat.m_file_index, file_stat.m_uncomp_size);
					}
				}
				else
				{
					//- failed to open zip file
					logging::log_error(fmt::format("Failed reading a ZIP file at '{}'", path.data()));
				}
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		czip::~czip()
		{
			CORE_FREE(m_zip_archive);
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool czip::map_file(stringview_t filename, vector_t<uint8_t>& data)
		{
			if (const auto it = m_entries.find(filename.data()); it != m_entries.end())
			{
				const auto index = it->second.first;
				const auto size = it->second.second;

				data.resize(size);

				return miniz::mz_zip_reader_extract_to_mem_no_alloc(m_zip_archive, index, data.data(), size, 0, 0, 0);
			}

			logging::log_warn(fmt::format("Failed reading file entry in ZIP file at '{}'", filepath().data()));

			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		stringview_t czip::filepath() const
		{
			return m_filename;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool czip::read_only() const
		{
			try
			{
				std::filesystem::file_status s = std::filesystem::status(filepath().data());

				const auto perms = SCAST(int, s.permissions());

				return algorithm::bit_check(perms, std::filesystem::perms::_File_attribute_readonly);
			}
			//- for any reason we could not get an anwser, this implies that we won´t be able to write to it anyway
			catch (...)
			{
				logging::log_warn(fmt::format("Could not query permissions for ZIP file at '{}'", filepath().data()));
				return true;
			}
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	czip_file::czip_file(const core::fs::cfileinfo& filepath, detail::zipfile_ref_t zip) :
		m_info(filepath), m_state(core::file_state_read_only), m_mode(core::file_mode_none),
		m_zip(zip), m_seek_pos(0)
	{
		CORE_ASSERT(m_zip, "Invalid operation. Cannot create ZIP file from an empty ZIP archive!");
	}

	//------------------------------------------------------------------------------------------------------------------------
	czip_file::~czip_file()
	{
		close();
	}

	//------------------------------------------------------------------------------------------------------------------------
	const core::fs::cfileinfo& czip_file::info() const
	{
		return m_info;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned czip_file::size()
	{
		if (opened())
		{
			return SCAST(unsigned, m_memory.size());
		}

		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool czip_file::read_only()
	{
		return m_zip && m_zip->read_only() && algorithm::bit_check(m_state, core::file_state_read_only);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool czip_file::opened()
	{
		return algorithm::bit_check(m_state, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void czip_file::open(int file_mode)
	{
		//- TODO: read-write mode
		if (algorithm::bit_check(file_mode, core::file_mode_write) || algorithm::bit_check(file_mode, core::file_mode_append))
		{
			CORE_ASSERT(false, "Invalid operation. Writing to ZIP file is not supported!");
			return;
		}

		if (info().absolute_path().empty() || (opened() && m_mode == file_mode) || !m_zip)
		{
			return;
		}

		auto path = info().absolute_path();

		if (core::string_utils::starts_with(path, "/"))
		{
			path = path.substr(1, path.length() - 1);
		}

		if (const auto result = m_zip->map_file(path, m_memory); result)
		{
			m_mode = file_mode;
			m_seek_pos = 0;
			algorithm::bit_set(m_state, core::file_state_read_only);

			if (algorithm::bit_check(m_mode, core::file_mode_write))
			{
				algorithm::bit_clear(m_state, core::file_state_read_only);
			}
			if (algorithm::bit_check(m_mode, core::file_mode_append))
			{
				algorithm::bit_clear(m_state, core::file_state_read_only);
				m_seek_pos = size() > 0 ? size() - 1 : 0;
			}
			if (algorithm::bit_check(m_mode, core::file_mode_truncate))
			{
				m_memory.clear();
			}

			algorithm::bit_set(m_mode, core::file_state_opened);
		}
		else
		{
			//- could not open file
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void czip_file::close()
	{
		//- TODO: read-write mode
		if (read_only() || !algorithm::bit_check(m_state, core::file_state_has_changes))
		{
			algorithm::bit_clear(m_mode, core::file_state_opened);
			return;
		}

		algorithm::bit_clear(m_mode, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned czip_file::seek(unsigned offset, core::file_seek_origin origin)
	{
		if (!opened())
		{
			return 0;
		}

		switch (origin)
		{
		case core::file_seek_origin_begin:
		{
			m_seek_pos = offset;
			break;
		}
		case core::file_seek_origin_end:
		{
			m_seek_pos = size() - offset;
			break;
		}
		case core::file_seek_origin_set:
		{
			m_seek_pos += offset;
			break;
		}
		default:
		case core::file_seek_origin_none:
		{
			return 0;
		}
		}

		m_seek_pos = glm::min(m_seek_pos, size() - 1);

		return tell();
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned czip_file::tell()
	{
		return m_seek_pos;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned czip_file::read(byte_t* buffer, unsigned datasize)
	{
		if (!opened())
		{
			return 0;
		}

		auto max_size = glm::min(datasize, size() - tell());

		if (max_size > 0)
		{
			std::memcpy(buffer, m_memory.data(), SCAST(uint64_t, max_size));
		}
		else
		{
			max_size = 0;
		}

		return max_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned czip_file::write(const byte_t* buffer, unsigned datasize)
	{
		if (!opened() || read_only())
		{
			return 0;
		}

		auto buffer_size = size() - tell();

		if (datasize > buffer_size)
		{
			m_memory.resize(m_memory.size() + (datasize - buffer_size));
		}

		std::memcpy(m_memory.data() + tell(), buffer, SCAST(uint64_t, datasize));

		algorithm::bit_set(m_state, core::file_state_has_changes);

		return datasize;
	}

} //- io