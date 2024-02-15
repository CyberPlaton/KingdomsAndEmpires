#include "enginelauncher_module.hpp"
#include <iostream>

struct stest
{
	kingdoms::sskills skills;
	core::cuuid uuid;
	core::srect rect;
	vec2_t vec;
	vector_t<float> damages;

	RTTR_ENABLE();
};

REFLECT_INLINE(stest)
{
	rttr::registration::class_<stest>("stest")
		.constructor<>()
		(
			rttr::policy::ctor::as_object
		)
		.property("skills", &stest::skills)
		.property("uuid", &stest::uuid)
		.property("damages", &stest::damages)
		.property("rect", &stest::rect)
		.property("vec", &stest::vec)
		;

	rttr::default_constructor<vector_t<float>>();
};

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

	logging::log_debug("Registered rttr types");
	for (auto type : rttr::type::get_types())
	{
		logging::log_debug(fmt::format("\t'{}'", type.get_name().data()));
	}

	//- set core io callback
	core::io::serror_reporter::instance().m_callback = core_io_error_function;

	core::crandom rand;

	stest test;
	test.skills.m_block = 100;
	test.skills.m_acrobatics = 25;
	test.rect = {10.0f, 10.0f, 20.0f, 20.0f};
	test.vec = {91.0f, 1.0f};
	for(auto i = 0; i < 10; ++i)
	{
		test.damages.push_back(rand.normalized_float());
	}

	logging::log_debug("----------Serializing----------");
	auto json = core::io::to_json_string(test, true);

	logging::log_info(fmt::format("stest: '{}'", json));


	logging::log_debug("----------Deserializing----------");
	auto deser = core::io::from_json_string(test.get_type(), json);

	auto deser_json = core::io::to_json_string(deser, true);

	logging::log_info(fmt::format("deser stest: '{}'", deser_json));


// 	ecs::ccomponent_manager manager;
// 
// 	uint64_t index = 0;
// 	uint64_t components = 0;
// 	logging::log_debug(fmt::format("Components: '{}'", components));
// 
// 	auto& tr = manager.add<ecs::stransform>(&index);
// 	tr.m_x = 24.0f;
// 	tr.m_y = 11920;
// 	tr.m_w = 10;
// 	tr.m_h = 10;
// 	tr.m_rotation = 11;
// 
// 	bitset(components, rttr::type::get<ecs::stransform>().get_id());
// 	logging::log_debug(fmt::format("Components: '{}' (added transform)", components));
// 
// 	bitset(components, rttr::type::get<ecs::ssprite>().get_id());
// 	logging::log_debug(fmt::format("Components: '{}' (added sprite)", components));
// 
// 	if (bitcheck(components, rttr::type::get<ecs::ssprite>().get_id()))
// 	{
// 		logging::log_debug("...has sprite");
// 	}
// 	if (bitcheck(components, rttr::type::get<ecs::stransform>().get_id()))
// 	{
// 		logging::log_debug("...has transform");
// 	}
// 	bitclear(components, rttr::type::get<ecs::stransform>().get_id());
// 	if (bitcheck(components, rttr::type::get<ecs::ssprite>().get_id()))
// 	{
// 		logging::log_debug("...has sprite");
// 	}
// 	if (bitcheck(components, rttr::type::get<ecs::stransform>().get_id()))
// 	{
// 		logging::log_debug("...has transform");
// 	}
// 
// 
// 	uint64_t second_index = 0;
// 
// 	auto& tran = manager.add<ecs::stransform>(&second_index);
// 	tran.m_x = 1.0f;
// 	tran.m_y = 120;
// 	tran.m_w = 25;
// 	tran.m_h = 25;
// 	tran.m_rotation = 0.5f;
// 
// 
// 	uint64_t third_index = 0;
// 
// 	auto& sprite = manager.add<ecs::ssprite>(&third_index);


	logging::log_debug("//------------------------------------------------------------------------------------------------------------------------");

	logging::log_debug("Registered rttr types");
	for (auto type : rttr::type::get_types())
	{
		logging::log_debug(fmt::format("\t'{}'", type.get_name().data()));
	}

	logging::log_debug("//------------------------------------------------------------------------------------------------------------------------");

	auto type = rttr::type::get_by_name("chuman_race_module");

	flecs::world world;

	auto var = type.create({world});

	auto human_prefab = world.lookup(races::C_HUMAN_CITIZEN_PREFAB_NAME);

	auto inst = world.entity("Walther").is_a(human_prefab);

	auto* prefab = human_prefab.get_mut<races::shuman>();
	auto* walther = inst.get_mut<races::shuman>();

	logging::log_warn(fmt::format("Prefab Light Armor '{}'", prefab->m_skills.m_light_armor));
	logging::log_warn(fmt::format("Walther Light Armor '{}'", walther->m_skills.m_light_armor));
	logging::log_warn("\tchanging Walthers skill level...");

	walther->m_skills.m_light_armor = 50;
	walther->m_attributes.m_agility = 68;

	logging::log_warn(fmt::format("Prefab Light Armor '{}'", prefab->m_skills.m_light_armor));
	logging::log_warn(fmt::format("Walther Light Armor '{}'", walther->m_skills.m_light_armor));

	logging::log_warn("\tchanging prefabs skill level and creating another human instance...");
	prefab->m_skills.m_light_armor = 30;

	auto sec_inst = world.entity("Manfred").is_a(human_prefab);
	auto* manfred = sec_inst.get_mut<races::shuman>();

	logging::log_warn(fmt::format("Prefab Light Armor '{}'", prefab->m_skills.m_light_armor));
	logging::log_warn(fmt::format("Walther Light Armor '{}'", walther->m_skills.m_light_armor));
	logging::log_warn(fmt::format("Manfred Light Armor '{}'", manfred->m_skills.m_light_armor));
	logging::log_debug("//------------------------------------------------------------------------------------------------------------------------");

	//- check components
