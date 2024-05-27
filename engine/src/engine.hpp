#pragma once
#include <core.h>
#include <spritemancer.h>
#include "physics/b2_physics.hpp"
#include "ecs/ecs.hpp"
#include "editor/editor_tool.hpp"
#include "editor/editor_visualizer.hpp"
#include "layer.hpp"
#include "services/thread_service.hpp"
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
		engine_run_result_failed_pushing_layers,
		engine_run_result_fail = 255,
	};

	//- Central entry point of the application (not counting main). Configures the application to be executed,
	//- configures self and registers services etc. Does create a window and a rendering context and provides the main loop.
	//- Constructible from cengine::instance() only to ensure correct functioning.
	//------------------------------------------------------------------------------------------------------------------------
	class cengine final : public sm::iapp, core::cnon_copyable
	{
	public:
		struct sconfig
		{
			vector_t<string_t> m_services_cfg;
			vector_t<string_t> m_layers_cfg;

			RTTR_ENABLE();
		};

	public:
		template<class TService>
		static auto service();

		STATIC_INSTANCE(cengine, s_cengine);
		~cengine();

		bool on_init(void* config, argparse::ArgumentParser& args) override final;
		void on_update(float dt) override final;
		void on_shutdown() override final;

	private:
		cengine() = default;

	private:
		class clayers
		{
		public:
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

		clayers m_layers;
		sconfig m_config;
		engine_run_result m_result;

	private:
		bool try_push_layer(stringview_t name);
	};

	//- Shortcuts for common operations
	//------------------------------------------------------------------------------------------------------------------------
	template<class TService>
	auto engine::cengine::service()
	{
		return core::cservice_manager::find<TService>();
	}

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cengine::sconfig)
	{
		rttr::registration::class_<cengine::sconfig>("cengine::sconfig")
			.property("m_services_cfg", &cengine::sconfig::m_services_cfg)
			.property("m_layers_cfg", &cengine::sconfig::m_layers_cfg)
			;

		rttr::default_constructor<cengine::sconfig>();
		rttr::default_constructor<vector_t<string_t>>();
	}

} //- engine
