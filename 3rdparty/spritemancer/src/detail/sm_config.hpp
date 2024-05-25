#pragma once
#include "platform.hpp"
#include "raylib_integration/raylib.hpp"
#include <core.h>
#include <argparse.h>
#include <plugin_logging.h>

namespace sm
{
	class irenderer;
	class iplatform;
	class iapp;
	class cshader;
	class crendertarget;
	class cimage;
	class ctexture;

	raylib::Color to_cliteral(const core::scolor& color);
	bool is_valid(const cshader& shader);
	bool is_valid(const cimage& image);
	bool is_valid(const ctexture& texture);
	bool is_valid(const crendertarget& target);

	//------------------------------------------------------------------------------------------------------------------------
	enum window_flag : uint32_t
	{
		window_flag_none		= BIT(0),
		window_flag_fullscreen	= BIT(1),
		window_flag_msaa		= BIT(2),
		window_flag_vsync		= BIT(3),
		window_flag_resizable	= BIT(4),
		window_flag_decorated	= BIT(5),
		window_flag_show		= BIT(6),
		window_flag_focus		= BIT(7),
		window_flag_minimized	= BIT(8),
		window_flag_run_minimized = BIT(9),
		window_flag_has_icon	= BIT(10),
		window_flag_fxaa		= BIT(11),
	};