// 	ecs::sidentifier identifier; identifier.m_self = inst;
// 	ecs::stransform transform;
// 	ecs::ssprite sprite;
// 	ecs::sanimation animation;
// 	ecs::shierarchy hierarchy;
// 
// 	inst.set<ecs::sidentifier>(identifier);
// 	inst.set<ecs::stransform>(transform);
// 	inst.set<ecs::ssprite>(sprite);
// 	inst.set<ecs::sanimation>(animation);
// 	inst.set<ecs::shierarchy>(hierarchy);

// 	auto json = io::to_json(identifier);
// 	logging::log_info(fmt::format("sidentifier: '{}'", json));
// 
// 	json = io::to_json(transform);
// 	logging::log_info(fmt::format("transform: '{}'", json));
// 
// 	json = io::to_json(sprite);
// 	logging::log_info(fmt::format("sprite: '{}'", json));
// 
// 	json = io::to_json(animation);
// 	logging::log_info(fmt::format("animation: '{}'", json));
// 
// 	json = io::to_json(hierarchy);
// 	logging::log_info(fmt::format("hierarchy: '{}'", json));

	logging::log_debug("//------------------------------------------------------------------------------------------------------------------------");

	engine::cengine::sconfig cfg;

	//- service registration can be done from json file
	cfg.m_service_cfg.m_services.emplace_back(rttr::type::get<camera_system::ccamera_manager>().get_name().data());

	cfg.m_window_cfg.m_title = "Kingdoms & Empires";
	cfg.m_window_cfg.m_width = 1024;
	cfg.m_window_cfg.m_height = 920;
	cfg.m_window_cfg.m_target_fps = 60;
	cfg.m_window_cfg.m_resolution = sm::window_resolution_steamdeck;
	cfg.m_window_cfg.m_flags = sm::window_flag_vsync | sm::window_flag_show;
	
	{
		auto cfg_json = core::io::to_json_string(cfg);

		core::cfile::save_text("config.json", cfg_json);

		logging::log_info(fmt::format("cengine::sconfig: '{}'", cfg_json));
	}


	if (engine::cengine::instance().configure("config.json") == engine::engine_run_result_ok)
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

	auto uuid_json = core::io::to_json_string(uuid);
	auto color_json = core::io::to_json_string(color);
	auto srect_json = core::io::to_json_string(rect);

	logging::log_info(fmt::format("uuid: '{}'", uuid_json));
	logging::log_info(fmt::format("color: '{}'", color_json));
	logging::log_info(fmt::format("rect: '{}'", srect_json));

	logging::log_info(fmt::format("Type itech valid: '{}'", rttr::type::get_by_name("itech").is_valid() ? "True" : "False"));
	logging::log_info(fmt::format("Type sskills valid: '{}'", rttr::type::get_by_name("sskills").is_valid() ? "True" : "False"));

	auto skills_type = rttr::type::get_by_name("sskills");
	logging::log_info("kingdoms::sskills properties:");
	for (const auto& prop : skills_type.get_properties())
	{
		auto meta = prop.get_metadata(kingdoms::C_DISPLAY_NAME_PROP);

		logging::log_info(fmt::format("\tDisplayName: '{}'", meta.get_value<std::string>()));
	}

	auto tech_type = rttr::type::get_by_name("itech");
	logging::log_info("technology::itech properties:");
	for (const auto& prop : tech_type.get_properties())
	{
		logging::log_info(fmt::format("\tproperty: '{}'", prop.get_name().data()));
	}

	//- Important! Create a new world
	//auto& inst = ecs::cworld_manager::instance();
	//inst.create("My World");

	//- Get current world
	//auto& world = inst.active();

	//world.mm().import_module<module_example::cmy_second_module>();
	//world.mm().import_module<module_example::cmy_third_module>();

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

