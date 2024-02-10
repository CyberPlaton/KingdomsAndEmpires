#pragma once
#include <core.h>
#include <raylib.h>
#include <plugin_logging.h>

namespace sm
{
	//- forward decl. and constants
	class crenderer;
	constexpr auto C_MATERIAL_RESERVE_COUNT	= 2048;
	constexpr auto C_TEXTURE_RESERVE_COUNT	= 2048;
	constexpr auto C_TECHNIQUE_RESERVE_COUNT= 512;
	
	//- @reference: BC_* formats: https://learn.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression#bc3
	//------------------------------------------------------------------------------------------------------------------------
	enum texture_format : uint8_t
	{
		texture_format_none			= 0,
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
	enum window_flag
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
		window_flag_has_icon		= BIT(10)
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

	//------------------------------------------------------------------------------------------------------------------------
	class cwindow
	{
	public:
		struct sconfig
		{
			string_t m_title				= {};
			string_t m_window_icon_path		= {};
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

		cwindow(const sconfig& cfg);
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

	//------------------------------------------------------------------------------------------------------------------------
	class ccamera
	{
	public:
		ccamera() = default;
		virtual ~ccamera() = default;

		core::srect viewport() const;
		void viewport(float x, float y, float w, float h);

		inline float viewport_x() const { return m_viewport.x(); }
		inline float viewport_y() const { return m_viewport.y(); }
		inline float viewport_w() const { return m_viewport.w(); }
		inline float viewport_h() const { return m_viewport.h(); }

		core::scolor clearcolor() const;
		void clearcolor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);

		void move(vec2_t v);
		void move_to(vec2_t v);

		void rotate(float v);
		void rotate_to(float v);

		void zoom(float v);
		void zoom_to(float v);

		vec2_t position() const;
		float rotation() const;
		float zoom() const;

		vec2_t vector_screen_to_world(const vec2_t& point);
		vec2_t vector_world_to_screen(const vec2_t& point);

		//- extendability
		virtual core::srect world_visible_area() const;
		virtual void use();
		virtual void end();
		virtual void update(float dt) {}

		//- utility, debug purposes.
		raylib::Camera2D get_camera() const;

	protected:
		core::srect m_viewport;
		raylib::Camera2D m_camera;
		core::scolor m_clearcolor;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cshader_uniform
	{
	public:
		cshader_uniform() = default;
		cshader_uniform(const string_t& name, void* value, raylib::ShaderUniformDataType type);

		void* get() const;
		template< typename T >
		T get_as();
		raylib::ShaderUniformDataType get_type() const;
		string_t get_name() const;
		const char* c_str() const;

	private:
		string_t m_name;
		raylib::ShaderUniformDataType m_type;
		void* m_data;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template< typename T >
	T sm::cshader_uniform::get_as()
	{
		if (m_data)
		{
			return reinterpret_cast<T>(m_data);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial
	{
	public:
		cmaterial(technique_t technique, blending_mode mode, blending_equation equation, blending_factor src, blending_factor dst);
		cmaterial(technique_t technique, blending_mode mode, blending_equation equation, blending_factor src, blending_factor dst,
			const vector_t< cshader_uniform >& static_uniforms, const vector_t< cshader_uniform >& dynamic_uniforms);
		~cmaterial();

		void bind_shader(const raylib::Shader& technique) const;
		void bind_blend_mode() const;
		void reset();
		void bind_static_uniforms(const raylib::Shader& technique) const;
		void bind_dynamic_uniforms(const raylib::Shader& technique) const;
		void end_shader() const;
		void end_blend_mode() const;
		void set_blend_mode(blending_mode mode);
		void set_blend_mode_src_factor(blending_factor factor);
		void set_blend_mode_dst_factor(blending_factor factor);
		void set_blend_mode_equation(blending_equation equation);
		void restore_default_blend_mode();
		void set_shader(technique_t technique);
		technique_t get_shader() const;
		template < typename T >
		void add_static_uniform(const string_t& uniform_name, const T& value, raylib::ShaderUniformDataType type);
		template < typename T >
		void add_dynamic_uniform(const string_t& uniform_name, const T& value, raylib::ShaderUniformDataType type);

	private:
		technique_t m_program;
		blending_mode m_blending_mode = blending_mode_alpha;
		blending_factor m_blending_src = blending_factor_src_color;
		blending_factor m_blending_dst = blending_factor_one_minus_src_alpha;
		blending_equation m_blending_equation = blending_equation_blend_color;
		vector_t< cshader_uniform > m_static_uniforms;
		vector_t< cshader_uniform > m_dynamic_uniforms;

	private:
		void bind_uniforms(const raylib::Shader& technique, const vector_t< cshader_uniform >& vector) const;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template < typename T >
	void sm::cmaterial::add_dynamic_uniform(const string_t& uniform_name, const T& value, raylib::ShaderUniformDataType type)
	{
		m_dynamic_uniforms.emplace_back(uniform_name, value, type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template < typename T >
	void sm::cmaterial::add_static_uniform(const string_t& uniform_name, const T& value, raylib::ShaderUniformDataType type)
	{
		m_static_uniforms.emplace_back(uniform_name, value, type);
	}

	//------------------------------------------------------------------------------------------------------------------------
	class csprite_atlas
	{
	public:
		csprite_atlas() = default;
		csprite_atlas(const csprite_atlas& other);
		csprite_atlas& operator=(const csprite_atlas& other);
		~csprite_atlas() = default;

		void reset();
		core::srect subtexture(subtexture_t texture) const;
		texture_t atlas() const;
		unsigned subtexture_count() const;

		//- atlas constructor
		csprite_atlas& begin(texture_t texture);
		//- implicitly create a subtexture, where the order of creation is the same as the handle
		csprite_atlas& subtexture(const core::srect& rect);
		csprite_atlas& end();

	private:
		texture_t m_texture;
		vector_t< core::srect > m_subtextures;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class icamera_manager : public core::cservice
	{
	public:
		virtual ccamera* get_active() const { ASSERT(false, "Invalid operation. Using icamera_manager interface  function"); return nullptr; };

		RTTR_ENABLE(core::cservice);
	};

} //- sm

namespace compression
{
// 	sm::texture_format convert_raylib_texture_format(raylib::PixelFormat format);
// 	u64 estimate_texture_gpu_size(u32 w, u32 h, sm::texture_format format);
// 	bool compress_image_to_qoi(const core::cpath& path, const raylib::Image& image);
// 	bool compress_image_to_bc(const core::cpath& compressed_image_path, core::cpath& path,
// 		stringview_t filename, nvtt::Format format, nvtt::Quality quality, string_t* error = nullptr);
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
	}

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(icamera_manager)
	{
		rttr::registration::class_<icamera_manager>("icamera_manager")
			.constructor<>()
			(
				rttr::policy::ctor::as_raw_ptr
			)
			.method("get_active", &icamera_manager::get_active)
			;
	}

} //- sm