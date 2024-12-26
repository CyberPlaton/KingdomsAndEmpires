#include "sm_config.hpp"
#include "sm_image.hpp"
#include "sm_program.hpp"
#include "sm_rendertarget.hpp"
#include "sm_shader.hpp"
#include "sm_spriteatlas.hpp"
#include "sm_texture.hpp"
#include "sm_material.hpp"
#include "sm_uniform.hpp"
#include "sm_context.hpp"

namespace sm
{
	namespace entry
	{
		static ptr_t<ios> S_OS				= nullptr;
		static ptr_t<iplatform> S_PLATFORM	= nullptr;
		static iapp* S_APP					= nullptr;
		static ptr_t<irenderer> S_RENDERER	= nullptr;
        static ptr_t<ccontext> S_CTX        = nullptr;

        //------------------------------------------------------------------------------------------------------------------------
        bool create_context(ptr_t<ccontext>&& ctx /*= nullptr*/)
        {
            if(ctx)
            {
                S_CTX = std::move(ctx);
            }
            else
            {
                S_CTX = std::make_unique<ccontext>();
            }
        }
    
        //------------------------------------------------------------------------------------------------------------------------
        void destroy_context()
        {
            S_CTX.reset();
        }
    
        //------------------------------------------------------------------------------------------------------------------------
        ccontext* ctx()
        {
            return S_CTX.get();
        }
    
		//------------------------------------------------------------------------------------------------------------------------
		sm::irenderer* renderer()
		{
			return S_RENDERER.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iplatform* platform()
		{
			return S_PLATFORM.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool has_platform()
		{
			return platform() != nullptr;
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::ios* os()
		{
			return S_OS.get();
		}

		//------------------------------------------------------------------------------------------------------------------------
		sm::iapp* app()
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
	bool is_valid(const cshader& shader)
	{
		return bgfx::isValid(bgfx::ShaderHandle{ shader.shader() });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cimage& image)
	{
		return image.image() != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ctexture& texture)
	{
		return bgfx::isValid(bgfx::TextureHandle{ texture.texture() });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const crendertarget& target)
	{
		return bgfx::isValid(bgfx::FrameBufferHandle{ target.target() });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ccamera& camera)
	{
		return camera.m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cmaterial& material)
	{
		return bgfx::isValid(bgfx::TextureHandle{ material.texture() }) &&
			bgfx::isValid(bgfx::ProgramHandle{ material.program() });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cspriteatlas& atlas)
	{
		const auto size = atlas.dimension();

		return atlas.subtextures() > 0 && size.x > 0.0f && size.y > 0.0f;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cprogram& program)
	{
		return bgfx::isValid(bgfx::ProgramHandle{ program.handle() });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cuniform& uniform)
	{
		return bgfx::isValid(bgfx::UniformHandle{ uniform.uniform() });
	}

	//------------------------------------------------------------------------------------------------------------------------
	ccamera::ccamera() :
		m_position({ 0.0f, 0.0f }), m_offset({ 0.0f, 0.0f }), m_zoom(0.0f), m_rotation(0.0f), m_ready(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sblending::operator!=(const sblending& other)
	{
		return !this->operator==(other);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sblending::operator==(const sblending& other)
	{
		return m_mode == other.m_mode;
	}

} //- sm
