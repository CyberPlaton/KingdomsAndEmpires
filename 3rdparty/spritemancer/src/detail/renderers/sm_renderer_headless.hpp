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

		void on_event(const rttr::variant& event) override final {}

		void prepare_frame() override final {}
		void display_frame() override final {}
		void update_viewport(const vec2_t& position, const vec2_t& size) override final {}
		void blendmode(blending_mode mode) override final {}

		void clear(const slayer& layer, bool depth) override final {}
		bool begin(const slayer& layer) override final { return false; }
		void draw(const slayer& layer) override final {}
		void end(const slayer& layer) override final {}
		bool combine(const slayer& layer) override final { return false; }

		void update_texture_gpu(uint64_t id, unsigned w, unsigned h, texture_format format, const void* data) override final {}
		void update_texture_cpu(uint64_t id, unsigned w, unsigned h, texture_format format, void*& data) override final {}

		RTTR_ENABLE(irenderer);
	};

} //- sm