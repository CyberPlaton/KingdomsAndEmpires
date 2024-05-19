#pragma once
#include "platform.hpp"
#include <core.h>
#include <argparse.h>
#include <sdl2.h>
#include <bimg.h>
#include <bx.h>
#include <bgfx.h>
#include <plugin_logging.h>

namespace sm
{
	class irenderer;
	class iplatform;
	class iapp;
	using texture_format_t = bgfx::TextureFormat::Enum;
	using primitive_topology_t = bgfx::Topology::Enum;

	//------------------------------------------------------------------------------------------------------------------------
	struct sshader_compile_options
	{
		enum type : uint8_t
		{
			type_none = 0,
			type_vertex,
			type_fragment,
			type_compute,
		};

		enum optimization : uint8_t
		{
			optimization_none = 0,
			optimization_full,
		};

		enum platform : uint8_t
		{
			platform_none = 0,
			platform_android,
			platform_emscripten,
			platform_ios,
			platform_linux,
			platform_macos,
			platform_playstation,
			platform_windows,
		};

		enum language : uint8_t
		{
			language_none = 0,
			language_glsl,
			language_hlsl,
			language_pssl,
			language_spirv,
		};

		type m_type				= type_none;
		optimization m_optimize = optimization_none;
		platform m_platform		= platform_none;
		language m_language		= language_none;
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_mode : uint8_t
	{
		blending_mode_none = 0,
		blending_mode_default,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_equation
	{
		blending_equation_none = 0,
		blending_equation_add = BGFX_STATE_BLEND_EQUATION_ADD,
		blending_equation_sub = BGFX_STATE_BLEND_EQUATION_SUB,
	};
	
	//------------------------------------------------------------------------------------------------------------------------
	enum blending_factor
	{
		blending_factor_none = 0,
		blending_factor_zero = BGFX_STATE_BLEND_ZERO,
		blending_factor_one = BGFX_STATE_BLEND_ONE,
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


		//------------------------------------------------------------------------------------------------------------------------
		class cfilereader final : public bx::FileReader
		{
		public:
			bool open(const bx::FilePath& path, bx::Error* error) override final;
		};

		//- Note: can be use anywhere, where bx::WriterI* is required, i.e. when you want to compile shaders as binary files
		//------------------------------------------------------------------------------------------------------------------------
		class cfilewriter final : public bx::FileWriter
		{
		public:
			bool open(const bx::FilePath& path, bool append, bx::Error* error) override final;
		};

		//- Write incoming data to a string form
		//------------------------------------------------------------------------------------------------------------------------
		class cstringwriter final : public bx::WriterI
		{
		public:
			int32_t write(const void* data, int32_t size, bx::Error* error) override final;

			[[nodiscard]] inline string_t take() { return std::move(m_string); }

		private:
			string_t m_string;
		};

		bx::FileReaderI*	filereader();
		bx::FileWriterI*	filewriter();
		bx::WriterI*		stringwriter();
		bx::AllocatorI*		allocator();
		irenderer*			renderer();
		iplatform*			platform();
		iapp*				app();
		void				set_platform(ptr_t<iplatform>&& platform);
		void				set_app(iapp* app);
		void				set_renderer(ptr_t<irenderer>&& renderer);

	} //- entry

	//- Responsible for compiling a ready-to-use shader in order to be loaded to a shader object or exported to file.
	//------------------------------------------------------------------------------------------------------------------------
	class cshader_compiler final
	{
	public:
		enum shader_type : uint8_t
		{
			shader_type_none = 0,
			shader_type_vertex,
			shader_type_fragment,
			shader_type_compute,
		};

		enum shader_optimization : uint8_t
		{
			shader_optimization_none = 0,
			shader_optimization_full,
		};

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

		enum shader_language : uint8_t
		{
			shader_language_none = 0,
			shader_language_glsl,
			shader_language_hlsl,
			shader_language_pssl,
			shader_language_spirv,
		};

		cshader_compiler();
		~cshader_compiler() = default;

		cshader_compiler& type(shader_type option);
		cshader_compiler& optimization(shader_optimization option);
		cshader_compiler& platform(shader_platform option);
		cshader_compiler& language(shader_language option);

		//- If not custom varying definition file is specified, the default one is used.
		//- Here we expect an input string with the definition text and not a filepath.
		cshader_compiler& varying(stringview_t string);

		cshader_compiler& dependencies(stringview_t string);
		cshader_compiler& includes(stringview_t string);
		cshader_compiler& defines(stringview_t string);

		opresult compile();

		[[nodiscard]] core::spair<uint8_t*, unsigned> take();

	private:

	};

	//- A shader. Can be vertex, fragment, compute etc.
	//- Lightweight class that can be copied around, as a consequence it does not own the shader,
	//- when done with it you have to manually call destroy, otherwise the memory will not be freed.
	//- Note: when loading from string, a hex representation is excepted.
	//------------------------------------------------------------------------------------------------------------------------
	class cshader final
	{
	public:
		static shaderc::Options options(sshader_compile_options ops, );
		static void destroy(cshader& shader);

		explicit cshader(shader_type type, stringview_t filepath);
		explicit cshader(shader_type type, uint8_t* data, unsigned size);
		cshader();
		~cshader();

		opresult load_from_string(shader_type type, const char* string);
		opresult load_from_file(shader_type type, stringview_t filepath);

		//- Note: does not expect a null terminated string, appends it automatically
		opresult load_from_memory(shader_type type, uint8_t* data, unsigned size);

		inline bgfx::ShaderHandle handle() const { return m_handle; }
		inline shader_type type() const { return m_type; }

	private:
		bgfx::ShaderHandle m_handle;
		shader_type m_type;
	};

	//- Container for vertex and fragment shader. Destroying destroys individual shaders.
	//------------------------------------------------------------------------------------------------------------------------
	class cprogram final
	{
	public:
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

	//- CPU resident image representation. Can easily be copied around, as the data is managed elsewhere, as a
	//- consequence, you have to manually call destroy to signal that an image is no longer needed,
	//- otherwise the memory will not be freed.
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

		inline bimg::ImageContainer* image() const { return m_container; }

		//- utility functions for image generation and manipulation
		void create_solid(unsigned w, unsigned h, const core::scolor& color);
		void create_checkerboard(unsigned w, unsigned h, unsigned step, const core::scolor& first, const core::scolor& second);

	private:
		bimg::ImageContainer* m_container;
	};

	//- GPU resident image representation. Can easily be copied around, as the data is managed elsewhere, as a
	//- consequence, you have to manually call destroy to signal that a texture is no longer needed,
	//- otherwise the memory will not be freed.
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final
	{
	public:
		static void destroy(ctexture& texture);

		explicit ctexture(bgfx::TextureHandle handle, const bgfx::TextureInfo& info);
		explicit ctexture(const cimage& image);
		ctexture();
		~ctexture();


		opresult load_from_image(const cimage& image);
		opresult load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format_t format, uint64_t flags);

		inline const bgfx::TextureInfo& info() const { return m_info; }
		inline bgfx::TextureHandle handle() const { return m_handle; }

	private:
		bgfx::TextureHandle m_handle;
		bgfx::TextureInfo m_info;
	};

