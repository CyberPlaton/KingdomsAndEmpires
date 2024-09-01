#pragma once
#include "platform.hpp"
#include <core.h>
#include <argparse.h>
#include "bgfx_integration/bgfx.hpp"
#include "imgui_integration/imgui.hpp"
#include "imgui_integration/imgui_scopes.hpp"

#if PROFILE_ENABLE
//------------------------------------------------------------------------------------------------------------------------
#define SM_DRAW_CALL(vertices)				\
sm::profile::gpu::cscoped_drawcall			\
ANONYMOUS_VARIABLE(gpu_profile_function)	\
(											\
	static_cast<uint64_t>(vertices),		\
	CORE_FUNC_SIG							\
)
//------------------------------------------------------------------------------------------------------------------------
#define SM_NAMED_DRAW_CALL(vertices, name)	\
sm::profile::gpu::cscoped_drawcall			\
ANONYMOUS_VARIABLE(gpu_profile_function)	\
(											\
	static_cast<uint64_t>(vertices),		\
	name									\
)
#else
#define SM_DRAW_CALL(vertices)
#define SM_NAMED_DRAW_CALL(vertices, name)
#endif

namespace sm
{
	constexpr auto C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT			= 256;
	constexpr auto C_TEXTURE_RESOURCE_MANAGER_RESERVE_COUNT			= 512;
	constexpr auto C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT			= 512;
	constexpr auto C_SPRITEATLAS_RESOURCE_MANAGER_RESERVE_COUNT		= 128;
	constexpr auto C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT	= 64;

	using image_handle_t		= uint16_t;
	using texture_handle_t		= uint16_t;
	using shader_handle_t		= uint16_t;
	using spriteatlas_handle_t	= uint16_t;
	using rendertarget_handle_t = uint16_t;

	class irenderer;
	class iplatform;
	class ios;
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

	bool is_valid(const cshader& shader);
	bool is_valid(const cimage& image);
	bool is_valid(const ctexture& texture);
	bool is_valid(const crendertarget& target);
	bool is_valid(const cmaterial& material);
	bool is_valid(const cspriteatlas& atlas);

