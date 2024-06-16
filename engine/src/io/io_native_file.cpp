#include "io_native_file.hpp"

namespace io
{
	//------------------------------------------------------------------------------------------------------------------------
	cnative_file::cnative_file(const core::fs::cfileinfo& filepath) :
		m_info(filepath), m_state(core::file_state_read_only), m_mode(core::file_mode_none)
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
			const auto cursor = tell();

			seek(0, core::file_seek_origin_end);

			const auto filesize = tell();

			seek(cursor, core::file_seek_origin_begin);

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
		if (opened() && m_mode == file_mode)
		{
			seek(0, core::file_seek_origin_begin);

			return;
		}

		m_mode = file_mode;
		m_state = core::file_state_read_only;

		std::ios_base::openmode open_mode = (std::ios_base::openmode)0;

		if (algorithm::bit_check(file_mode, core::file_mode_read))
		{
			algorithm::bit_set(open_mode, std::fstream::in);
		}
		if (algorithm::bit_check(file_mode, core::file_mode_write))
		{
			algorithm::bit_set(open_mode, std::fstream::out);
			algorithm::bit_clear(m_state, core::file_state_read_only);
		}
		if (algorithm::bit_check(file_mode, core::file_mode_append))
		{
			algorithm::bit_set(open_mode, std::fstream::app);
			algorithm::bit_clear(m_state, core::file_state_read_only);
		}
		if (algorithm::bit_check(m_mode, core::file_mode_truncate))
		{
			algorithm::bit_set(open_mode, std::fstream::trunc);
		}

		m_stream.open(info()->absolute_path().c_str(), open_mode);

		if (m_stream.is_open())
		{
			algorithm::bit_set(m_state, core::file_state_opened);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cnative_file::close()
	{
		m_stream.close();

		algorithm::bit_clear(m_state, core::file_state_opened);
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::seek(unsigned offset, core::file_seek_origin origin)
	{
		if (!opened())
		{
			return 0;
		}

		std::ios_base::seekdir way;

		switch (origin)
		{
		case core::file_seek_origin_begin:
		{
			way = std::ios_base::beg;
			break;
		}
		case core::file_seek_origin_end:
		{
			way = std::ios_base::end;
			break;
		}
		case core::file_seek_origin_set:
		{
			way = std::ios_base::cur;
			break;
		}
		default:
		case core::file_seek_origin_none:
		{
			return 0;
		}
		}

		m_stream.seekg(SCAST(uint64_t, offset), way);
		m_stream.seekp(SCAST(uint64_t, offset), way);

		return tell();
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::tell()
	{
		return SCAST(unsigned, m_stream.tellg());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::read(byte_t* buffer, unsigned datasize)
	{
		if (!opened())
		{
			return 0;
		}

		m_stream.read(RCAST(char*, buffer), SCAST(std::streamsize, datasize));

		if (m_stream)
		{
			return datasize;
		}

		return SCAST(unsigned, m_stream.gcount());
	}

	//------------------------------------------------------------------------------------------------------------------------
	unsigned cnative_file::write(const byte_t* buffer, unsigned datasize)
	{
		if (!opened() || read_only())
		{
			return 0;
		}

		m_stream.write(RCAST(const char*, buffer), SCAST(std::streamsize, datasize));

		if (m_stream)
		{
			return datasize;
		}

		return SCAST(unsigned, m_stream.gcount());
	}

} //- io