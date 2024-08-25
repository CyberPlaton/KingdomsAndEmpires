#pragma once
#include "slang_config.hpp"
#include "slang_value.hpp"

namespace slang
{
	//------------------------------------------------------------------------------------------------------------------------
	class cchunk final
	{
	public:
		cchunk() = default;
		~cchunk() = default;

		void write_opcode(byte_t byte, unsigned line);
		void write_value(cvalue&& value);

	private:
		vector_t<cvalue> m_constants;
		vector_t<byte_t> m_code;
		vector_t<unsigned> m_lines;
	};

} //- slang