	//- Texture along with image data that can be used as rendertarget or as something to be rendered
	//------------------------------------------------------------------------------------------------------------------------
	struct srenderable
	{
		cimage m_image;
		ctexture m_texture;
	};

	//- The 'thing' that we actually draw; internal. Using vectors mostly for extendability
	//------------------------------------------------------------------------------------------------------------------------
	struct sdecal
	{
		vector_t<vec2_t> m_position; //- individual vertex positions
		vector_t<vec2_t> m_uv; //- individual UVs of vertices
		vector_t<float> m_w;
		vector_t<core::scolor> m_tint; //- color assigned to each vertex
		unsigned m_points = 0; //- count of vertices
		bool m_depth = false;
		blending_mode m_blending = blending_mode_default;
		primitive_topology_t m_topology = primitive_topology_t::TriList;
		bgfx::TextureHandle m_texture; //- texture to be rendered
	};

	//- Description of a layer we render upon, including all the decals to be rendered on it
	//------------------------------------------------------------------------------------------------------------------------
	struct slayer
	{
		vector_t<sdecal> m_decals;
		srenderable m_layer_target;
		core::scolor m_tint;
		vec2_t m_offset= { 0.0f, 0.0f };
		vec2_t m_scale = { 0.0f, 0.0f };
		bool m_show = false;
		bool m_want_update = false;
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
		virtual void clear(const core::scolor& color, bool depth) = 0;
		virtual void blendmode(blending_mode mode) = 0;

		virtual void bind_texture(uint64_t id) = 0;
		virtual void render_layer_quad(const vec2_t& position, const vec2_t& size, const core::scolor& color) = 0;
		virtual void render_decal(const sdecal& decal) = 0;

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