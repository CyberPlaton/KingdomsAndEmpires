#include "sm_rendertarget.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	void crendertarget::destroy(crendertarget& target)
	{
		if (is_valid(target))
		{
			raylib::UnloadRenderTexture(target.target());
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget(unsigned w, unsigned h)
	{
		create(w, h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::crendertarget() :
		m_texture({ 0 })
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	crendertarget::~crendertarget()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crendertarget::create(unsigned w, unsigned h)
	{
		if (m_texture = raylib::LoadRenderTexture(w, h); !raylib::IsRenderTextureReady(m_texture))
		{
			if (serror_reporter::instance().m_callback)
			{
				serror_reporter::instance().m_callback(core::logging_verbosity_error,
					"Failed loading render target");
			}

			return opresult_fail;
		}

		return opresult_ok;
	}

	//------------------------------------------------------------------------------------------------------------------------
	sm::opresult crendertarget::resize(unsigned w, unsigned h)
	{
		crendertarget::destroy(*this);

		return create(w, h);
	}

} //- sm