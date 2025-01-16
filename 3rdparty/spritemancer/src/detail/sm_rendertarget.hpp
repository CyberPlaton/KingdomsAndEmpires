#pragma once
#include "sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget final : public core::cresource
	{
	public:
		explicit crendertarget(unsigned w, unsigned h);
        explicit crendertarget(framebuffer_ratio ratio);
		crendertarget();
		~crendertarget();

		opresult create(unsigned w, unsigned h);
        opresult create(framebuffer_ratio ratio);
		opresult resize(unsigned w, unsigned h);

		inline unsigned w() const { return SCAST(unsigned, m_width); }
		inline unsigned h() const { return SCAST(unsigned, m_height); }
		inline rendertarget_handle_t target() const { return m_framebuffer; }
		inline texture_handle_t texture() const { return m_texture; }

		operator bgfx::FrameBufferHandle() const noexcept { return bgfx::FrameBufferHandle{ SCAST(uint16_t, m_framebuffer) }; }

	private:
		rendertarget_handle_t m_framebuffer;
		texture_handle_t m_texture;
		uint16_t m_width;
		uint16_t m_height;

		RTTR_ENABLE(core::cresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget_manager final :
		public core::cservice,
		public core::cresource_manager<crendertarget>
	{
	public:
		crendertarget_manager(unsigned reserve = C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT);
		~crendertarget_manager();

		bool on_start() override final;
		void on_shutdown() override final;
		void on_update(float) override final;

		rendertarget_handle_t load_sync(stringview_t name, unsigned w, unsigned h);
        rendertarget_handle_t load_sync(stringview_t name, framebuffer_ratio ratio);

		core::cfuture_type<rendertarget_handle_t> load_async(stringview_t name, unsigned w, unsigned h);
        core::cfuture_type<rendertarget_handle_t> load_async(stringview_t name, framebuffer_ratio ratio);

	private:
		RTTR_ENABLE(core::cservice, core::cresource_manager<crendertarget>);
	};

} //- sm
