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

		void prepare_frame() override final;
		void display_frame() override final;
		void update_viewport(const vec2_t& position, const vec2_t& size) override final;
		void blendmode(sblending mode) override final;

		void clear(const slayer& layer, bool depth) override final;
		void begin(const slayer& layer) override final;
		void draw(const slayer& layer) override final;
		void end(const slayer& layer) override final;
		void combine(const slayer& layer) override final;

		void update_texture_gpu(uint64_t id, unsigned w, unsigned h, void* data) override final;
		void update_texture_cpu(uint64_t id) override final;

		RTTR_ENABLE(irenderer);
	};

} //- sm

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(crenderer_raylib)
	{
		rttr::cregistrator<crenderer_raylib>("crenderer_raylib")
			;
	};

} //- sm