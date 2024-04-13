#pragma once
#include <core.h>
#include "physics/b2_physics.hpp"
#include "spritemancer/sm.hpp"
#include "ecs/ecs.hpp"
#include "editor/editor_tool.hpp"
#include "editor/editor_visualizer.hpp"
#include "layer.hpp"
#include "services/event_service.hpp"
#include "services/thread_service.hpp"
#include "services/resource_management_service.hpp"
#include <argparse.h>

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	enum engine_run_result : uint8_t
	{
		engine_run_result_ok = 0,
		engine_run_result_failed_parsing_arguments,
		engine_run_result_failed_loading_config,
		engine_run_result_failed_parsing_invalid_config,
		engine_run_result_failed_starting_spritemancer,
		engine_run_result_failed_registering_services,
		engine_run_result_failed_registering_resource_managers,
		engine_run_result_failed_pushing_layers,
		engine_run_result_fail = 255,
	};

	//- Central entry point of the application (not counting main). Configures the application to be executed,
	//- configures self and registers services etc. Does create a window and a rendering context and provides the main loop.
	//- Constructible from cengine::instance() only to ensure correct functioning.
	//------------------------------------------------------------------------------------------------------------------------
	class cengine final : core::cnon_copyable
	{
	private:
		class clayers
		{
		public:
			struct sconfig
			{
				vector_t<std::string> m_layers;
			};

			void init();
			void shutdown();
			void on_update(float dt);
			void on_world_render();
			void on_ui_render();
			void on_post_update(float dt);

			vector_t<rttr::method> m_layer_update;
			vector_t<rttr::method> m_layer_world_render;
			vector_t<rttr::method> m_layer_ui_render;
			vector_t<rttr::method> m_layer_post_update;
			vector_t<rttr::method> m_layer_shutdown;
			vector_t<rttr::method> m_layer_init;
		};

	public:
		struct sconfig
		{
			sm::cwindow::sconfig m_window_cfg;
			core::cservice_manager::sconfig m_service_cfg;
			clayers::sconfig m_layer_cfg;
			cresource_management_service::sconfig m_resources_cfg;

			RTTR_ENABLE();
		};

	public:
		template<class TService>
		static auto service();

		template<class TResourceManager>
		static auto resource_manager();

		STATIC_INSTANCE(cengine, s_cengine);
		~cengine();

		engine_run_result configure(sconfig cfg, int argc = 0, char* argv[] = {});
		engine_run_result configure(const core::cpath& cfg, int argc = 0, char* argv[] = {});
		engine_run_result run();

	private:
		cengine() = default;

	private:
		sconfig m_config;
		clayers m_layers;
		engine_run_result m_result;

		void handle_arguments(argparse::ArgumentParser& args, int argc, char* argv[]);
		void register_services(argparse::ArgumentParser& args);
		void register_resource_managers(argparse::ArgumentParser& args);
		bool init_layers(const clayers::sconfig& cfg);
	};

	//- Shortcuts for common operations
	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	auto engine::cengine::service()
	{
		return core::cservice_manager::find<TService>();
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TResourceManager>
	auto engine::cengine::resource_manager()
	{
		return service<cresource_management_service>()->find<TResourceManager>();
	}

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cengine::sconfig)
	{
		rttr::registration::class_<cengine::sconfig>("cengine::sconfig")
			.property("m_service_cfg", &cengine::sconfig::m_service_cfg)
			.property("m_window_cfg", &cengine::sconfig::m_window_cfg)
			.property("m_layer_cfg", &cengine::sconfig::m_layer_cfg)
			.property("m_resources_cfg", &cengine::sconfig::m_resources_cfg)
			;

		rttr::default_constructor<cengine::sconfig>();
		rttr::default_constructor<vector_t<std::string>>();
	}

} //- engine