#include "enginelauncher_module.hpp"
#include <iostream>

struct stest
{
	kingdoms::sskills skills;
	core::cuuid uuid;
	core::srect rect;
	vec2_t vec;
	vector_t<float> damages;
	ecs::stransform transform;
	ecs::ssprite sprite;

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
		.property("transform", &stest::transform)
		.property("sprite", &stest::sprite)
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

void core_io_slang_error_function(uint8_t level, const char* message)
{
	core_io_error_function(level, message);
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

	slang::slang_logger().init(core_io_slang_error_function, slang::detail::log_level_trace);

	/*
	class Tree
	{
		def init(depth)
		{
			this.depth = depth;
			if (depth > 0)
			{
				this.a = Tree(depth - 1);
				this.b = Tree(depth - 1);
				this.c = Tree(depth - 1);
				this.d = Tree(depth - 1);
				this.e = Tree(depth - 1);
			}
		}

		def walk()
		{
			if (this.depth == 0) return 0;
			return this.depth
			+ this.a.walk()
			+ this.b.walk()
			+ this.c.walk()
			+ this.d.walk()
			+ this.e.walk();
		}
	}

	var tree = Tree(8);

	var start = clock();

	for (var i = 0; i < 100; i = i + 1)
	{
		if (tree.walk() != 122068) print "Error";
	}

	print clock() - start;
	*/

	const char* code =
		"class Tree"
		"{"
		"	def init(depth)"
		"	{"
		"		this.depth = depth;"
		"	}"
		"}"
		""
		"var tree = Tree(8);"
		;

	auto* state = slang::slang_open();
	auto result = slang::slang_compile(state, code);
	slang::slang_close(state);


// 	slang::detail::schunk chunk;
// 	chunk.m_constants.push_back(slang::svalue::create(1.2f, slang::value_type_float));
// 	chunk.m_code.push_back(slang::detail::opcode_constant);
// 	chunk.m_code.push_back(0);
// 
// 	auto p = slang::debug::print_chunk(chunk);
// 
// 	slang::slang_print(slang::detail::log_level_info, true, p.c_str());

// 	logging::log_debug("Registered rttr types");
// 	for (auto type : rttr::type::get_types())
// 	{
// 		logging::log_debug(fmt::format("\t'{}'", type.get_name().data()));
// 	}
// 
// 	//- set core io callback
// 	core::serror_reporter::instance().m_callback = core_io_error_function;
// 
// 	engine::cengine::sconfig cfg;
// 
// 	//- service registration can be done from json file
// 	cfg.m_service_cfg.m_services.emplace_back(rttr::type::get<camera_system::ccamera_manager>().get_name().data());
// 
// 	cfg.m_window_cfg.m_title = "Kingdoms & Empires";
// 	cfg.m_window_cfg.m_width = 1280;
// 	cfg.m_window_cfg.m_height = 1024;
// 	cfg.m_window_cfg.m_target_fps = 0;
// 	cfg.m_window_cfg.m_resolution = sm::window_resolution_steamdeck;
// 	cfg.m_window_cfg.m_flags = sm::window_flag_vsync | sm::window_flag_show;
// 	
// 	{
// 		auto cfg_json = core::io::to_json_string(cfg);
// 
// 		core::cfile::save_text("config.json", cfg_json);
// 
// 		logging::log_info(fmt::format("cengine::sconfig: '{}'", cfg_json));
// 	}
// 
// 	if (engine::cengine::instance().configure("config.json") == engine::engine_run_result_ok)
// 	{
// 		engine::cengine::instance().run();
// 	}

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
	core::serror_reporter::instance().m_callback = core_io_error_function;

	{
		core::clinked_tree<stest> tree(10);

		auto* node = tree.append_to(nullptr);

		logging::log_debug("Tree depth-first iterative:");
		tree.visit_depth_first([&](core::clinked_tree<stest>::snode* node) -> bool
			{
				logging::log_debug(fmt::format("'{}'", node->uuid.string()));

				return true;
			});
	}

	return 0;


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