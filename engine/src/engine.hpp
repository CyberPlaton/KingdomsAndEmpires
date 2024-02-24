#pragma once
#include <core.h>
#include "physics/b2_physics.hpp"
#include "spritemancer/sm.hpp"
#include "ecs/ecs.hpp"
#include "services/service_manager.hpp"
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
		engine_run_result_fail = 255,
	};

	//- Central entry point of the application (not counting main). Configures the application to be executed,
	//- configures self and registers services etc. Does create a window and a rendering context and provides the main loop.
	//- Constructible from cengine::instance() only to ensure correct functioning.
	//------------------------------------------------------------------------------------------------------------------------
	class cengine final : core::cnon_copyable
	{
	public:
		struct sconfig
		{
			cservice_manager::sconfig m_service_cfg;
			sm::cwindow::sconfig m_window_cfg;

			RTTR_ENABLE();
		};

	public:
		STATIC_INSTANCE(cengine, s_cengine);
		~cengine();

		engine_run_result configure(sconfig cfg, int argc = 0, char* argv[] = {});
		engine_run_result configure(const core::cpath& cfg, int argc = 0, char* argv[] = {});
		engine_run_result run();

		bool push_layer(const std::string& name);

	private:
		cengine() = default;

		class clayers
		{
		public:
			using layer_update_func_t = std::function<void(float)>;
			using layer_render_func_t = std::function<void()>;

			static constexpr std::string_view C_LAYER_UPDATE_FUNC_NAME = "on_update";
			static constexpr std::string_view C_LAYER_WORLD_RENDER_FUNC_NAME = "on_world_render";
			static constexpr std::string_view C_LAYER_UI_RENDER_FUNC_NAME = "on_ui_render";

			void on_update(float dt);
			void on_world_render();
			void on_ui_render();

			vector_t<rttr::method> m_layer_update;
			vector_t<rttr::method> m_layer_world_render;
			vector_t<rttr::method> m_layer_ui_render;
		};

	private:
		sconfig m_config;
		clayers m_layers;
		engine_run_result m_result;

		void handle_arguments(argparse::ArgumentParser& args, int argc, char* argv[]);
		void register_services(argparse::ArgumentParser& args);
	};

} //- engine

namespace engine
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cengine::sconfig)
	{
		rttr::registration::class_<cengine::sconfig>("cengine::sconfig")
			.property("m_service_cfg", &cengine::sconfig::m_service_cfg)
			.property("m_window_cfg", &cengine::sconfig::m_window_cfg)
			;

		rttr::default_constructor<cengine::sconfig>();
	}

} //- engine