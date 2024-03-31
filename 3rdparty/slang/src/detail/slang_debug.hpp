#pragma once
#include "slang_os_api.hpp"

namespace slang
{
	void print_chunk(const detail::schunk& chunk);
	void print_instruction(stringview_t name, const vector_t<byte_t> );

} //- slang