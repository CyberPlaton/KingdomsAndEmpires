#include "tiny_editor_module.hpp"
#include <iostream>
#if MIMALLOC_ENABLE
	#include <mimalloc-override.h>
#endif

//------------------------------------------------------------------------------------------------------------------------
void core_io_error_function(uint8_t level, const std::string& message)
{
	switch (level)
	{
	case SPDLOG_LEVEL_TRACE:
	{
		log_trace(message);
		break;
	}
	case SPDLOG_LEVEL_DEBUG:
	{
		log_debug(message);
		break;
	}
	case SPDLOG_LEVEL_INFO:
	{
		log_info(message);
		break;
	}
	case SPDLOG_LEVEL_WARN:
	{
		log_warn(message);
		break;
	}
	case SPDLOG_LEVEL_ERROR:
	{
		log_error(message);
		break;
	}
	case SPDLOG_LEVEL_CRITICAL:
	{
		log_critical(message);
		break;
	}
	}
}

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool ceditor::init()
	{
		auto& docks = cdock_system::instance();

		bool result = true;

		result &= docks.push_back<cmain_menu>();
		result &= docks.push_back<cbottom_panel>();
		result &= docks.push_back<cleft_panel>();
		result &= docks.push_back<cright_panel>();
		result &= docks.push_back<centity_context_panel>();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::shutdown()
	{
		auto& docks = cdock_system::instance();

		while (!docks.docks().empty())
		{
			docks.pop_back();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_update(float dt)
	{
		cdock_system::on_update(dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_world_render()
	{
		cdock_system::on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_ui_render()
	{
		cdock_system::on_ui_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_post_update(float dt)
	{
		cdock_system::on_post_update(dt);
	}

} //- editor

RTTR_REGISTRATION
{
	using namespace editor;

	REGISTER_LAYER(ceditor);
}

//------------------------------------------------------------------------------------------------------------------------
void std_string_test_run(int object_count, int modifying_operations_count)
{
	//- Use a small and a big const char for testing
	const char* C_SMALL_STRING = "Hello World!";
	const char* C_BIG_STRING =
		"object_access_time_cumulative += access_timer.microsecs();"
		"auto* object = core::iallocator::construct<Object>((Object*)memory);"
		"void* memory = allocator.allocate(sizeof(Object), alignof(Object));"
		"auto* object = core::iallocator::construct<Object>((Object*)memory);"
		;

	//- Use a small and a big string for testing
	std::string SMALL_STRING(C_SMALL_STRING);
	std::string BIG_STRING(C_BIG_STRING);

	constexpr auto C_RANGE_START = 3;
	constexpr auto C_RANGE_END = 9;

	core::ctimer timer;
	core::ctimer timer_empty_string;
	float time_empty_string = 0.0f;
	core::ctimer timer_const_char;
	float time_const_char = 0.0f;
	core::ctimer timer_substring;
	float time_substring = 0.0f;
	core::ctimer timer_copy_ctor;
	float time_copy_ctor = 0.0f;
	core::ctimer timer_access;
	float time_access = 0.0f;
	core::ctimer timer_push;
	float time_push = 0.0f;
	core::ctimer timer_pop;
	float time_pop = 0.0f;

	timer.start();

	for (auto i = 0; i < object_count; ++i)
	{
		//- Create empty string
		{
			timer_empty_string.start();
			std::string s;
			time_empty_string += timer_empty_string.microsecs();
		}

		//- Create string from const char*
		{
			timer_const_char.start();
			std::string sma(C_SMALL_STRING);
			std::string big(C_BIG_STRING);
			time_const_char += timer_const_char.microsecs();
		}

		//- Create string from substring
		{
			timer_substring.start();
			std::string sma_sub(SMALL_STRING, C_RANGE_START, C_RANGE_END);
			std::string big_sub(BIG_STRING, C_RANGE_START, C_RANGE_END);
			time_substring += timer_substring.microsecs();
		}

		//- Copy construct string
		{
			std::string small_string(C_SMALL_STRING);
			std::string big_string(C_BIG_STRING);

			timer_copy_ctor.start();
			//- Shallow copy
			std::string sma_copy = small_string;
			std::string big_copy = big_string;

			//- Deep Copy
			std::string sma_deep_copy = std::move(small_string);
			std::string big_deep_copy = std::move(big_string);
			time_copy_ctor += timer_copy_ctor.microsecs();
		}

		//- Access and print the string
		{
			timer_access.start();
			auto c = SMALL_STRING.c_str();
			auto cc = BIG_STRING.c_str();
			time_access += timer_access.microsecs();
		}

		//- Basic modification, push and pop
		{
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_push.start();
				SMALL_STRING.push_back('X');
				time_push += timer_push.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_pop.start();
				SMALL_STRING.pop_back();
				time_pop += timer_pop.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_push.start();
				BIG_STRING.push_back('X');
				time_push += timer_push.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_pop.start();
				BIG_STRING.pop_back();
				time_pop += timer_pop.microsecs();
			}
		}
	}

	const auto ms = timer.millisecs();

	log_warn("Testing std::string----------------------------------------------------------------------");
	log_warn("-----------------------------------------------------------------------------------------");
	log_debug(fmt::format("Object Count: '{}', Modifying Operations Count: '{}'", object_count, modifying_operations_count));
	log_debug(fmt::format("Total Time: '{}ms'", ms));
	log_debug(fmt::format("Empty String Time: '{}us' (Total), '{}us' (Per Object)", time_empty_string, time_empty_string / object_count));
	log_debug(fmt::format("Create String from const char*: '{}us' (Total), '{}us' (Per Object)", time_const_char, time_const_char / object_count));
	log_debug(fmt::format("Create String from substring: '{}us' (Total), '{}us' (Per Object)", time_substring, time_substring / object_count));
	log_debug(fmt::format("Copy construct String: '{}us' (Total), '{}us' (Per Object)", time_copy_ctor, time_copy_ctor / object_count));
	log_debug(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)", time_access, time_access / object_count));
	log_debug(fmt::format("'push_back' Time: '{}us' (Total), '{}us' (Per OP)", time_push, time_push / modifying_operations_count));
	log_debug(fmt::format("'pop_back' Time: '{}us' (Total), '{}us' (Per OP)", time_pop, time_pop / modifying_operations_count));
}

//------------------------------------------------------------------------------------------------------------------------
void simd_string_test_run(int object_count, int modifying_operations_count)
{
	//- Use a small and a big const char for testing
	const char* C_SMALL_STRING = "Hello World!";
	const char* C_BIG_STRING =
		"object_access_time_cumulative += access_timer.microsecs();"
		"auto* object = core::iallocator::construct<Object>((Object*)memory);"
		"void* memory = allocator.allocate(sizeof(Object), alignof(Object));"
		"auto* object = core::iallocator::construct<Object>((Object*)memory);"
		;

	//- Use a small and a big string for testing
	SIMDString SMALL_STRING(C_SMALL_STRING);
	SIMDString BIG_STRING(C_BIG_STRING);

	constexpr auto C_RANGE_START = 3;
	constexpr auto C_RANGE_END = 9;

	core::ctimer timer;
	core::ctimer timer_empty_string;
	float time_empty_string = 0.0f;
	core::ctimer timer_const_char;
	float time_const_char = 0.0f;
	core::ctimer timer_substring;
	float time_substring = 0.0f;
	core::ctimer timer_copy_ctor;
	float time_copy_ctor = 0.0f;
	core::ctimer timer_access;
	float time_access = 0.0f;
	core::ctimer timer_push;
	float time_push = 0.0f;
	core::ctimer timer_pop;
	float time_pop = 0.0f;

	timer.start();

	for(auto i = 0; i < object_count; ++i)
	{
		//- Create empty string
		{
			timer_empty_string.start();
			SIMDString s;
			time_empty_string += timer_empty_string.microsecs();
		}

		//- Create string from const char*
		{
			timer_const_char.start();
			SIMDString sma(C_SMALL_STRING);
			SIMDString big(C_BIG_STRING);
			time_const_char += timer_const_char.microsecs();
		}

		//- Create string from substring
		{
			timer_substring.start();
			SIMDString sma_sub(SMALL_STRING, C_RANGE_START, C_RANGE_END);
			SIMDString big_sub(BIG_STRING, C_RANGE_START, C_RANGE_END);
			time_substring += timer_substring.microsecs();
		}

		//- Copy construct string
		{
			SIMDString small_string(C_SMALL_STRING);
			SIMDString big_string(C_BIG_STRING);

			timer_copy_ctor.start();
			//- Shallow copy
			SIMDString sma_copy = small_string;
			SIMDString big_copy = big_string;

			//- Deep Copy
			SIMDString sma_deep_copy = std::move(small_string);
			SIMDString big_deep_copy = std::move(big_string);
			time_copy_ctor += timer_copy_ctor.microsecs();
		}

		//- Access and print the string
		{
			timer_access.start();
			auto c = SMALL_STRING.c_str();
			auto cc = BIG_STRING.c_str();
			time_access += timer_access.microsecs();
		}

		//- Basic modification, push and pop
		{
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_push.start();
				SMALL_STRING.push_back('X');
				time_push += timer_push.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_pop.start();
				SMALL_STRING.pop_back();
				time_pop += timer_pop.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_push.start();
				BIG_STRING.push_back('X');
				time_push += timer_push.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_pop.start();
				BIG_STRING.pop_back();
				time_pop += timer_pop.microsecs();
			}
		}
	}

	const auto ms = timer.millisecs();

	log_warn("Testing SIMDString-----------------------------------------------------------------------");
	log_warn("-----------------------------------------------------------------------------------------");
	log_debug(fmt::format("Object Count: '{}', Modifying Operations Count: '{}'", object_count, modifying_operations_count));
	log_debug(fmt::format("Total Time: '{}ms'", ms));
	log_debug(fmt::format("Empty String Time: '{}us' (Total), '{}us' (Per Object)", time_empty_string, time_empty_string / object_count));
	log_debug(fmt::format("Create String from const char*: '{}us' (Total), '{}us' (Per Object)", time_const_char, time_const_char / object_count));
	log_debug(fmt::format("Create String from substring: '{}us' (Total), '{}us' (Per Object)", time_substring, time_substring / object_count));
	log_debug(fmt::format("Copy construct String: '{}us' (Total), '{}us' (Per Object)", time_copy_ctor, time_copy_ctor / object_count));
	log_debug(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)", time_access, time_access / object_count));
	log_debug(fmt::format("'push_back' Time: '{}us' (Total), '{}us' (Per OP)", time_push, time_push / modifying_operations_count));
	log_debug(fmt::format("'pop_back' Time: '{}us' (Total), '{}us' (Per OP)", time_pop, time_pop / modifying_operations_count));
}

//------------------------------------------------------------------------------------------------------------------------
void core_string_test_run(int object_count, int modifying_operations_count)
{
	//- Use a small and a big const char for testing
	const char* C_SMALL_STRING = "Hello World!";
	const char* C_BIG_STRING =
		"object_access_time_cumulative += access_timer.microsecs();"
		"auto* object = core::iallocator::construct<Object>((Object*)memory);"
		"void* memory = allocator.allocate(sizeof(Object), alignof(Object));"
		"auto* object = core::iallocator::construct<Object>((Object*)memory);"
		;

	//- Use a small and a big string for testing
	core::cstring SMALL_STRING(C_SMALL_STRING);
	core::cstring BIG_STRING(C_BIG_STRING);

	constexpr auto C_RANGE_START = 3;
	constexpr auto C_RANGE_END = 9;

	core::ctimer timer;
	core::ctimer timer_empty_string;
	float time_empty_string = 0.0f;
	core::ctimer timer_const_char;
	float time_const_char = 0.0f;
	core::ctimer timer_substring;
	float time_substring = 0.0f;
	core::ctimer timer_copy_ctor;
	float time_copy_ctor = 0.0f;
	core::ctimer timer_access;
	float time_access = 0.0f;
	core::ctimer timer_push;
	float time_push = 0.0f;
	core::ctimer timer_pop;
	float time_pop = 0.0f;

	timer.start();

	for (auto i = 0; i < object_count; ++i)
	{
		//- Create empty string
		{
			timer_empty_string.start();
			core::cstring s;
			time_empty_string += timer_empty_string.microsecs();
		}

		//- Create string from const char*
		{
			timer_const_char.start();
			core::cstring sma(C_SMALL_STRING);
			core::cstring big(C_BIG_STRING);
			time_const_char += timer_const_char.microsecs();
		}

		//- Create string from substring
		{
			timer_substring.start();
			core::cstring sma_sub(SMALL_STRING, C_RANGE_START, C_RANGE_END);
			core::cstring big_sub(BIG_STRING, C_RANGE_START, C_RANGE_END);
			time_substring += timer_substring.microsecs();
		}

		//- Copy construct string
		{
			core::cstring small_string(C_SMALL_STRING);
			core::cstring big_string(C_BIG_STRING);

			timer_copy_ctor.start();
			//- Shallow copy
			core::cstring sma_copy = small_string;
			core::cstring big_copy = big_string;

			//- Deep copy
			core::cstring sma_deep_copy = std::move(small_string);
			core::cstring big_deep_copy = std::move(big_string);
			time_copy_ctor += timer_copy_ctor.microsecs();
		}

		//- Access and print the string
		{
			timer_access.start();
			auto c = SMALL_STRING.c_str();
			auto cc = BIG_STRING.c_str();
			time_access += timer_access.microsecs();
		}

		//- Basic modification, push and pop
		{
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_push.start();
				SMALL_STRING.push_back('X');
				time_push += timer_push.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_pop.start();
				SMALL_STRING.pop_back();
				time_pop += timer_pop.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_push.start();
				BIG_STRING.push_back('X');
				time_push += timer_push.microsecs();
			}
			for (auto i = 0; i < modifying_operations_count; ++i)
			{
				timer_pop.start();
				BIG_STRING.pop_back();
				time_pop += timer_pop.microsecs();
			}
		}
	}

	const auto ms = timer.millisecs();

	log_warn("Testing core::cstring--------------------------------------------------------------------");
	log_warn("-----------------------------------------------------------------------------------------");
	log_debug(fmt::format("Object Count: '{}', Modifying Operations Count: '{}'", object_count, modifying_operations_count));
	log_debug(fmt::format("Total Time: '{}ms'", ms));
	log_debug(fmt::format("Memory: '{}/{}KiB'(used/peak). Allocations: '{}/{}'(allocs/frees)",
		algorithm::bytes_to_kilobytes(core::cstring::memory_used()),
		algorithm::bytes_to_kilobytes(core::cstring::peak_memory_used()),
		core::cstring::allocations_count(),
		core::cstring::deallocations_count()));
	log_debug(fmt::format("Empty String Time: '{}us' (Total), '{}us' (Per Object)", time_empty_string, time_empty_string / object_count));
	log_debug(fmt::format("Create String from const char*: '{}us' (Total), '{}us' (Per Object)", time_const_char, time_const_char / object_count));
	log_debug(fmt::format("Create String from substring: '{}us' (Total), '{}us' (Per Object)", time_substring, time_substring / object_count));
	log_debug(fmt::format("Copy construct String: '{}us' (Total), '{}us' (Per Object)", time_copy_ctor, time_copy_ctor / object_count));
	log_debug(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)", time_access, time_access / object_count));
	log_debug(fmt::format("'push_back' Time: '{}us' (Total), '{}us' (Per OP)", time_push, time_push / modifying_operations_count));
	log_debug(fmt::format("'pop_back' Time: '{}us' (Total), '{}us' (Per OP)", time_pop, time_pop / modifying_operations_count));
}

//------------------------------------------------------------------------------------------------------------------------
int strings_test_run(int object_count, int modifying_operations_count)
{
	std_string_test_run(object_count, modifying_operations_count);
	core_string_test_run(object_count, modifying_operations_count);
	simd_string_test_run(object_count, modifying_operations_count);
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------
int text_filter_algorithms_test_run()
{
	string_t text = "Hello, my name is Bogdan Strohonov";
	string_t pattern = "ogda";
	vector_t<int> matches;

	if (const auto result = algorithm::matching::direct(pattern, text, matches); result)
	{
		log_info(fmt::format("'Direct': Text: '{}'. Pattern: '{}'", text, pattern));

		for (const auto& idx : matches)
		{
			log_info(fmt::format("\tMatch at index '{}' with word '{}'",
				idx, text.substr(idx, pattern.length())));
		}
	}

	matches.clear();
	if (const auto result = algorithm::matching::fuzzy(pattern, text, matches); result)
	{
		log_info(fmt::format("'Fuzzy': Text: '{}'. Pattern: '{}'", text, pattern));

		for (const auto& idx : matches)
		{
			log_info(fmt::format("\tMatch at index '{}' with word '{}'",
				idx, text.substr(idx, pattern.length())));
		}
	}
	return 0;
}

//------------------------------------------------------------------------------------------------------------------------
int __real_main(int argc, char* argv[])
{
#if PROFILE_ENABLE
#if CORE_PLATFORM_WINDOWS && TRACY_ENABLE
	ZoneScoped;
#endif

	engine::cprofiler_service::init();
#endif
	core::sentry::init();

	AllocConsole();

	logging::clog::instance().init(core::logging_verbosity::logging_verbosity_trace);

	core::set_logger(core_io_error_function);
	sm::set_logger(core_io_error_function);

	engine::cengine::sconfig cfg;

	cfg.m_mode = core::launch_context_editor;
	cfg.m_startup_project = "/default_startup_project.project";

	cfg.m_layers_cfg.emplace_back("cgame");
	cfg.m_layers_cfg.emplace_back("ceditor");

	cfg.m_plugins_cfg.emplace_back("plugin_rttr_example");

	sm::configure(&engine::cengine::instance(),	//- engine class as the application
		(void*)&cfg,							//- engine configuration
		argc,									//- command line args count
		argv);									//- command line args values

	sm::init("tiny editor", 1360, 768, false, false);

// 	core::profile::cpu::get_aggregator()->update();
// 	core::profile::memory::get_aggregator()->update();
// 	sm::profile::gpu::get_aggregator()->update();
// 
// 	auto cpustats = core::profile::cprofiler::cpu_stats();
// 	auto funcstats = core::profile::cprofiler::function_data();
// 	auto memstats = core::profile::cprofiler::memory_stats();
// 	auto gpustats = sm::profile::gpu::get_aggregator()->stats();
// 	auto drawcallstats = sm::profile::gpu::get_aggregator()->drawcall_data();

	sm::run();

	logging::clog::instance().shutdown();
	core::sentry::shutdown();
	return 0;
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
//------------------------------------------------------------------------------------------------------------------------
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	//- FIXME: just for decoy, use any type of class so that they wont get optimized out for not being used...
	{
		editor::ceditor editor_decoy;
		cgame game_decoy;
	}
	return __real_main(0, nullptr);
}
#else
//------------------------------------------------------------------------------------------------------------------------
int main(int argc, char* argv[])
{
	//- FIXME: just for decoy, use any type of class so that they wont get optimized out for not being used...
	{
		editor::ceditor editor_decoy;
		cgame game_decoy;
	}
	return __real_main(0, nullptr);
}
#endif