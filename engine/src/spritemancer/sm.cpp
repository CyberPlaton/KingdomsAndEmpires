#include "sm.hpp"

namespace sm
{
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
		return internal::ctx().sm().create(technique_name.data(), vs_path.data(), ps_path.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t create_technique_embedded(const std::string& technique_name, const char* vs, const char* ps)
	{
		return internal::ctx().sm().create_embedded(technique_name.data(), vs, ps);
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t get_technique(const std::string& technique_name)
	{
		return internal::ctx().sm().get(technique_name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void unload_technique(technique_t handle)
	{
		internal::ctx().sm().unload(handle);
	}


	//------------------------------------------------------------------------------------------------------------------------
	texture_t create_texture(const std::string& texture_name, const std::string& path)
	{
		return internal::ctx().tm().create(texture_name.data(), path.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t get_texture(const std::string& texture_name)
	{
		return internal::ctx().tm().get(texture_name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void unload_texture(texture_t handle)
	{
		internal::ctx().tm().unload(handle);
	}


	//------------------------------------------------------------------------------------------------------------------------
	const raylib::Texture2D* get_gl_texture(const std::string& texture_name)
	{
		return internal::ctx().tm().native(texture_name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t create_material(const std::string& material_name, technique_t technique, blending_mode mode /*= blending_mode_alpha*/,
		blending_equation equation /*= blending_equation_blend_color*/, blending_factor src /*= blending_factor_src_color*/,
		blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
	{
		return internal::ctx().mm().create(material_name.data(), technique, mode, equation, src, dst);
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t get_material(const std::string& material_name)
	{
		return internal::ctx().mm().get(material_name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void unload_material(material_t handle)
	{
		internal::ctx().mm().unload(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	spriteatlas_t create_spriteatlas(const std::string& spriteatlas_name, const std::string& texture_path, const vec2_t& frames)
	{
		return internal::ctx().am().create(spriteatlas_name.data(), texture_path.data(), frames);
	}

	//------------------------------------------------------------------------------------------------------------------------
	spriteatlas_t get_spriteatlas(const std::string& spriteatlas_name)
	{
		return internal::ctx().am().get(spriteatlas_name.data());
	}

	//------------------------------------------------------------------------------------------------------------------------
	void unload_spriteatlas(spriteatlas_t handle)
	{
		internal::ctx().am().unload(handle);
	}

	namespace internal
	{
		//------------------------------------------------------------------------------------------------------------------------
		sm::internal::ccontext& ctx()
		{
			return ccontext::instance();
		}

	} //- internal

} //- sm