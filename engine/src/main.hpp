#pragma once
#include <core.h>
#include "config.hpp"

#define ENGINE_MAIN_ENTRY(entry_function, args_config_function, argc, argv) \
engine::detail::engine_main_entry(entry_function, args_config_function, argc, argv)

#if CORE_PLATFORM_WINDOWS
#include <windows.h>
#define ENGINE_MAIN(entry_function, args_config_function) \
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) \
{ \
	return ENGINE_MAIN_ENTRY(entry_function, args_config_function, 0, nullptr); \
}
#else
#define ENGINE_MAIN(entry_function, args_config_function) \
int main(int argc, char* argv[]) \
{ \
	return ENGINE_MAIN_ENTRY(entry_function, args_config_function, argc, argv); \
}
#endif

namespace engine
{
	namespace detail
	{
		using engine_main_func_t = std::function<engine::sconfig(const args_ref_t&)>;
		using engine_args_config_func_t = std::function<void(const args_ref_t&)>;

		extern int engine_main_entry(engine_main_func_t main_entry, engine_args_config_func_t args_config, int argc, char* argv[]);

	} //- detail

} //- engine