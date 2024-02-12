#include "sm_renderer.hpp"
#include "sm_embedded_shaders.hpp"

namespace sm
{
	namespace
	{

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	crenderer::crenderer() :
		m_camera(nullptr), m_spriteatlas_manager(*this)
	{
		//- create default technique and default material
		auto program = m_technique_manager.create_technique_embedded("sprite", programs::pixelperfect::s_vs, programs::pixelperfect::s_ps);
		ASSERT(algorithm::is_valid_handle(program), "Failed creating embedded default shader");

		m_material_manager.create_default_material(program, blending_mode_alpha, blending_equation_blend_color,
			blending_factor_src_color, blending_factor_one_minus_src_alpha);
	}

	//------------------------------------------------------------------------------------------------------------------------
	crenderer::~crenderer()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::sort_and_submit()
	{
		vec3_t _position, _rotation, _scale;
		raylib::Rectangle src, dst;

		//- this does the actual rendering through raylib
		for (const auto& pair : m_draw_commands)
		{
			//- TODO: shader should be set if it is not the same as the previous one,
			//- the same goes for the blend mode,
			//- the dynamic uniforms should be set everytime.
			//- For this we have to implement sorting by material first.
			for (const auto& dc : pair.second)
			{
				const auto& _material = m_material_manager.get_internal_material(dc.m_material);
				ASSERT(algorithm::is_valid_handle(_material.get_shader()), "Technique handle invalid");
				const auto& technique = m_technique_manager.raylib_shader(_material.get_shader());
				const auto& texture = m_texture_manager.get_raylib_texture(dc.m_texture);
				const auto& _transform = m_sprite_matrices[dc.m_index];
				const auto& _rect = m_sprite_rects[dc.m_index];
				const auto& _color = m_sprite_colors[dc.m_index];

				algorithm::decompose_translation(_transform, _position);
				algorithm::decompose_rotation(_transform, _rotation);
				algorithm::decompose_scale(_transform, _scale);

				_material.bind_dynamic_uniforms(technique);
				_material.bind_shader(technique);
				_material.bind_blend_mode();

				src = { _rect.x(), _rect.y() , _rect.w() , _rect.h() };
				dst = {_position.x, _position.y,
						src.width, src.height };

				//- TODO: the material should not have a pointer to the texture,
				//- he should have a handle.
				DrawTexturePro(texture, src, dst, { 0.0f , 0.0f },
					_rotation.z, { _color.r(), _color.r() , _color.r() , _color.r() });

				_material.end_blend_mode();
				_material.end_shader();
			}
		}

		m_draw_commands.clear();
		m_sprite_matrices.clear();
		m_sprite_rects.clear();
		m_sprite_colors.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::touch()
	{
		ASSERT(m_draw_commands.empty() && m_sprite_matrices.empty() &&
			m_sprite_rects.empty() && m_sprite_colors.empty(),
			"Drawcommands are not empty, you probably forgot to call submit after drawing");

		m_draw_commands.clear();
		m_sprite_matrices.clear();
		m_sprite_rects.clear();
		m_sprite_colors.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(unsigned layer, const vec2_t& position, float rotation, const vec2_t& scale, texture_t texture, material_t material,
		const core::srect& rect, const core::scolor& color, bool flipx, bool flipy)
	{
		const mat4_t transform = glm::translate(mat4_t(1.0f), vec3_t(position, 0.0f)) *
			glm::rotate(mat4_t(1.0f), glm::radians(rotation), vec3_t(0.0f, 0.0f, 1.0f)) *
			glm::scale(mat4_t(1.0f), vec3_t(scale, 1.0f));

		draw_sprite(layer, transform, texture, material, rect, color, flipx, flipy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite(unsigned layer, const mat4_t& transform, texture_t texture, material_t material, const core::srect& rect,
		const core::scolor& color, bool flipx, bool flipy)
	{
		core::cscope_mutex m(m_mutex);

		//- create or get layer
		auto& dclayer = m_draw_commands[layer];

		auto& dc = dclayer.emplace_back();
		//- index used is the same for rectangle, transform and color
		dc.m_index = m_sprite_matrices.size();
		m_sprite_matrices.push_back(transform);
		m_sprite_colors.push_back(color);
		dc.m_material = material;
		dc.m_texture = texture;
		auto& _rect = m_sprite_rects.emplace_back(rect);
		if (flipx)
		{
			_rect.m_w = -_rect.m_w;
		}
		if (flipy)
		{
			_rect.m_h = -_rect.m_h;
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite_atlas_sprite(unsigned layer, const vec2_t& position, float rotation, const vec2_t& scale, spriteatlas_t atlas,
		subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy)
	{
		const mat4_t transform = glm::translate(mat4_t(1.0f), vec3_t(position, 0.0f)) *
			glm::rotate(mat4_t(1.0f), glm::radians(rotation), vec3_t(0.0f, 0.0f, 1.0f)) *
			glm::scale(mat4_t(1.0f), vec3_t(scale, 1.0f));

		draw_sprite_atlas_sprite(layer, transform, atlas, subtexture, color, flipx, flipy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::draw_sprite_atlas_sprite(unsigned layer, const mat4_t& transform, spriteatlas_t atlas, subtexture_t subtexture, const core::scolor& color, bool flipx, bool flipy)
	{
		const auto& __atlas = m_spriteatlas_manager.get_internal_spriteatlas(atlas);
		draw_sprite(layer, transform, __atlas.atlas(), m_material_manager.default_material(), __atlas.subtexture(subtexture), color, flipx, flipy);
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t crenderer::get_material(const std::string& material_name) const
	{
		return m_material_manager.get_material(material_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::unload_material(material_t handle)
	{
		m_material_manager.unload_material(handle);
	}


	//------------------------------------------------------------------------------------------------------------------------
	texture_t crenderer::create_texture(const std::string& texture_name, const std::string& path)
	{
		return m_texture_manager.create_texture(texture_name, path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	texture_t crenderer::get_texture(const std::string& texture_name) const
	{
		return m_texture_manager.get_texture(texture_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::unload_texture(texture_t handle)
	{
		m_texture_manager.unload_texture(handle);
	}

	//------------------------------------------------------------------------------------------------------------------------
	const raylib::Texture2D* crenderer::get_gl_texture(const std::string& texture_name) const
	{
		return m_texture_manager.get_gl_texture(texture_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t crenderer::create_technique(const std::string& technique_name, const std::string& vs_path, const std::string& ps_path)
	{
		return m_technique_manager.create_technique(technique_name, vs_path, ps_path);
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t crenderer::create_technique_embedded(const std::string& technique_name, const char* vs, const char* ps)
	{
		return m_technique_manager.create_technique_embedded(technique_name, vs, ps);
	}

	//------------------------------------------------------------------------------------------------------------------------
	technique_t crenderer::get_technique(const std::string& technique_name)
	{
		return m_technique_manager.get_technique(technique_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::unload_technique(technique_t handle)
	{
		m_technique_manager.unload_technique(handle);
	}


	//------------------------------------------------------------------------------------------------------------------------
	spriteatlas_t crenderer::create_spriteatlas(const std::string& spriteatlas_name, const std::string& texture_path, const vec2_t& frames)
	{
		return m_spriteatlas_manager.create_spriteatlas(spriteatlas_name, texture_path, frames);
	}

	//------------------------------------------------------------------------------------------------------------------------
	spriteatlas_t crenderer::get_spriteatlas(const std::string& spriteatlas_name) const
	{
		return m_spriteatlas_manager.get_spriteatlas(spriteatlas_name);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void crenderer::unload_spriteatlas(spriteatlas_t handle)
	{
		m_spriteatlas_manager.unload_spriteatlas(handle);
	}


	//------------------------------------------------------------------------------------------------------------------------
	core::srect crenderer::get_subtexture(spriteatlas_t atlas, const std::string& subtexture_name)
	{
		auto h = algorithm::hash(subtexture_name.c_str());
		const auto& spriteatlas = m_spriteatlas_manager.get_internal_spriteatlas(h);
		return spriteatlas.subtexture(h);
	}

	//------------------------------------------------------------------------------------------------------------------------
	vec2_t crenderer::get_texture_size(texture_t texture)
	{
		const auto& tex = m_texture_manager.get_raylib_texture(texture);

		return { (float)tex.width, (float)tex.height };
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool crenderer::load_default_and_embedded_shaders()
	{
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	material_t crenderer::create_material(const std::string& material_name, technique_t technique,
		blending_mode mode /*= blending_mode_alpha*/, blending_equation equation /*= blending_equation_blend_color*/,
		blending_factor src /*= blending_factor_src_color*/, blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
	{
		return m_material_manager.create_material(material_name, technique, mode, equation, src, dst);
	}

// 	bool sort_draw_command_by_technique(const sdraw_command& a, const sdraw_command& b)
// 	{
// 		return a.m_material.m_technique.idx < b.m_material.m_technique.idx;
// 	}
// 
// 	bool sort_draw_command_by_texture(const sdraw_command& a, const sdraw_command& b)
// 	{
// 		return a.m_material.m_texture.idx < b.m_material.m_texture.idx;
// 	}
// 
// 	void sort_draw_commands(vector_t< sdraw_command >& dc)
// 	{
// 		sort_draw_commands(dc, 0, dc.size(), sorting_criteria_technique);
// 
// 		auto x = 0, y = 0;
// 		for (auto ii = 0; ii < dc.size(); ii++)
// 		{
// 			if (ii + 1 == dc.size())
// 			{
// 				sort_draw_commands(dc, x, dc.size(), sorting_criteria_texture);
// 			}
// 			else if (dc[ii].m_material.m_technique.idx != dc[ii + 1].m_material.m_technique.idx)
// 			{
// 				y = ii + 1;
// 				sort_draw_commands(dc, x, y, sorting_criteria_texture);
// 				x = y;
// 			}
// 		}
// 	}
// 
// 	void sort_draw_commands(vector_t< sdraw_command >& dc, u32 begin, u32 end, sorting_criteria criteria)
// 	{
// 		switch (criteria)
// 		{
// 		case sorting_criteria_technique:
// 		{
// 			std::sort(dc.begin() + begin, dc.begin() + end, sort_draw_command_by_technique);
// 			break;
// 		}
// 		case sorting_criteria_texture:
// 		{
// 			std::sort(dc.begin() + begin, dc.begin() + end, sort_draw_command_by_texture);
// 			break;
// 		}
// 		}
// 	}

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		ctexture_manager::ctexture_manager() :
			m_fragmented(false), m_fragmentation_count(0), m_texture_count(0)
		{

		}

		//------------------------------------------------------------------------------------------------------------------------
		ctexture_manager::~ctexture_manager()
		{
			for(const auto& tex : m_texture_manager)
			{
				if(!tex.m_removed)
				{
					UnloadTexture(tex.m_texture);
				}
			}
			m_texture_manager.clear();
			m_texture_names.clear();
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ctexture_manager::unload_texture(texture_t handle)
		{
			raylib::UnloadTexture(m_texture_manager[handle].m_texture);

			m_texture_manager[handle].m_removed = true;

			algorithm::erase_if(m_texture_names, [&](auto it)
				{
					return it->second == handle;
				});

			--m_texture_count;
			++m_fragmentation_count;
			m_fragmented = true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		texture_t ctexture_manager::create_texture(const std::string& texture_name, const std::string& path)
		{
			auto texture = raylib::LoadTexture(path.c_str());

			if (raylib::IsTextureReady(texture))
			{
				if (m_fragmented)
				{
					//- 
					for (auto ii = 0; ii < m_texture_manager.size(); ++ii)
					{
						auto& tex = m_texture_manager[ii];

						if (tex.m_removed)
						{
							tex.m_texture = std::move(texture);
							tex.m_removed = false;
							m_fragmentation_count = (m_fragmentation_count - 1 < 0) ? 0 : m_fragmentation_count - 1;

							//- 
							if (m_fragmentation_count == 0)
							{
								m_fragmented = false;
							}

							return ii;
						}
					}
				}
				else
				{
					texture_t h = m_texture_count++;
					auto hash = algorithm::hash(texture_name.c_str());

					m_texture_names[hash] = h;
					m_texture_manager.emplace_back(texture);

					return h;
				}
			}
			return invalid_handle_t;
		}

		//------------------------------------------------------------------------------------------------------------------------
		texture_t ctexture_manager::get_texture(const std::string& texture_name) const
		{
			auto h = algorithm::hash(texture_name.c_str());
			return m_texture_names.at(h);
		}

		//------------------------------------------------------------------------------------------------------------------------
		const raylib::Texture2D* ctexture_manager::get_gl_texture(const std::string& texture_name) const
		{
			auto tex = get_texture(texture_name);
			return &m_texture_manager.at(tex).m_texture;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const raylib::Texture2D& ctexture_manager::get_raylib_texture(texture_t handle) const
		{
			ASSERT(m_texture_manager[handle].m_removed == false, "Trying to access an unloaded texture");
			return m_texture_manager[handle].m_texture;
		}

		//------------------------------------------------------------------------------------------------------------------------
		ctechnique_manager::ctechnique_manager() :
			m_technique_count(0), m_fragmented(false), m_fragmentation_count(0)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		ctechnique_manager::~ctechnique_manager()
		{
			for(const auto& tech : m_technique_manager)
			{
				if(!tech.m_removed)
				{
					UnloadShader(tech.m_shader);
				}
			}
			m_technique_manager.clear();
			m_technique_names.clear();
		}

		//------------------------------------------------------------------------------------------------------------------------
		technique_t ctechnique_manager::create_technique(const std::string& technique_name,
			const std::string& vs_path, const std::string& ps_path)
		{
			auto program = raylib::LoadShader(vs_path.c_str(), ps_path.c_str());

			if (raylib::IsShaderReady(program))
			{
				if(m_fragmented)
				{
					for (auto ii = 0; ii < m_technique_manager.size(); ++ii)
					{
						auto& tech = m_technique_manager[ii];

						if (tech.m_removed)
						{
							tech.m_shader = std::move(program);
							tech.m_removed = false;
							m_fragmentation_count = (m_fragmentation_count - 1 < 0) ? 0 : m_fragmentation_count - 1;

							//- defragmented
							if (m_fragmentation_count == 0)
							{
								m_fragmented = false;
							}

							return ii;
						}
					}
				}
				else
				{
					technique_t h = m_technique_count++;
					auto hash = algorithm::hash(technique_name.c_str());

					m_technique_names[hash] = h;
					m_technique_manager.emplace_back(program);
					return h;
				}
			}
			return invalid_handle_t;
		}

		//------------------------------------------------------------------------------------------------------------------------
		technique_t ctechnique_manager::create_technique_embedded(const std::string& technique_name, const char* vs, const char* ps)
		{
			auto h = algorithm::hash(technique_name.c_str());

			if (m_technique_names.find(h) == m_technique_names.end())
			{
				auto program = raylib::LoadShaderFromMemory(vs, ps);

				if (raylib::IsShaderReady(program))
				{
					if(m_fragmented)
					{
						for (auto ii = 0; ii < m_technique_manager.size(); ++ii)
						{
							auto& tech = m_technique_manager[ii];

							if (tech.m_removed)
							{
								tech.m_shader = std::move(program);
								tech.m_removed = false;
								m_fragmentation_count = (m_fragmentation_count - 1 < 0) ? 0 : m_fragmentation_count - 1;

								//- 
								if (m_fragmentation_count == 0)
								{
									m_fragmented = false;
								}

								return ii;
							}
						}
					}
					else
					{
						technique_t handle = m_technique_count++;

						m_technique_names[h] = handle;
						m_technique_manager.emplace_back(program);
						return handle;
					}
				}
			}
			return invalid_handle_t;
		}

		//------------------------------------------------------------------------------------------------------------------------
		technique_t ctechnique_manager::get_technique(const std::string& technique_name)
		{
			auto h = algorithm::hash(technique_name.c_str());
			
			if (m_technique_names.find(h) != m_technique_names.end())
			{
				return m_technique_names.at(h);
			}
			return invalid_handle_t;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ctechnique_manager::unload_technique(technique_t handle)
		{
			UnloadShader(m_technique_manager[handle].m_shader);

			m_technique_manager[handle].m_removed = true;

			algorithm::erase_if(m_technique_names, [&](auto it)
				{
					return it->second == handle;
				});

			--m_technique_count;
			++m_fragmentation_count;
			m_fragmented = true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const raylib::Shader& ctechnique_manager::raylib_shader(technique_t handle) const
		{
			return m_technique_manager[handle].m_shader;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cmaterial_manager::cmaterial_manager() :
			m_material_count(0), m_fragmented(false), m_fragmentation_count(0)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		cmaterial_manager::~cmaterial_manager()
		{
			m_material_manager.clear();
			m_material_names.clear();
		}

		//------------------------------------------------------------------------------------------------------------------------
		material_t cmaterial_manager::create_material(const std::string& material_name, technique_t technique,
			blending_mode mode /*= blending_mode_alpha*/, blending_equation equation /*= blending_equation_blend_color*/,
			blending_factor src /*= blending_factor_src_color*/, blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
		{
			ASSERT(algorithm::is_valid_handle(technique), "Technique handle must point to a valid Program");
			if (algorithm::is_valid_handle(technique))
			{
				if(m_fragmented)
				{
					for (auto ii = 0; ii < m_material_manager.size(); ++ii)
					{
						auto& mat = m_material_manager[ii];

						if (mat.m_removed)
						{
							mat.m_material = std::move(cmaterial(technique, mode, equation , src, dst));
							mat.m_removed = false;
							m_fragmentation_count = (m_fragmentation_count - 1 < 0) ? 0 : m_fragmentation_count - 1;

							//- 
							if (m_fragmentation_count == 0)
							{
								m_fragmented = false;
							}

							return ii;
						}
					}
				}
				else
				{
					auto handle = m_material_count++;
					auto h = algorithm::hash(material_name.c_str());
					m_material_names[h] = handle;

					m_material_manager.emplace_back(cmaterial(technique, mode, equation, src, dst));
					return handle;
				}
			}
			return invalid_handle_t;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cmaterial_manager::create_default_material(technique_t technique, blending_mode mode /*= blending_mode_alpha*/, blending_equation equation /*= blending_equation_blend_color*/, blending_factor src /*= blending_factor_src_color*/, blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
		{
			if(m_material_manager.empty())
			{
				auto& mat = m_material_manager.emplace_back(std::move(cmaterial(technique, mode, equation, src, dst)));
				++m_material_count;
				return true;
			}
			//- does already exist
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		material_t cmaterial_manager::get_material(const std::string& material_name) const
		{
			auto h = algorithm::hash(material_name.c_str());

			if (m_material_names.find(h) != m_material_names.end())
			{
				return m_material_names.at(h);
			}
			return invalid_handle_t;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cmaterial_manager::unload_material(material_t handle)
		{
			m_material_manager[handle].m_material.reset();
			m_material_manager[handle].m_removed = true;

			algorithm::erase_if(m_material_names, [&](auto it)
				{
					return it->second == handle;
				});

			--m_material_count;
			++m_fragmentation_count;
			m_fragmented = true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		material_t cmaterial_manager::default_material() const
		{
			return m_default_material;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const cmaterial& cmaterial_manager::get_internal_material(material_t handle) const
		{
			return m_material_manager[handle].m_material;
		}

		//------------------------------------------------------------------------------------------------------------------------
		cspriteatlas_manager::cspriteatlas_manager(crenderer& renderer) :
			m_renderer(renderer), m_spriteatlases_count(0), m_fragmented(false), m_fragmentation_count(0)
		{
		}

		//------------------------------------------------------------------------------------------------------------------------
		cspriteatlas_manager::~cspriteatlas_manager()
		{
			m_spriteatlas_manager.clear();
			m_spriteatlas_names.clear();
		}

		//------------------------------------------------------------------------------------------------------------------------
		spriteatlas_t cspriteatlas_manager::create_spriteatlas(const std::string& spriteatlas_name, const std::string& texture_path, const vec2_t& frames)
		{
			auto h = algorithm::hash(spriteatlas_name.c_str());
			//- create only if not already created
			if (m_spriteatlas_names.find(h) == m_spriteatlas_names.end())
			{
				//- initialize the sprite atlas only if we did load the texture
				auto texture_handle = m_renderer.create_texture(fmt::format("{}_texture", spriteatlas_name.c_str()).c_str(), texture_path);
				if (algorithm::is_valid_handle(texture_handle))
				{
					if (m_fragmented)
					{
						for (auto ii = 0; ii < m_spriteatlas_manager.size(); ++ii)
						{
							auto& atlas = m_spriteatlas_manager[ii];

							if (atlas.m_removed)
							{
								const auto texture_size = m_renderer.get_texture_size(texture_handle);
								const vec2_t frame_size = { texture_size.x / frames.x, texture_size.y / frames.y };
								atlas.m_atlas.begin(texture_handle);
								for (auto yy = 0; yy < frames.y; ++yy)
								{
									for (auto xx = 0; xx < frames.x; ++xx)
									{
										atlas.m_atlas.subtexture({ xx * frame_size.x / texture_size.x,
																	yy * frame_size.y / texture_size.y,
																	frame_size.x / texture_size.x,
																	frame_size.y / texture_size.y });
									}
								}

								atlas.m_atlas.end();

								atlas.m_removed = false;
								m_fragmentation_count = (m_fragmentation_count - 1 < 0) ? 0 : m_fragmentation_count - 1;

								//- defragmented
								if (m_fragmentation_count == 0)
								{
									m_fragmented = false;
								}

								return ii;
							}
						}
					}
					else
					{
						spriteatlas_t handle = m_spriteatlases_count++;
						m_spriteatlas_names[h] = handle;
						auto& atlas = m_spriteatlas_manager.emplace_back();

						const auto texture_size = m_renderer.get_texture_size(texture_handle);
						const vec2_t frame_size = { texture_size.x / frames.x, texture_size.y / frames.y };
						atlas.m_atlas.begin(texture_handle);
						for (auto yy = 0; yy < frames.y; ++yy)
						{
							for (auto xx = 0; xx < frames.x; ++xx)
							{
								atlas.m_atlas.subtexture({ xx * frame_size.x / texture_size.x,
															yy * frame_size.y / texture_size.y,
															frame_size.x / texture_size.x,
															frame_size.y / texture_size.y });
							}
						}

						atlas.m_atlas.end();

						return handle;
					}
				}
			}
			return invalid_handle_t;
		}

		//------------------------------------------------------------------------------------------------------------------------
		spriteatlas_t cspriteatlas_manager::get_spriteatlas(const std::string& spriteatlas_name) const
		{
			auto h = algorithm::hash(spriteatlas_name.c_str());
			return m_spriteatlas_names.at(h);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cspriteatlas_manager::unload_spriteatlas(spriteatlas_t handle)
		{
			m_spriteatlas_manager[handle].m_atlas.reset();
			m_spriteatlas_manager[handle].m_removed = true;

			algorithm::erase_if(m_spriteatlas_names, [&](auto it)
				{
					return it->second == handle;
				});

			--m_spriteatlases_count;
			++m_fragmentation_count;
			m_fragmented = true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::srect cspriteatlas_manager::get_subtexture(spriteatlas_t atlas, const std::string& subtexture_name)
		{
			auto h = algorithm::hash(subtexture_name.c_str());
			const auto& spriteatlas = m_spriteatlas_manager[atlas];
			return spriteatlas.m_atlas.subtexture(h);
		}

		//------------------------------------------------------------------------------------------------------------------------
		const sm::csprite_atlas& cspriteatlas_manager::get_internal_spriteatlas(spriteatlas_t handle)
		{
			return m_spriteatlas_manager[handle].m_atlas;
		}


	} //- detail

} //- sm