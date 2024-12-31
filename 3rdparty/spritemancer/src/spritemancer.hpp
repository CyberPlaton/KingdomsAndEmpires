#pragma once
#include "detail/sm_config.hpp"
#include "detail/sm_context.hpp"
#include "detail/sm_image.hpp"
#include "detail/sm_material.hpp"
#include "detail/sm_mesh.hpp"
#include "detail/sm_program.hpp"
#include "detail/sm_rendertarget.hpp"
#include "detail/sm_shader.hpp"
#include "detail/sm_spriteatlas.hpp"
#include "detail/sm_texture.hpp"
#include "detail/sm_uniform.hpp"
#include "detail/sm_renderpass.hpp"
#include "detail/imgui_integration/imgui.hpp"

namespace sm
{
	opresult	prepare(iapp* app, void* config);
	opresult	run(stringview_t title, unsigned w, unsigned h, bool fullscreen, bool vsync);
	void		set_logger(core::error_report_function_t callback);
	void		configure_args(const args_ref_t& args);

	vec2_t		window_size();
	vec2_t		window_position();

	const renderpasses_t& renderpasses();
	renderpass_ref_t renderpass_create(stringview_t type);
	renderpass_ref_t renderpass_create(rttr::type type);
	void renderpass_begin(const renderpass_ref_t& pass);
	void renderpass_end(const renderpass_ref_t& pass);
	void renderpass_reset(const renderpasses_t& pass);

	void draw_mesh(const void* mtx, mesh_handle_t mesh_handle, material_handle_t material_handle);

} //- sm
