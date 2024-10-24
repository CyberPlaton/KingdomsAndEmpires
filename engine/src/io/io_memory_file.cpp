#include "io_memory_file.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	cmemory_file::cmemory_file(const fs::cfileinfo& filepath) :
		m_info(filepath), m_state(core::file_state_read_only), m_seek_position(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cmemory_file::~cmemory_file()
	{
		close();
	}

	//------------------------------------------------------------------------------------------------------------------------
	const fs::cfileinfo& cmemory_file::info() const
	{
		return m_info;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cmemory_file::size()
	{
		return SCAST(unsigned, m_memory.size());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_file::read_only()
	{
		return algorithm::bit_check(m_state, core::file_state_read_only);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_file::opened()
	{
		return algorithm::bit_check(m_state, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmemory_file::open(int file_mode)
	{
		if (opened() && filemode() == file_mode)
		{
			//- reset to default begin state
			seek_to_start();
			return;
		}

		m_filemode = file_mode;
		m_seek_position = 0;
		m_state = core::file_state_read_only;

		if (algorithm::bit_check(m_filemode, core::file_mode_write))
		{
			algorithm::bit_clear(m_state, core::file_state_read_only);
		}
		if (algorithm::bit_check(m_filemode, core::file_mode_append))
		{
			algorithm::bit_clear(m_state, core::file_state_read_only);

			m_seek_position = size() > 0 ? size() - 1 : 0;
		}
		if (algorithm::bit_check(m_filemode, core::file_mode_truncate))
		{
			m_memory.clear();
		}

		algorithm::bit_set(m_state, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cmemory_file::close()
	{
		m_seek_position = 0;
		algorithm::bit_set(m_state, core::file_state_read_only);
		algorithm::bit_clear(m_state, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cmemory_file::seek(int offset, core::file_seek_origin origin)
	{
		if (!opened())
		{
			return 0;
		}

		//- set memory cursor to appropriate position based on mode
		switch (origin)
		{
		case core::file_seek_origin_begin:
		{
			m_seek_position = offset;
			break;
		}
		case core::file_seek_origin_end:
		{
			m_seek_position = size() - offset;
			break;
		}
		case core::file_seek_origin_set:
		{
			m_seek_position += offset;
			break;
		}
		default:
		case core::file_seek_origin_none:
		{
			return 0;
		}
		}

		return tell();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_file::seek_to_start()
	{
		if (opened())
		{
			m_seek_position = 0;

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_file::seek_to_end()
	{
		if (opened())
		{
			m_seek_position = size();

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cmemory_file::seek_to(unsigned offset)
	{
		if (opened())
		{
			const auto file_size = size();

			CORE_ASSERT(offset >= 0 && offset <= file_size, "Invalid operation. Offset lies outside of file start and file end!");

			m_seek_position = offset;

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cmemory_file::tell()
	{
		return m_seek_position;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cmemory_file::read(byte_t* buffer, unsigned datasize)
	{
		if (!opened())
		{
			return 0;
		}

		auto max_size = glm::min(datasize, size() - tell());

		if (max_size > 0)
		{
			std::memcpy(buffer, (byte_t*)m_memory.data(), max_size);
		}
		else
		{
			max_size = 0;
		}

		return max_size;
	}

	//------------------------------------------------------------------------------------------------------------------------
	memory_ref_t cmemory_file::read_sync()
	{
		if (!opened() || m_memory.size() == 0)
		{
			return nullptr;
		}

		const auto datasize = (unsigned)m_memory.size();

		auto memory = core::cmemory::make_ref(datasize);

		read(memory->data(), datasize);

		return memory;
	}

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<memory_ref_t> cmemory_file::read_async()
	{
		core::cfuture_type<memory_ref_t> result = core::casync::launch_async([&]() -> memory_ref_t
			{
				return read_sync();

			}).share();

			return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cmemory_file::write(const byte_t* buffer, unsigned datasize)
	{
		if (!opened() || read_only())
		{
			return 0;
		}

		const auto buffer_size = size() - tell();

		if (datasize > buffer_size)
		{
			m_memory.resize(SCAST(uint64_t, m_memory.size() + datasize - buffer_size));
		}

		std::memcpy(m_memory.data() + tell(), (void*)buffer, SCAST(uint64_t, datasize));

		return datasize;
	}

} //- io