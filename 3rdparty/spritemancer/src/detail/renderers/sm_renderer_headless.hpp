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
		opresult init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync) override final { return opresult_ok; }
		opresult shutdown_device() override final { return opresult_ok; }
		void reset(unsigned w, unsigned h, bool fullscreen, bool vsync) override final {}
		void begin() override final {}
		void end() override final {}

		void renderpass_begin(const renderpass_ref_t&) override final {}
		void renderpass_end(const renderpass_ref_t&) override final {}
		void renderpass_reset(const renderpasses_t&) override final {}

		RTTR_ENABLE(irenderer);
	};

} //- sm