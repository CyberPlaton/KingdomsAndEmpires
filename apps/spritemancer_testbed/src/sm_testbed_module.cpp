#include "sm_testbed_module.hpp"
#include <iostream>

RTTR_REGISTRATION
{
	REGISTER_LAYER(cexample_app_layer);

//- You wont be able to register the layer below, catching at compile time
//	REGISTER_LAYER(cexample_bad_app_layer);
}

//------------------------------------------------------------------------------------------------------------------------
void core_io_error_function(uint8_t level, const std::string& message)
{
	switch (level)
	{
	case SPDLOG_LEVEL_TRACE:
	{
		log_trace(message.data());
		break;
	}
	case SPDLOG_LEVEL_DEBUG:
	{
		log_debug(message.data());
		break;
	}
	case SPDLOG_LEVEL_INFO:
	{
		log_info(message.data());
		break;
	}
	case SPDLOG_LEVEL_WARN:
	{
		log_warn(message.data());
		break;
	}
	case SPDLOG_LEVEL_ERROR:
	{
		log_error(message.data());
		break;
	}
	case SPDLOG_LEVEL_CRITICAL:
	{
		log_critical(message.data());
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
			log_critical(fmt::format("\t...failed creating testing file!"));

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

		log_warn("-----------------------------------------------------------------------------------------");
		log_info(fmt::format("Total Time: '{}ms' with '{}' Operations", ms, C_COUNT));
		log_info(fmt::format("'open' Time: '{}ms' (Total), '{}ms' (Per Operation)", open_time_cumulative, open_time_cumulative / C_COUNT));
		log_info(fmt::format("'close' Time: '{}ms' (Total), '{}ms' (Per Operation)", close_time_cumulative, close_time_cumulative / C_COUNT));
		log_info(fmt::format("'read' Time for '{}' Bytes: '{}ms' (Total), '{}ms' (Per Operation)", C_DATA.length(), read_time_cumulative, read_time_cumulative / C_COUNT));
		log_info(fmt::format("'write' Time for '{}' Bytes: '{}ms' (Total), '{}ms' (Per Operation)", C_DATA.length(), write_time_cumulative, write_time_cumulative / C_COUNT));
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
				log_critical(fmt::format("\t...failed creating file!"));

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
				log_critical(fmt::format("\t...failed opening file!"));

				//- fatal error, does not make sense to proceed
				return false;
			}
			else
			{
				log_info(fmt::format("\t...success opening file '{}'", file->info().relative()));
			}

			auto written = 0;
			if (written = file->write((const uint8_t*)m_data.data(), m_data.length()); written != m_data.length())
			{
				log_warn(fmt::format("\t...written only '{}', but should have '{}'", written, m_data.length()));
			}
			else
			{
				log_info(fmt::format("\t...success writing '{}' bytes", written));
			}

			//- set cursor position to start of file for read/write
			if (!file->seek_to_start())
			{
				log_error(fmt::format("\t...failed seeking to start. Expect subsequent operations to be failures"));
			}
			else
			{
				log_info(fmt::format("\t...file cursor position '{}'", file->tell()));
			}
		}

		//- read from file and write to console
		{
			vector_t<uint8_t> buffer(m_data.length());

			auto read = 0;
			if (read = file->read(buffer.data(), m_data.length()); read != m_data.length())
			{
				log_warn(fmt::format("\t...read only '{}', but should have '{}'", read, m_data.length()));
			}
			else
			{
				log_info(fmt::format("\t...success reading '{}' bytes", read));
			}

			//- set cursor position to start of file for read/write
			if (!file->seek_to_start())
			{
				log_error(fmt::format("\t...failed seeking to start. Expect subsequent operations to be failures"));
			}
			else
			{
				log_info(fmt::format("\t...file cursor position '{}'", file->tell()));
			}
		}

		//- copy file from one to another
		{
			if (!fs->copy_file({ C_FILENAME.data() }, { C_COPY_FILENAME.data() }))
			{
				log_error(fmt::format("\t...failed copying file!"));
			}
		}

		//- read from copy and write to console
		{
			if (copied_file = fs->open({ C_COPY_FILENAME.data() }, core::file_mode_read_write); !copied_file)
			{
				log_critical(fmt::format("\t...failed opening copied file!"));

				//- fatal error, does not make sense to proceed
				return false;
			}

			vector_t<uint8_t> buffer(m_data.length());

			if (const auto read = copied_file->read(buffer.data(), m_data.length()); read != m_data.length())
			{
				log_warn(fmt::format("\t...read only '{}' from copied file, but should have '{}'", read, m_data.length()));
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
				log_error(fmt::format("\t...failed renaming file!"));
			}
		}

		//- remove all files created for testing
		{
			if (!fs->remove_file({ C_COPY_RENAME_FILENAME.data() }))
			{
				log_error(fmt::format("\t...failed removing file '{}'!", C_COPY_RENAME_FILENAME.data()));
			}
			if (!fs->remove_file({ C_FILENAME.data() }))
			{
				log_error(fmt::format("\t...failed removing file '{}'!", C_FILENAME.data()));
			}
			if (!fs->remove_file({ C_COPY_FILENAME.data() }))
			{
				log_error(fmt::format("\t...failed removing file '{}'!", C_COPY_FILENAME.data()));
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
	log_warn(fmt::format("\n#### Filesystem '{}' Testing Run ##########################################",
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

//------------------------------------------------------------------------------------------------------------------------
int __real_main(int argc, char* argv[])
{
	AllocConsole();

	logging::clog::instance().init(core::logging_verbosity::logging_verbosity_debug);

	core::set_logger(core_io_error_function);
	sm::set_logger(core_io_error_function);

	log_info(fmt::format("Unittests result: '{}'", UnitTest::RunAllTests()));

	if (false)
	{
		virtual_filesystem_tests();
	}

	engine::cengine::sconfig cfg;

	cfg.m_layers_cfg.emplace_back("cexample_bad_app_layer");
	cfg.m_layers_cfg.emplace_back("cexample_app_layer");
	cfg.m_layers_cfg.emplace_back("cgame");
	cfg.m_layers_cfg.emplace_back("ceditor");

	sm::configure(&engine::cengine::instance(),	//- engine class as the application
		(void*)&cfg,							//- engine configuration
		argc,									//- command line args count
		argv);									//- command line args values

	sm::init("spritemancer testbed", 1920, 1080, false, true);

	sm::run();

	logging::clog::instance().shutdown();

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
