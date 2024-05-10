#pragma once
#include "platform.hpp"
#include <core.h>
#include <argparse.h>
#include <sdl2.h>
#include <bimg.h>
#include <bx.h>
#include <plugin_logging.h>

namespace sm
{
	class irenderer;
	class iplatform;
	class iapp;
	class cimage;
	class ctexture;

	//------------------------------------------------------------------------------------------------------------------------
	enum opresult : uint8_t
	{
		opresult_ok = 0,
		opresult_fail_platform_init,
		opresult_fail_renderer_init,
		opresult_fail = 255,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum blending_mode : uint8_t
	{
		blending_mode_none = 0,
		blending_mode_normal,
		blending_mode_additive,
		blending_mode_multiply,
		blending_mode_stencil,
		blending_mode_illuminate,
	};

	//------------------------------------------------------------------------------------------------------------------------
	enum topology_type : uint8_t
	{
		topology_type_none = 0,
		topology_type_line,
		topology_type_fan,
		topology_type_strip,
		topology_type_list,
	};

	namespace entry
	{
		//------------------------------------------------------------------------------------------------------------------------
		class ifilereader : public bx::FileReader
		{
		public:
		};

		//------------------------------------------------------------------------------------------------------------------------
		class ifilewriter : public bx::FileWriter
		{
		public:
		};

		//- Interface class for loading any image data
		//------------------------------------------------------------------------------------------------------------------------
		class iimage_loader
		{
		public:
			virtual ~iimage_loader() = default;

			virtual opresult from_file(bimg::ImageContainer* &container, stringview_t filepath,
				bimg::TextureFormat::Enum format) = 0;

			virtual opresult create(bimg::ImageContainer*& container, unsigned w, unsigned h, const core::scolor& tint,
				bimg::TextureFormat::Enum format) = 0;

			virtual void free(bimg::ImageContainer*& container) = 0;
		};

		//- Interface class for loading shaders
		//------------------------------------------------------------------------------------------------------------------------
		class ishader_loader
		{
		public:
			virtual ~ishader_loader() = default;
		};

		//- set these to required implementations before first use of functions below, otherwise default implementations are used.
		//- Except for renderer and platform, those do not have defaults and must be set
		//------------------------------------------------------------------------------------------------------------------------
		struct sdata
		{
			STATIC_INSTANCE_EX(sdata);

			ptr_t<ifilereader> S_FILEREADER			= nullptr;
			ptr_t<ifilewriter> S_FILEWRITER			= nullptr;
			ptr_t<bx::AllocatorI> S_ALLOCATOR		= nullptr;
			ptr_t<iimage_loader> S_IMAGE_LOADER		= nullptr;
			ptr_t<ishader_loader> S_SHADER_LOADER	= nullptr;
			ptr_t<irenderer> S_RENDERER				= nullptr;
			ptr_t<iplatform> S_PLATFORM				= nullptr;
			iapp* S_APP								= nullptr;
		};

		ifilereader*	file_reader();
		ifilewriter*	file_writer();
		bx::AllocatorI*	allocator();
		iimage_loader*	image_loader();
		ishader_loader* shader_loader();
		irenderer*		renderer();
		iplatform*		platform();
		iapp*			app();

