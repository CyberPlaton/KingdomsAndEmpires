#pragma once
#include <core.h>
#include <flecs.h>
#include <spritemancer.h>
#include "physics/b2_physics.hpp"
#include "ecs/ecs.hpp"
#include "editor/editor_tool.hpp"
#include "editor/editor_visualizer.hpp"
#include "editor/editor_profiler_visualizer.hpp"
#include "editor/elements/editor_element_dialog.hpp"
#include "editor/elements/editor_element_tab_bar.hpp"
#include "editor/elements/editor_element_table.hpp"
#include "editor/elements/editor_element_text_filter.hpp"
#include "editor/elements/editor_element_text_input.hpp"
#include "editor/elements/editor_element_window.hpp"
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
#include "animation/animation_module.hpp"
#include "render/render_module.hpp"
#include "main.hpp"

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
		template<typename TService>
		static auto service();

		template<typename TResource>
		static auto manager();

		static void configure_args(const args_ref_t& args);
		static void prepare(sconfig& cfg);

		STATIC_INSTANCE_EX(cengine);
		~cengine();

		bool on_init(void* config) override final;
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

	//- Find given service, returns nullptr if non-existent
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TService>
	auto engine::cengine::service()
	{
		return core::cservice_manager::find<TService>();
	}

	//- Find resource manager for given resource, returns nullptr if non-existent
	//------------------------------------------------------------------------------------------------------------------------
	template<typename TResource>
	auto engine::cengine::manager()
	{
		if (const auto* rs = core::cservice_manager::find<cresource_service>(); rs)
		{
			return rs->manager<TResource>();
		}
		return nullptr;
	}

} //- engine
