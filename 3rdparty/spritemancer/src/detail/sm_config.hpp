#pragma once
#include "sm_entry.hpp"

//- Use macro to reflect your vertex type, the vertex type function must be declared and implemented.
//-------------------------------------------------------------------------------------------------------------------------
#define REGISTER_VERTEX_LAYOUT(s) \
	rttr::cregistrator<s>(STRINGIFY(s)) \
		.meth(ecs::vertexlayout::svertex::C_VERTEX_LAYOUT_FUNC_NAME, &s::vertex_layout) \
		.meth(ecs::vertexlayout::svertex::C_VERTEX_LAYOUT_HANDLE_FUNC_NAME, &s::vertex_layout_handle) \
		.meth(ecs::vertexlayout::svertex::C_INIT_FUNC_NAME, &s::init) \
		.meth(ecs::vertexlayout::svertex::C_MAKE_VERTEX_FUNC_NAME, &s::make) \
		;

namespace sm
{
	class irenderer;
	class iplatform;
	class iapp;
	class ios;
	class cuniform;
	class cshader;
	class cprogram;
	class crendertarget;
	class cimage;
	class ctexture;
	class ccamera;
	class cmaterial;
	class cspriteatlas;
	class ccontext;

	constexpr auto C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT = 256;
	constexpr auto C_TEXTURE_RESOURCE_MANAGER_RESERVE_COUNT = 512;
	constexpr auto C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT = 512;
	constexpr auto C_PROGRAM_RESOURCE_MANAGER_RESERVE_COUNT = 256;
	constexpr auto C_SPRITEATLAS_RESOURCE_MANAGER_RESERVE_COUNT = 128;
	constexpr auto C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT = 64;
	constexpr auto C_MESH_RESOURCE_MANAGER_RESERVE_COUNT = 2048;
	constexpr auto C_MATERIAL_RESOURCE_MANAGER_RESERVE_COUNT = 512;
	constexpr auto C_UNIFORM_RESOURCE_MANAGER_RESERVE_COUNT = 8192;

	constexpr stringview_t C_UNIFORM_TEXTURE0 = "texture0";
	constexpr stringview_t C_UNIFORM_TEXTURE1 = "texture1";
	constexpr stringview_t C_UNIFORM_TEXTURE2 = "texture2";
	constexpr stringview_t C_UNIFORM_TEXTURE3 = "texture3";
	constexpr stringview_t C_UNIFORM_TEXTURE4 = "texture4";
	constexpr stringview_t C_UNIFORM_TEXTURE5 = "texture5";
	constexpr stringview_t C_UNIFORM_TEXTURE6 = "texture6";
	constexpr stringview_t C_UNIFORM_TEXTURE7 = "texture7";

	using handle_type_t			= uint16_t;
	using image_handle_t		= handle_type_t;
	using texture_handle_t		= handle_type_t;
	using shader_handle_t		= handle_type_t;
	using program_handle_t		= handle_type_t;
	using spriteatlas_handle_t	= handle_type_t;
	using rendertarget_handle_t = handle_type_t;
	using renderpass_id_t		= handle_type_t;
	using uniform_handle_t		= handle_type_t;
	using vertex_layout_handle_t= handle_type_t;
	using mesh_handle_t			= handle_type_t;
	using material_handle_t		= handle_type_t;
	using index_type_t			= uint16_t;
	using renderpass_order_t	= vector_t<renderpass_id_t>;
	using context_state_flags_t = int;
	constexpr auto C_INVALID_HANDLE = MAX(handle_type_t);

	bool is_valid(const cshader& shader);
	bool is_valid(const cprogram& program);
	bool is_valid(const cimage& image);
	bool is_valid(const ctexture& texture);
	bool is_valid(const crendertarget& target);
	bool is_valid(const ccamera& camera);
	bool is_valid(const cmaterial& material);
	bool is_valid(const cspriteatlas& atlas);
	bool is_valid(const cuniform& uniform);

	//------------------------------------------------------------------------------------------------------------------------
	enum context_state_flag : uint8_t
	{
		context_state_flag_none = 0,
		context_state_flag_want_resize,
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
    enum view_mode : uint8_t
    {
        view_mode_none = 0,
        view_mode_default,          //- default sort order
        view_mode_sequential,       //- sort in same order in which submit calls were called
        view_mode_depth_ascending,  //- sort draw calls in ascending order
        view_mode_depth_descending, //- sort draw calls in descending order
        
    };

    //------------------------------------------------------------------------------------------------------------------------
    enum framebuffer_ratio : uint8_t
    {
        framebuffer_ratio_none = 0,
        framebuffer_ratio_equal,
        framebuffer_ratio_half,
        framebuffer_ratio_quarter,
        framebuffer_ratio_eighth,
        framebuffer_ratio_sixteenth,
        framebuffer_ratio_double,
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
        bool        create_context(ptr_t<ccontext>&& ctx = nullptr);
        void        destroy_context();
        ccontext*   ctx();
    
