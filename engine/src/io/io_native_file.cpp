#include "io_native_file.hpp"

namespace io
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		asio::stream_file::flags to_openmode(int file_mode)
		{
			asio::stream_file::flags mode = (asio::stream_file::flags)0;

			if (algorithm::bit_check(file_mode, core::file_mode_read) && algorithm::bit_check(file_mode, core::file_mode_write))
			{
				mode = asio::stream_file::read_write;
			}
			else if (algorithm::bit_check(file_mode, core::file_mode_read))
			{
				mode = asio::stream_file::read_only;
			}
			else if (algorithm::bit_check(file_mode, core::file_mode_write))
			{
				mode = asio::stream_file::write_only;
			}

			if (algorithm::bit_check(file_mode, core::file_mode_append))
			{
				mode |= asio::stream_file::append;
			}
			if (algorithm::bit_check(file_mode, core::file_mode_truncate))
			{
				mode |= asio::stream_file::truncate;
			}

			//- by default, if file does not exist on open we want to create it,
			//- and also we want flush directly after write and not wait for close
			mode |= (asio::stream_file::create | asio::stream_file::sync_all_on_write);

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
			return (unsigned)m_file->size();
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

		m_context = std::make_unique<asio::io_context>();
		m_file = std::make_unique<asio::stream_file>(*m_context.get(), info().path().c_str(), to_openmode(m_filemode));

		if (m_context && m_file)
		{
			algorithm::bit_set(m_state, core::file_state_opened);
		}
		else
		{
			//- report failure to open
			log_warn(fmt::format("Failed to open native file '{}' with error '{}'", info().path(), strerror(errno)));
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_file::close()
	{
		m_file.reset();
		m_context.reset();
		m_seek_position = 0;
		algorithm::bit_clear(m_state, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::seek(int offset, core::file_seek_origin origin)
	{
		if (!opened())
		{
			return 0;
		}

		asio::file_base::seek_basis way;

		switch (origin)
		{
		case core::file_seek_origin_begin:
		{
			way = asio::file_base::seek_basis::seek_cur;
			break;
		}
		case core::file_seek_origin_end:
		{
			way = asio::file_base::seek_basis::seek_end;
			break;
		}
		case core::file_seek_origin_set:
		{
			way = asio::file_base::seek_basis::seek_set;
			break;
		}
		default:
		case core::file_seek_origin_none:
		{
			return 0;
		}
		}

		m_file->seek(SCAST(int64_t, offset), way);

		return tell();
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::tell()
	{
		if (opened())
		{
			return m_seek_position;
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

		std::error_code err;

		auto count = (unsigned)m_file->read_some(asio::buffer(buffer, datasize), err);

		if (!err)
		{
			m_seek_position += count;

			return count;
		}

		log_warn(fmt::format("Failed to read from native file '{}' with error '{}'", info().path(), err.message()));

		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cnative_file::read_sync()
	{
		if (!opened())
		{
			return nullptr;
		}

		auto memory = core::cmemory::make_ref(size(), true);

		read(memory->data(), size());

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

		std::error_code err;

		auto count = (unsigned)m_file->write_some(asio::buffer(buffer, datasize), err);

		if (!err)
		{
			m_seek_position += count;

			return count;
		}

		log_warn(fmt::format("Failed to write to native file '{}' with error '{}'", info().path(), err.message()));

		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_file::seek_to_start()
	{
		if (opened())
		{
			const auto file_size = (int64_t)size();

			const auto p = (unsigned)m_file->seek(-file_size, asio::file_base::seek_basis::seek_end);

			m_seek_position = p;

			return (p == 0);
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_file::seek_to_end()
	{
		if (opened())
		{
			const auto file_size = (int64_t)size();

			const auto p = (unsigned)m_file->seek(file_size, asio::file_base::seek_basis::seek_set);

			m_seek_position = p;

			return (p == file_size);
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cnative_file::seek_to(unsigned offset)
	{
		if (opened())
		{
			const auto file_size = (int64_t)size();

			CORE_ASSERT(offset >= 0 && offset <= file_size, "Invalid operation. Offset lies outside of file start and file end!");

			const auto p = (unsigned)m_file->seek((int64_t)offset, asio::file_base::seek_basis::seek_set);

			m_seek_position = p;

			return (p == offset);
		}
		return false;
	}

} //- io