	//------------------------------------------------------------------------------------------------------------------------
	enum vertex_type : uint8_t
	{
		vertex_type_none = 0,
		vertex_type_transient,
		vertex_type_dynamic,
		vertex_type_static,
	};

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
		blending_factor_zero						= BGFX_STATE_BLEND_ZERO,
		blending_factor_one							= BGFX_STATE_BLEND_ONE,
		blending_factor_src_color					= BGFX_STATE_BLEND_SRC_COLOR,
		blending_factor_one_minus_src_color			= BGFX_STATE_BLEND_INV_SRC_COLOR,
		blending_factor_src_alpha					= BGFX_STATE_BLEND_SRC_ALPHA,
		blending_factor_one_minus_src_alpha			= BGFX_STATE_BLEND_INV_SRC_ALPHA,
		blending_factor_dst_alpha					= BGFX_STATE_BLEND_DST_ALPHA,
		blending_factor_dst_color					= BGFX_STATE_BLEND_DST_COLOR,
		blending_factor_one_minus_dst_color			= BGFX_STATE_BLEND_INV_DST_COLOR,
		blending_factor_one_minus_dst_alpha			= BGFX_STATE_BLEND_INV_DST_ALPHA,
		blending_factor_src_alpha_saturate			= BGFX_STATE_BLEND_SRC_ALPHA_SAT,
		blending_factor_constant_color				= BGFX_STATE_BLEND_FACTOR,
		blending_factor_one_minus_constant_color	= BGFX_STATE_BLEND_INV_FACTOR,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_equation
	{
		blending_equation_add					= BGFX_STATE_BLEND_EQUATION_ADD,
		blending_equation_min					= BGFX_STATE_BLEND_EQUATION_MIN,
		blending_equation_max					= BGFX_STATE_BLEND_EQUATION_MAX,
		blending_equation_subtract				= BGFX_STATE_BLEND_EQUATION_SUB,
		blending_equation_reverse_subtract		= BGFX_STATE_BLEND_EQUATION_REVSUB,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_mode : int
	{
		blending_mode_alpha				= BGFX_STATE_BLEND_FUNC(blending_factor_src_alpha, blending_factor_one_minus_src_alpha) | blending_equation_add,
		blending_mode_additive			= BGFX_STATE_BLEND_FUNC(blending_factor_one, blending_factor_one) | blending_equation_add,
		blending_mode_multiplied		= BGFX_STATE_BLEND_FUNC(blending_factor_dst_color, blending_factor_zero) | blending_equation_add,
		blending_mode_add_colors		= BGFX_STATE_BLEND_FUNC(blending_factor_one, blending_factor_one) | blending_equation_add,
		blending_mode_subtract_colors	= BGFX_STATE_BLEND_FUNC(blending_factor_one, blending_factor_one) | blending_equation_subtract,
		blending_mode_alpha_premultiply = BGFX_STATE_BLEND_FUNC(blending_factor_src_alpha, blending_factor_one) | blending_equation_add
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

	//- Similar to the error reporter for core library. Set own callback to get reports from inside the library.
	//------------------------------------------------------------------------------------------------------------------------
	struct serror_reporter
	{
		STATIC_INSTANCE_EX(serror_reporter);

		core::error_report_function_t m_callback = nullptr;
	};

	namespace entry
	{
		bx::FileReaderI*	filreader();
		bx::FileWriterI*	filewriter();
		bx::AllocatorI*		allocator();
		bool				has_platform();
		iplatform*			get_platform();
		irenderer*			get_renderer();
		ios*				get_os();
		iapp*				get_app();
		void				set_os(ptr_t<ios>&& os);
		void				set_platform(ptr_t<iplatform>&& platform);
		void				set_app(iapp* app);
		void				set_renderer(ptr_t<irenderer>&& renderer);

	} //- entry

	//------------------------------------------------------------------------------------------------------------------------
	struct sblending final
	{
		bool operator==(const sblending& other) const;
		bool operator!=(const sblending& other) const;

		blending_mode m_mode;
	};

	//- Lightweight class containing a shader uniform. Can easily be copied around and has to be manually destroyed when
	//- created.
	//------------------------------------------------------------------------------------------------------------------------
	class cuniform final
	{
	public:
		static void destroy(cuniform& uniform);

		explicit cuniform(stringview_t name, bgfx::UniformType::Enum type);
		cuniform();
		~cuniform();

		opresult create(stringview_t name, bgfx::UniformType::Enum type);

		inline bgfx::UniformHandle handle() const { return m_handle; }

	private:
		bgfx::UniformHandle m_handle;
	};

	//- A shader. Can be a single vertex, fragment or compute shader. A vertex and fragment shader can be used to create
	//- a program to be used for rendering.
	//------------------------------------------------------------------------------------------------------------------------
	class cshader final : public core::cresource
	{
	public:
		static void destroy(cshader& shader);

		explicit cshader(stringview_t path);
		explicit cshader(const char* text);
		explicit cshader(const uint8_t* data, unsigned size);
		cshader();
		~cshader();

		opresult load_from_file(stringview_t path);
		opresult load_from_string(const char* text);
		opresult load_from_memory(const uint8_t* data, unsigned size);

		inline bgfx::ShaderHandle shader() const { return m_handle; }

		cshader& operator=(const cshader& other);

	private:
		bgfx::ShaderHandle m_handle;

		RTTR_ENABLE(core::cresource);
	};

	//- CPU resident image representation
	//------------------------------------------------------------------------------------------------------------------------
	//- CPU resident image representation. Can easily be copied around, as the data is managed elsewhere, as a
	//- consequence, you have to manually call destroy to signal that an image is no longer needed,
	//- otherwise the memory will not be freed.
	//------------------------------------------------------------------------------------------------------------------------
	class cimage final : public core::cresource
	{
	public:
		static void destroy(cimage& image);

		explicit cimage(stringview_t filepath);
		explicit cimage(void* data, unsigned size);
		cimage();
		~cimage();

		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(void* data, unsigned size);

		inline bimg::ImageContainer* image() const { return m_container; }

		//- utility functions for image generation and manipulation
		void create_solid(unsigned w, unsigned h, const core::scolor& color);
		void create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second);

	private:
		bimg::ImageContainer* m_container;

		RTTR_ENABLE(core::cresource);
	};

	//- GPU resident image representation
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final : public core::cresource
	{
	public:
		static void destroy(ctexture& texture);

		explicit ctexture(const cimage& image);
		explicit ctexture(stringview_t filepath);
		explicit ctexture(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format format, uint64_t flags);
		ctexture();
		~ctexture();

		opresult load_from_image(const cimage& image);
		opresult load_from_file(stringview_t filepath);
		opresult load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format format, uint64_t flags);

		inline unsigned w() const { return m_info.width; }
		inline unsigned h() const { return m_info.height; }
		inline bgfx::TextureHandle texture() const { return m_handle; }

	private:
		bgfx::TextureInfo m_info;
		bgfx::TextureHandle m_handle;

		RTTR_ENABLE(core::cresource);
	};

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

