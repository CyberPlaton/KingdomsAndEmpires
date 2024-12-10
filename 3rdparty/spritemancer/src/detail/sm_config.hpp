#pragma once
#include "sm_entry.hpp"

namespace sm
{
	class irenderer;
	class iplatform;
	class iapp;
	class ios;
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

	constexpr auto C_IMAGE_RESOURCE_MANAGER_RESERVE_COUNT = 256;
	constexpr auto C_TEXTURE_RESOURCE_MANAGER_RESERVE_COUNT = 512;
	constexpr auto C_SHADER_RESOURCE_MANAGER_RESERVE_COUNT = 512;
	constexpr auto C_PROGRAM_RESOURCE_MANAGER_RESERVE_COUNT = 256;
	constexpr auto C_SPRITEATLAS_RESOURCE_MANAGER_RESERVE_COUNT = 128;
	constexpr auto C_RENDERTARGET_RESOURCE_MANAGER_RESERVE_COUNT = 64;

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

	//------------------------------------------------------------------------------------------------------------------------
	struct sblending final
	{
		bool operator==(const sblending& other);
		bool operator!=(const sblending& other);

		blending_mode m_mode;
	};

	//- A shader, a single vertex or fragment, or compute etc. shader.
	//- TODO: As of now, we can only load embedded shader types compiled externally. Each function to load a shader expects
	//- that exactly format.
	//------------------------------------------------------------------------------------------------------------------------
	class cshader final : public core::cresource
	{
	public:
		static void destroy(cshader& shader);

		explicit cshader(stringview_t filepath);
		explicit cshader(const char* string);
		explicit cshader(const uint8_t* data, unsigned size);
		cshader();
		~cshader();

		opresult load_from_file(stringview_t filepath);
		opresult load_from_string(const char* string);
		opresult load_from_memory(const uint8_t* data, unsigned size);

		inline bgfx::ShaderHandle shader() const { return m_handle; }

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
		bgfx::ShaderHandle m_handle;

		RTTR_ENABLE(core::cresource);
	};

	//- Container for vertex and fragment shader. Destroying destroys individual shaders.
	//------------------------------------------------------------------------------------------------------------------------
	class cprogram final : public core::cresource
	{
	public:
		static void destroy(cprogram& program);

		explicit cprogram(shader_handle_t shader);
		explicit cprogram(const cshader& vertex, const cshader& fragment);
		explicit cprogram(shader_handle_t vertex, shader_handle_t fragment);
		cprogram();
		~cprogram();

		opresult load_from_shader(shader_handle_t shader);
		opresult load_from_shaders(const cshader& vertex, const cshader& fragment);
		opresult load_from_handles(shader_handle_t vertex, shader_handle_t fragment);

		inline bgfx::ProgramHandle handle() const { return m_handle; }
		inline const cshader& vertex() const { return m_vertex; }
		inline const cshader& fragment() const { return m_fragment; }

	private:
		cshader m_vertex;
		cshader m_fragment;
		bgfx::ProgramHandle m_handle;

		RTTR_ENABLE(core::cresource);
	};

	//- CPU resident image representation
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

		inline const bimg::ImageContainer* image() const { return m_container; }

		//- utility functions for image generation and manipulation
		void create_solid(unsigned w, unsigned h, const core::scolor& color);
		void create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second);

	private:
		bimg::ImageContainer* m_container;

		RTTR_ENABLE(core::cresource);
	};

	//- GPU resident image representation.
	//- TODO: information does not need to be held by each texture. Store it somewhere else and in texture only the handle.
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final : public core::cresource
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

		inline unsigned w() const { return SCAST(unsigned, m_width); }
		inline unsigned h() const { return SCAST(unsigned, m_height); }
		inline bgfx::FrameBufferHandle target() const { return m_framebuffer; }
		inline bgfx::TextureHandle texture() const { return m_texture; }

	private:
		bgfx::FrameBufferHandle m_framebuffer;
		bgfx::TextureHandle m_texture;
		uint16_t m_width;
		uint16_t m_height;

		RTTR_ENABLE(core::cresource);
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