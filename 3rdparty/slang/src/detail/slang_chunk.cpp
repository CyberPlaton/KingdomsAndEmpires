#include "slang_chunk.hpp"

namespace slang
{
	namespace
	{
		constexpr auto C_CONSTANTS_COUNT_MAX = MAX(byte_t);

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cchunk::cchunk()
	{
		m_constants.reserve(C_CONSTANTS_COUNT_MAX);
		m_code.reserve(C_CONSTANTS_COUNT_MAX);
		m_lines.reserve(C_CONSTANTS_COUNT_MAX);
	}

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