		inline unsigned w() const { return  m_info.width; }
		inline unsigned h() const { return m_info.height; }
		inline bgfx::FrameBufferHandle target() const { return m_target; }
		inline bgfx::TextureHandle texture() const { return bgfx::getTexture(target()); }
		inline const bgfx::TextureInfo& info() const { return m_info; }

	private:
		bgfx::TextureInfo m_info;
		bgfx::FrameBufferHandle m_target;

		RTTR_ENABLE(core::cresource);
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

		void matrix_view_update();
		void matrix_projection_update();

		mat4_t& matrix_view() { return m_view; }
		mat4_t& matrix_projection() { return m_projection; }

		vec3_t& translation() { return m_translation; }
		vec3_t& rotation() { return m_rotation; }
		vec3_t& scale() { return m_scale; }

		core::srect viewport() { return m_viewport; }
		core::scolor clearcolor() { return m_clear_color; }

		float plane_near() { return m_near; }
		float plane_far() { return m_far; }

		uint64_t state() { return m_state; }
		uint64_t clear_state() { return m_clear_state; }

		void* window_handle() { return m_window_handle; }

	private:
		mat4_t m_view;
		mat4_t m_projection;
		vec3_t m_translation;
		vec3_t m_rotation;
		vec3_t m_scale;
		core::srect m_viewport;
		float m_near;
		float m_far;
		float m_zoom;
		core::scolor m_clear_color;
		uint64_t m_state;
		uint64_t m_clear_state;
		void* m_window_handle;

		RTTR_ENABLE();
		RTTR_REGISTRATION_FRIEND
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspriteatlas final : public core::cresource
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

