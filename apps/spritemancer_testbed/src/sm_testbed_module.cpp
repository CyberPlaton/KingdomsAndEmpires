#include "sm_testbed_module.hpp"
#include <iostream>

void core_io_error_function(uint8_t level, const std::string& message);
void configure_args(const args_ref_t& args);

//------------------------------------------------------------------------------------------------------------------------
void try_compile_shaders()
{
	constexpr stringview_t C_VERTEX_DEFAULT =
		"$input a_position, a_color0, a_texcoord0					\n"
		"$output v_color0, v_texcoord0								\n"
		"#include <common.sh>										\n"
		"void main()												\n"
		"{															\n"
		"	gl_Position = mul(u_viewProj, vec4(a_position, 1.0));	\n"
		"	v_color0 = a_color0;									\n"
		"	v_texcoord0 = a_texcoord0;								\n"
		"};															\n"
		"\0"
		;

	constexpr stringview_t C_FRAGMENT_DEFAULT =
		"$input v_color0, v_texcoord0								\n"
		"#include <common.sh>										\n"
		"SAMPLER2D(s_tex, 0);										\n"
		"void main()												\n"
		"{															\n"
		"	vec4 color = v_color0 * texture2D(s_tex, v_texcoord0);	\n"
		"	if (color.w < 1.0/255.0) { discard; }					\n"
		"	gl_FragColor = color;									\n"
		"};															\n"
		"\0"
		;

	auto* sm= engine::cengine::service<sm::cshader_manager>();

	string_t vertex_name = "default_vertex_shader";
	string_t pixel_name = "default_pixel_shader";

	sm->load_sync(vertex_name.data(), sm::shader_type_vertex, C_VERTEX_DEFAULT.data());
	sm->load_sync(pixel_name.data(), sm::shader_type_pixel, C_FRAGMENT_DEFAULT.data());
}

//------------------------------------------------------------------------------------------------------------------------
bool cspritemancer::init()
{
	//- Create world for testing
	ecs::world::sconfig cfg;
	cfg.m_name = "spritemancer_world";
	cfg.m_threads = MAX(unsigned);

	ecs::cworld_manager::instance().create(cfg, true);

	//- Add shaders virtual file path and load color program
	auto* vfs = engine::cengine::service<fs::cvirtual_filesystem>();
	const auto& vfs_base = vfs->find_filesystem("/");
	if (auto filesystem = std::make_shared<io::cnative_filesystem>(); filesystem->init(string_utils::join(vfs_base->base_path(), "shaders"), "/shaders"))
	{
		vfs->add_filesystem("/shaders", filesystem);
	}

	//try_compile_shaders();

	const auto& vfs_shaders = vfs->find_filesystem("/shaders");
	auto* sm = engine::cengine::service<sm::cshader_manager>();
	auto* pm = engine::cengine::service<sm::cprogram_manager>();

	auto vs = sm->load_async("vs_color", sm::shader_type_vertex, string_utils::join(vfs_shaders->base_path(), "color/vs_color.sc"));
	auto fs = sm->load_async("fs_color", sm::shader_type_pixel, string_utils::join(vfs_shaders->base_path(), "color/fs_color.sc"));
	
	auto color_program = pm->load_sync("program_color", vs.wait(), fs.wait());

	return true;
}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::shutdown()
{

}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::on_update(float dt)
{
}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::on_world_render()
{

}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::on_ui_render()
{
	ImGui::ShowDemoWindow(nullptr);
}

//------------------------------------------------------------------------------------------------------------------------
void cspritemancer::on_post_update(float dt)
{

}

//------------------------------------------------------------------------------------------------------------------------
engine::sconfig spritemancer_main(const args_ref_t& args)
{
	engine::sconfig cfg;

	cfg.m_startup_project = "/spritemancer.project";
	cfg.m_layers_cfg.emplace_back("cspritemancer");
	cfg.m_mode = core::launch_context_client;
	cfg.m_title = "spritemancer";
	cfg.m_fullscreen = false;
	cfg.m_vsync = false;
	cfg.m_window_width = 1680;
	cfg.m_window_height = 920;

#if CORE_DEBUG
	cfg.m_logging_verbosity = core::logging_verbosity_debug;
	cfg.m_show_console = true;
#else
	cfg.m_logging_verbosity = core::logging_verbosity_error;
	cfg.m_show_console = false;
#endif
	cfg.m_logging_reporter = core_io_error_function;

	return cfg;
}

ENGINE_MAIN(spritemancer_main, configure_args);

//------------------------------------------------------------------------------------------------------------------------
void core_io_error_function(uint8_t level, const std::string& message)
{
	switch (level)
	{
	case SPDLOG_LEVEL_TRACE:
	{
		log_trace(message);
		break;
	}
	case SPDLOG_LEVEL_DEBUG:
	{
		log_debug(message);
		break;
	}
	case SPDLOG_LEVEL_INFO:
	{
		log_info(message);
		break;
	}
	case SPDLOG_LEVEL_WARN:
	{
		log_warn(message);
		break;
	}
	case SPDLOG_LEVEL_ERROR:
	{
		log_error(message);
		break;
	}
	case SPDLOG_LEVEL_CRITICAL:
	{
		log_critical(message);
		break;
	}
	}
}

//------------------------------------------------------------------------------------------------------------------------
void configure_args(const args_ref_t& args)
{

}

RTTR_REGISTRATION
{
	REGISTER_LAYER(cspritemancer);
}