#pragma once
#include "../sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class crenderer_raylib final : public irenderer
	{
	public:
		crenderer_raylib() = default;
		~crenderer_raylib() = default;

		void prepare_device() override final;
		opresult init_device(void* nwh, unsigned w, unsigned h,
			bool fullscreen, bool vsync) override final;
		opresult shutdown_device() override final;

		void update_viewport(const vec2_t& position, const vec2_t& size) override final;
		void blendmode(sblending mode) override final;
		void blendmode_end() override final;
		void update_frame_camera(const ccamera& camera) override final;
		void state_reset_to_default() override final;

		void begin_main_render_texture(const crendertarget& target) override final;
		void end_main_render_texture(const crendertarget& target) override final;
		void clear_main_render_texture(const crendertarget& target, bool depth) override final;
		void draw_main_render_texture(const crendertarget& target) override final;

		void begin_default_backbuffer_drawing() override final;
		void end_default_backbuffer_drawing() override final;

		void layer_draw(const srendering_layer& layer) override final;

		bool imgui_begin() override final;
		void imgui_end() override final;

		void update_texture_gpu(uint64_t id, unsigned w, unsigned h, texture_format format, const void* data) override final;
		void update_texture_cpu(uint64_t id, unsigned w, unsigned h, texture_format format, void*& data) override final;

		RTTR_ENABLE(irenderer);
	};

} //- sm