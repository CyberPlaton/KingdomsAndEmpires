#include "io_native_filesystem.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	cnative_filesystem::cnative_filesystem() :
		m_basepath({}), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cnative_filesystem::~cnative_filesystem()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::init(stringview_t basepath)
	{
		m_basepath.assign(basepath.data());

		if (!core::string_utils::ends_with(m_basepath, "/"))
		{
			m_basepath += "/";
		}

		build_file_list(m_basepath, m_file_list);

		m_ready = true;
		return m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_filesystem::shutdown()
	{
		m_ready = false;
		m_basepath.clear();
		m_file_list.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::ready() const
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	stringview_t cnative_filesystem::filesystem_name() const
	{
		constexpr stringview_t C_NAME = "cnative_filesystem";

		return C_NAME;
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t cnative_filesystem::base_path() const
	{
		return m_basepath;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::filelist_t cnative_filesystem::files() const
	{
		return m_file_list;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::does_exist(const core::fs::cfileinfo& filepath) const
	{
		core::fs::cfileinfo info(base_path(), filepath.relative());

		return find_file(info) != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::file_ref_t cnative_filesystem::open(const core::fs::cfileinfo& filepath, int file_mode)
	{
		core::fs::cfileinfo info(base_path(), filepath.relative());

		auto file = find_file(info);
		const auto exists = (file != nullptr);

		if (!exists)
		{
			algorithm::bit_set(file_mode, core::file_mode_truncate);

			file = std::move(std::make_shared<cnative_file>(info));
		}

		file->open(file_mode);

		if (!exists && file->opened())
		{
			m_file_list.insert(file);
		}

		return file;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_filesystem::close(core::fs::file_ref_t file)
	{
		if (file)
		{
			file->close();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::create_file(const core::fs::cfileinfo& filepath)
	{
		auto result = false;

		if (!does_exist(filepath))
		{
			core::fs::cfileinfo info(base_path(), filepath.relative());

			if (const auto file = open(info, core::file_mode_read_write | core::file_mode_truncate); file)
			{
				result = true;

				file->close();
			}
		}
		else
		{
			//- file does exist already
			result = true;
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::remove_file(const core::fs::cfileinfo& filepath)
	{
		auto result = true;

		core::fs::cfileinfo info(base_path(), filepath.relative());

		if (const auto file = find_file(info); file)
		{
			std::error_code ec;

			//- close file in case it is open
			file->close();

			if (std::filesystem::remove(info.path().data(), ec))
			{
				m_file_list.erase(file);
			}
			else
			{
				log_error(fmt::format("Failed to remove file '{}' with message '{}'", info.relative(), ec.message()));

				result = false;
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::copy_file(const core::fs::cfileinfo& source, const core::fs::cfileinfo& dest)
	{
		auto result = false;

		core::fs::cfileinfo info(base_path(), source.relative());
		auto source_file = find_file(info);

		info = core::fs::cfileinfo(base_path(), dest.relative());
		auto dest_file = open(info, core::file_mode_write);

		if (source_file && dest_file)
		{
			auto datasize = source_file->size();
			vector_t<byte_t> buffer(SCAST(uint64_t, datasize));

			//- open and go to beginning of file for complete reading/writing
			source_file->open(source_file->filemode());

			source_file->seek_to_start();
			dest_file->seek_to_start();

			int size = (int)datasize;
			auto bytes_read = 0;
			auto bytes_written = 0;

			//- read and write chunk-wise until all has been copied or we could not read or write
			bytes_read = source_file->read(buffer.data(), size);
			while (bytes_read > 0)
			{
				bytes_written = dest_file->write(buffer.data(), bytes_read);

				size -= bytes_read;

				//- stop if we are done reading
				if (bytes_read != bytes_written || size <= 0)
				{
					break;
				}

				bytes_read = source_file->read(buffer.data(), size);
			}

			//- success only if we have read and copied complete file data
			result = (bytes_written == datasize);

			source_file->close();
			dest_file->close();
		}
		else if (!source_file)
		{
			log_error(fmt::format("Failed to find source file '{}' for copyingto file '{}'",
				source.relative(), dest.relative()));
		}
		else if (!dest_file)
		{
			log_error(fmt::format("Failed to open destination file '{}' for copying from source '{}'",
				dest.relative(), source.relative()));
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_filesystem::rename_file(const core::fs::cfileinfo& source, const core::fs::cfileinfo& dest)
	{
		auto result = false;

		core::fs::cfileinfo source_info(base_path(), source.relative());
		core::fs::cfileinfo dest_info(base_path(), dest.relative());

		auto source_file = find_file(source_info);
		auto dest_file = find_file(dest_info);

		if (source_file && !dest_file)
		{
			//- we have to assume that it is successful
			std::filesystem::rename(source_info.path(), dest_info.path());

			m_file_list.erase(source_file);

			if (dest_file = open(dest, core::file_mode_read); dest_file)
			{
				result = true;

				dest_file->close();
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	vector_t<core::fs::cfileinfo> cnative_filesystem::iterate(const core::fs::cfileinfo& path,
		core::filesystem_lookup_type type, bool recursive) const
	{
		vector_t<core::fs::cfileinfo> result;

		const auto base = base_path();

		for (const auto& entry : std::filesystem::directory_iterator{ path })
		{
			if (entry.is_directory())
			{
				if (type == core::filesystem_lookup_type_directory || type == core::filesystem_lookup_type_any)
				{
					result.emplace_back(entry.path().generic_u8string());
				}

				if (recursive)
				{
					auto recursive_result = iterate({ entry.path().generic_u8string() }, type, recursive);

					algorithm::insert(result, recursive_result.begin(), recursive_result.end());
				}
			}
			else if (type == core::filesystem_lookup_type_file || type == core::filesystem_lookup_type_any)
			{
				result.emplace_back(entry.path().generic_u8string());
			}
		}

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::fs::file_ref_t cnative_filesystem::find_file(const core::fs::cfileinfo& fileinfo) const
	{
		const auto it = algorithm::find_if(m_file_list.begin(), m_file_list.end(), [&](const core::fs::file_ref_t& file)
			{
				return file->info() == fileinfo;
			});

		if (it != m_file_list.end())
		{
			return *it;
		}

		return nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_filesystem::build_file_list(string_t path, core::fs::filelist_t& out)
	{
		if (!core::string_utils::ends_with(path, "/"))
		{
			path += "/";
		}

		const auto base = base_path();

		//- recursively iterate over base directory and gather all files contained within
		for (const auto& entry : std::filesystem::directory_iterator{ path })
		{
			if (entry.is_directory())
			{
				build_file_list(entry.path().generic_u8string(), out);
			}
			else if (entry.is_regular_file())
			{
				auto relative = std::filesystem::relative(entry.path(), base);

				core::fs::cfileinfo info(base, relative.generic_u8string());

				out.insert(std::move(std::make_shared<cnative_file>(info)));
			}
		}
	}

} //- io