		//------------------------------------------------------------------------------------------------------------------------
		class cfilereader final : public ifilereader
		{
		public:
			bool open(const bx::FilePath& path, bx::Error* error) override final;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cfilewriter final : public ifilewriter
		{
		public:
			bool open(const bx::FilePath& path, bool append, bx::Error* error) override final;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cimage_loader final : public iimage_loader
		{
		public:
			opresult from_file(bimg::ImageContainer*& container, stringview_t filepath, bimg::TextureFormat::Enum format) override final;

			opresult create(bimg::ImageContainer*& container, unsigned w, unsigned h, const core::scolor& tint,
				bimg::TextureFormat::Enum format) override final;

			void free(bimg::ImageContainer*& container) override final;
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cshader_loader final : public ishader_loader
		{
		public:
		};

	} //- entry

	//------------------------------------------------------------------------------------------------------------------------
	struct serror_reporter
	{
		STATIC_INSTANCE_EX(serror_reporter);

		core::error_report_function_t m_callback = nullptr;
	};

	//- CPU side image data
	//------------------------------------------------------------------------------------------------------------------------
	class cimage final
	{
	public:
		cimage() = default;
		~cimage();

		opresult load_from_file(stringview_t filepath, bimg::TextureFormat::Enum format = bimg::TextureFormat::RGBA8);
		opresult create(unsigned w, unsigned h, bimg::TextureFormat::Enum format = bimg::TextureFormat::RGBA8);

	public:
		bimg::ImageContainer* m_container = nullptr;
	};

	//- GPU side image data
	//------------------------------------------------------------------------------------------------------------------------
	class ctexture final
	{
	public:
		ctexture() = default;

		inline vec2_t uv() const { return m_uv; }
		inline unsigned id() const { return m_id; }

	public:
		//- TODO: decide whether this is a scale value or in pixel size
		vec2_t m_uv = { 1.0f, 1.0f };
		unsigned m_id = MAX(unsigned);
	};

	//- Texture along with image data that can be used as rendertarget
	//------------------------------------------------------------------------------------------------------------------------
	class crenderable final : core::cnon_copyable
	{
	public:
		crenderable() = default;
		crenderable(crenderable&& other) :
			m_image(std::move(other.m_image)), m_texture(std::move(other.m_texture))
		{
		}

		opresult load(stringview_t filepath, bool filtering = false, bool clamped = false, bimg::TextureFormat::Enum format = bimg::TextureFormat::RGBA8);
		void create(unsigned w, unsigned h, bool filtering = false, bool clamped = false, bimg::TextureFormat::Enum format = bimg::TextureFormat::RGBA8);

		inline const cimage& image() const { return m_image; }
		inline const ctexture& texture() const { return m_texture; }

	private:
		cimage m_image;
		ctexture m_texture;
	};

	//- The 'thing' that we actually draw; internal
	//------------------------------------------------------------------------------------------------------------------------
	struct sdecal
	{
		vector_t<vec2_t> m_position;
		vector_t<vec2_t> m_uv;
		vector_t<float> m_w;
		vector_t<ivec4_t> m_tint;
		unsigned m_points = 0;
		bool m_depth = false;
		blending_mode m_blending = blending_mode_normal;
		topology_type m_topology = topology_type_fan;
		ctexture* m_texture = nullptr;
	};

	//- Description of a layer we render upon, including all the decals to be rendered on it
	//------------------------------------------------------------------------------------------------------------------------
	struct slayer
	{
		vector_t<sdecal> m_decals;
		crenderable m_target;
		core::scolor m_tint;
		vec2_t m_offset= { 0.0f, 0.0f };
		vec2_t m_scale = { 0.0f, 0.0f };
		bool m_show = false;
		bool m_want_update = false;
		unsigned m_id = 0;
	};

	//- Renderer interface class. Implementing API functionality, such as rendering primitives, loading resources
	//- such as shaders and textures
	//------------------------------------------------------------------------------------------------------------------------
	class irenderer
	{
	public:
		virtual ~irenderer() = default;

		virtual void prepare_device() = 0;						//- prepare for init of GPU device
		virtual opresult init_device(void* nwh, bool fullscreen, bool vsync) = 0;//- create and init GPU device etc.
		virtual opresult shutdown_device() = 0;					//- destroy and clean GPU device
		virtual void prepare_frame() = 0;						//- prepare to start drawing
		virtual void display_frame() = 0;						//- present current frame to window
		virtual void set_blending_mode(blending_mode mode) = 0;	//- set blending mode to render with
		virtual unsigned create_texture(unsigned w, unsigned h,
			bool filtering, bool clamping) = 0;					//- load a texture on GPU
		virtual unsigned destroy_texture() = 0;					//- unload a texture from GPU

		virtual void draw_layer_quad(unsigned x, unsigned y,
			unsigned w, unsigned h, core::scolor tint) = 0;		//- draw layer quad. Basically clearing layer with a color

		virtual void draw_decal(const sdecal& decal) = 0;		//- draw a GPU texture
		virtual void bind_texture(unsigned id) = 0;				//- bind texture resource for use on GPU as render target

		virtual void update_texture(unsigned id,
			unsigned w, unsigned h, void* data) = 0;			//- update texture resource on GPU with new image data

		virtual void update_image(unsigned id,
			unsigned w, unsigned h, void* data) = 0;			//- update image data with data from GPU texture resource

		virtual void clear(core::scolor tint, bool depth) = 0;	//- clear render target

		virtual void update_viewport(unsigned x, unsigned y,
			unsigned w, unsigned h) = 0;						//- update viewport dimension

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
		virtual opresult init_gfx(bool fullscreen, bool vsync) = 0;//- create graphical context

		virtual opresult init_mainwindow(stringview_t title,
			unsigned x, unsigned y, unsigned w, unsigned h,
			bool fullscreen) = 0;							//- create application main window

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