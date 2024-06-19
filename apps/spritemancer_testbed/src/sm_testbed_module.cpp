#include "sm_testbed_module.hpp"
#include <iostream>

//------------------------------------------------------------------------------------------------------------------------
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

//------------------------------------------------------------------------------------------------------------------------
void linear_allocator_test(int object_count)
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
	core::ctimer access_timer;
	float allocation_time_cumulative = 0.0f;
	float object_access_time_cumulative = 0.0f;

	timer.start();
	core::clinear_allocator allocator(C_BYTES);

	//- perform test with allocation and setting some data
	for (auto i = 0u; i < C_OBJECTS; ++i)
	{
		allocation_timer.start();
		void* memory = allocator.allocate(sizeof(Object), alignof(Object));
		allocation_time_cumulative += allocation_timer.microsecs();

		//- object access
		access_timer.start();
		auto* object =  core::iallocator::construct<Object>((Object*)memory);
		object->name = fmt::to_string(i);
		object->value = i;
		object->velocity = i * glm::pi<float>();
		object->mass = object->velocity * glm::pi<float>();
		object_access_time_cumulative += access_timer.microsecs();
	}

	//- stop time
	const auto ms = timer.millisecs();

	logging::log_warn("-----------------------------------------------------------------------------------------");
	logging::log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
	logging::log_debug(fmt::format("Memory Capacity: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_debug(fmt::format("Memory Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_debug(fmt::format("Memory Peak Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_info(fmt::format("Total Time: '{}ms'", ms));
	logging::log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
		allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
	logging::log_info(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)",
		object_access_time_cumulative, object_access_time_cumulative / C_OBJECTS));
}

//------------------------------------------------------------------------------------------------------------------------
void stack_allocator_test(int object_count)
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
	core::ctimer deallocation_timer;
	core::ctimer access_timer;
	float allocation_time_cumulative = 0.0f;
	float deallocation_time_cumulative = 0.0f;
	float object_access_time_cumulative = 0.0f;

	timer.start();
	core::cstack_allocator allocator(C_BYTES);

	//- perform test with allocation and setting some data
	for (auto i = 0u; i < C_OBJECTS; ++i)
	{
		//- allocation
		allocation_timer.start();
		void* memory = allocator.allocate(sizeof(Object), alignof(Object));
		allocation_time_cumulative += allocation_timer.microsecs();

		//- object access
		access_timer.start();
		auto* object = core::iallocator::construct<Object>((Object*)memory);
		object->name = fmt::to_string(i);
		object->value = i;
		object->velocity = i * glm::pi<float>();
		object->mass = object->velocity * glm::pi<float>();
		object_access_time_cumulative += access_timer.microsecs();

		//- deallocation
		deallocation_timer.start();
		allocator.deallocate(memory);
		deallocation_time_cumulative += deallocation_timer.microsecs();
	}

	//- stop time
	const auto ms = timer.millisecs();

	logging::log_warn("-----------------------------------------------------------------------------------------");
	logging::log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
	logging::log_debug(fmt::format("Memory Capacity: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_debug(fmt::format("Memory Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_debug(fmt::format("Memory Peak Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_info(fmt::format("Total Time: '{}ms'", ms));
	logging::log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
		allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
	logging::log_info(fmt::format("Deallocation Time: '{}us' (Total), '{}us' (Per Object)",
		deallocation_time_cumulative, deallocation_time_cumulative / C_OBJECTS));
	logging::log_info(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)",
		object_access_time_cumulative, object_access_time_cumulative / C_OBJECTS));
}

//------------------------------------------------------------------------------------------------------------------------
void pool_allocator_test(int object_count)
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

	//- timing
	core::ctimer timer;
	core::ctimer allocation_timer;
	core::ctimer deallocation_timer;
	core::ctimer access_timer;
	float allocation_time_cumulative = 0.0f;
	float deallocation_time_cumulative = 0.0f;
	float object_access_time_cumulative = 0.0f;

	timer.start();
	core::cpool_allocator<Object> allocator(C_OBJECTS);

	//- perform test with allocation and setting some data
	for (auto i = 0u; i < C_OBJECTS; ++i)
	{
		//- allocation
		allocation_timer.start();
		void* memory = allocator.allocate(0);
		allocation_time_cumulative += allocation_timer.microsecs();

		//- object access
		access_timer.start();
		auto* object = core::iallocator::construct<Object>((Object*)memory);
		object->name = fmt::to_string(i);
		object->value = i;
		object->velocity = i * glm::pi<float>();
		object->mass = object->velocity * glm::pi<float>();
		object_access_time_cumulative += access_timer.microsecs();

		//- deallocation
		deallocation_timer.start();
		allocator.deallocate(memory);
		deallocation_time_cumulative += deallocation_timer.microsecs();
	}

	//- stop time
	const auto ms = timer.millisecs();

	logging::log_warn("-----------------------------------------------------------------------------------------");
	logging::log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
	logging::log_debug(fmt::format("Memory Capacity: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_debug(fmt::format("Memory Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_debug(fmt::format("Memory Peak Used: '{}KB'", algorithm::bytes_to_kilobytes(allocator.capacity())));
	logging::log_info(fmt::format("Total Time: '{}ms'", ms));
	logging::log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
		allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
	logging::log_info(fmt::format("Deallocation Time: '{}us' (Total), '{}us' (Per Object)",
		deallocation_time_cumulative, deallocation_time_cumulative / C_OBJECTS));
	logging::log_info(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)",
		object_access_time_cumulative, object_access_time_cumulative / C_OBJECTS));
}

//------------------------------------------------------------------------------------------------------------------------
void std_allocator_test_run(int object_count)
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

	//- timing
	core::ctimer timer;
	core::ctimer allocation_timer;
	core::ctimer deallocation_timer;
	core::ctimer access_timer;
	float allocation_time_cumulative = 0.0f;
	float deallocation_time_cumulative = 0.0f;
	float object_access_time_cumulative = 0.0f;

	timer.start();
	std::allocator<Object> allocator;

	//- perform test with allocation and setting some data
	for (auto i = 0u; i < C_OBJECTS; ++i)
	{
		//- allocation
		allocation_timer.start();
		void* memory = allocator.allocate(1);
		allocation_time_cumulative += allocation_timer.microsecs();

		//- object access
		access_timer.start();
		auto* object = core::iallocator::construct<Object>((Object*)memory);
		object->name = fmt::to_string(i);
		object->value = i;
		object->velocity = i * glm::pi<float>();
		object->mass = object->velocity * glm::pi<float>();
		object_access_time_cumulative += access_timer.microsecs();

		//- deallocation
		deallocation_timer.start();
		allocator.deallocate((Object*)memory, 1);
		deallocation_time_cumulative += deallocation_timer.microsecs();
	}

	//- stop time
	const auto ms = timer.millisecs();

	logging::log_warn("-----------------------------------------------------------------------------------------");
	logging::log_debug(fmt::format("Object Count: '{}'", C_OBJECTS));
	logging::log_debug("Memory Capacity: '-/-'");
	logging::log_debug("Memory Used: '-/-'");
	logging::log_debug("Memory Peak Used: '-/-'");
	logging::log_info(fmt::format("Total Time: '{}ms'", ms));
	logging::log_info(fmt::format("Allocation Time: '{}us' (Total), '{}us' (Per Object)",
		allocation_time_cumulative, allocation_time_cumulative / C_OBJECTS));
	logging::log_info(fmt::format("Deallocation Time: '{}us' (Total), '{}us' (Per Object)",
		deallocation_time_cumulative, deallocation_time_cumulative / C_OBJECTS));
	logging::log_info(fmt::format("Access Time: '{}us' (Total), '{}us' (Per Object)",
		object_access_time_cumulative, object_access_time_cumulative / C_OBJECTS));
}

