#pragma once
#include "../sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class crenderer_headless final : public irenderer
	{
	public:
		crenderer_headless() = default;
		~crenderer_headless() = default;

		void prepare_device() override final {}
		opresult init_device(void* nwh, unsigned w, unsigned h,
			bool fullscreen, bool vsync) override final { return opresult_ok; }
		opresult shutdown_device() override final { return opresult_ok; }

		void update_viewport(const vec2_t& position, const vec2_t& size) override final {}
		void blendmode(sblending mode) override final {}
		void update_frame_camera(const ccamera&) override final {}
		void state_reset_to_default() override final {}

		void begin() override final {}
		void clear() override final {}
		void end() override final {}

		void layer_draw(const srendering_layer& layer) override final {}

		bool imgui_begin() override final { return false; }
		void imgui_end() override final {}

		void update_texture_gpu(uint64_t id, unsigned w, unsigned h, texture_format format, const void* data) override final {}
		void update_texture_cpu(uint64_t id, unsigned w, unsigned h, texture_format format, void*& data) override final {}

		RTTR_ENABLE(irenderer);
	};

} //- sm