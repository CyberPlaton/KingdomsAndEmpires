#pragma once
#include "sm_renderer.hpp"

namespace sm
{
	//- startup and shutdown of the library.
	bool init(cwindow::sconfig& cfg);
	void shutdown();

	//- render functions.
	//- order of declaration is expected order of calling
	void begin_drawing(ccamera* camera);
	void end_frame();
	void ui_frame();
	void end_ui_frame();
	void end_drawing();

	//- render utility
	void begin_default_render_target();
	void begin_default_blend_mode();
	void begin_blend_mode(blending_mode mode);
	void end_blend_mode();
	void begin_render_target(render_target_t& texture);
	void end_render_target(raylib::Shader combine_technique = { 0 });

	//- texture and shader program management shortcuts
	technique_t create_technique(const std::string& technique_name, const std::string& vs_path, const std::string& ps_path);
	technique_t create_technique_embedded(const std::string& technique_name, const char* vs, const char* ps);
	technique_t get_technique(const std::string& technique_name);
	void unload_technique(technique_t handle);
	texture_t create_texture(const std::string& texture_name, const std::string& path);
	texture_t get_texture(const std::string& texture_name);
	void unload_texture(texture_t handle);
	const raylib::Texture2D* get_gl_texture(const std::string& texture_name);
	material_t create_material(const std::string& material_name, technique_t technique, blending_mode mode = blending_mode_alpha, blending_equation equation = blending_equation_blend_color, blending_factor src = blending_factor_src_color, blending_factor dst = blending_factor_one_minus_src_alpha);
	material_t get_material(const std::string& material_name);
	void unload_material(material_t handle);
	spriteatlas_t create_spriteatlas(const std::string& spriteatlas_name, const std::string& texture_path, const vec2_t& frames);
	spriteatlas_t get_spriteatlas(const std::string& spriteatlas_name);
	void unload_spriteatlas(spriteatlas_t handle);

	//- utilities and general functions
	bool is_technique_ready(technique_t technique);
	bool is_texture_ready(texture_t texture);
	bool is_material_ready(material_t material);
	bool is_spriteatlas_ready(spriteatlas_t atlas);

	namespace internal
	{
		ccontext& ctx();

	} //- internal

} //- sm