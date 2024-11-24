#include "sm_config.hpp"
#include "sm_resource_manager.hpp"
#include "../sm_image.hpp"
#include "../sm_texture.hpp"
#include "../sm_rendertarget.hpp"
#include "../sm_spriteatlas.hpp"
#include "../sm_shader.hpp"

namespace sm
{
	namespace entry
	{
		static ptr_t<ios> S_OS = nullptr;
		static iapp* S_APP = nullptr;
		static ptr_t<irenderer> S_RENDERER = nullptr;
		static ptr_t<iplatform> S_PLATFORM = nullptr;

		//------------------------------------------------------------------------------------------------------------------------
		bool has_platform()
		{
			return S_PLATFORM.get() != nullptr;
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iplatform* get_platform()
		{
			return S_PLATFORM.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::irenderer* get_renderer()
		{
			return S_RENDERER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::ios* get_os()
		{
			return S_OS.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iapp* get_app()
		{
			return S_APP;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_os(ptr_t<ios>&& os)
		{
			S_OS = std::move(os);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_platform(ptr_t<iplatform>&& platform)
		{
			S_PLATFORM = std::move(platform);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_app(iapp* app)
		{
			S_APP = app;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void set_renderer(ptr_t<irenderer>&& renderer)
		{
			S_RENDERER = std::move(renderer);
		}

	} //- entry

	//------------------------------------------------------------------------------------------------------------------------
	raylib::Color to_cliteral(const core::scolor& color)
	{
		return { color.r(), color.g(), color.b(), color.a() };
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cshader& shader)
	{
		return raylib::IsShaderReady(shader.shader());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cimage& image)
	{
		return raylib::IsImageReady(image.image());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ctexture& texture)
	{
		return raylib::IsTextureReady(texture.texture());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const crendertarget& target)
	{
		return raylib::IsRenderTextureReady(target.target());
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ccamera& camera)
	{
		return camera.ready();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cmaterial& material)
	{
		CORE_ASSERT(false, "Invalid operation. Not supported");

		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cspriteatlas& atlas)
	{
		const auto size = atlas.dimension();

		return atlas.subtextures() > 0 && size.x > 0.0f && size.y > 0.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(handle_type_t handle)
	{
		return handle != C_INVALID_HANDLE;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccommand::create(render_callback_t&& callback)
	{
		m_callback = std::move(callback);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ccommand::execute() const
	{
		m_callback();
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccommand::ccommand(render_callback_t&& callback) :
		m_callback(std::move(callback))
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccamera::ccamera() :
		m_position({ 0 }), m_offset({ 0 }), m_zoom(0.0f), m_rotation(0.0f), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	raylib::Camera2D ccamera::camera() const
	{
		return { {m_offset.x, m_offset.y}, {m_position.x, m_position.y}, m_rotation, m_zoom };
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool srenderstate::operator==(const srenderstate& other) const
	{
		return m_blending == other.m_blending && m_flags == other.m_flags;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool srenderstate::operator!=(const srenderstate& other) const
	{
		return !this->operator==(other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sblending::operator!=(const sblending& other) const
	{
		return !this->operator==(other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sblending::operator==(const sblending& other) const
	{
		return m_mode == other.m_mode && m_equation == other.m_equation &&
			m_dst_factor == other.m_dst_factor && m_src_factor == other.m_src_factor;
	}

} //- sm
