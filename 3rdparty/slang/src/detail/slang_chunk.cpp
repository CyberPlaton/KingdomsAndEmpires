#include "slang_chunk.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	void cchunk::write_opcode(byte_t byte, unsigned line)
	{
		m_code.emplace_back(byte);
		m_lines.emplace_back(line);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cchunk::write_value(cvalue&& value)
	{
		m_constants.emplace_back(std::move(value));
	}

} //- slang