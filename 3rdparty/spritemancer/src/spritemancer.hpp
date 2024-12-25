#pragma once
#include "detail/sm_config.hpp"
#include "detail/sm_image.hpp"
#include "detail/sm_material.hpp"
#include "detail/sm_mesh.hpp"
#include "detail/sm_program.hpp"
#include "detail/sm_rendertarget.hpp"
#include "detail/sm_shader.hpp"
#include "detail/sm_spriteatlas.hpp"
#include "detail/sm_texture.hpp"
#include "detail/sm_uniform.hpp"
#inlcude "detail/sm_renderpass.hpp"

namespace sm
{
	opresult	prepare(iapp* app, void* config);
	opresult	run(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync);
	void		set_logger(core::error_report_function_t callback);
	void		configure_args(const args_ref_t& args);

	//-- FIXME: in raylib we have only one window, so this function makes sense. When transitioning to bgfx,
	//-- we should have a window service that allows for querying of the main window size and any window size.
	vec2_t		window_size();
	vec2_t		window_position();

	//- Create a new layer for rendering. Everything rendered on this layer are drawn on a separate render target and
	//- are then drawn as a whole one previously drawn render targets (layers)
	unsigned	create_layer();

	//- Sync point of application camera data and spritemancer internal camera data used per frame drawing
	void update_frame_camera(const vec2_t& position, const vec2_t& offset, float zoom, float rotation);

    void draw_mesh(mesh_handle_t mesh_handle, material_handle_t material_handle);

} //- sm
