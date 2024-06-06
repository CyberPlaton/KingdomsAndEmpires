#include "sm_testbed_module.hpp"
#include <iostream>

void core_io_error_function(uint8_t level, const std::string& message)
{
	switch (level)
	{
	case SPDLOG_LEVEL_TRACE:
	{
		logging::log_trace(message);
		break;
	}
	case SPDLOG_LEVEL_DEBUG:
	{
		logging::log_debug(message);
		break;
	}
	case SPDLOG_LEVEL_INFO:
	{
		logging::log_info(message);
		break;
	}
	case SPDLOG_LEVEL_WARN:
	{
		logging::log_warn(message);
		break;
	}
	case SPDLOG_LEVEL_ERROR:
	{
		logging::log_error(message);
		break;
	}
	case SPDLOG_LEVEL_CRITICAL:
	{
		logging::log_critical(message);
		break;
	}
	}
}

void allocators_test(int object_count)
{
	struct Object
	{
		std::string name;
		int value;
		float velocity;
		float mass;
	};

	//- setup data
	const auto C_OBJECTS = object_count;
	const auto C_BYTES = C_OBJECTS * sizeof(Object);

	//- timing
	core::ctimer timer;
	core::ctimer allocation_timer;
	float allocation_time_cumulative = 0.0f;

	timer.start();
	core::clinear_allocator allocator(C_BYTES);

	//- perform test with allocation and setting some data
	for (auto i = 0u; i < C_OBJECTS; ++i)
	{

		allocation_timer.start();
		void* memory = allocator.allocate(sizeof(Object), alignof(Object));
		allocation_time_cumulative += allocation_timer.microsecs();

		auto* object =  core::iallocator::construct<Object>((Object*)memory);

		object->name = fmt::to_string(i);
		object->value = i;
		object->velocity = i * glm::pi<float>();
		object->mass = object->velocity * glm::pi<float>();
	}

	//- stop time
	const auto ms = timer.millisecs();

	logging::log_warn("--// Testing Run //--");
	logging::log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
	logging::log_debug(fmt::format("Memory Capacity: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_debug(fmt::format("Memory Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_debug(fmt::format("Memory Peak Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_info(fmt::format("Time: '{}ms'", ms));
	logging::log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
		allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
}

void allocators_test_runs()
{
	const auto C_RUNS = 8;
	const auto C_INCREMENT = 2;
	auto OBJECTS = 2048;

	for (auto i = 0; i < C_RUNS; ++i)
	{
		allocators_test(OBJECTS);

		OBJECTS *= C_INCREMENT;
	}
}

int __real_main(int argc, char* argv[])
{
	AllocConsole();

	logging::init(core::logging_verbosity::logging_verbosity_debug);

	allocators_test_runs();

	return 0;

	engine::cengine::sconfig cfg;
	cfg.m_services_cfg.emplace_back("cthread_service");
	cfg.m_services_cfg.emplace_back("cevent_service");

	cfg.m_layers_cfg.emplace_back("cgame");
	cfg.m_layers_cfg.emplace_back("ceditor");

	sm::set_logger(core_io_error_function);

	sm::configure(&engine::cengine::instance(),	//- engine class as the application
		(void*)&cfg,							//- engine configuration
		argc,									//- command line args count
		argv);									//- command line args values

	sm::init("spritemancer testbed", 1920, 1080, false, true);

	sm::run();

	return 0;
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	return __real_main(0, nullptr);
}
#else
int main(int argc, char* argv[])
{
	return __real_main(argc, argv);
}
#endif
