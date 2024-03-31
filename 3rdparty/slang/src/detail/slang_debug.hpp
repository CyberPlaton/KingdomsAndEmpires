#pragma once
#include "slang_os_api.hpp"

namespace slang
{
	namespace detail
	{
		const char* opcode_name(opcode code);

	} //- detail

	void slang_print(detail::log_level level, bool force, const char* fmt);
	void print_chunk(const detail::schunk& chunk);
	void print_instruction(detail::opcode instruction, const detail::schunk& chunk, uint64_t& index);

} //- slang