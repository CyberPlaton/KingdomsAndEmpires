#pragma once
#include <core.h>
#include "raylib_integration/raylib.hpp"
#include "../math.hpp"
#include <plugin_logging.h>

namespace sm
{
	//- forward decl. and constants
	constexpr auto C_MATERIAL_RESERVE_COUNT			= 2048;
	constexpr auto C_TEXTURE_RESERVE_COUNT			= 2048;
	constexpr auto C_TECHNIQUE_RESERVE_COUNT		= 512;
	constexpr auto C_SPRITEATLAS_RESERVE_COUNT		= 512;
	constexpr auto C_RENDERTARGET_RESERVE_COUNT		= 8;
	constexpr material_t C_DEFAULT_MATERIAL			= 0;
	inline static const core::scolor C_COLOR_WHITE	= { 255, 255, 255, 255 };
	inline static const core::srect C_DEFAULT_RECT	= { 0.0f, 0.0f, 1.0f, 1.0f };
	inline static const vec2_t C_DEFAULT_SCALE		= {1.0f, 1.0f};
	constexpr mat4_t C_MATRIX_4x4_ID				= mat4_t(1.0f);
	constexpr stringview_t C_FXAA_TECHNIQUE_NAME	= "fxaa";
	constexpr stringview_t C_SPRITE_TECHNIQUE_NAME	= "sprite";
	constexpr stringview_t C_DEFAULT_MATERIAL_NAME	= "default";
	constexpr stringview_t C_FXAA_TECHNIQUE_RESOLUTION_PROP = "resolution";

	raylib::Color to_cliteral(const core::scolor& c);
	
