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
	cengine::~cengine()
	{
		//- FIXME: just for decoy, use some of the services so that they wont get optimized out for not being used...
		cthread_service thread_service_decoy;
		casset_service asset_service_decoy;
		io::cmodule_service module_service_decoy;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cengine::on_init(void* config, argparse::ArgumentParser& args)
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
		CORE_NAMED_ZONE("cengine::on_update");

		core::cservice_manager::on_update(dt);

		ecs::cworld_manager::instance().tick(dt);

		m_layers.on_update(dt);

		m_layers.on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cengine::on_imgui()
	{
		imgui::begin();

		m_layers.on_ui_render();

		imgui::end();
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
		core::cservice_manager::emplace<core::fs::cvirtual_filesystem>()->on_start();

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
	using namespace ecs;
	using namespace ecs::tag;

	rttr::registration::class_<cengine::sconfig>("cengine::sconfig")
		.property("m_startup_project",	&cengine::sconfig::m_startup_project)
		.property("m_mode",				&cengine::sconfig::m_mode)
		.property("m_services_cfg",		&cengine::sconfig::m_services_cfg)
		.property("m_layers_cfg",		&cengine::sconfig::m_layers_cfg)
		.property("m_plugins_cfg",		&cengine::sconfig::m_plugins_cfg)
		;
	
	rttr::default_constructor<cengine::sconfig>();
	rttr::default_constructor<vector_t<string_t>>();

	rttr::cregistrator<sinvisible>("sinvisible")
		;

	rttr::ccomponent<shierarchy>("shierarchy")
		.prop("m_parent", &shierarchy::m_parent)
		.prop("m_children", &shierarchy::m_children)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &shierarchy::show_ui)
		;

	rttr::ccomponent<scamera>("scamera")
		.prop("m_position", &scamera::m_position)
		.prop("m_offset", &scamera::m_offset)
		.prop("m_zoom", &scamera::m_zoom)
		.prop("m_rotation", &scamera::m_rotation)
		.prop("m_viewrect", &scamera::m_viewrect)
		.prop("m_active", &scamera::m_active)
		.prop("m_renderlayer", &scamera::m_renderlayer)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &scamera::show_ui)
		;

	rttr::ccomponent<sanimation>("sanimation")
		.prop("m_animation", &sanimation::m_animation)
		.prop("m_animation_state", &sanimation::m_animation_state)
		.prop("m_animation_state_changed", &sanimation::m_animation_state_changed)
		.prop("m_current_keyframe", &sanimation::m_current_keyframe)
		.prop("m_current_keyframe_index", &sanimation::m_current_keyframe_index)
		.prop("m_cursor", &sanimation::m_cursor)
		.prop("m_duration", &sanimation::m_duration)
		.prop("m_frame_countx", &sanimation::m_frame_countx)
		.prop("m_frame_county", &sanimation::m_frame_county)
		.prop("m_frame_height", &sanimation::m_frame_height)
		.prop("m_frame_width", &sanimation::m_frame_width)
		.prop("m_keyframes", &sanimation::m_keyframes)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &sanimation::show_ui)
		;

	rttr::ccomponent<smesh>("smesh")
		.prop("m_vertices", &smesh::m_vertices)
		.prop("m_indices", &smesh::m_indices)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &smesh::show_ui)
		;

	rttr::ccomponent<smaterial>("smaterial")
		.prop("m_renderstate", &smaterial::m_renderstate)
		.prop("m_texture", &smaterial::m_texture)
		.prop("m_program", &smaterial::m_program)
		.prop("m_flags", &smaterial::m_flags)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &smesh::show_ui)
		;

	rttr::ccomponent<ssprite_renderer>("ssprite_renderer")
		.prop("m_source_rect", &ssprite_renderer::m_source_rect)
		.prop("m_tint", &ssprite_renderer::m_tint)
		.prop("m_layer", &ssprite_renderer::m_layer)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &ssprite_renderer::show_ui)
		;

	rttr::ccomponent<stransform>("stransform")
		.prop("m_matrix", &stransform::m_matrix)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &stransform::show_ui)
		;

	rttr::ccomponent<sidentifier>("sidentifier")
		.prop("m_uuid", &sidentifier::m_uuid)
		.prop("m_name", &sidentifier::m_name)
		.meth(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME, &sidentifier::show_ui)
		;

	rttr::registration::class_<icomponent>("icomponent")
		.method("name", &icomponent::name)
		;
}