	//- BC_* formats: https://learn.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression#bc3
	//------------------------------------------------------------------------------------------------------------------------
	enum texture_format : uint8_t
	{
		texture_format_none = 0,
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
		texture_format_astc_4x4 = raylib::PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,
		texture_format_astc_8x8 = raylib::PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA,
		texture_format_qoi,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum shader_optimization : uint8_t
	{
		shader_optimization_none = 0,
		shader_optimization_full,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum shader_platform : uint8_t
	{
		shader_platform_none = 0,
		shader_platform_android,
		shader_platform_emscripten,
		shader_platform_ios,
		shader_platform_linux,
		shader_platform_macos,
		shader_platform_playstation,
		shader_platform_windows,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum shader_language : uint8_t
	{
		shader_language_none = 0,
		shader_language_glsl,
		shader_language_essl,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum shader_options : uint32_t
	{
		shader_options_none						= 0,
		shader_options_debug_information		= BIT(1),
		shader_options_avoid_flow_control		= BIT(2),
		shader_options_no_preshader				= BIT(3),
		shader_options_partial_precision		= BIT(4),
		shader_options_prefer_flow_control		= BIT(5),
		shader_options_backwards_compatability	= BIT(6),
		shader_options_warnings_are_errors		= BIT(7),
		shader_options_keep_intermediate		= BIT(8),
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_factor
	{
		blending_factor_zero = 0,
		blending_factor_one = 1,
		blending_factor_src_color = RL_SRC_COLOR,
		blending_factor_one_minus_src_color = RL_ONE_MINUS_SRC_COLOR,
		blending_factor_src_alpha = RL_SRC_ALPHA,
		blending_factor_one_minus_src_alpha = RL_ONE_MINUS_SRC_ALPHA,
		blending_factor_dst_alpha = RL_DST_ALPHA,
		blending_factor_dst_color = RL_DST_COLOR,
		blending_factor_one_minus_dst_alpha = RL_ONE_MINUS_DST_ALPHA,
		blending_factor_src_alpha_saturate = RL_SRC_ALPHA_SATURATE,
		blending_factor_constant_color = RL_CONSTANT_COLOR,
		blending_factor_one_minus_constant_color = RL_ONE_MINUS_CONSTANT_COLOR,
		blending_factor_constant_alpha = RL_CONSTANT_ALPHA,
		blending_factor_one_minus_constant_alpha = RL_ONE_MINUS_CONSTANT_ALPHA,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_equation
	{
		blending_equation_add = RL_FUNC_ADD,
		blending_equation_min = RL_MIN,
		blending_equation_max = RL_MAX,
		blending_equation_subtract = RL_FUNC_SUBTRACT,
		blending_equation_reverse_subtract = RL_FUNC_REVERSE_SUBTRACT,
		blending_equation_blend_equation_rgb = RL_BLEND_EQUATION_RGB,
		blending_equation_blend_equation_alpha = RL_BLEND_EQUATION_ALPHA,
		blending_equation_blend_dst_rgb = RL_BLEND_DST_RGB,
		blending_equation_blend_src_rgb = RL_BLEND_SRC_RGB,
		blending_equation_blend_dst_alpha = RL_BLEND_DST_ALPHA,
		blending_equation_blend_src_alpha = RL_BLEND_SRC_ALPHA,
		blending_equation_blend_color = RL_BLEND_COLOR,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_mode : uint8_t
	{
		blending_mode_alpha = raylib::BLEND_ALPHA,
		blending_mode_additive = raylib::BLEND_ADDITIVE,
		blending_mode_multiplied = raylib::BLEND_MULTIPLIED,
		blending_mode_add_colors = raylib::BLEND_ADD_COLORS,
		blending_mode_subtract_colors = raylib::BLEND_SUBTRACT_COLORS,
		blending_mode_alpha_premultiply = raylib::BLEND_ALPHA_PREMULTIPLY
	};

	//- @see https://subscription.packtpub.com/book/programming/9781849698009/1/ch01lvl1sec10/types-of-shaders
	//------------------------------------------------------------------------------------------------------------------------
	enum shader_type : uint8_t
	{
		shader_type_none = 0,
		shader_type_vertex,		//- individual
		shader_type_fragment,	//- individual
		shader_type_program,	//- combined vertex and fragment shaders
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum image_type : uint8_t
	{
		image_type_none = 0,
		image_type_png,
		image_type_bmp,
		image_type_tga,
		image_type_jpg,
		image_type_gif,
		image_type_pic,
		image_type_psd,
		image_type_hdr,
		image_type_qoi,
		image_type_svg,
		image_type_dds,
		image_type_pkm,
		image_type_ktx,
		image_type_pvr,
		image_type_astc
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum renderable_flag
	{
		renderable_flag_none			= 0,
		renderable_flag_invisible		= BIT(1),
		renderable_flag_flipx			= BIT(2),
		renderable_flag_flipy			= BIT(3),
		renderable_flag_origin_center	= BIT(4),
		renderable_flag_origin_custom	= BIT(5),
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum opresult : uint8_t
	{
		opresult_ok = 0,
		opresult_fail = 255,
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct serror_reporter
	{
		STATIC_INSTANCE_EX(serror_reporter);

		core::error_report_function_t m_callback = nullptr;
	};

	namespace entry
	{
		irenderer*			renderer();
		iplatform*			platform();
		iapp*				app();
		void				set_platform(ptr_t<iplatform>&& platform);
		void				set_app(iapp* app);
		void				set_renderer(ptr_t<irenderer>&& renderer);

	} //- entry

	//------------------------------------------------------------------------------------------------------------------------
	struct sblending final
	{
		blending_mode m_mode;
		blending_equation m_equation;
		blending_factor m_dst_factor;
		blending_factor m_src_factor;
	};

	//- A shader. Can be a single vertex or fragment shader or a combination of them, becoming a program
	//------------------------------------------------------------------------------------------------------------------------
	class cshader final
	{
	public:
		static void destroy(cshader& shader);

		explicit cshader(shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath);
		explicit cshader(shader_type type, const char* vs, const char* fs);
		explicit cshader(shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size);
		cshader();
		~cshader();

		opresult load_from_file(shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath);
		opresult load_from_string(shader_type type, const char* vs, const char* fs);
		opresult load_from_memory(shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size);

		inline raylib::Shader shader() const { return m_shader; }
		inline shader_type type() const { return m_type; }

		void set_uniform_float(stringview_t name, float value);
		void set_uniform_int(stringview_t name, int value);
		void set_uniform_vec2(stringview_t name, const vec2_t& value);
		void set_uniform_vec3(stringview_t name, const vec3_t& value);
		void set_uniform_vec4(stringview_t name, const vec4_t& value);
		void set_uniform_matrix(stringview_t name, const mat4_t& value);
		void set_uniform_texture(stringview_t name, const ctexture& value);
		void remove_uniform(stringview_t name);

	private:
		raylib::Shader m_shader;
		shader_type m_type;
	};

	//- CPU resident image representation
	//------------------------------------------------------------------------------------------------------------------------
	class cimage final
	{
	public:
		static void destroy(cimage& image);

		explicit cimage(stringview_t filepath);
		explicit cimage(image_type type, void* data, unsigned size);
		cimage();
		~cimage();

		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(image_type type, void* data, unsigned size);

		inline raylib::Image image() const { return m_container; }

		//- utility functions for image generation and manipulation
		void create_solid(unsigned w, unsigned h, const core::scolor& color);
		void create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second);

	private:
		raylib::Image m_container;
	};

	//- GPU resident image representation
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final
	{
	public:
		static void destroy(ctexture& texture);

		explicit ctexture(const cimage& image);
		explicit ctexture(stringview_t filepath);
		explicit ctexture(image_type type, void* data, unsigned size);
		ctexture();
		~ctexture();


		opresult load_from_image(const cimage& image);
		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(image_type type, void* data, unsigned size);

		inline unsigned w() const { return m_texture.width; }
		inline unsigned h() const { return m_texture.height; }
		inline raylib::Texture2D texture() const { return m_texture; }

	private:
		raylib::Texture2D m_texture;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget final
	{
	public:
		static void destroy(crendertarget& target);

		explicit crendertarget(unsigned w, unsigned h);
		crendertarget();
		~crendertarget();

		opresult create(unsigned w, unsigned h);

		inline unsigned w() const { return m_texture.texture.width; }
		inline unsigned h() const { return m_texture.texture.height; }
		inline raylib::RenderTexture2D target() const { return m_texture; }
		inline raylib::Texture2D texture() const { return m_texture.texture; }

	private:
		raylib::RenderTexture2D m_texture;
	};

	//- Texture along with image data that can be used as rendertarget or as something to be rendered
	//------------------------------------------------------------------------------------------------------------------------
	struct srenderable
	{
		static void destroy(srenderable& renderable);

		core::srect m_src = { 0.0f, 0.0f, 1.0f, 1.0f };
		cimage m_image;
		ctexture m_texture;
		vec2_t m_origin = { 0.0f, 0.0f };
		float m_rotation = 0.0f;//- degrees
		int m_flags = 0;		//- bitwise concated renderable_flag
	};

	//------------------------------------------------------------------------------------------------------------------------
	class ccommand final
	{
	public:
		using render_callback_t = std::function<void()>;

		explicit ccommand(render_callback_t&& callback);
		ccommand() = default;
		~ccommand() = default;

		void create(render_callback_t&& callback);

		void execute() const;

	private:
		render_callback_t m_callback;
	};

	//- Description of a layer we render upon, including all the decals to be rendered on it
	//------------------------------------------------------------------------------------------------------------------------
	struct slayer
	{
		crendertarget m_target;
		vector_t<ccommand> m_commands;
		cshader m_shader;
		core::scolor m_tint;
		bool m_show = false;
		unsigned m_id = 0;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class irenderer
	{
	public:
		virtual ~irenderer() = default;

		virtual void prepare_device() = 0;
		virtual opresult init_device(void* nwh, unsigned w, unsigned h,
			bool fullscreen, bool vsync) = 0;
		virtual opresult shutdown_device() = 0;

		virtual void prepare_frame() = 0;
		virtual void display_frame() = 0;
		virtual void update_viewport(const vec2_t& position, const vec2_t& size) = 0;
		virtual void blendmode(sblending mode) = 0;

		virtual void clear(const slayer& layer, bool depth) = 0;
		virtual void begin(const slayer& layer) = 0;
		virtual void draw(const slayer& layer) = 0;
		virtual void end(const slayer& layer) = 0;
		virtual void combine(const slayer& layer) = 0;

		virtual void update_texture_gpu(uint64_t id, unsigned w, unsigned h, void* data) = 0;
		virtual void update_texture_cpu(uint64_t id) = 0;

		RTTR_ENABLE();
	};

	//- Platform interface class. Implementing hardware functionality, such as window creation, HDI interface and
	//- processing and emitting hardware events
	//------------------------------------------------------------------------------------------------------------------------
	class iplatform
	{
	public:
		virtual ~iplatform() = default;

		virtual opresult init() = 0;						//- create and init of client application
		virtual opresult shutdown() = 0;					//- destroy and clean of client application
		virtual opresult init_on_thread() = 0;				//- create and init on internal 'engine' thread, where rendering and main update happens
		virtual opresult shutdown_on_thread() = 0;			//- destroy and clean on internal 'engine' thread, where rendering and main update happens

		virtual opresult init_gfx(unsigned w, unsigned h,
			bool fullscreen, bool vsync) = 0;				//- create graphical context

		virtual opresult init_mainwindow(stringview_t title,
			unsigned w, unsigned h, bool fullscreen) = 0;	//- create application main window

		//- TODO: does not seem to make sense, I think it was for platforms
		//- that might require processing some events and OS handshakes before
		//- the window can start and then exit, so not a real loop.
		//- Consider removing or at least renaming to be more descriptive.
		virtual opresult optional_init_event_mainloop() = 0;//- process hardware events in a loop; use where required
		virtual opresult process_event() = 0;				//- process one hardware event

		virtual void main_window_position(unsigned* x, unsigned* y) = 0;
		virtual void main_window_size(unsigned* x, unsigned* y) = 0;

		RTTR_ENABLE();
	};

	//- Interface class for application to derive from and implement only three basic functions
	//------------------------------------------------------------------------------------------------------------------------
	class iapp
	{
	public:
		virtual ~iapp() = default;
		virtual bool on_init(void* config, argparse::ArgumentParser& args) = 0;
		virtual void on_update(float) = 0;
		virtual void on_shutdown() = 0;
	};

} //- sm