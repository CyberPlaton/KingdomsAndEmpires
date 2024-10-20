#include "io_native_file.hpp"

namespace io
{
	namespace
	{
		//- Note: truncate and text reading is the default
		//------------------------------------------------------------------------------------------------------------------------
		const char* to_openmode(int file_mode)
		{
			const char* mode = nullptr;

			if (algorithm::bit_check(file_mode, core::file_mode_read) && algorithm::bit_check(file_mode, core::file_mode_write))
			{
				mode = "r+";
			}
			else if (algorithm::bit_check(file_mode, core::file_mode_read))
			{
				mode = "r";
			}
			else if (algorithm::bit_check(file_mode, core::file_mode_write))
			{
				mode = "w";
			}

			return mode;
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cnative_file::cnative_file(const core::fs::cfileinfo& filepath) :
		m_info(filepath), m_file(nullptr), m_state(core::file_state_read_only)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cnative_file::~cnative_file()
	{
		close();
	}

	//------------------------------------------------------------------------------------------------------------------------
	const core::fs::cfileinfo& cnative_file::info() const
	{
		return m_info;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::size()
	{
		if (opened())
		{
			auto current = tell();
			seek(0, core::file_seek_origin_end);
			auto filesize = tell();
			seek(current, core::file_seek_origin_begin);

			return filesize;
		}
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_file::read_only()
	{
		return algorithm::bit_check(m_state, core::file_state_read_only);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_file::opened()
	{
		return algorithm::bit_check(m_state, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_file::open(int file_mode)
	{
		if (opened() && filemode() == file_mode)
		{
			seek(0, core::file_seek_origin_begin);

			return;
		}

		m_filemode = file_mode;
		m_state = core::file_state_read_only;

		if (algorithm::bit_check(m_filemode, core::file_mode_write) ||
			algorithm::bit_check(m_filemode, core::file_mode_append))
		{
			algorithm::bit_clear(m_state, core::file_state_read_only);
		}

		m_file = fopen(info().path().c_str(), to_openmode(m_filemode));

		if (m_file)
		{
			algorithm::bit_set(m_state, core::file_state_opened);
		}
		else
		{
			//- report failure to open
			log_warn(fmt::format("Failed to open native file '{}' with error '{}'", info().path(), strerror(errno)));
		}

		seek_to_start();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_file::close()
	{
		if(m_file) fclose(m_file);
		m_file = nullptr;
		algorithm::bit_clear(m_state, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::seek(int offset, core::file_seek_origin origin)
	{
		if (!opened())
		{
			return 0;
		}

		int way = 0;

		switch (origin)
		{
		case core::file_seek_origin_begin:
		{
			way = SEEK_CUR;
			break;
		}
		case core::file_seek_origin_end:
		{
			way = SEEK_END;
			break;
		}
		case core::file_seek_origin_set:
		{
			way = SEEK_SET;
			break;
		}
		default:
		case core::file_seek_origin_none:
		{
			return 0;
		}
		}

		fseek(m_file, SCAST(int64_t, offset), way);

		return tell();
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::tell()
	{
		if (opened())
		{
			return static_cast<unsigned>(ftell(m_file));
		}
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::read(byte_t* buffer, unsigned datasize)
	{
		if (!opened() || !buffer || datasize == 0)
		{
			return 0;
		}

		seek_to(0);

		auto finished_reading = fread(buffer, sizeof(char), datasize, m_file);

		if (finished_reading == datasize)
		{
			return datasize;
		}
		else if (finished_reading < datasize)
		{
			log_warn(fmt::format("Failed to fully read from native file '{}'. Read '{}/{}' Bytes.",
				info().path(), finished_reading, datasize));
		}
		else
		{
			log_error(fmt::format("Failed to read from native file '{}' with error '{}'",
				info().path(), strerror(errno)));
		}

		return finished_reading;
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cnative_file::read_sync()
	{
		if (!opened())
		{
			return nullptr;
		}

		auto memory = core::fs::load_text_from_file(info().path());

		return memory;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<memory_ref_t> cnative_file::read_async()
	{
		core::cfuture_type<memory_ref_t> result = core::casync::launch_async([&]() -> memory_ref_t
			{
				return read_sync();

			}).share();

			return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::write(const byte_t* buffer, unsigned datasize)
	{
		if (!opened() || read_only())
		{
			return 0;
		}

		auto finished_writing = fwrite(buffer, sizeof(char), datasize, m_file);

		if (finished_writing == datasize)
		{
			return datasize;
		}

		log_warn(fmt::format("Failed to write to native file '{}' with error '{}'. Wrote '{}/{}' Bytes.",
			info().path(), strerror(errno), finished_writing, datasize));

		return finished_writing;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_file::seek_to_start()
	{
		if (opened())
		{
			const auto file_size = size();

			return (fseek(m_file, 0, SEEK_SET) == 0);
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_file::seek_to_end()
	{
		if (opened())
		{
			const auto file_size = size();

			return (fseek(m_file, file_size, SEEK_SET) == file_size);
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_file::seek_to(unsigned offset)
	{
		if (opened())
		{
			const auto file_size = size();

			CORE_ASSERT(offset >= 0 && offset <= file_size, "Invalid operation. Offset lies outside of file start and file end!");

			return (fseek(m_file, offset, SEEK_SET) == offset);
		}
		return false;
	}

} //- io