//- set core io callback
	core::io::serror_reporter::instance().m_callback = core_io_error_function;

	core::crandom rand;

	stest test;
	test.skills.m_block = 100;
	test.skills.m_acrobatics = 25;
	test.rect = {10.0f, 10.0f, 20.0f, 20.0f};
	test.vec = {91.0f, 1.0f};
	for(auto i = 0; i < 10; ++i)
	{
		test.damages.push_back(rand.normalized_float());
	}

	logging::log_debug("----------Serializing----------");
	auto json = core::io::to_json_string(test);

	logging::log_info(fmt::format("stest: '{}'", json));


	logging::log_debug("----------Deserializing----------");
	auto deser = core::io::from_json_string(test.get_type(), json);

	auto deser_json = core::io::to_json_string(deser);

	logging::log_info(fmt::format("deser stest: '{}'", deser_json));


	return 0;

	auto type = rttr::type::get_by_name("cmy_third_module");

	flecs::world world;

	auto var = type.create({world});

	if (!var.is_valid())
	{
		return -1;
	}



	lua_State* state = luaL_newstate();
	luaL_openlibs(state);

	lua_close(state);

	//- use rttr registered structures at least once in main application
	//- in order for them to be registered
	kingdoms::sskills skills;
	auto race = kingdoms::kingdom_race_dark_elf;

	core::cuuid uuid;
	core::scolor color(0, 0, 255, 255);
	core::srect rect(0.0f, 0.0f, 1.0f, 1.0f);

	auto uuid_json = core::io::to_json_string(uuid);
	auto color_json = core::io::to_json_string(color);
	auto srect_json = core::io::to_json_string(rect);

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

		logging::log_info(fmt::format("\tDisplayName: '{}'", meta.get_value<std::string>()));
	}

	auto tech_type = rttr::type::get_by_name("itech");
	logging::log_info("technology::itech properties:");
	for (const auto& prop : tech_type.get_properties())
	{
		logging::log_info(fmt::format("\tproperty: '{}'", prop.get_name().data()));
	}

	//- Important! Create a new world
	//auto& inst = ecs::cworld_manager::instance();
	//inst.create("My World");

	//- Get current world
	//auto& world = inst.active();

	//world.mm().import_module<module_example::cmy_second_module>();
	//world.mm().import_module<module_example::cmy_third_module>();

	engine::cengine::sconfig cfg;
	cfg.m_window_cfg.m_title = "Kingdoms & Empires";
	cfg.m_window_cfg.m_width = 720;
	cfg.m_window_cfg.m_height = 648;
	cfg.m_window_cfg.m_target_fps = 60;

	if (engine::cengine::instance().configure(cfg) == engine::engine_run_result_ok)
	{
		engine::cengine::instance().run();
	}

	return 0;
}
#endif