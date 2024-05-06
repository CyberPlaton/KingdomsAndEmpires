#include "tiny_editor_module.hpp"
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

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	AllocConsole();

	logging::init();

	engine::cengine::sconfig cfg;
	cfg.m_service_cfg.m_services.emplace_back("cthread_service");
	cfg.m_service_cfg.m_services.emplace_back("cevent_service");
	cfg.m_service_cfg.m_services.emplace_back("ctexture_manager");
	cfg.m_service_cfg.m_services.emplace_back("cshader_manager");
	cfg.m_service_cfg.m_services.emplace_back("cmaterial_manager");
	cfg.m_service_cfg.m_services.emplace_back("cspriteatlas_manager");
	cfg.m_service_cfg.m_services.emplace_back("crendertarget_manager");

	cfg.m_layer_cfg.m_layers.emplace_back("cgame");
	cfg.m_layer_cfg.m_layers.emplace_back("ceditor");

	cfg.m_window_cfg.m_title = "Tiny Editor";
	cfg.m_window_cfg.m_width = 1280;
	cfg.m_window_cfg.m_height = 1024;
	cfg.m_window_cfg.m_target_fps = 0;
	cfg.m_window_cfg.m_flags = sm::window_flag_vsync | sm::window_flag_show;

	if (engine::cengine::instance().configure(cfg/*"config.json"*/) == engine::engine_run_result_ok)
	{
		logging::log_debug(fmt::format("Cwd: '{}'", core::cfilesystem::cwd().view()));

		ecs::cworld_manager::instance().create("World");

		auto& w = ecs::cworld_manager::instance().active();

		//- can´t use more than what was configured as foreground threads
		//w.use_threads(engine::cthread_service::hardware_threads() / 2);

		core::crandom rand;

		//- import module with required components
		w.mm().import_module<module_example::cmy_second_module>();
		w.mm().import_module<effects::cstatus_effects_module>();
		w.mm().import_module<render_system::cscene_render_module>();
		w.mm().import_module<render_system::canimation_module>();

		auto& human = w.pm().create("Human");

		auto cam = w.em().create_entity();
		cam.add<ecs::scamera>();

		auto* cam_camera = cam.get_mut<ecs::scamera>();
		cam_camera->m_position = { 0.0f, 0.0f };
		cam_camera->m_zoom = 0.5f;
		cam_camera->m_rotation = 0.0f;
		cam_camera->m_active = true;
		cam_camera->m_offset = { 0.0f, 0.0f };
		cam_camera->m_layer = 0;
		cam_camera->m_viewrect = { 0.0f, 0.0f, 1280.0f, 1024.0f };

		//- load texture for sprite
		auto tex = sm::ctx().tm().load("sprite", "resources/figure_paladin_14.png");
		const auto& texture = sm::ctx().tm().get(tex);

		for (auto i = 0u; i < 15; ++i)
		{
			for (auto j = 0u; j < 15; ++j)
			{
				auto e = w.em().create_entity();

				e.add<module_example::stargeting_component>();
				e.add<ecs::stransform>();
				e.add<ecs::ssprite>();

				auto* transform = e.get_mut<ecs::stransform>();
				transform->m_position = { i * 64 % 1024, j * 64 % 1024 };
				transform->m_size = { 64, 64 };

				auto* sprite = e.get_mut<ecs::ssprite>();
				sprite->m_source_rectangle = { 0.0f, 0.0f, (float)texture.width(), (float)texture.height() };
				sprite->m_tint = core::scolor(rand.in_range_int(50, 255), rand.in_range_int(50, 255), rand.in_range_int(50, 255), 255);
				sprite->m_materials.emplace_back(tex, sm::ctx().mm().at(sm::C_DEFAULT_MATERIAL_NAME));

			}
		}

		//w.save("MyWorld.world");

		//- create several entities for testing
// 		auto walther = w.em().create_entity();
// 		auto adolf = w.em().create_entity();
// 		auto manfred = w.em().create_entity();
// 		auto hans = w.em().create_entity();
// 		auto peter = w.em().create_entity();
// 
// 		walther.add<module_example::stargeting_component>();
// 		adolf.add<module_example::stargeting_component>();
// 		manfred.add<module_example::stargeting_component>();
// 		hans.add<module_example::stargeting_component>();
// 		peter.add<module_example::stargeting_component>();

		//- TODO: individual resource managers should either be thread safe or
		//- we have to synchronize access otherwise, as the access will be across threads.
		//- auto* rms = engine::cengine::service<engine::cresource_management_service>();
		//- auto* tm = engine::cengine::resource_manager<sm::internal::ctexture_manager>();
		//- return type should be adjusted for async too, and allow immediate and async loading.
		//- e.g. core::cfuture_type<texture_t> result
		//- auto texture = tm->create("Dude", "some/path/to/Dude.png");

		engine::cengine::instance().run();
	}
	return 0;
}
#else
int main(int argc, char* argv[])
{
	logging::init();

	engine::cengine::sconfig cfg;
	cfg.m_service_cfg.m_services.emplace_back("ccamera_manager");
	cfg.m_service_cfg.m_services.emplace_back("cthread_service");

	cfg.m_layer_cfg.m_layers.emplace_back("cgame");
	cfg.m_layer_cfg.m_layers.emplace_back("ceditor");

	cfg.m_window_cfg.m_title = "Tiny Editor";
	cfg.m_window_cfg.m_width = 1280;
	cfg.m_window_cfg.m_height = 1024;
	cfg.m_window_cfg.m_target_fps = 0;
	cfg.m_window_cfg.m_flags = sm::window_flag_vsync | sm::window_flag_show;

	if (engine::cengine::instance().configure(cfg/*"config.json"*/) == engine::engine_run_result_ok)
	{
		ecs::cworld_manager::instance().create("World");

		auto& w = ecs::cworld_manager::instance().active();

		//- can´t use more than what was configured as foreground threads
		w.use_threads(engine::cthread_service::hardware_threads() / 2);

		//- import module with required components
		w.mm().import_module<module_example::cmy_second_module>();

		//- create several entities for testing
		auto walther = w.em().create_entity();
		auto adolf = w.em().create_entity();
		auto manfred = w.em().create_entity();
		auto hans = w.em().create_entity();
		auto peter = w.em().create_entity();

		walther.add<module_example::stargeting_component>();
		adolf.add<module_example::stargeting_component>();
		manfred.add<module_example::stargeting_component>();
		hans.add<module_example::stargeting_component>();
		peter.add<module_example::stargeting_component>();

		engine::cengine::instance().run();
	}
	return 0;
}
#endif