//------------------------------------------------------------------------------------------------------------------------
void allocators_test_runs()
{
	auto OBJECTS = 0;
	const auto C_RUNS = 8;
	const auto C_INCREMENT = 2;

	logging::log_warn("\n#### Linear Allocator Testing Run ##########################################");
	OBJECTS = 2048;
	for (auto i = 0; i < C_RUNS; ++i)
	{
		linear_allocator_test(OBJECTS);
		OBJECTS *= C_INCREMENT;
	}

	logging::log_warn("\n#### Stack Allocator Testing Run ###########################################");
	OBJECTS = 2048;
	for (auto i = 0; i < C_RUNS; ++i)
	{
		stack_allocator_test(OBJECTS);

		OBJECTS *= C_INCREMENT;
	}

	logging::log_warn("\n#### Pool Allocator Testing Run ############################################");
	OBJECTS = 2048;
	for (auto i = 0; i < C_RUNS; ++i)
	{
		pool_allocator_test(OBJECTS);

		OBJECTS *= C_INCREMENT;
	}

	logging::log_warn("\n#### STD Allocator Testing Run ############################################");
	OBJECTS = 2048;
	for (auto i = 0; i < C_RUNS; ++i)
	{
		std_allocator_test_run(OBJECTS);

		OBJECTS *= C_INCREMENT;
	}
}

