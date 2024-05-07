#pragma once
#include "platform.hpp"
#include <core.h>
#include <sdl2.h>
#include <bimg.h>
#include <bx.h>
#include <plugin_logging.h>

namespace sm
{
	class cimage;
	class ctexture;
	using texture_format = bimg::TextureFormat::Enum;

	//------------------------------------------------------------------------------------------------------------------------
	enum opresult : uint8_t
	{
		opresult_ok = 0,
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

	//- Texture along with image data that can be used as rendertarget.
	//------------------------------------------------------------------------------------------------------------------------
	class crenderable final : core::cnon_copyable
	{
	public:
		crenderable() = default;
		crenderable(crenderable&& other) :
			m_image(std::move(other.m_image)), m_texture(std::move(other.m_texture))
		{
		}

		opresult load();
		void create();

		inline const cimage* image() const { return m_image.get(); }
		inline const ctexture* texture() const { return m_texture.get(); }

	private:
		ptr_t<cimage> m_image;
		ptr_t<ctexture> m_texture;
	};

	//- The 'thing' that we actually draw; internal
	//------------------------------------------------------------------------------------------------------------------------
	struct sdecal
	{
		vector_t<vec2_t> m_position;
		vector_t<vec2_t> m_uv;
		vector_t<float> m_w;
		vector_t<core::scolor> m_tint;
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
		virtual opresult init_device() = 0;						//- create and init GPU device
		virtual opresult shutdown_device() = 0;					//- destroy and clean GPU device
		virtual void prepare_frame() = 0;						//- prepare to start drawing
		virtual void display_frame() = 0;						//- present current frame to window
		virtual void set_blending_mode(blending_mode mode) = 0;	//- set blending mode to render with
		virtual unsigned create_texture() = 0;					//- load a texture on GPU
		virtual unsigned destroy_texture() = 0;					//- unload a texture from GPU
		virtual void draw_decal(const sdecal& decal) = 0;		//- draw a GPU texture
		virtual void bind_texture(unsigned id) = 0;				//- bind texture resource for use on GPU as render target

		virtual void update_texture(unsigned id,
			unsigned w, unsigned h, void* data) = 0;			//- update texture resource on GPU with new image data

		virtual void read_texture(unsigned id,
			unsigned w, unsigned h, void* data) = 0;			//- update image data with data from GPU texture resource

		virtual void clear(core::scolor tint, bool depth) = 0;	//- clear render target

		virtual void update_viewport(unsigned x, unsigned y,
			unsigned w, unsigned h) = 0;						//- update viewport dimension
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
		virtual opresult init_thread() = 0;					//- create and init internal 'engine' thread
		virtual opresult shutdown_thread() = 0;				//- destroy and clean internal 'engine' thread
		virtual opresult init_gfx() = 0;					//- create graphical context
		virtual opresult init_mainwindow() = 0;				//- create application main window
		virtual opresult optional_init_event_mainloop() = 0;//- process hardware events in a loop; use where required
		virtual opresult process_event() = 0;				//- process one hardware event
	};

} //- sm