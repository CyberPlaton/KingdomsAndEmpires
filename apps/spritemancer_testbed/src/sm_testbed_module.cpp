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
bool cexample_app_layer::init()
{
	return true;
}

//------------------------------------------------------------------------------------------------------------------------
void cexample_app_layer::shutdown()
{

}

//------------------------------------------------------------------------------------------------------------------------
void cexample_app_layer::on_update(float dt)
{
}

//------------------------------------------------------------------------------------------------------------------------
void cexample_app_layer::on_world_render()
{

}

//------------------------------------------------------------------------------------------------------------------------
void cexample_app_layer::on_ui_render()
{

}

//------------------------------------------------------------------------------------------------------------------------
void cexample_app_layer::on_post_update(float dt)
{

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

//- Test a filesystems performance for key operations
//------------------------------------------------------------------------------------------------------------------------
void filesystem_performance_tests(const core::fs::filesystem_ref_t& fs)
{
	constexpr auto C_COUNT = 1000;
	constexpr stringview_t C_FILENAME = "testing_file.txt";
	constexpr stringview_t C_DATA = 
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
		"Fusce volutpat suscipit mattis. Morbi porta ipsum ut dapibus vehicula.\n"
		"Etiam in bibendum urna. Quisque in diam vitae leo pellentesque lobortis non quis nisi.\n"
		"Fusce auctor aliquam lacus eu laoreet. Etiam pretium, leo ut molestie iaculis, ante purus ultricies nisl, sit amet pharetra arcu neque nec urna.\n"
		"Morbi pretium ligula in libero scelerisque, eu maximus nulla mollis. Integer commodo lorem id mollis interdum. Morbi eget ipsum eu ex efficitur finibus sed at mi.\n"
		"Etiam in bibendum urna. Quisque in diam vitae leo pellentesque lobortis non quis nisi.\n"
		;

	//- 'profiling' times for:
	//- open, close, write, read
	core::ctimer general_timer;
	core::ctimer open_timer;
	core::ctimer close_timer;
	core::ctimer read_timer;
	core::ctimer write_timer;
	float open_time_cumulative = 0.0f;
	float close_time_cumulative = 0.0f;
	float read_time_cumulative = 0.0f;
	float write_time_cumulative = 0.0f;
	core::fs::file_ref_t file = nullptr;

	//- testing individual filesystems for functionality
	auto cwd = core::cfilesystem::cwd();

	general_timer.start();

	if (fs->init(cwd.view()))
	{
		if (!fs->create_file({ C_FILENAME.data() }))
		{
			logging::log_critical(fmt::format("\t...failed creating testing file!"));

			//- fatal error, does not make sense to proceed
			return;
		}

		for (auto i = 0u; i < C_COUNT; ++i)
		{
			//- open
			open_timer.start();
			if (file = fs->open({ C_FILENAME.data() }, core::file_mode_read_write | core::file_mode_truncate); file)
			{
				open_time_cumulative += open_timer.millisecs();

				//- write
				write_timer.start();
				file->write((const uint8_t*)C_DATA.data(), C_DATA.length());
				write_time_cumulative += write_timer.millisecs();

				file->seek_to_start();

				//- read
				vector_t<uint8_t> buffer(C_DATA.length());
				read_timer.start();
				file->read(buffer.data(), C_DATA.length());
				read_time_cumulative += read_timer.millisecs();

				file->seek_to_start();

				//- close
				close_timer.start();
				file->close();
				close_time_cumulative += close_timer.millisecs();
			}
		}

		fs->shutdown();

		//- get time data for all tests
		const auto ms = general_timer.millisecs();

		logging::log_warn("-----------------------------------------------------------------------------------------");
		logging::log_info(fmt::format("Total Time: '{}ms' with '{}' Operations", ms, C_COUNT));
		logging::log_info(fmt::format("'open' Time: '{}ms' (Total), '{}ms' (Per Operation)", open_time_cumulative, open_time_cumulative / C_COUNT));
		logging::log_info(fmt::format("'close' Time: '{}ms' (Total), '{}ms' (Per Operation)", close_time_cumulative, close_time_cumulative / C_COUNT));
		logging::log_info(fmt::format("'read' Time for '{}' Bytes: '{}ms' (Total), '{}ms' (Per Operation)", C_DATA.length(), read_time_cumulative, read_time_cumulative / C_COUNT));
		logging::log_info(fmt::format("'write' Time for '{}' Bytes: '{}ms' (Total), '{}ms' (Per Operation)", C_DATA.length(), write_time_cumulative, write_time_cumulative / C_COUNT));
	}
}

//- Test all functionality of a filesystem to work as expected
//------------------------------------------------------------------------------------------------------------------------
bool filesystem_functionality_tests(const core::fs::filesystem_ref_t& fs)
{
	std::string m_data =
		"Lorem ipsum dolor sit amet, consectetur adipiscing elit.\n"
		"Fusce volutpat suscipit mattis. Morbi porta ipsum ut dapibus vehicula.\n"
		"Etiam in bibendum urna. Quisque in diam vitae leo pellentesque lobortis non quis nisi.\n"
		"Fusce auctor aliquam lacus eu laoreet. Etiam pretium, leo ut molestie iaculis, ante purus ultricies nisl, sit amet pharetra arcu neque nec urna.\n"
		"Morbi pretium ligula in libero scelerisque, eu maximus nulla mollis. Integer commodo lorem id mollis interdum. Morbi eget ipsum eu ex efficitur finibus sed at mi.\n"
		"Etiam in bibendum urna. Quisque in diam vitae leo pellentesque lobortis non quis nisi.\n"
		;
	constexpr stringview_t C_FILENAME = "testing_file.txt";
	constexpr stringview_t C_COPY_FILENAME = "testing_copy_file.txt";
	constexpr stringview_t C_COPY_RENAME_FILENAME = "testing_copy_rename_file.txt";

	//- testing individual filesystems for functionality
	auto cwd = core::cfilesystem::cwd();

	if (fs->init(cwd.view()))
	{
		//- create file
		{
			if (!fs->create_file({ C_FILENAME.data() }))
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
			if (file = fs->open({ C_FILENAME.data() }, core::file_mode_read_write | core::file_mode_truncate); !file)
			{
				logging::log_critical(fmt::format("\t...failed opening file!"));

				//- fatal error, does not make sense to proceed
				return false;
			}
			else
			{
				logging::log_info(fmt::format("\t...success opening file '{}'", file->info().relative()));
			}

			auto written = 0;
			if (written = file->write((const uint8_t*)m_data.data(), m_data.length()); written != m_data.length())
			{
				logging::log_warn(fmt::format("\t...written only '{}', but should have '{}'", written, m_data.length()));
			}
			else
			{
				logging::log_info(fmt::format("\t...success writing '{}' bytes", written));
			}

			//- set cursor position to start of file for read/write
			if (!file->seek_to_start())
			{
				logging::log_error(fmt::format("\t...failed seeking to start. Expect subsequent operations to be failures"));
			}
			else
			{
				logging::log_info(fmt::format("\t...file cursor position '{}'", file->tell()));
			}
		}

		//- read from file and write to console
		{
			vector_t<uint8_t> buffer(m_data.length());

			auto read = 0;
			if (read = file->read(buffer.data(), m_data.length()); read != m_data.length())
			{
				logging::log_warn(fmt::format("\t...read only '{}', but should have '{}'", read, m_data.length()));
			}
			else
			{
				logging::log_info(fmt::format("\t...success reading '{}' bytes", read));
			}

			//- set cursor position to start of file for read/write
			if (!file->seek_to_start())
			{
				logging::log_error(fmt::format("\t...failed seeking to start. Expect subsequent operations to be failures"));
			}
			else
			{
				logging::log_info(fmt::format("\t...file cursor position '{}'", file->tell()));
			}
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

		//- remove all files created for testing
		{
			if (!fs->remove_file({ C_COPY_RENAME_FILENAME.data() }))
			{
				logging::log_error(fmt::format("\t...failed removing file '{}'!", C_COPY_RENAME_FILENAME.data()));
			}
			if (!fs->remove_file({ C_FILENAME.data() }))
			{
				logging::log_error(fmt::format("\t...failed removing file '{}'!", C_FILENAME.data()));
			}
			if (!fs->remove_file({ C_COPY_FILENAME.data() }))
			{
				logging::log_error(fmt::format("\t...failed removing file '{}'!", C_COPY_FILENAME.data()));
			}
		}

		fs->shutdown();
		return true;
	}
	return false;
}

//- Test a filesystem implementation complete for functionality and performance
//------------------------------------------------------------------------------------------------------------------------
bool filesystem_tests(const core::fs::filesystem_ref_t& fs)
{
	logging::log_warn(fmt::format("\n#### Filesystem '{}' Testing Run ##########################################",
		fs->filesystem_name().data()));

	if (filesystem_functionality_tests(fs))
	{
		filesystem_performance_tests(fs);

		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------
void virtual_filesystem_tests()
{
	filesystem_tests(std::make_shared<io::cnative_filesystem>());
	filesystem_tests(std::make_shared<io::cmemory_filesystem>());
}


constexpr auto C_DO_TESTS = false;

//------------------------------------------------------------------------------------------------------------------------
int __real_main(int argc, char* argv[])
{
	AllocConsole();

	logging::init(core::logging_verbosity::logging_verbosity_debug);

	logging::log_info(fmt::format("Unittests result: '{}'", UnitTest::RunAllTests()));


	if (C_DO_TESTS)
	{
		allocators_test_runs();
		virtual_filesystem_tests();
	}

	engine::cengine::sconfig cfg;

	cfg.m_layers_cfg.emplace_back("cexample_bad_app_layer");
	cfg.m_layers_cfg.emplace_back("cexample_app_layer");
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
