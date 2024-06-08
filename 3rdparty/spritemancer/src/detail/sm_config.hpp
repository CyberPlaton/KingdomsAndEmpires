#pragma once
#include "sm_entry.hpp"

namespace sm
{
	class irenderer;
	class iplatform;
	class iapp;
	class cshader;
	class cprogram;
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

	bool is_valid(const cshader& shader);
	bool is_valid(const cprogram& program);
	bool is_valid(const cimage& image);
	bool is_valid(const ctexture& texture);
	bool is_valid(const crendertarget& target);
	bool is_valid(const ccamera& camera);
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
	using texture_format = bgfx::TextureFormat::Enum;

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
		shader_language_hlsl,
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
		blending_factor_none = 0,
		blending_factor_zero,
		blending_factor_one,
		blending_factor_src_color,
		blending_factor_one_minus_src_color,
		blending_factor_src_alpha,
		blending_factor_one_minus_src_alpha,
		blending_factor_dst_alpha,
		blending_factor_dst_color,
		blending_factor_one_minus_dst_alpha,
		blending_factor_src_alpha_saturate,
		blending_factor_constant_color,
		blending_factor_one_minus_constant_color,
		blending_factor_constant_alpha,
		blending_factor_one_minus_constant_alpha,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_equation
	{
		blending_equation_none = 0,
		blending_equation_add,
		blending_equation_min,
		blending_equation_max,
		blending_equation_subtract,
		blending_equation_reverse_subtract,
		blending_equation_blend_equation_rgb,
		blending_equation_blend_equation_alpha,
		blending_equation_blend_dst_rgb,
		blending_equation_blend_src_rgb,
		blending_equation_blend_dst_alpha,
		blending_equation_blend_src_alpha,
		blending_equation_blend_color,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_mode : uint8_t
	{
		blending_mode_none = 0,
		blending_mode_alpha,
		blending_mode_additive,
		blending_mode_multiplied,
		blending_mode_add_colors,
		blending_mode_subtract_colors,
		blending_mode_alpha_premultiply,
	};

	//- @see https://subscription.packtpub.com/book/programming/9781849698009/1/ch01lvl1sec10/types-of-shaders
	//------------------------------------------------------------------------------------------------------------------------
	enum shader_type : uint8_t
	{
		shader_type_none = 0,
		shader_type_vertex,
		shader_type_fragment,
		shader_type_geometry,	//- creates new rendering primitives from output of vertex shader to be processed by fragment shader
		shader_type_compute,	//- general purpose shader that can affect all others
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
		renderable_flag_blending_custom = BIT(7),
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
		bool operator==(const sblending& other);
		bool operator!=(const sblending& other);

		blending_mode m_mode;
		blending_equation m_equation;
		blending_factor m_dst_factor;
		blending_factor m_src_factor;
	};

	//- A shader, a single vertex or fragment, or compute etc. shader.
	//- TODO: As of now, we can only load embedded shader types compiled externally. Each function to load a shader expects
	//- that exactly format.
	//------------------------------------------------------------------------------------------------------------------------
	class cshader final
	{
	public:
		static void destroy(cshader& shader);

		explicit cshader(shader_type type, stringview_t filepath);
		explicit cshader(shader_type type, const char* string);
		explicit cshader(shader_type type, const uint8_t* data, unsigned size);
		cshader();
		~cshader();

		opresult load_from_file(shader_type type, stringview_t filepath);
		opresult load_from_string(shader_type type, const char* string);
		opresult load_from_memory(shader_type type, const uint8_t* data, unsigned size);

		inline bgfx::ShaderHandle shader() const { return m_shader; }
		inline shader_type type() const { return m_type; }

		void set_uniform_float(stringview_t name, float value);
		void set_uniform_int(stringview_t name, int value);
		void set_uniform_vec2(stringview_t name, const vec2_t& value);
		void set_uniform_vec3(stringview_t name, const vec3_t& value);
		void set_uniform_vec4(stringview_t name, const vec4_t& value);
		void set_uniform_matrix(stringview_t name, const mat4_t& value);
		void set_uniform_texture(stringview_t name, const ctexture& value);
		void remove_uniform(stringview_t name);

		cshader& operator=(const cshader& other);

	private:
		bgfx::ShaderHandle m_shader;
		shader_type m_type;
	};

	//- Container for vertex and fragment shader. Destroying destroys individual shaders.
	//------------------------------------------------------------------------------------------------------------------------
	class cprogram final
	{
	public:
		static bgfx::ProgramHandle create(const cshader& shader);
		static void destroy(cprogram& program);

		explicit cprogram(const cshader& vertex, const cshader& fragment);
		cprogram();
		~cprogram();

		opresult load_from_shaders(const cshader& vertex, const cshader& fragment);
		opresult load_from_handles(bgfx::ShaderHandle vertex, bgfx::ShaderHandle fragment);

		inline bgfx::ProgramHandle handle() const { return m_handle; }
		inline const cshader& vertex() const { return m_vertex; }
		inline const cshader& fragment() const { return m_fragment; }

	private:
		cshader m_vertex;
		cshader m_fragment;
		bgfx::ProgramHandle m_handle;
	};

	//- CPU resident image representation
	//------------------------------------------------------------------------------------------------------------------------
	class cimage final
	{
	public:
		static void destroy(cimage& image);

		explicit cimage(stringview_t filepath);
		explicit cimage(void* data, unsigned size);
		cimage();
		~cimage();

		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(void* data, unsigned size);

		inline const bimg::ImageContainer* image() const { return m_container; }

		//- utility functions for image generation and manipulation
		void create_solid(unsigned w, unsigned h, const core::scolor& color);
		void create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second);

	private:
		bimg::ImageContainer* m_container;
	};

