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

		//------------------------------------------------------------------------------------------------------------------------
		class cfilewriter final : public bx::FileWriter
		{
		public:
			bool open(const bx::FilePath& path, bool append, bx::Error* error) override final;
		};

		bx::FileReaderI*	filereader();
		bx::FileWriterI*	filewriter();
		bx::AllocatorI*		allocator();
		bgfx::PlatformData& platformdata();
		irenderer*			renderer();
		iplatform*			platform();
		iapp*				app();
		void				set_platform(iplatform* platform);
		void				set_app(iapp* app);
		void				set_renderer(irenderer* renderer);

	} //- entry

	//- CPU resident image representation. Can easily be copied around, as the data is managed elsewhere.
	//------------------------------------------------------------------------------------------------------------------------
	class cimage final
	{
	public:
		explicit cimage(stringview_t filepath);
		explicit cimage(void* data, unsigned size);
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

	//- GPU resident image representation. Can easily be copied around, as the data is managed elsewhere.
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final
	{
	public:
		explicit ctexture(bgfx::TextureHandle handle, bgfx::TextureInfo* info);
		explicit ctexture(const cimage& image);

		opresult load_from_image(const cimage& image);
		opresult load_from_memory(void* data, unsigned size, unsigned w, unsigned h, unsigned depth,
			bool mips, unsigned layers, texture_format_t format, uint64_t flags);

		inline bgfx::TextureInfo* info() const { return m_info; }
		inline bgfx::TextureHandle handle() const { return m_handle; }

	private:
		bgfx::TextureHandle m_handle;
		bgfx::TextureInfo* m_info;
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
		virtual opresult init_gfx(bool fullscreen, bool vsync) = 0;//- create graphical context

		virtual opresult init_mainwindow(stringview_t title,
			unsigned x, unsigned y, unsigned w, unsigned h,
			bool fullscreen) = 0;							//- create application main window

		//- TODO: does not seem to make sense, I think it was for platforms
		//- that might require processing some events and OS handshakes before
		//- the window can start and then exit, so not a real loop.
		//- Consider removing or at least renaming to be more descriptive.
		virtual opresult optional_init_event_mainloop() = 0;//- process hardware events in a loop; use where required
		virtual opresult process_event() = 0;				//- process one hardware event

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