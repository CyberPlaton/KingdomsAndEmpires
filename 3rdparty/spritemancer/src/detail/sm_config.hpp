#pragma once
#include "platform.hpp"
#include <core.h>
#include <argparse.h>
#include "raylib_integration/raylib.hpp"
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
	//- How much resources to expect, not a fixed size, merely for reserving memory.
	//------------------------------------------------------------------------------------------------------------------------
	constexpr auto C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT			= 256;
	constexpr auto C_TEXTURE_RESOURCE_MANAGER_RESERVE_COUNT			= 512;
	constexpr auto C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT			= 512;
	constexpr auto C_SPRITEATLAS_RESOURCE_MANAGER_RESERVE_COUNT		= 128;
	constexpr auto C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT	= 64;

	//------------------------------------------------------------------------------------------------------------------------
	using image_handle_t = unsigned;
	using texture_handle_t = unsigned;
	using shader_handle_t = unsigned;
	using spriteatlas_handle_t = unsigned;
	using rendertarget_handle_t = unsigned;
	static constexpr auto C_INVALID_HANDLE = MAX(unsigned);

	//------------------------------------------------------------------------------------------------------------------------
	struct scontext;
	class irenderer;
	class iplatform;
	class ios;
	class iapp;
	struct slayer;
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
		shader_language_glsl,	//- OpenGL 3.3
		shader_language_essl,	//- OpenGL ES
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

	//- BC_* formats: https://learn.microsoft.com/en-us/windows/win32/direct3d10/d3d10-graphics-programming-guide-resources-block-compression#bc3
	//------------------------------------------------------------------------------------------------------------------------
	enum texture_format : uint8_t
	{
		texture_format_none = 0,
		//- uncompressed
		texture_format_r8			= raylib::PIXELFORMAT_UNCOMPRESSED_GRAYSCALE,
		texture_format_r8g8			= raylib::PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA,
		texture_format_r5g6b5		= raylib::PIXELFORMAT_UNCOMPRESSED_R5G6B5,
		texture_format_r16g16b16a16 = raylib::PIXELFORMAT_UNCOMPRESSED_R16G16B16A16,
		texture_format_r8g8b8		= raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8,
		texture_format_r8g8b8a8		= raylib::PIXELFORMAT_UNCOMPRESSED_R8G8B8A8,
		texture_format_r32g32b32a32 = raylib::PIXELFORMAT_UNCOMPRESSED_R32G32B32A32,
		//- compressed
		texture_format_bc1			= raylib::PIXELFORMAT_COMPRESSED_DXT1_RGBA,
		texture_format_bc2			= raylib::PIXELFORMAT_COMPRESSED_DXT3_RGBA,
		texture_format_bc3			= raylib::PIXELFORMAT_COMPRESSED_DXT5_RGBA,
		texture_format_astc_4x4		= raylib::PIXELFORMAT_COMPRESSED_ASTC_4x4_RGBA,
		texture_format_astc_8x8		= raylib::PIXELFORMAT_COMPRESSED_ASTC_8x8_RGBA,
		texture_format_qoi,
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
		enum mode : uint8_t
		{
			blending_mode_alpha				= raylib::BLEND_ALPHA,
			blending_mode_additive			= raylib::BLEND_ADDITIVE,
			blending_mode_multiplied		= raylib::BLEND_MULTIPLIED,
			blending_mode_add_colors		= raylib::BLEND_ADD_COLORS,
			blending_mode_subtract_colors	= raylib::BLEND_SUBTRACT_COLORS,
			blending_mode_alpha_premultiply = raylib::BLEND_ALPHA_PREMULTIPLY
		};

		enum equation
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

		enum factor
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


		bool operator==(const sblending& other) const;
		bool operator!=(const sblending& other) const;

		mode m_mode;
		equation m_equation;
		factor m_dst_factor;
		factor m_src_factor;
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

	//------------------------------------------------------------------------------------------------------------------------
	class irenderer
	{
	public:
		virtual ~irenderer() = default;

		virtual void		prepare_device() = 0;
		virtual opresult	init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync) = 0;
		virtual opresult	shutdown_device() = 0;
		virtual void		prepare_frame() = 0;
		virtual void		display_frame() = 0;
		virtual void		update_viewport(const vec2_t& position, const vec2_t& size) = 0;
		virtual void		blendmode(sblending mode) = 0;
		virtual void		clear(const slayer& layer, bool depth) = 0;
		virtual bool		begin(const slayer& layer) = 0;
		virtual void		draw(const slayer& layer) = 0;
		virtual void		end(const slayer& layer) = 0;
		virtual bool		combine(const slayer& layer) = 0;
		virtual void		update_texture_gpu(uint64_t id, unsigned w, unsigned h, texture_format format, const void* data) = 0;
		virtual void		update_texture_cpu(uint64_t id, unsigned w, unsigned h, texture_format format, void*& data) = 0;

		RTTR_ENABLE();
	};

	//- Operating system interface class. Implementing hardware functionality, such as window creation, HDI interface and
	//- processing and emitting hardware events
	//------------------------------------------------------------------------------------------------------------------------
	class ios
	{
	public:
		virtual ~ios() = default;

		virtual opresult	init() = 0;																			//- create and init of client application
		virtual opresult	shutdown() = 0;																		//- destroy and clean of client application
		virtual opresult	init_gfx(unsigned w, unsigned h, bool fullscreen, bool vsync) = 0;					//- create graphical context
		virtual opresult	init_mainwindow(stringview_t title, unsigned w, unsigned h, bool fullscreen) = 0;	//- create application main window
		virtual opresult	process_events() = 0;																//- process one hardware events
		virtual void		main_window_position(unsigned* x, unsigned* y) = 0;
		virtual void		main_window_size(unsigned* x, unsigned* y) = 0;
		virtual float		frametime() const = 0;

		RTTR_ENABLE();
	};

	//- Optional platform such as steam
	//------------------------------------------------------------------------------------------------------------------------
	class iplatform
	{
	public:
		virtual ~iplatform() = default;

		virtual opresult	pre_init() = 0;		//- perform setup before os and graphics context are created
		virtual opresult	init() = 0;			//- initialize in turn after os and graphics context
		virtual void		shutdown() = 0;		//- shutdown before os and graphics context
		virtual void		post_shutdown() = 0;//- shutdown after os and graphics context
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