	//- GPU resident image representation.
	//- TODO: information does not need to be held by each texture. Store it somewhere else and in texture only the handle.
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final
	{
	public:
		static void destroy(ctexture& texture);

		explicit ctexture(const cimage& image);
		explicit ctexture(stringview_t filepath);
		explicit ctexture(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format format, uint64_t flags);
		explicit ctexture(bgfx::TextureHandle handle, const bgfx::TextureInfo& info);
		ctexture();
		~ctexture();


		opresult load_from_image(const cimage& image);
		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format format, uint64_t flags);

		inline unsigned w() const { return SCAST(unsigned, m_info.width); }
		inline unsigned h() const { return SCAST(unsigned, m_info.height); }
		inline bgfx::TextureHandle texture() const { return m_texture; }

	private:
		bgfx::TextureHandle m_texture;
		bgfx::TextureInfo m_info;
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
		opresult resize(unsigned w, unsigned h);

		inline unsigned w() const { return SCAST(unsigned, m_width); }
		inline unsigned h() const { return SCAST(unsigned, m_height); }
		inline bgfx::FrameBufferHandle target() const { return m_framebuffer; }
		inline bgfx::TextureHandle texture() const { return m_texture; }

	private:
		bgfx::FrameBufferHandle m_framebuffer;
		bgfx::TextureHandle m_texture;
		uint16_t m_width;
		uint16_t m_height;
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

		vec2_t m_position;
		vec2_t m_offset;
		float m_zoom;
		float m_rotation;
		bool m_ready;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas final
	{
	public:
		static void destroy(cspriteatlas& atlas);

		explicit cspriteatlas(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);
		cspriteatlas();
		~cspriteatlas() = default;

		opresult create(unsigned w, unsigned h, const vector_t<string_t>& names, const vec2_t& frames);
		const core::srect& at(stringview_t name) const;
		vec2_t dimension() const;
		unsigned subtextures() const;

		//- Routines for manual atlas creation
		cspriteatlas& begin(unsigned w, unsigned h);
		cspriteatlas& subtexture(stringview_t name, const core::srect& rect);
		cspriteatlas& end();

	private:
		umap_t<unsigned, core::srect> m_subtextures;
		vec2_t m_size;
	};

	//- Basically a sprite to be rendered on screen, along with some flags and customization.
	//- TODO: materials, spriteatlases
	//------------------------------------------------------------------------------------------------------------------------
	struct srenderable
	{
		core::srect m_src = { 0.0f, 0.0f, 1.0f, 1.0f };
		cshader m_shader;
		cimage m_image;
		ctexture m_texture;
		sblending m_blending;
		vec2_t m_origin = { 0.0f, 0.0f };
		float m_rotation = 0.0f;//- degrees
		int m_flags = 0;		//- bitwise concated renderable_flag
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
		virtual void on_imgui() = 0;
		virtual void on_shutdown() = 0;
	};

} //- sm