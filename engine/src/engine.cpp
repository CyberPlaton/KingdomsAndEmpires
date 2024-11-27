#include "engine.hpp"

namespace engine
{
	namespace
	{
		//- Default services required for engine and others
		constexpr array_t<stringview_t, 7> C_SERVICES = { "cthread_service"
														, "cvirtual_filesystem"
														, "cevent_service"
														, "casset_service"
														, "cresource_service"
														, "cmodule_service"
														, "cproject_service" };

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::configure_args(const args_ref_t& args)
	{
		auto& arguments = *args.get();
#if DEBUG
		const auto show_console = true;
#else
		const auto show_console = false;
#endif

		arguments
			.arg("--world",		"-w", std::string())
			.flag("--console",	"-c", show_console);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::prepare(sconfig& cfg)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cengine::~cengine()
	{
		//- FIXME: just for decoy, use some of the services so that they wont get optimized out for not being used...
		cthread_service thread_service_decoy;
		casset_service asset_service_decoy;
		io::cmodule_service module_service_decoy;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cengine::on_init(void* config)
	{
		m_result = engine_run_result_ok;
		m_config = std::move(*(sconfig*)config);

		//- do configuration based on provided arguments

		//- initialize
		register_plugins();
		register_services();
		register_resource_managers();
		register_layers();

		//- load startup project
		auto* project_service = service<cproject_service>();

		if (!project_service->open_project({ m_config.m_startup_project }))
		{
			m_result = engine_run_result_failed_loading_startup_project;
		}

		project_service->set_current(m_config.m_startup_project);

		return m_result == engine_run_result_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::on_update(float dt)
	{
		CORE_ZONE;

		core::cservice_manager::on_update(dt);

		ecs::cworld_manager::instance().tick(dt);

		m_layers.on_update(dt);

		m_layers.on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::on_imgui()
	{
		m_layers.on_ui_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::on_shutdown()
	{
		m_layers.shutdown();

		core::cservice_manager::shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cengine::try_push_layer(stringview_t name)
	{
		if (auto type = rttr::type::get_by_name(name.data()); type.is_valid())
		{
			auto result = true;

			//- check that at least one function is present
			auto update_method			= type.get_method(slayer::C_LAYER_UPDATE_FUNC_NAME.data());
			auto world_render_method	= type.get_method(slayer::C_LAYER_WORLD_RENDER_FUNC_NAME.data());
			auto ui_render_method		= type.get_method(slayer::C_LAYER_UI_RENDER_FUNC_NAME.data());
			auto post_update_method		= type.get_method(slayer::C_LAYER_POST_UPDATE_FUNC_NAME.data());
			auto init_method			= type.get_method(slayer::C_LAYER_INIT_FUNC_NAME.data());
			auto shutdown_method		= type.get_method(slayer::C_LAYER_SHUTDOWN_FUNC_NAME.data());

			//- Check that all methods are valid and issue a warning if not, as this can be detremental to performance
			if (!(update_method.is_valid() && world_render_method.is_valid() && ui_render_method.is_valid() &&
				post_update_method.is_valid() && init_method.is_valid() && shutdown_method.is_valid()))
			{
				log_error("\t\tA function not defined, this can be detremental to performance!");

				//- Check which functions are defined and which not
				for (const auto& func_name : slayer::C_LAYER_FUNC_NAMES)
				{
					if (const auto& meth = type.get_method(func_name.data()); meth.is_valid())
					{
						log_info(fmt::format("\t\t\t'{}'... OK", meth.get_signature().data()));
					}
					else
					{
						log_warn(fmt::format("\t\t\t'{}'... UNDEFINED", func_name.data()));
					}
				}

				result = false;
			}
			//- Check that all methods are declared as static, if not undefined behavior will occurr, so this is an error actually.
			if (!(update_method.is_static() && world_render_method.is_static() && ui_render_method.is_static() &&
				post_update_method.is_static() && init_method.is_static() && shutdown_method.is_static()))
			{
				log_error("\t\tA function not decalred 'static', this will result in undefined behavior!");

				//- Check which functions are static and which not
				for (const auto& func_name : slayer::C_LAYER_FUNC_NAMES)
				{
					if (const auto& meth = type.get_method(func_name.data()); meth.is_valid())
					{
						log_info(fmt::format("\t\t\t'{}'... OK", meth.get_signature().data()));
					}
					else
					{
						log_warn(fmt::format("\t\t\t'{}'... NOT 'static'", func_name.data()));
					}
				}

				result = false;
			}

			if (result)
			{
				m_layers.emplace_new_layer(name, update_method, world_render_method, ui_render_method, post_update_method, init_method, shutdown_method);
				return true;
			}
		}
		else
		{
			log_critical("\t\t...layer type unknown. Make sure the layer is reflected and actually used somewhere throughout the program!");
		}

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::register_plugins()
	{
		log_info("Initializing plugins...");

		//- But first, we have to create the plugin service and all it depends on
		core::cservice_manager::emplace<fs::cvirtual_filesystem>()->on_start();

		auto* plugin_service =  core::cservice_manager::emplace<cplugin_service>();

		for (const auto& plugin : m_config.m_plugins_cfg)
		{
			plugin_service->load(plugin);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::register_services()
	{
		log_info("Initialize services...");

		m_config.m_services_cfg.assign(C_SERVICES.begin(), C_SERVICES.end());

#if PROFILE_ENABLE
		m_config.m_services_cfg.emplace_back("cprofiler_service");
#endif

		for (const auto& name : m_config.m_services_cfg)
		{
			log_info(fmt::format("\t'{}'", name));

			const auto type = rttr::type::get_by_name(name.c_str());

			if (type.is_valid())
			{
				if (core::cservice_manager::emplace(type))
				{
					log_info("\t\t...ok");
				}
				else
				{
					log_critical("\t\t...failed to emplace");
					m_result = engine_run_result_failed_registering_services;
				}
			}
			else
			{
				log_critical("\t\t...service type unknown. Make sure the service is reflected and actually used somewhere throughout the program!");

				//- fail configuration but let all try to register so we know all those that are bogus
				m_result = engine_run_result_failed_registering_services;
			}
		}

		core::cservice_manager::init();

		//- by default we set path to projects resources
		//- TODO: this should be revised and more flexible, e.g. when the game is published the cwd will
		//- certainly change and we must handle it
		auto fs = core::cfilesystem(core::cfilesystem::cwd());
		fs.backwards();
		fs.backwards();

		if (fs.forwards("resources"))
		{
			const char* path = fs.current().view();

			if (auto filesystem = std::make_shared<io::cnative_filesystem>(); filesystem->init(path, "/"))
			{
				service<fs::cvirtual_filesystem>()->add_filesystem("/", filesystem);
			}
			else
			{
				log_error(fmt::format("Failed initializing virtual file system at path '/' for real path '{}'", path));
				m_result = engine_run_result_fail;
			}
		}
		else
		{
			log_error(fmt::format("Failed locating default resources path at '{}'",
				fs.current().view()));
			m_result = engine_run_result_fail;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::register_resource_managers()
	{
		log_info("Initialize resource managers...");

		if (auto* rs = core::cservice_manager::find<cresource_service>(); rs)
		{
			rs->register_manager<sm::cshader, sm::cshader_manager>();
			rs->register_manager<sm::cimage, sm::cimage_manager>();
			rs->register_manager<sm::ctexture, sm::ctexture_manager>();
			rs->register_manager<sm::crendertarget, sm::crendertarget_manager>();
			rs->register_manager<sm::cspriteatlas, sm::cspriteatlas_manager>();
		}
		else
		{
			log_critical("\t... Resource Service does not exist!");

			m_result = engine_run_result_failed_registering_resource_managers;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::register_layers()
	{
		log_info("Pushing layers...");

		for (const auto& layer : m_config.m_layers_cfg)
		{
			log_info(fmt::format("\t'{}'", layer));

			if (!try_push_layer(layer))
			{
				log_error("\t\t...fail");

				//- fail configuration but let all try to register so we know all those that are bogus
				m_result = engine_run_result_failed_pushing_layers;
			}
			else
			{
				log_info("\t\t...ok");
			}
		}

		log_info("Initializing layers...");

		m_layers.init();
	}

} //- engine

RTTR_REGISTRATION
{
	using namespace engine;
	using namespace editor;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<sconfig>("sconfig")
		.prop("m_title",			&sconfig::m_title)
		.prop("m_window_width",		&sconfig::m_window_width)
		.prop("m_window_height",	&sconfig::m_window_height)
		.prop("m_fullscreen",		&sconfig::m_fullscreen)
		.prop("m_vsync",			&sconfig::m_vsync)
		.prop("m_services_cfg",		&sconfig::m_services_cfg)
		.prop("m_layers_cfg",		&sconfig::m_layers_cfg)
		.prop("m_plugins_cfg",		&sconfig::m_plugins_cfg)
		.prop("m_startup_project",	&sconfig::m_startup_project)
		.prop("m_mode",				&sconfig::m_mode)
		.prop("m_logging_verbosity",&sconfig::m_logging_verbosity)
		.prop("m_show_console",		&sconfig::m_show_console)
		;

	rttr::detail::default_constructor<vector_t<string_t>>();

	//- Section: editor
	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<sproject>("sproject")
		.prop("m_cfg", &sproject::m_cfg)
		;

	rttr::cregistrator<sproject::sconfig>("sproject::sconfig")
		.prop("m_project_plugins",	&sproject::sconfig::m_project_plugins)
		.prop("m_basepath",			&sproject::sconfig::m_basepath)
		.prop("m_project_name",		&sproject::sconfig::m_project_name)
		.prop("m_startup_world",	&sproject::sconfig::m_startup_world)
		;

	//- Section: ecs
	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<ecs::tag::sinvisible>("sinvisible")
		;

	rttr::ccomponent<ecs::shierarchy>("shierarchy")
		.prop("m_parent", &ecs::shierarchy::m_parent)
		.prop("m_children", &ecs::shierarchy::m_children)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ecs::shierarchy::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &ecs::shierarchy::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &ecs::shierarchy::remove)
		;

	rttr::ccomponent<ecs::scamera>("scamera")
		.prop("m_position", &ecs::scamera::m_position)
		.prop("m_offset", &ecs::scamera::m_offset)
		.prop("m_zoom", &ecs::scamera::m_zoom)
		.prop("m_rotation", &ecs::scamera::m_rotation)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ecs::scamera::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &ecs::scamera::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &ecs::scamera::remove)
		;

	rttr::ccomponent<ecs::sanimation>("sanimation")
		.prop("m_animation", &ecs::sanimation::m_animation)
		.prop("m_animation_state", &ecs::sanimation::m_animation_state)
		.prop("m_animation_state_changed", &ecs::sanimation::m_animation_state_changed)
		.prop("m_current_keyframe", &ecs::sanimation::m_current_keyframe)
		.prop("m_current_keyframe_index", &ecs::sanimation::m_current_keyframe_index)
		.prop("m_cursor", &ecs::sanimation::m_cursor)
		.prop("m_duration", &ecs::sanimation::m_duration)
		.prop("m_frame_countx", &ecs::sanimation::m_frame_countx)
		.prop("m_frame_county", &ecs::sanimation::m_frame_county)
		.prop("m_frame_height", &ecs::sanimation::m_frame_height)
		.prop("m_frame_width", &ecs::sanimation::m_frame_width)
		.prop("m_keyframes", &ecs::sanimation::m_keyframes)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ecs::sanimation::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &ecs::sanimation::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &ecs::sanimation::remove)
		;

	rttr::ccomponent<ecs::smaterial>("smaterial")
		.prop("m_renderstate", &ecs::smaterial::m_renderstate)
		.prop("m_texture", &ecs::smaterial::m_texture)
		.prop("m_program", &ecs::smaterial::m_program)
		.prop("m_flags", &ecs::smaterial::m_flags)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ecs::smaterial::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &ecs::smaterial::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &ecs::smaterial::remove)
		;

	rttr::ccomponent<ecs::ssprite_renderer>("ssprite_renderer")
		.prop("m_source_rect", &ecs::ssprite_renderer::m_source_rect)
		.prop("m_origin", &ecs::ssprite_renderer::m_origin)
		.prop("m_tint", &ecs::ssprite_renderer::m_tint)
		.prop("m_layer", &ecs::ssprite_renderer::m_layer)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ecs::ssprite_renderer::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &ecs::ssprite_renderer::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &ecs::ssprite_renderer::remove)
		;

	rttr::ccomponent<ecs::stransform>("stransform")
		.prop("m_position", &ecs::stransform::m_position)
		.prop("m_scale", &ecs::stransform::m_scale)
		.prop("m_shear", &ecs::stransform::m_shear)
		.prop("m_rotation", &ecs::stransform::m_rotation)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ecs::stransform::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &ecs::stransform::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &ecs::stransform::remove)
		;

	rttr::ccomponent<ecs::sidentifier>("sidentifier")
		.prop("m_uuid", &ecs::sidentifier::m_uuid)
		.prop("m_name", &ecs::sidentifier::m_name)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ecs::sidentifier::show_ui)
		.meth(ecs::detail::C_COMPONENT_ADD_FUNC_NAME, &ecs::sidentifier::add)
		.meth(ecs::detail::C_COMPONENT_REMOVE_FUNC_NAME, &ecs::sidentifier::remove)
		;

	rttr::registration::class_<ecs::icomponent>("icomponent")
		.method("name", &ecs::icomponent::name)
		;

	rttr::cregistrator<ecs::modules::sconfig, rttr::detail::as_object>("sconfig")
		.prop("m_name", &ecs::modules::sconfig::m_name)
		.prop("m_components", &ecs::modules::sconfig::m_components)
		.prop("m_systems", &ecs::modules::sconfig::m_systems)
		.prop("m_modules", &ecs::modules::sconfig::m_modules)
		;

	//- Section: Modules and Systems
	//- Render
	//------------------------------------------------------------------------------------------------------------------------
	using namespace render;
	REGISTER_MODULE(srender_module);
	REGISTER_SYSTEM(srender_system);
	REGISTER_SYSTEM(sdebug_render_system);
	REGISTER_SYSTEM(srender_frame_begin_system);
	REGISTER_SYSTEM(srender_frame_end_system);

	//- Animation
	//------------------------------------------------------------------------------------------------------------------------
	using namespace animation;
	REGISTER_MODULE(sanimation_module);
	REGISTER_SYSTEM(sanimation_system);

	//- Camera
	//------------------------------------------------------------------------------------------------------------------------
	using namespace camera;
	REGISTER_MODULE(scamera_module);
	REGISTER_SYSTEM(scamera_sync_system);
	REGISTER_SYSTEM(scamera_editor_controller_system);
}