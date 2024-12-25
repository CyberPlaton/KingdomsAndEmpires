#include "sm_rendertarget.hpp"

namespace sm
{
    namespace
    {
        //------------------------------------------------------------------------------------------------------------------------
        bgfx::BackbufferRatio::Enum to_bgfx_ratio(framebuffer_ratio ratio)
        {
            switch(ratio)
            {
            default:
            case framebuffer_ratio_none: return bgfx::BackbufferRatio::Enum::Count;
            case framebuffer_ratio_equal: return bgfx::BackbufferRatio::Enum::Equal;
            case framebuffer_ratio_quarter: return bgfx::BackbufferRatio::Enum::Quarter;
            case framebuffer_ratio_eighth: return bgfx::BackbufferRatio::Enum::Eighth;
            case framebuffer_ratio_sixteenth: return bgfx::BackbufferRatio::Enum::Sixteenth;
            case framebuffer_ratio_double: return bgfx::BackbufferRatio::Enum::Double;
            }
        }
    
    } //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget::destroy(crendertarget& target)
	{
		if (is_valid(target))
		{
			bgfx::destroy(bgfx::FrameBufferHandle{ target.target() });
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget(unsigned w, unsigned h)
	{
		create(w, h);
	}

    //------------------------------------------------------------------------------------------------------------------------
    crendertarget::crendertarget(framebuffer_ratio ratio)
    {
        create(ratio);
    }

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget() :
		m_texture(MAX(uint16_t))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::~crendertarget()
	{
	}

    //------------------------------------------------------------------------------------------------------------------------
    opresult crendertarget::create(framebuffer_ratio ratio)
    {
        if (m_framebuffer = bgfx::createFrameBuffer(to_bgfx_ratio(ratio),
            texture_format::RGBA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP).idx; !bgfx::isValid(bgfx::FrameBufferHandle{ m_framebuffer }))
        {
            if (serror_reporter::instance().m_callback)
            {
                serror_reporter::instance().m_callback(core::logging_verbosity_error,
                    "Failed loading render target");
            }
            return opresult_fail;
        }

        m_texture = bgfx::getTexture(bgfx::FrameBufferHandle{ m_framebuffer }).idx;
        m_width = MAX(uint16_t);
        m_height = MAX(uint16_t);

        return opresult_ok;
    }

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crendertarget::create(unsigned w, unsigned h)
	{
		if (m_framebuffer = bgfx::createFrameBuffer(SCAST(uint16_t, w), SCAST(uint16_t, h),
			texture_format::RGBA8, BGFX_SAMPLER_U_CLAMP | BGFX_SAMPLER_V_CLAMP).idx; !bgfx::isValid(bgfx::FrameBufferHandle{ m_framebuffer }))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading render target");
			}
			return opresult_fail;
		}

		m_texture = bgfx::getTexture(bgfx::FrameBufferHandle{ m_framebuffer }).idx;
		m_width = SCAST(uint16_t, w);
		m_height = SCAST(uint16_t, h);

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crendertarget::resize(unsigned w, unsigned h)
	{
		crendertarget::destroy(*this);

		return create(w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget_manager::crendertarget_manager(unsigned reserve /*= C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT*/)
	{
		m_data.reserve(reserve);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget_manager::~crendertarget_manager()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool crendertarget_manager::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget_manager::on_shutdown()
	{
		clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget_manager::on_update(float)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::rendertarget_handle_t crendertarget_manager::load_sync(stringview_t name, unsigned w, unsigned h)
	{
		return load_of_sync<rendertarget_handle_t>(name, m_data, w, h);
	}

    //------------------------------------------------------------------------------------------------------------------------
    sm::rendertarget_handle_t crendertarget_manager::load_sync(stringview_t name, framebuffer_ratio ratio)
    {
        return load_of_sync<rendertarget_handle_t>(name, m_data, ratio);
    }

	//------------------------------------------------------------------------------------------------------------------------
	core::cfuture_type<sm::rendertarget_handle_t> crendertarget_manager::load_async(stringview_t name, unsigned w, unsigned h)
	{
		return load_of_async<rendertarget_handle_t>(name, m_data, w, h);
	}

    //------------------------------------------------------------------------------------------------------------------------
    core::cfuture_type<sm::rendertarget_handle_t> crendertarget_manager::load_async(stringview_t name, framebuffer_ratio ratio)
    {
        return load_of_async<rendertarget_handle_t>(name, m_data, ratio);
    }


} //- sm

RTTR_REGISTRATION
{
	using namespace sm;

	//------------------------------------------------------------------------------------------------------------------------
	rttr::registration::class_<crendertarget_manager>("crendertarget_manager")
		.constructor<>()
		(
			rttr::policy::ctor::as_raw_ptr
		)
		;

}
