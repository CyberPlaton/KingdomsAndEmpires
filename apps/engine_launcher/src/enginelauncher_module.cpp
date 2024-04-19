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

#define MAX_BUILDINGS   100
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


	// Initialization
	//--------------------------------------------------------------------------------------
	const int screenWidth = 800;
	const int screenHeight = 450;

	raylib::InitWindow(screenWidth, screenHeight, "raylib [core] example - 2d camera");

	raylib::Rectangle player = { 400, 280, 40, 40 };
	raylib::Rectangle buildings[MAX_BUILDINGS] = { 0 };
	raylib::Color buildColors[MAX_BUILDINGS] = { 0 };

	int spacing = 0;

	for (int i = 0; i < MAX_BUILDINGS; i++)
	{
		buildings[i].width = (float)raylib::GetRandomValue(50, 200);
		buildings[i].height = (float)raylib::GetRandomValue(100, 800);
		buildings[i].y = screenHeight - 130.0f - buildings[i].height;
		buildings[i].x = -6000.0f + spacing;

		spacing += (int)buildings[i].width;

		buildColors[i].r = raylib::GetRandomValue(200, 240);
		buildColors[i].g = raylib::GetRandomValue(200, 240);
		buildColors[i].b = raylib::GetRandomValue(200, 240);
		buildColors[i].a = 255;
	}

	raylib::Camera2D camera = { 0 };
	camera.target = { player.x + 20.0f, player.y + 20.0f };
	camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	raylib::SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!raylib::WindowShouldClose())        // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		// Player movement
		if (raylib::IsKeyDown(raylib::KEY_RIGHT)) player.x += 2;
		else if (raylib::IsKeyDown(raylib::KEY_LEFT)) player.x -= 2;

		// Camera target follows player
		camera.target = { player.x + 20, player.y + 20 };

		// Camera rotation controls
		if (raylib::IsKeyDown(raylib::KEY_A)) camera.rotation--;
		else if (raylib::IsKeyDown(raylib::KEY_S)) camera.rotation++;

		// Limit camera rotation to 80 degrees (-40 to 40)
		if (camera.rotation > 40) camera.rotation = 40;
		else if (camera.rotation < -40) camera.rotation = -40;

		// Camera zoom controls
		camera.zoom += ((float)raylib::GetMouseWheelMove() * 0.05f);

		if (camera.zoom > 3.0f) camera.zoom = 3.0f;
		else if (camera.zoom < 0.1f) camera.zoom = 0.1f;

		// Camera reset (zoom and rotation)
		if (raylib::IsKeyPressed(raylib::KEY_R))
		{
			camera.zoom = 1.0f;
			camera.rotation = 0.0f;
		}
		//----------------------------------------------------------------------------------

		// Draw
		//----------------------------------------------------------------------------------
		raylib::BeginDrawing();

		raylib::ClearBackground(raylib::RAYWHITE);

		raylib::BeginMode2D(camera);

		raylib::DrawRectangle(-6000, 320, 13000, 8000, raylib::DARKGRAY);

		for (int i = 0; i < MAX_BUILDINGS; i++) DrawRectangleRec(buildings[i], buildColors[i]);

		raylib::DrawRectangleRec(player, raylib::RED);

		raylib::DrawLine((int)camera.target.x, -screenHeight * 10, (int)camera.target.x, screenHeight * 10, raylib::GREEN);
		raylib::DrawLine(-screenWidth * 10, (int)camera.target.y, screenWidth * 10, (int)camera.target.y, raylib::GREEN);

		raylib::EndMode2D();

		raylib::DrawText("SCREEN AREA", 640, 10, 20, raylib::RED);

		raylib::DrawRectangle(0, 0, screenWidth, 5, raylib::RED);
		raylib::DrawRectangle(0, 5, 5, screenHeight - 10, raylib::RED);
		raylib::DrawRectangle(screenWidth - 5, 5, 5, screenHeight - 10, raylib::RED);
		raylib::DrawRectangle(0, screenHeight - 5, screenWidth, 5, raylib::RED);

		raylib::DrawRectangle(10, 10, 250, 113, raylib::Fade(raylib::SKYBLUE, 0.5f));
		raylib::DrawRectangleLines(10, 10, 250, 113, raylib::BLUE);

		raylib::DrawText("Free 2d camera controls:", 20, 20, 10, raylib::BLACK);
		raylib::DrawText("- Right/Left to move Offset", 40, 40, 10, raylib::DARKGRAY);
		raylib::DrawText("- Mouse Wheel to Zoom in-out", 40, 60, 10, raylib::DARKGRAY);
		raylib::DrawText("- A / S to Rotate", 40, 80, 10, raylib::DARKGRAY);
		raylib::DrawText("- R to reset Zoom and Rotation", 40, 100, 10, raylib::DARKGRAY);

		raylib::EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	raylib::ShutdownWindow();        // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;

	//--------------------------------------------------------------------------------------

// 	ai::bt::cbehavior_tree tree("Testing");
// 
// 	auto i = tree.emplace<ai::bt::cfallback>();
// 	auto j = tree.attach_to<ai::bt::caction>(i);
// 	auto k = tree.emplace<ai::bt::csequence>();
// 	auto l = tree.attach_to<ai::bt::ccondition>(k);
// 	auto m = tree.attach_to<ai::bt::caction_logg>(k, "Hello World 1");
// 	auto n = tree.attach_to<ai::bt::caction_logg>(k, "Hello World 2");
// 	auto o = tree.attach_to<ai::bt::caction_logg>(k, "Hello World 3");
// 	auto d = tree.attach_decorator_to<ai::bt::cdecorator>(o);
// 	auto e = tree.attach_decorator_to<ai::bt::cdecorator_inverter>(d);
// 
// 	tree.tick();
// 
// 	Sleep(10);

	//--------------------------------------------------------------------------------------

// 	const char* code =
// 		"1.2;\n"
// 		"true;\n"
// 		"99;\n"
// 		"false;\n"
// 		";\n"
// 		">=\n"
// 		"class Tree\n"
// 		"{\n"
// 		"\t def init(depth)\n"
// 		"\t {\n"
// 		"\t\t this.depth = depth;\n"
// 		"\t }\n"
// 		"}\n"
// 		"\n"
// 		"var tree = Tree(8);\n"
// 		"\0"
// 		;
// 
// 	auto* state = slang::slang_open();
// 	auto result = slang::slang_compile(state, code);
// 	slang::slang_close(state);

	//--------------------------------------------------------------------------------------

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