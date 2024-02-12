#include "sm.hpp"

namespace sm
{
	constexpr material_t C_DEFAULT_MATERIAL = 0;
	inline static const core::scolor C_COLOR_WHITE = { 255, 255, 255, 255 };
	inline static const core::srect C_FULL_SCALE_TEXTURE = { 0.0f, 0.0f, 1.0f, 1.0f };

	//------------------------------------------------------------------------------------------------------------------------
	bool init(cwindow::sconfig& cfg)
	{
		return internal::ctx().init(cfg);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void shutdown()
	{
		internal::ctx().shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void begin_drawing(ccamera* camera)
	{
		internal::ctx().on_begin_drawing(camera);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void end_frame()
	{
		internal::ctx().on_frame_end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ui_frame()
	{
		internal::ctx().on_ui_frame();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void end_ui_frame()
	{
		internal::ctx().on_ui_frame_end();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void end_drawing()
	{
		internal::ctx().on_end_drawing();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void begin_default_render_target()
	{
		internal::ctx().begin_default_render_target();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void begin_default_blend_mode()
	{
		internal::ctx().begin_default_blend_mode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void begin_blend_mode(blending_mode mode)
	{
		internal::ctx().begin_blend_mode(mode);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void end_blend_mode()
	{
		internal::ctx().end_blend_mode();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void begin_render_target(render_target_t& texture)
	{
		internal::ctx().begin_render_target(texture);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void end_render_target(raylib::Shader combine_technique /*= { 0 }*/)
	{
		internal::ctx().end_render_target(combine_technique);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const mat4_t& transform, texture_t texture, material_t material, const core::srect& rect,
		const core::scolor& color, bool flipx, bool flipy)
	{
		internal::ctx().renderer().draw_sprite(layer, transform, texture, material, rect, color, flipx, flipy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const mat4_t& transform, texture_t texture, material_t material, const core::srect& rect,
		const core::scolor& color)
	{
		draw_sprite(layer, transform, texture, material, rect, color, false, false);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const mat4_t& transform, texture_t texture, material_t material, const core::srect& rect)
	{
		draw_sprite(layer, transform, texture, material, rect, C_COLOR_WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const mat4_t& transform, texture_t texture, material_t material)
	{
		draw_sprite(layer, transform, texture, material, C_FULL_SCALE_TEXTURE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const mat4_t& transform, texture_t texture)
	{
		draw_sprite(layer, transform, texture, C_DEFAULT_MATERIAL);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const vec2_t& position, texture_t texture, material_t material, float rotation, const vec2_t& scale,
		const core::srect& rect, const core::scolor& color, bool flipx, bool flipy)
	{
		internal::ctx().renderer().draw_sprite(layer, position, rotation, scale, texture, material, rect, color, flipx, flipy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const vec2_t& position, texture_t texture, material_t material, float rotation, const vec2_t& scale,
		const core::srect& rect, const core::scolor& color)
	{
		draw_sprite(layer, position, texture, material, rotation, scale, rect, color, false, false);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const vec2_t& position, texture_t texture, material_t material, float rotation, const vec2_t& scale,
		const core::srect& rect)
	{
		draw_sprite(layer, position, texture, material, rotation, scale, rect, C_COLOR_WHITE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const vec2_t& position, texture_t texture, material_t material, float rotation, const vec2_t& scale)
	{
		draw_sprite(layer, position, texture, material, rotation, scale, C_FULL_SCALE_TEXTURE);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const vec2_t& position, texture_t texture, material_t material, float rotation)
	{
		draw_sprite(layer, position, texture, material, rotation, {1.0f, 1.0f});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const vec2_t& position, texture_t texture, material_t material)
	{
		draw_sprite(layer, position, texture, material, 0.0f);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite(unsigned char layer, const vec2_t& position, texture_t texture)
	{
		draw_sprite(layer, position, texture, C_DEFAULT_MATERIAL);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite_atlas_sprite(unsigned char layer, const vec2_t& position, float rotation, const vec2_t& scale, spriteatlas_t atlas,
		subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy)
	{
		internal::ctx().renderer().draw_sprite_atlas_sprite(layer, position, rotation, scale, atlas, subtexture, color, flipx, flipy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void draw_sprite_atlas_sprite(unsigned char layer, const mat4_t& transform, spriteatlas_t atlas, subtexture_t subtexture,
		const core::scolor& color, bool flipx, bool flipy)
	{
		internal::ctx().renderer().draw_sprite_atlas_sprite(layer, transform, atlas, subtexture, color, flipx, flipy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_technique_ready(technique_t technique)
	{
		return algorithm::is_valid_handle(technique);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_texture_ready(texture_t texture)
	{
		return algorithm::is_valid_handle(texture);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_material_ready(material_t material)
	{
		return algorithm::is_valid_handle(material);
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool is_spriteatlas_ready(spriteatlas_t atlas)
	{
		return algorithm::is_valid_handle(atlas);
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t create_technique(const std::string& technique_name, const std::string& vs_path, const std::string& ps_path)
	{
		return internal::ctx().renderer().create_technique(technique_name, vs_path, ps_path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t create_technique_embedded(const std::string& technique_name, const char* vs, const char* ps)
	{
		return internal::ctx().renderer().create_technique_embedded(technique_name, vs, ps);
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t get_technique(const std::string& technique_name)
	{
		return internal::ctx().renderer().get_technique(technique_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void unload_technique(technique_t handle)
	{
		internal::ctx().renderer().unload_technique(handle);
	}


	//------------------------------------------------------------------------------------------------------------------------
	texture_t create_texture(const std::string& texture_name, const std::string& path)
	{
		return internal::ctx().renderer().create_texture(texture_name, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t get_texture(const std::string& texture_name)
	{
		return internal::ctx().renderer().get_texture(texture_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void unload_texture(texture_t handle)
	{
		internal::ctx().renderer().unload_texture(handle);
	}


	//------------------------------------------------------------------------------------------------------------------------
	const raylib::Texture2D* get_gl_texture(const std::string& texture_name)
	{
		return internal::ctx().renderer().get_gl_texture(texture_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t create_material(const std::string& material_name, technique_t technique, blending_mode mode /*= blending_mode_alpha*/,
		blending_equation equation /*= blending_equation_blend_color*/, blending_factor src /*= blending_factor_src_color*/,
		blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
	{
		return internal::ctx().renderer().create_material(material_name, technique, mode, equation, src, dst);
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t get_material(const std::string& material_name)
	{
		return internal::ctx().renderer().get_material(material_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void unload_material(material_t handle)
	{
		internal::ctx().renderer().unload_material(handle);
	}


	//------------------------------------------------------------------------------------------------------------------------
	spriteatlas_t create_spriteatlas(const std::string& spriteatlas_name, const std::string& texture_path, const vec2_t& frames)
	{
		return internal::ctx().renderer().create_spriteatlas(spriteatlas_name, texture_path, frames);
	}

	//------------------------------------------------------------------------------------------------------------------------
	spriteatlas_t get_spriteatlas(const std::string& spriteatlas_name)
	{
		return internal::ctx().renderer().get_spriteatlas(spriteatlas_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void unload_spriteatlas(spriteatlas_t handle)
	{
		internal::ctx().renderer().unload_spriteatlas(handle);
	}


	namespace internal
	{
		//------------------------------------------------------------------------------------------------------------------------
		sm::internal::ccontext& ctx()
		{
			return ccontext::instance();
		}

	}
}