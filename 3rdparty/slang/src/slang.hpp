#pragma once
#include "detail/slang_os_api.hpp"
#include "detail/3rdparty/TINYSTL/vector.h"

namespace slang
{
	//- Operations to be executed
	//------------------------------------------------------------------------------------------------------------------------
	enum class opcode : uint32_t
	{
		opcode_noop = 0,
		opcode_add,
		opcode_subtract,
		opcode_multiply,
		opcode_divide,
	};

	//- TODO: where do we store data, such as primitive values and class, string and other objects
	//------------------------------------------------------------------------------------------------------------------------
	struct sbytecode
	{
		tinystl::vector<opcode> m_bytes;
	};

	class cscanner
	{
	public:
		static inline cscanner& instance() { static cscanner s_scanner; return s_scanner; }

		[[nodiscard]] sbytecode scan(std::string_view code);
	};

} //- slang