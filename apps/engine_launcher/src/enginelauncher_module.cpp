#include "enginelauncher_module.hpp"
#include <iostream>

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

	engine::cengine::sconfig cfg;
	cfg.m_window_cfg.m_title = "Kingdoms & Empires";
	cfg.m_window_cfg.m_width = 720;
	cfg.m_window_cfg.m_height = 648;
	cfg.m_window_cfg.m_target_fps = 60;

	if (engine::cengine::instance().configure(cfg) == engine::engine_run_result_ok)
	{
		engine::cengine::instance().run();
	}

	//- use rttr registered structures at least once in main application
	//- in order for them to be registered
	kingdoms::sskills skills;
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

	logging::log_info(fmt::format("Type itech valid: '{}'", rttr::type::get_by_name("itech").is_valid() ? "True" : "False"));
	logging::log_info(fmt::format("Type sskills valid: '{}'", rttr::type::get_by_name("sskills").is_valid() ? "True" : "False"));

	logging::log_debug("Registered rttr types");
	for (auto type : rttr::type::get_types())
	{
		logging::log_debug(fmt::format("\t'{}'", type.get_name().data()));
	}

	auto skills_type = rttr::type::get_by_name("sskills");
	logging::log_info("kingdoms::sskills properties:");
	for (const auto& prop : skills_type.get_properties())
	{
		auto meta = prop.get_metadata(kingdoms::C_DISPLAY_NAME_PROP);

		logging::log_info(fmt::format("\tDisplayName: '{}'", meta.get_value<string_t>()));
	}

	auto tech_type = rttr::type::get_by_name("itech");
	logging::log_info("technology::itech properties:");
	for (const auto& prop : tech_type.get_properties())
	{
		logging::log_info(fmt::format("\tproperty: '{}'", prop.get_name().data()));
	}

	//- Important! Create a new world
	auto& inst = ecs::cworld_manager::instance();
	inst.create("My World");

	//- Get current world
	auto& world = inst.active();

	world.mm().import_module<module_example::cmy_second_module>();
	world.mm().import_module<module_example::cmy_third_module>();

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

	lua_State* state = luaL_newstate();
	luaL_openlibs(state);

	lua_close(state);

	return 0;
}
#endif