//------------------------------------------------------------------------------------------------------------------------
bool filesystem_tests(const core::fs::filesystem_ref_t& fs)
{
	logging::log_info(fmt::format("\n#### '{}' Filesystem Testing Run ##########################################",
		fs->filesystem_name().data()));

	std::string m_data =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
		"Fusce volutpat suscipit mattis. Morbi porta ipsum ut dapibus vehicula.\n"
		"Etiam in bibendum urna. Quisque in diam vitae leo pellentesque lobortis non quis nisi.\n"
		"Fusce auctor aliquam lacus eu laoreet. Etiam pretium, leo ut molestie iaculis, ante purus ultricies nisl, sit amet pharetra arcu neque nec urna.\n"
		"Morbi pretium ligula in libero scelerisque, eu maximus nulla mollis. Integer commodo lorem id mollis interdum. Morbi eget ipsum eu ex efficitur finibus sed at mi.\n"
		;
	constexpr stringview_t C_FILENAME = "/testing_file.txt";
	constexpr stringview_t C_COPY_FILENAME = "/testing_copy_file.txt";
	constexpr stringview_t C_COPY_RENAME_FILENAME = "/testing_copy_rename_file.txt";

	//- testing individual filesystems for functionality
	auto cwd = core::cfilesystem::cwd();

	if (fs->init(cwd.view()))
	{
		//- create file
		{
			if (!fs->create_file({ C_FILENAME.data()}))
			{
				logging::log_critical(fmt::format("\t...failed creating file!"));

				//- fatal error, does not make sense to proceed
				return false;
			}
		}

		core::fs::file_ref_t file = nullptr;
		core::fs::file_ref_t copied_file = nullptr;

		//- open and write to file
		{
			if (file = fs->open({ C_FILENAME.data() }, core::file_mode_read_write); !file)
			{
				logging::log_critical(fmt::format("\t...failed opening file!"));

				//- fatal error, does not make sense to proceed
				return false;
			}

			if(const auto written = file->write((const uint8_t*)m_data.data(), m_data.length()); written != m_data.length())
			{
				logging::log_warn(fmt::format("\t...written only '{}', but should have '{}'", written, m_data.length()));
			}
		}

		//- read from file and write to console
		{
			vector_t<uint8_t> buffer(m_data.length());

			if (const auto read = file->read(buffer.data(), m_data.length()); read != m_data.length())
			{
				logging::log_warn(fmt::format("\t...read only '{}', but should have '{}'", read, m_data.length()));
			}

			buffer.push_back('\0');

			logging::log_info(fmt::format("\t...file contents: '{}'", (const char*)buffer.data()));
		}

		//- copy file from one to another
		{
			if (!fs->copy_file({ C_FILENAME.data() }, { C_COPY_FILENAME.data() }))
			{
				logging::log_error(fmt::format("\t...failed copying file!"));
			}
		}

		//- read from copy and write to console
		{
			if (copied_file = fs->open({ C_COPY_FILENAME.data() }, core::file_mode_read_write); !copied_file)
			{
				logging::log_critical(fmt::format("\t...failed opening copied file!"));

				//- fatal error, does not make sense to proceed
				return false;
			}

			vector_t<uint8_t> buffer(m_data.length());

			if (const auto read = copied_file->read(buffer.data(), m_data.length()); read != m_data.length())
			{
				logging::log_warn(fmt::format("\t...read only '{}' from copied file, but should have '{}'", read, m_data.length()));
			}

			buffer.push_back('\0');

			logging::log_info(fmt::format("\t...copied file contents: '{}'", (const char*)buffer.data()));
		}

		//- save copy file to destination
		{
			copied_file->close();
		}

		//- rename copy file to another name
		{
			fs->rename_file({ C_COPY_FILENAME.data() }, { C_COPY_RENAME_FILENAME.data() });

			if (!fs->does_exist({ C_COPY_RENAME_FILENAME.data() }))
			{
				logging::log_error(fmt::format("\t...failed renaming file!"));
			}
		}

		//- delete copy file leaving only source
		{
			if(!fs->remove_file({ C_COPY_RENAME_FILENAME.data() }))
			{
				logging::log_error(fmt::format("\t...failed removing copied file!"));
			}
		}

		fs->shutdown();
		return true;
	}
	return false;
}


//------------------------------------------------------------------------------------------------------------------------
void virtual_filesystem_tests()
{
	filesystem_tests(std::make_shared<io::cnative_filesystem>());
	filesystem_tests(std::make_shared<io::cmemory_filesystem>());

	return;

	//- dry run tests without using actual service, testing only implementation of individual filesystems
	auto cwd = core::cfilesystem::cwd();

	//- create native filesystem and start it up
	core::fs::filesystem_ref_t fs = std::make_shared<io::cnative_filesystem>();

	fs->init(cwd.view());

	core::vfs::instance().add_filesystem("/", fs);

	auto file = core::vfs::instance().open({ "/imgui.ini" }, core::file_mode_read);


	//- shut all filesystems down
	fs->shutdown();
}

//------------------------------------------------------------------------------------------------------------------------
int __real_main(int argc, char* argv[])
{
	AllocConsole();

	logging::init(core::logging_verbosity::logging_verbosity_debug);

	//allocators_test_runs();
	virtual_filesystem_tests();

	engine::cengine::sconfig cfg;
	cfg.m_services_cfg.emplace_back("cthread_service");
	cfg.m_services_cfg.emplace_back("cvirtual_filesystem");
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

//------------------------------------------------------------------------------------------------------------------------
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
