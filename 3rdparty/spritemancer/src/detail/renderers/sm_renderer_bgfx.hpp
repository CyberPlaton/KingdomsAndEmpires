#pragma once
#include "../sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class crenderer_bgfx final : public irenderer
	{
	public:
		crenderer_bgfx() = default;
		~crenderer_bgfx() = default;

		void		prepare_device() override final;
		opresult	init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync) override final;
		opresult	shutdown_device() override final;
		void		reset(unsigned w, unsigned h, bool fullscreen, bool vsync) override final;
		void		begin() override final;
		void		end() override final;

		void renderpass_begin(const renderpass_ref_t& pass) override final;
		void renderpass_end(const renderpass_ref_t& pass) override final;
		void renderpass_reset(const renderpasses_t& passes) override final;

		RTTR_ENABLE(irenderer);
	};

} //- sm