		bool		has_platform();
		irenderer*	renderer();
		ios*		os();
		iapp*		app();
		iplatform*	platform();
		void		set_os(ptr_t<ios>&& os);
		void		set_platform(ptr_t<iplatform>&& platform);
		void		set_app(iapp* app);
		void		set_renderer(ptr_t<irenderer>&& renderer);

	} //- entry

	namespace vertexlayout
	{
		//- This is a dummy vertex layout to show how one should be defined. While creating you must not inherit from it.
		//- Define all required functions and reflect them to RTTR using the macro REGISTER_VERTEX_LAYOUT().
		//------------------------------------------------------------------------------------------------------------------------
		struct svertex final
		{
			static constexpr rttr::string_view C_VERTEX_LAYOUT_FUNC_NAME		= "vertex_layout";
			static constexpr rttr::string_view C_VERTEX_LAYOUT_HANDLE_FUNC_NAME = "vertex_layout_handle";
			static constexpr rttr::string_view C_INIT_FUNC_NAME					= "init";
			static constexpr rttr::string_view C_MAKE_VERTEX_FUNC_NAME			= "make";

			static constexpr array_t<rttr::string_view, 4> C_LAYOUTS_FUNC_NAMES = { C_VERTEX_LAYOUT_FUNC_NAME, C_VERTEX_LAYOUT_HANDLE_FUNC_NAME, C_INIT_FUNC_NAME, C_MAKE_VERTEX_FUNC_NAME };

			static bool init() { return true; } //- Do initialization here using bgfx.
			static bgfx::VertexLayout vertex_layout() { return {}; }
			static vertex_layout_handle_t vertex_layout_handle() { return MAX(vertex_layout_handle_t); }
			static svertex make(float x, float y, float z) { return svertex{x, y, z}; }

			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
		};

	} //- vertexlayout

	//------------------------------------------------------------------------------------------------------------------------
	struct sblending final
	{
		bool operator==(const sblending& other);
		bool operator!=(const sblending& other);

		blending_mode m_mode;
	};

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
	class irenderer
	{
	public:
		virtual ~irenderer() = default;

		virtual void prepare_device() = 0;
		virtual opresult init_device(void* nwh, unsigned w, unsigned h, bool fullscreen, bool vsync) = 0;
		virtual opresult shutdown_device() = 0;

		RTTR_ENABLE();
	};

	//- Platform interface class. Implementing hardware functionality, such as window creation, HDI interface and
	//- processing and emitting hardware events
	//------------------------------------------------------------------------------------------------------------------------
	class iplatform
	{
	public:
		virtual ~iplatform() = default;

		virtual opresult	pre_init() = 0;		//- perform setup before os and graphics context are created
		virtual opresult	init() = 0;			//- initialize in turn after os and graphics context
		virtual void		shutdown() = 0;		//- shutdown before os and graphics context
		virtual void		post_shutdown() = 0;//- shutdown after os and graphics context

		RTTR_ENABLE();
	};

	//- Operating system interface class. Implementing hardware functionality, such as window creation, HDI interface and
	//- processing and emitting hardware events
	//------------------------------------------------------------------------------------------------------------------------
	class ios
	{
	public:
		virtual ~ios() = default;

		//- create and init of client application
		virtual opresult init() = 0;

		//- destroy and clean of client application
		virtual opresult shutdown() = 0;

		//- create graphical context
		virtual opresult init_gfx(int w, int h, bool fullscreen, bool vsync) = 0;

		//- create application main window
		virtual opresult init_mainwindow(stringview_t title, int w, int h, bool fullscreen) = 0;

		//- handle a hardware event from glfw or SDL etc.
		virtual void on_event(const rttr::variant& event) = 0;
		virtual opresult process_events() = 0;

		virtual core::smouse_state mouse_state() const = 0;
		virtual core::skeyboard_state keyboard_state() const = 0;
		virtual core::sgamepad_state gamepad_state() const = 0;

		virtual float frametime() const = 0;
		virtual unsigned read_input_character() = 0;

		virtual void main_window_position(int* x, int* y) = 0;
		virtual void main_window_size(int* x, int* y) = 0;

		virtual bool is_key_held(core::key k) const = 0;
		virtual bool is_key_pressed(core::key k) const = 0;
		virtual bool is_key_released(core::key k) const = 0;
		virtual bool is_modifier_active(int modifiers) const = 0;

		virtual void mouse_position(double* x, double* y) = 0;
		virtual void mouse_scroll_dt(double* x, double* y) = 0;
		virtual bool is_mouse_button_held(core::mouse_button b) = 0;
		virtual bool is_mouse_button_pressed(core::mouse_button b) = 0;
		virtual bool is_mouse_button_released(core::mouse_button b) = 0;

		virtual float gamepad_axis(core::gamepad_axis a) = 0;

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
