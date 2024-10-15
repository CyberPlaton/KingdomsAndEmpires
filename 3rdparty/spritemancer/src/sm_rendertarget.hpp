#pragma once
#include "detail/sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget final : public core::cresource
	{
	public:
		static void destroy(crendertarget& target);

		explicit crendertarget(unsigned w, unsigned h);
		crendertarget();
		~crendertarget();

		opresult create(unsigned w, unsigned h);
		opresult resize(unsigned w, unsigned h);

		inline unsigned w() const { return m_texture.texture.width; }
		inline unsigned h() const { return m_texture.texture.height; }
		inline raylib::RenderTexture2D target() const { return m_texture; }
		inline raylib::Texture2D texture() const { return m_texture.texture; }

	private:
		raylib::RenderTexture2D m_texture;

		RTTR_ENABLE(core::cresource);
	};

} //- sm