	//- BC_* formats: https://learn.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression#bc3
	//------------------------------------------------------------------------------------------------------------------------
	enum texture_format : uint8_t
	{
		texture_format_none		= 0,
		//- uncompressed
		texture_format_8bpp		= raylib::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
		texture_format_8x2bpp	= raylib::PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,
		texture_format_16bpp	= raylib::PIXELFORMAT_UNCOMPRESSED_R5G6B5,
		texture_format_16x4bpp	= raylib::PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,
		texture_format_24bpp	= raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8,
		texture_format_32bpp	= raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
		texture_format_32x4bpp	= raylib::PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,
		//- compressed
		texture_format_bc1		= raylib::PIXELFORMAT_COMPRESSED_DXT1_RGBA,
		texture_format_bc2		= raylib::PIXELFORMAT_COMPRESSED_DXT3_RGBA,
		texture_format_bc3		= raylib::PIXELFORMAT_COMPRESSED_DXT5_RGBA,
		texture_format_astc_4x4	= raylib::PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,
		texture_format_astc_8x8	= raylib::PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA,
		texture_format_qoi,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_factor
	{
		blending_factor_zero						= 0,
		blending_factor_one							= 1,
		blending_factor_src_color					= RL_SRC_COLOR,
		blending_factor_one_minus_src_color			= RL_ONE_MINUS_SRC_COLOR,
		blending_factor_src_alpha					= RL_SRC_ALPHA,
		blending_factor_one_minus_src_alpha			= RL_ONE_MINUS_SRC_ALPHA,
		blending_factor_dst_alpha					= RL_DST_ALPHA,
		blending_factor_dst_color					= RL_DST_COLOR,
		blending_factor_one_minus_dst_alpha			= RL_ONE_MINUS_DST_ALPHA,
		blending_factor_src_alpha_saturate			= RL_SRC_ALPHA_SATURATE,
		blending_factor_constant_color				= RL_CONSTANT_COLOR,
		blending_factor_one_minus_constant_color	= RL_ONE_MINUS_CONSTANT_COLOR,
		blending_factor_constant_alpha				= RL_CONSTANT_ALPHA,
		blending_factor_one_minus_constant_alpha	= RL_ONE_MINUS_CONSTANT_ALPHA,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_equation
	{
		blending_equation_add					= RL_FUNC_ADD,
		blending_equation_min					= RL_MIN,
		blending_equation_max					= RL_MAX,
		blending_equation_subtract				= RL_FUNC_SUBTRACT,
		blending_equation_reverse_subtract		= RL_FUNC_REVERSE_SUBTRACT,
		blending_equation_blend_equation_rgb	= RL_BLEND_EQUATION_RGB,
		blending_equation_blend_equation_alpha	= RL_BLEND_EQUATION_ALPHA,
		blending_equation_blend_dst_rgb			= RL_BLEND_DST_RGB,
		blending_equation_blend_src_rgb			= RL_BLEND_SRC_RGB,
		blending_equation_blend_dst_alpha		= RL_BLEND_DST_ALPHA,
		blending_equation_blend_src_alpha		= RL_BLEND_SRC_ALPHA,
		blending_equation_blend_color			= RL_BLEND_COLOR,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_mode : uint8_t
	{
		blending_mode_alpha				= raylib::BLEND_ALPHA,
		blending_mode_additive			= raylib::BLEND_ADDITIVE,
		blending_mode_multiplied		= raylib::BLEND_MULTIPLIED,
		blending_mode_add_colors		= raylib::BLEND_ADD_COLORS,
		blending_mode_subtract_colors	= raylib::BLEND_SUBTRACT_COLORS,
		blending_mode_alpha_premultiply	= raylib::BLEND_ALPHA_PREMULTIPLY
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum window_flag : uint32_t
	{
		window_flag_none			= BIT(0),
		window_flag_fullscreen		= BIT(1),
		window_flag_msaa			= BIT(2),
		window_flag_vsync			= BIT(3),
		window_flag_resizable		= BIT(4),
		window_flag_decorated		= BIT(5),
		window_flag_show			= BIT(6),
		window_flag_focus			= BIT(7),
		window_flag_minimized		= BIT(8),
		window_flag_run_minimized	= BIT(9),
		window_flag_has_icon		= BIT(10),
		window_flag_fxaa			= BIT(11),
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum window_resolution : uint8_t
	{
		window_resolution_custom = 0,
		window_resolution_switch,
		window_resolution_steamdeck,
		window_resolution_sxga,
		window_resolution_hd,
		window_resolution_hd_plus,
		window_resolution_fhd,
		window_resolution_wuxga,
		window_resolution_qhd,
		window_resolution_wqhd,
		window_resolution_uhd,
	};

	//- Font used to render text on screen.
	//------------------------------------------------------------------------------------------------------------------------
	class cfont
	{
	public:

		inline raylib::Font& font() { return m_font; }
		inline const raylib::Font& font() const { return m_font; }

	private:
		raylib::Font m_font;
	};

	//- CPU representation of image data. Can be loaded without an active GPU context and also in a multithreaded way.
	//------------------------------------------------------------------------------------------------------------------------
	class cimage
	{
	public:
		cimage();
		~cimage();

		inline raylib::Image& image() {return m_image;}
		inline const raylib::Image& image() const { return m_image; }

	private:
		raylib::Image m_image;
	};

	//- GPU representation of image data. Must be loaded with an active GPU context on the mainthread.
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture
	{
	public:
		ctexture(const cimage& image);
		ctexture(const raylib::Texture& texture);
		ctexture();
		~ctexture();

		unsigned width() const;
		unsigned height() const;

		inline bool ready() const { return raylib::IsTextureReady(texture()); }
		inline raylib::Texture& texture() { return m_texture; }
		inline const raylib::Texture& texture() const { return m_texture; }

	private:
		raylib::Texture m_texture;
	};

	//- Thin wrapper around raylib::Shader.
	//------------------------------------------------------------------------------------------------------------------------
	class ctechnique final : public core::cnon_copyable
	{
	public:
		ctechnique(const raylib::Shader& shader);
		ctechnique();
		~ctechnique();

		void bind() const;
		void unbind() const;

		int uniform_location(stringview_t name) const;

		inline bool ready() const { return raylib::IsShaderReady(shader()); }
		inline raylib::Shader& shader() { return m_shader; }
		inline const raylib::Shader& shader() const { return m_shader; }

	private:
		raylib::Shader m_shader;
	};

	//- Framebuffer that can be rendered upon.
	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget
	{
	public:
		crendertarget(const raylib::RenderTexture& texture);
		~crendertarget();

		void resize(unsigned w, unsigned h);

		void bind() const;
		void unbind() const;

		inline bool ready() const { return raylib::IsRenderTextureReady(target()); }
		inline raylib::RenderTexture& target() { return m_image; }
		inline const raylib::RenderTexture& target() const { return m_image; }

	private:
		raylib::RenderTexture m_image;
	};

	//- Describes shader settings and uniforms along with blend mode.
	//- TODO: reconsider concept of uniforms, whether we should have dynamic and static uniforms,
	//- or maybe set them in immediate mode and don�t store them. Need to experiment with what works and what not.
	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial final : public core::cnon_copyable
	{
	public:
		cmaterial(ctechnique& shader, technique_t technique, blending_mode mode = blending_mode_alpha,
			blending_equation equation = blending_equation_blend_color, blending_factor src = blending_factor_src_color,
			blending_factor dst = blending_factor_one_minus_src_alpha);
		~cmaterial() = default;

		void bind() const;
		void unbind() const;

		void set_uniform_float(stringview_t name, float value);
		void set_uniform_int(stringview_t name, int value);
		void set_uniform_vec2(stringview_t name, const vec2_t& value);
		void set_uniform_vec3(stringview_t name, const vec3_t& value);
		void set_uniform_vec4(stringview_t name, const vec4_t& value);
		void set_uniform_matrix(stringview_t name, const mat4_t& value);
		void set_uniform_texture(stringview_t name, const ctexture& value);
		void set_uniform_texture(stringview_t name, const raylib::Texture& value);
		void remove_uniform(stringview_t name);

		inline const ctechnique& shader() const { return m_shader; }
		inline technique_t id() const { return m_id; }

	private:
		//umap_t<string_t, core::cany> m_uniforms; //- Note: trying immediate mode for now
		ctechnique& m_shader;
		technique_t m_id = invalid_handle_t;
		blending_mode m_blending_mode			= blending_mode_alpha;
		blending_factor m_blending_src			= blending_factor_src_color;
		blending_factor m_blending_dst			= blending_factor_one_minus_src_alpha;
		blending_equation m_blending_equation	= blending_equation_blend_color;
	};

	//- A sprite sheet containing n-many different textures combined into one to save GPU memory.
	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas
	{
	public:
		cspriteatlas(texture_t texture);
		cspriteatlas() = default;
		~cspriteatlas() = default;

		const core::srect& get(subtexture_t texture) const;

		void reset();
		texture_t atlas() const;
		unsigned subtexture_count() const;

		cspriteatlas& begin(texture_t texture);
		//- implicitly create a subtexture, where the order of creation is the same as the handle
		cspriteatlas& subtexture(const core::srect& rect);
		cspriteatlas& end();

	private:
		vector_t<core::srect> m_subtextures;
		texture_t m_texture;
	};


	//------------------------------------------------------------------------------------------------------------------------
	class cwindow
	{
	public:
		struct sconfig
		{
			std::string m_title				= {};
			std::string m_window_icon_path	= {};
			window_resolution m_resolution	= window_resolution_custom;
			unsigned m_width				= 0;
			unsigned m_height				= 0;
			unsigned m_target_fps			= 0;
			int m_flags						= 0;

			RTTR_ENABLE();
		};

		static ivec2_t dimension_for_resolution(window_resolution resolution);
		static ivec2_t monitor_dimension();
		static unsigned monitor_refresh_rate();

		cwindow(sconfig& cfg);
		~cwindow();

		void enter_fullscreen();
		void exit_fullscreen();
		bool fullscreen() const;
		void show();
		void hide();
		void resize(unsigned w, unsigned h);
		void position(unsigned w, unsigned h);

		unsigned x() const;
		unsigned y() const;
		unsigned w() const;
		unsigned h() const;
	};

	//- TODO: Reworking camera system
	//- Following an entity: camera locks on to an in-game entity
	//- Edge-Snapping: camera can not move farther or below some coordinate
	//- Camera Window: camera locks on to an in-game entity and pushes camera
	//- position as the entity hits the window edge (4-sides)
	//- Smooth position change: lerping from current position to desired one,
	//- with different easing options
	//- Projected Focus: depending on controller input and velocity pan in some direction
	//- Target Focus: depending on looking direction/mouse position pan in that direction
	//- Camera Path: camera follows a predefined path throughout the level
	//- Zoom-To-Fit: zoom in or zoom out depending on some context, in order
	//- to provide a wide view or close up of something
	//- Attractors/Detractors: attractors pull camera view towards something and
	//- detractors push camera view away from something.
	//- Those attractors/detractors can be attached to different entities.
	//- Region Focus: position of camera is mostly based on a region anchor point
	//- but shifts slightly as the player entity moves
	//- Gesture Focus: camera does something when a specific in-game event happens,
	//- i.e. zoom in when the player does a special attack on an enemy
	//- Cinematic Paths: suspend normal function to show something special
	//- Multi-Focal: camera focuses on several entities and maintains a position
	//- centered between them and a zoom so that all of them are visible at once
	//- Manual Control: camera can be controlled directly by input
	//- Camera Shake (Screen Shake): change position/rotation slightly to create an effect
	//- Average-Oriented Region: position averaging between closest regions
	//- Cues: while in a camera region, some entities can activate/deactivate
	//- their attraction/detraction behavior
	//- 
	//------------------------------------------------------------------------------------------------------------------------

} //- sm

namespace compression
{
// 	sm::texture_format convert_raylib_texture_format(raylib::PixelFormat format);
// 	u64 estimate_texture_gpu_size(u32 w, u32 h, sm::texture_format format);
// 	bool compress_image_to_qoi(const core::cpath& path, const raylib::Image& image);
// 	bool compress_image_to_bc(const core::cpath& compressed_image_path, core::cpath& path,
// 		stringview_t filename, nvtt::Format format, nvtt::Quality quality, std::string* error = nullptr);
// 	bool decompress_qoi_to_image(const core::cpath& path, raylib::Image& image);
// 	bool decompress_qoi_to_image(void* image_data, u32 image_size, raylib::Image& image);

} //- compression

namespace raylib
{
// 	Image LoadQOIImage(const core::cpath& path);
// 	Image LoadDDSImage(const core::cpath& path);
// 	Image LoadDDSImageFromMemory(void* image_data, u32 image_size);
// 	Image LoadQOIImageFromMemory(void* image_data, u32 image_size);

} //- raylib

namespace sm
{
// 	enum window_resolution : uint8_t
// 	{
// 		window_resolution_custom = 0,
// 		window_resolution_switch,
// 		window_resolution_steamdeck,
// 		window_resolution_sxga,
// 		window_resolution_hd,
// 		window_resolution_hd_plus,
// 		window_resolution_fhd,
// 		window_resolution_wuxga,
// 		window_resolution_qhd,
// 		window_resolution_wqhd,
// 		window_resolution_uhd,
// 	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(window_resolution)
	{
		rttr::registration::enumeration<window_resolution>("window_resolution")
			(
				rttr::value("window_resolution_custom", window_resolution_custom),
				rttr::value("window_resolution_switch", window_resolution_switch),
				rttr::value("window_resolution_steamdeck", window_resolution_steamdeck),
				rttr::value("window_resolution_sxga", window_resolution_sxga),
				rttr::value("window_resolution_hd", window_resolution_hd),
				rttr::value("window_resolution_hd_plus", window_resolution_hd_plus),
				rttr::value("window_resolution_fhd", window_resolution_fhd),
				rttr::value("window_resolution_wuxga", window_resolution_wuxga),
				rttr::value("window_resolution_qhd", window_resolution_qhd),
				rttr::value("window_resolution_wqhd", window_resolution_wqhd),
				rttr::value("window_resolution_uhd", window_resolution_uhd)
			);
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(cwindow::sconfig)
	{
		rttr::registration::class_<cwindow::sconfig>("cwindow::sconfig")
			.property("m_title", &cwindow::sconfig::m_title)
			.property("m_window_icon_path", &cwindow::sconfig::m_window_icon_path)
			.property("m_resolution", &cwindow::sconfig::m_resolution)
			.property("m_width", &cwindow::sconfig::m_width)
			.property("m_height", &cwindow::sconfig::m_height)
			.property("m_target_fps", &cwindow::sconfig::m_target_fps)
			.property("m_flags", &cwindow::sconfig::m_flags)
			;

		rttr::default_constructor<cwindow::sconfig>();
	}

} //- sm
