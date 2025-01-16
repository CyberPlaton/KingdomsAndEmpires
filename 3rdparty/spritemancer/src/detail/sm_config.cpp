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

			return S_CTX.get() != nullptr;
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
		return bgfx::isValid(bgfx::ShaderHandle{ SCAST(uint16_t, shader.shader()) });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cimage& image)
	{
		return image.image() != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ctexture& texture)
	{
		return bgfx::isValid(bgfx::TextureHandle{ SCAST(uint16_t, texture.texture()) });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const crendertarget& target)
	{
		return bgfx::isValid(bgfx::FrameBufferHandle{ SCAST(uint16_t, target.target()) });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const ccamera& camera)
	{
		return camera.m_ready;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cmaterial& material)
	{
		return bgfx::isValid(bgfx::TextureHandle{ SCAST(uint16_t, material.texture()) }) &&
			bgfx::isValid(bgfx::ProgramHandle{ SCAST(uint16_t, material.program()) });
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
		return bgfx::isValid(bgfx::ProgramHandle{ SCAST(uint16_t, program.handle()) });
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_valid(const cuniform& uniform)
	{
		return bgfx::isValid(bgfx::UniformHandle{ SCAST(uint16_t, uniform.uniform()) });
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

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		bgfx::BackbufferRatio::Enum to_bgfx_ratio(framebuffer_ratio ratio)
		{
			switch (ratio)
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

		//------------------------------------------------------------------------------------------------------------------------
		bgfx::ViewMode::Enum to_bgfx_view_mode(view_mode mode)
		{
			switch (mode)
			{
			default:
			case view_mode_default: return bgfx::ViewMode::Default;
			case view_mode_sequential: return bgfx::ViewMode::Sequential;
			case view_mode_depth_ascending: return bgfx::ViewMode::DepthAscending;
			case view_mode_depth_descending: return bgfx::ViewMode::DepthDescending;
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		bgfx::UniformType::Enum to_bgfx_uniform_type(uniform_type type)
		{
			switch (type)
			{
			case uniform_type_float:
			case uniform_type_vector2:
			case uniform_type_vector3:
			case uniform_type_vector4:
				return bgfx::UniformType::Enum::Vec4;
			case uniform_type_mat3x3:
				return bgfx::UniformType::Enum::Mat3;
			case uniform_type_mat4x4:
				return bgfx::UniformType::Enum::Mat4;
			case uniform_type_sampler:
				return bgfx::UniformType::Enum::Sampler;
			default:
				return bgfx::UniformType::Enum::Count;
			}
		}

	} //- detail

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::shader_profile()
	{
#if CORE_PLATFORM_WINDOWS || CORE_PLATFORM_XBOXONE || CORE_PLATFORM_XBOXSERIES
		return "s_5_0";
#elif CORE_PLATFORM_LINUX || CORE_PLATFORM_ANDROID || CORE_PLATFORM_NX
		return "spirv";
#elif CORE_PLATFORM_IOS || CORE_PLATFORM_OSX
		return "metal";
#elif CORE_PLATFORM_PS4 || CORE_PLATFORM_PS5
		return "pssl";
#endif
		return {};
	}

	//------------------------------------------------------------------------------------------------------------------------
	string_t sinfo::shader_varying_default()
	{
		constexpr stringview_t C_VARYING =
			"vec4 v_color0    : COLOR0    = vec4(1.0, 0.0, 0.0, 1.0);\n"
			"vec2 v_texcoord0 : TEXCOORD0 = vec2(0.0, 0.0);\n"
			"vec3 a_position  : POSITION;\n"
			"vec4 a_color0    : COLOR0;\n"
			"vec2 a_texcoord0 : TEXCOORD0;\n"
			"\n\0";

		return C_VARYING.data();
	}

} //- sm
