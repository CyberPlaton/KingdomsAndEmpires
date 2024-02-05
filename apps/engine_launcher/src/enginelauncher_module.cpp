#include "enginelauncher_module.hpp"
#include <iostream>

void logging_function(const char* tag, uint32_t level, uint32_t item_id, const char* message, uint32_t line, const char* filename, void* data)
{
	logging::log_debug(fmt::format("[SOKOL] {}", message).data());
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	AllocConsole();

	logging::init();
	logging::log_debug(fmt::format("Starting on WinMain()").data());
	logging::log_trace("Log log_trace");
	logging::log_debug("Log log_debug");
	logging::log_info("Log log_info");
	logging::log_warn("Log log_warn");
	logging::log_error("Log log_error");
	logging::log_critical("Log log_critical");

	//- use rttr registered structures at least once in main application
	//- in order for them to be registered
	technology::itech tech;
	auto race = kingdoms::kingdom_race_dark_elf;

	core::cuuid uuid;
	core::scolor color(0, 0, 255, 255);
	core::srect rect(0.0f, 0.0f, 1.0f, 1.0f);

	auto uuid_json = io::to_json(uuid);
	auto color_json = io::to_json(color);
	auto srect_json = io::to_json(rect);

	logging::log_info(fmt::format("uuid: '{}'", uuid_json));
	logging::log_info(fmt::format("color: '{}'", color_json));
	logging::log_info(fmt::format("rect: '{}'", srect_json));

// 	core::cfile file(core::cfilesystem::construct("cereal_any", ".json"), core::file_read_write_mode_write
// 		| core::file_read_write_mode_text
// 		| core::file_read_write_mode_cereal);
// 
// 	core::cuuid uuid;
// 	ssome_data datastructure, datastructure2;
// 	datastructure.m_data.emplace_back(1);
// 	datastructure.m_uuid = uuid.c_str();
// 
// 	file.write_sync_cereal(datastructure);
// 
// 	core::cfile file2(core::cfilesystem::construct("somepath", ".json"), core::file_read_write_mode_read
// 		| core::file_read_write_mode_text
// 		| core::file_read_write_mode_cereal);
// 
// 	file2.read_sync_cereal(datastructure2);


	//- Important! Create a new world
	auto& inst = ecs::cworld_manager::instance();
	inst.create("My World");

	//- Get current world
	auto& world = inst.active();

	world.mm().import_module<module_example::cmy_second_module>();
	world.mm().import_module<module_example::cmy_third_module>();

	logging::log_info(fmt::format("Type itech valid: '{}'", rttr::type::get_by_name("itech").is_valid() ? "True" : "False"));

	logging::log_debug("Registered rttr types");
	for (auto type : rttr::type::get_types())
	{
		logging::log_debug(fmt::format("\t'{}'", type.get_name().data()));
	}

	Sleep(10);
	return 0;
}
#else
int main(int argc, char* argv[])
{
	logging::init();
	logging::log_debug(fmt::format("Starting on main()"));
	logging::log_trace("Log log_trace");
	logging::log_debug("Log log_debug");
	logging::log_info("Log log_info");
	logging::log_warn("Log log_warn");
	logging::log_error("Log log_error");
	logging::log_critical("Log log_critical");

	return 0;
}
#endif