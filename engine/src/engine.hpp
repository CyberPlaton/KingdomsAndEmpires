#pragma once
#include <core.h>
#include <spritemancer.h>
#include "physics/b2_physics.hpp"
#include "ecs/ecs.hpp"
#include "editor/editor_tool.hpp"
#include "editor/editor_visualizer.hpp"
#include "layer.hpp"
#include "services/profiler_service.hpp"
#include "services/thread_service.hpp"
#include "services/asset_service.hpp"
#include "services/resource_service.hpp"
#include "services/module_service.hpp"
#include "services/plugin_service.hpp"
#include "services/project_service.hpp"
#include "io/io_memory_filesystem.hpp"
#include "io/io_native_filesystem.hpp"
#include "render/render_scene_system.hpp"
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
		engine_run_result_failed_registering_services,
		engine_run_result_failed_registering_resource_managers,
		engine_run_result_failed_loading_plugins,
		engine_run_result_failed_pushing_layers,
		engine_run_result_failed_loading_startup_project,
		engine_run_result_fail = 255,
	};

	//- Central entry point of the application (not counting main). Configures the application to be executed,
	//- configures self and registers services etc. Does create a window and a rendering context and provides the main loop.
	//- Constructible from cengine::instance() only to ensure correct functioning.
	//------------------------------------------------------------------------------------------------------------------------
	class cengine final :
		public sm::iapp,
		private core::cnon_copyable,
		private core::cnon_movable
	{
	public:
		struct sconfig
		{
			vector_t<string_t> m_services_cfg;
			vector_t<string_t> m_layers_cfg;
			vector_t<string_t> m_plugins_cfg;

			string_t m_startup_project;
			launch_context_t m_mode;

			RTTR_ENABLE();
		};

	public:
		template<typename TService>
		static auto service();

		template<typename TResource>
		static auto manager();

		STATIC_INSTANCE_EX(cengine);
		~cengine();

		bool on_init(void* config, argparse::ArgumentParser& args) override final;
		void on_update(float dt) override final;
		void on_imgui() override final;
		void on_shutdown() override final;

	private:
		sconfig m_config;
		clayers m_layers;
		engine_run_result m_result;

	private:
		cengine() = default;
		bool try_push_layer(stringview_t name);
		void register_plugins();
		void register_services();
		void register_resource_managers();
		void register_layers();
	};

	//- Shortcuts for common operations
	//- Find given service
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TService>
	auto engine::cengine::service()
	{
		return core::cservice_manager::find<TService>();
	}

	//- Find resource manager for given resource
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	auto engine::cengine::manager()
	{
		return core::cservice_manager::find<cresource_service>()->manager<TResource>();
	}

} //- engine
