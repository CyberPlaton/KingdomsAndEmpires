#pragma once
#include "slang_os_api.hpp"
#define FMT_HEADER_ONLY
#include "3rdparty/fmt/format.h"

namespace slang
{
	void slang_print(detail::log_level level, bool force, const char* fmt);
	void slang_log(detail::log_level level, const char* fmt);

	namespace detail
	{
		const char* opcode_name(opcode code);

	} //- detail

	//- Internal usage. Using std::string for ease of use with fmt.
	//------------------------------------------------------------------------------------------------------------------------
	namespace debug
	{
		std::string print_token(const detail::stoken& token);
		std::string print_token_stream(const detail::stoken_stream& stream);
		std::string print_chunk(const detail::schunk& chunk);
		std::string print_value(const svalue& value);
		std::string print_object(const sobject* object);
		std::string print_instruction(detail::opcode instruction, const detail::schunk& chunk, uint64_t& index);

	} //- debug

} //- slang