		RTTR_ENABLE(core::cresource);
	};

	//------------------------------------------------------------------------------------------------------------------------
	struct srenderstate
	{
		bool operator ==(const srenderstate& other) const;
		bool operator !=(const srenderstate& other) const;

		sblending m_blending;
		int m_flags = 0; //- bitwise concated renderable_flag
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

	//- Operating system interface class. Implementing hardware functionality, such as window creation, HDI interface and
	//- processing and emitting hardware events
	//------------------------------------------------------------------------------------------------------------------------
	class ios
	{
	public:
		virtual ~ios() = default;

		virtual opresult init() = 0;						//- create and init of client application
		virtual opresult shutdown() = 0;					//- destroy and clean of client application

		virtual opresult init_gfx(int w, int h,
			bool fullscreen, bool vsync) = 0;				//- create graphical context

		virtual opresult init_mainwindow(stringview_t title,
			int w, int h, bool fullscreen) = 0;				//- create application main window

		virtual opresult optional_init_event_mainloop() = 0;//- process hardware events in a loop; use where required
		virtual opresult optional_process_event() = 0;		//- process one hardware event

		virtual void main_window_position(int* x, int* y) = 0;
		virtual void main_window_size(int* x, int* y) = 0;

		virtual bool is_key_held(core::key k) = 0;
		virtual bool is_key_pressed(core::key k) = 0;
		virtual bool is_key_released(core::key k) = 0;

		virtual bool is_mouse_button_held(core::mouse_button b) = 0;
		virtual bool is_mouse_button_pressed(core::mouse_button b) = 0;
		virtual bool is_mouse_button_released(core::mouse_button b) = 0;

		virtual float gamepad_axis(core::gamepad_axis a) = 0;

		RTTR_ENABLE();
	};

	//- Optional platform such as steam
	//------------------------------------------------------------------------------------------------------------------------
	class iplatform
	{
	public:
		virtual ~iplatform() = default;

		virtual opresult pre_init() = 0;	//- perform setup before os and graphics context are created
		virtual opresult init() = 0;		//- initialize in turn after os and graphics context
		virtual void shutdown() = 0;		//- shutdown before os and graphics context
		virtual void post_shutdown() = 0;	//- shutdown after os and graphics context
	};

	//- Interface class for application to derive from and implement functionality
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

	namespace profile::gpu
	{
		class iaggregator;
		using aggregator_ref_t = ref_t<iaggregator>;

		//------------------------------------------------------------------------------------------------------------------------
		struct sdrawcall_data
		{
			uint64_t m_vertices = 0;
			float m_time		= 0.0f;	//- microsecs
			const char* m_name	= nullptr;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cscoped_drawcall final
		{
		public:
			cscoped_drawcall(uint64_t vertices, const char* name);
			~cscoped_drawcall();

		private:
			sdrawcall_data m_data;
			core::ctimer m_timer;
		};

		//- As long as we use raylib the stats are meant to be quite inaccurate, giving only a general idea of resources used.
		//------------------------------------------------------------------------------------------------------------------------
		struct saggregated_drawcall_data
		{
			uint64_t m_drawcalls			= 0;
			uint64_t m_vertices				= 0;

			uint64_t m_textures_count		= 0;
			uint64_t m_shaders_count		= 0;
			uint64_t m_rendertargets_count	= 0;

			uint64_t m_textures_bytes		= 0;
			uint64_t m_shaders_bytes		= 0;
			uint64_t m_rendertargets_bytes	= 0;

			float m_drawing_time_total		= 0.0f;
			float m_drawing_time_mean		= 0.0f;
		};

		//- Basic information about GPU. Note: Currently cores and clock speed seem not to work correctly.
		//------------------------------------------------------------------------------------------------------------------------
		struct sgpu_stats
		{
			string_t m_model_vendor_driver;

			int64_t m_cores			= 0;
			int64_t m_clock_speed	= 0; //- MHz
			int64_t m_memory		= 0; //- Bytes
		};

		//- Interface class to gather stats on GPU memory usage and allocations.
		//------------------------------------------------------------------------------------------------------------------------
		class iaggregator
		{
		public:
			virtual ~iaggregator() = default;

			virtual vector_t<sgpu_stats>		stats() = 0;
			virtual saggregated_drawcall_data	drawcall_data() = 0;
			virtual void						update() = 0;
			virtual void						push(gpu::sdrawcall_data&& data) = 0;
		};

		void set_aggregator(aggregator_ref_t object);
		void set_default_aggregator();
		aggregator_ref_t get_aggregator();

	} //- profile::gpu

	namespace profile
	{
		//------------------------------------------------------------------------------------------------------------------------
		class cprofiler final
		{
		public:
			//- GPU
			static void								push(gpu::sdrawcall_data&& data);
			static gpu::saggregated_drawcall_data	drawcall_data();
		};

	} //- profile

} //- sm