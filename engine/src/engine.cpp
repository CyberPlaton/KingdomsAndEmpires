#include "engine.hpp"

namespace engine
{
	namespace
	{
		constexpr auto C_RESOURCES_PATH = "resources";

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

		//- set virtual path to resources
		auto fs = core::cfilesystem(core::cfilesystem::cwd());
		fs.backwards();
		fs.backwards();
		fs.backwards();
		fs.backwards();

		if (fs.forwards(C_RESOURCES_PATH))
		{
			const char* path = fs.current().view();

			if (auto filesystem = std::make_shared<io::cnative_filesystem>(); filesystem->init(path, "/"))
			{
				service<fs::cvirtual_filesystem>()->add_filesystem("/", filesystem);
			}
			else
			{
				log_error(fmt::format("Failed initializing virtual file system at path '/' for real path '{}'", path));
			}
		}
		else
		{
			log_error(fmt::format("Failed locating default resources path at '{}'",
				fs.current().view()));
		}

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
	using namespace editor;

	rttr::registration::class_<cengine::sconfig>("cengine::sconfig")
		.property("m_startup_project",	&cengine::sconfig::m_startup_project)
		.property("m_mode",				&cengine::sconfig::m_mode)
		.property("m_services_cfg",		&cengine::sconfig::m_services_cfg)
		.property("m_layers_cfg",		&cengine::sconfig::m_layers_cfg)
		.property("m_plugins_cfg",		&cengine::sconfig::m_plugins_cfg)
		;
	
	rttr::detail::default_constructor<cengine::sconfig>();
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

	//- Section: animation and rendering
	//------------------------------------------------------------------------------------------------------------------------
	rttr::cregistrator<render::srender_module>("srender_module")
		.ctor<flecs::world&>()
		;

	rttr::cregistrator<render::srender_system>("srender_system")
		.ctor<flecs::world&>()
		;

	rttr::cregistrator<render::sdebug_render_system>("sdebug_render_system")
		.ctor<flecs::world&>()
		;

	rttr::cregistrator<animation::sanimation_module>("sanimation_module")
		.ctor<flecs::world&>()
		;

	rttr::cregistrator<animation::sanimation_system>("sanimation_system")
		.ctor<flecs::world&>()
		;
}