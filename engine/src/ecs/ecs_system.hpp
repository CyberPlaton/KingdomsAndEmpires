#pragma once
#include <core.h>

namespace ecs
{
	namespace system
	{
		using system_flags_t = int;

		//------------------------------------------------------------------------------------------------------------------------
		enum system_flag : uint8_t
		{
			system_flag_none = 0,
			system_flag_multithreaded,
			system_flag_immediate,
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct sconfig final
		{
			string_t m_name;
			vector_t<string_t> m_run_after;
			vector_t<string_t> m_run_before;
			system_flags_t m_flags = 0;
		};

	} //- system

} //- ecs