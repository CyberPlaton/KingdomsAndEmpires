#pragma once
#include "platform.hpp"
#include "raylib_integration/raylib.hpp"
#include <core.h>
#include <argparse.h>
#include <plugin_logging.h>

namespace sm
{
	constexpr auto C_MATERIAL_RESERVE_COUNT		= 256;
	constexpr auto C_IMAGE_RESERVE_COUNT		= 512;
	constexpr auto C_TEXTURE_RESERVE_COUNT		= 512;
	constexpr auto C_TECHNIQUE_RESERVE_COUNT	= 256;
	constexpr auto C_SPRITEATLAS_RESERVE_COUNT	= 128;
	constexpr auto C_RENDERTARGET_RESERVE_COUNT = 16;
	constexpr material_t C_DEFAULT_MATERIAL		= 0;

	class irenderer;
	class iplatform;
	class iapp;
	class cshader;
	class crendertarget;
	class cimage;
	class ctexture;
	class ccamera;
	class cmaterial;
	class cspriteatlas;
	class ctexture_manager;
	class cshader_manager;
	class cmaterial_manager;
	class cspriteatlas_manager;
	class crendertarget_manager;

	raylib::Color to_cliteral(const core::scolor& color);
	bool is_valid(const cshader& shader);
	bool is_valid(const cimage& image);
	bool is_valid(const ctexture& texture);
	bool is_valid(const crendertarget& target);
	bool is_valid(const ccamera& camera);
	bool is_valid(renderlayer_t layer);
	bool is_valid(const cmaterial& material);
	bool is_valid(const cspriteatlas& atlas);

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
	enum renderable_flag : uint32_t
	{
		renderable_flag_none			= 0,
		renderable_flag_invisible		= BIT(1),
		renderable_flag_flipx			= BIT(2),
		renderable_flag_flipy			= BIT(3),
		renderable_flag_origin_center	= BIT(4),
		renderable_flag_origin_custom	= BIT(5),
		renderable_flag_has_material	= BIT(6),
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum layer_flags : uint32_t
	{
		layer_flags_none			= 0,
		layer_flags_2d				= BIT(1),
		layer_flags_non_fullscreen	= BIT(2),
		layer_flags_origin_custom	= BIT(3),
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum opresult : uint8_t
	{
		opresult_ok = 0,
		opresult_fail = 255,
	};

	//- Similar to the error reporter for core library. Set own callback to get reports from inside the library.
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
	//- Camera class designed to be lighweight and copied around and to be a thin layer over raylib::Camera2D.
	//------------------------------------------------------------------------------------------------------------------------
	class ccamera final
	{
	public:
		ccamera();
		~ccamera() = default;

		raylib::Camera2D camera() const;
		inline bool ready() const { return m_ready; }

		vec2_t m_position;
		vec2_t m_offset;
		float m_zoom;
		float m_rotation;
		bool m_ready;
	};

	//- Basically a shader with uniforms and blending settings.
	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial final
	{
	public:
		static void destroy(cmaterial& material);

		explicit cmaterial(effect_t shader, blending_mode mode = blending_mode_alpha, blending_equation equation = blending_equation_blend_color,
			blending_factor src = blending_factor_src_color, blending_factor dst = blending_factor_one_minus_src_alpha);

		explicit cmaterial(effect_t shader, sblending blending);
		cmaterial();
		~cmaterial() = default;

		opresult create(effect_t shader, blending_mode mode = blending_mode_alpha, blending_equation equation = blending_equation_blend_color,
			blending_factor src = blending_factor_src_color, blending_factor dst = blending_factor_one_minus_src_alpha);

		opresult create(effect_t shader, sblending blending);

		inline effect_t effect() const { return m_shader; }
		inline sblending blending() const { return m_blending; }

	private:
		sblending m_blending;
		effect_t m_shader;

	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas final
	{
	public:
		static void destroy(cspriteatlas& atlas);

		explicit cspriteatlas(texture_t texture);
		cspriteatlas();
		~cspriteatlas() = default;

		const core::srect& get(subtexture_t texture) const;

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

	//- Constant time access to textures by handle. Logarithmic time loading and unloading.
	//------------------------------------------------------------------------------------------------------------------------
	class cimage_manager final : public core::cservice
	{
	public:
		cimage_manager();
		~cimage_manager();

		image_t load(stringview_t name, stringview_t path);
		image_t load(stringview_t name, image_type type, void* data, unsigned size);

		image_t at(stringview_t name) const;
		void unload(texture_t handle);

		const cimage& get(image_t handle) const;
		cimage& get(image_t handle);

	private:
		core::detail::cdynamic_pool<cimage> m_images;
		umap_t<string_t, image_t> m_handles;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class ctexture_manager final : public core::cservice
	{
	public:
		ctexture_manager();
		~ctexture_manager();

		texture_t load(stringview_t name, stringview_t path);
		texture_t load(stringview_t name, const cimage& image);
		texture_t load(stringview_t name, image_type type, void* data, unsigned size);

		texture_t at(stringview_t name) const;
		void unload(texture_t handle);

		const ctexture& get(texture_t handle) const;
		ctexture& get(texture_t handle);

	private:
		core::detail::cdynamic_pool<ctexture> m_textures;
		umap_t<string_t, texture_t> m_handles;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cshader_manager final : public core::cservice
	{
	public:
		cshader_manager();
		~cshader_manager();

		effect_t load(stringview_t name, shader_type type, stringview_t vertex_filepath, stringview_t fragment_filepath);
		effect_t load(stringview_t name, shader_type type, const char* vs, const char* fs);
		effect_t load(stringview_t name, shader_type type, const uint8_t* vs, unsigned vs_size, const uint8_t* fs, unsigned fs_size);

		void unload(effect_t handle);
		effect_t at(stringview_t name) const;
		const cshader& get(effect_t handle) const;
		cshader& get(effect_t handle);

	private:
		core::detail::cdynamic_pool<cshader> m_shaders;
		umap_t<string_t, effect_t> m_handles;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cmaterial_manager final : public core::cservice
	{
	public:
		cmaterial_manager();
		~cmaterial_manager();

		material_t load(stringview_t name, effect_t technique, blending_mode mode = blending_mode_alpha,
			blending_equation equation = blending_equation_blend_color, blending_factor src = blending_factor_src_color,
			blending_factor dst = blending_factor_one_minus_src_alpha);
		material_t at(stringview_t name) const;
		void unload(material_t handle);

		const cmaterial& get(material_t handle) const;
		cmaterial& get(material_t handle);

	private:
		core::detail::cdynamic_pool<cmaterial> m_materials;
		umap_t<string_t, material_t> m_handles;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas_manager final : public core::cservice
	{
	public:
		cspriteatlas_manager();
		~cspriteatlas_manager();

		spriteatlas_t load(stringview_t name, texture_t texture, const vec2_t& frames);
		void unload(material_t handle);

		const cspriteatlas& get(spriteatlas_t handle) const;
		cspriteatlas& get(spriteatlas_t handle);

	private:
		core::detail::cdynamic_pool<cspriteatlas> m_spritesheets;
		umap_t<string_t, spriteatlas_t> m_handles;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	class crendertarget_manager final : public core::cservice
	{
	public:
		crendertarget_manager();
		~crendertarget_manager();

		rendertarget_t load(stringview_t name, unsigned width, unsigned height);
		void unload(rendertarget_t handle);

		const crendertarget& get(rendertarget_t handle) const;
		crendertarget& get(rendertarget_t handle);

		template<typename TCallable>
		void visit(TCallable callback);

	private:
		core::detail::cdynamic_pool<crendertarget> m_rendertargets;
		umap_t<string_t, spriteatlas_t> m_handles;

		RTTR_ENABLE(core::cservice);
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	void sm::crendertarget_manager::visit(TCallable callback)
	{
		auto* target = m_rendertargets.begin();
		while (target)
		{
			callback(*target);

			target = m_rendertargets.advance(target);
		}
	}

	//- Basically a sprite to be rendered on screen, along with some flags and customization.
	//- TODO: materials, spriteatlases
	//------------------------------------------------------------------------------------------------------------------------
	struct srenderable
	{
		static void destroy(srenderable& renderable);

		core::srect m_src = { 0.0f, 0.0f, 1.0f, 1.0f };
		cmaterial m_material;
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

	//- Description of a rendering layer. Some of the data becomes only relevant when appropriate flags are set
	//------------------------------------------------------------------------------------------------------------------------
	struct slayer
	{
		crendertarget m_target;
		vector_t<ccommand> m_commands;
		ccamera m_camera;					//- optional: camera to be used when rendering
		cshader m_shader;					//- optional: shader used to render the layer render texture on previous layers
		core::scolor m_combine_tint;		//- color put over layer render texture when drawing on previous layers
		core::scolor m_clear_tint;			//- color used to clear the layer render texture before drawing
		vec2_t m_position = { 0.0f, 0.0f }; //- optional: where to draw the render target when combining; by default we cover whole screen
		vec2_t m_scale = { 1.0f, 1.0f };	//- optional: scaling of the render target; by default normal scale
		vec2_t m_origin = { 0.0f, 0.0f };	//- optional: origin of the render target; by default top-left
		unsigned m_flags = 0;				//- bitwise concated layer_flags
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
		virtual bool begin(const slayer& layer) = 0;
		virtual void draw(const slayer& layer) = 0;
		virtual void end(const slayer& layer) = 0;
		virtual bool combine(const slayer& layer) = 0;

		virtual void update_texture_gpu(uint64_t id, unsigned w, unsigned h, texture_format format, const void* data) = 0;
		virtual void update_texture_cpu(uint64_t id, unsigned w, unsigned h, texture_format format, void*& data) = 0;

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

		virtual opresult init_gfx(unsigned w, unsigned h,
			bool fullscreen, bool vsync) = 0;				//- create graphical context

		virtual opresult init_mainwindow(stringview_t title,
			unsigned w, unsigned h, bool fullscreen) = 0;	//- create application main window

		virtual opresult optional_init_event_mainloop() = 0;//- process hardware events in a loop; use where required
		virtual opresult optional_process_event() = 0;				//- process one hardware event

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