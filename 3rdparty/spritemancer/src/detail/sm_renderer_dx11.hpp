#pragma once
#include "sm_config.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	class crenderer_dx final : public irenderer
	{
	public:
		crenderer_dx() = default;
		~crenderer_dx() = default;

		void prepare_device() override final;						//- prepare for init of GPU device
		opresult init_device(void* nwh, bool fullscreen, bool vsync) override final;//- create and init GPU device
		opresult shutdown_device() override final;					//- destroy and clean GPU device
		void prepare_frame() override final;						//- prepare to start drawing
		void display_frame() override final;						//- present current frame to window
		void set_blending_mode(blending_mode mode) override final;	//- set blending mode to render with
		unsigned create_texture(unsigned w, unsigned h,
			bool filtering, bool clamping) override final;			//- load a texture on GPU
		unsigned destroy_texture() override final;					//- unload a texture from GPU

		void draw_layer_quad(unsigned x, unsigned y,
			unsigned w, unsigned h, core::scolor tint) override final;//- draw layer quad. Basically clearing layer with a color

		void draw_decal(const sdecal& decal) override final;		//- draw a GPU texture
		void bind_texture(unsigned id) override final;				//- bind texture resource for use on GPU as render target

		void update_texture(unsigned id,
			unsigned w, unsigned h, void* data) override final;		//- update texture resource on GPU with new image data

		void update_image(unsigned id,
			unsigned w, unsigned h, void* data) override final;		//- update image data with data from GPU texture resource

		void clear(core::scolor tint, bool depth) override final;	//- clear render target

		void update_viewport(unsigned x, unsigned y,
			unsigned w, unsigned h) override final;					//- update viewport dimension
	};

} //- sm