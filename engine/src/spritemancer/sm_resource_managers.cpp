#include "sm_resource_managers.hpp"

namespace sm
{
	namespace internal
	{
		//------------------------------------------------------------------------------------------------------------------------
		texture_t ctexture_manager::create(stringview_t name, stringview_t path)
		{
			texture_t handle = invalid_handle_t;

			if (auto texture = raylib::LoadTexture(path); raylib::IsTextureReady(texture))
			{
				const auto h = algorithm::hash(name);

				if (fragmented())
				{
					if (const auto slot = fragmentation_slot(); slot != std::numeric_limits<unsigned>().max())
					{
						store_resource(m_resources, texture, slot);
						m_resources[slot].m_removed = false;
						decrement();
						handle = slot;
					}
				}
				else
				{
					handle = increment();
					m_lookup.emplace(h, handle);
					store_resource(m_resources, texture);
				}
			}
			return handle;
		}

		//------------------------------------------------------------------------------------------------------------------------
		const raylib::Texture2D* ctexture_manager::native(stringview_t name) const
		{
			return &m_resources.at(get(name)).m_resource;
		}

		//------------------------------------------------------------------------------------------------------------------------
		vec2_t ctexture_manager::texture_size(texture_t texture) const
		{
			const auto& tex = m_resources[texture].m_resource;

			return {SCAST(float, tex.width), SCAST(float, tex.height)};
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ctexture_manager::on_resource_unload(raylib::Texture2D& resource)
		{
			if (raylib::IsTextureReady(resource))
			{
				raylib::UnloadTexture(resource);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool ctexture_manager::init()
		{
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		technique_t ctechnique_manager::create(stringview_t name, stringview_t vspath, stringview_t pspath)
		{
			technique_t handle = invalid_handle_t;

			if (auto shader = raylib::LoadShader(vspath, pspath); raylib::IsShaderReady(shader))
			{
				const auto h = algorithm::hash(name);

				if (fragmented())
				{
					if (const auto slot = fragmentation_slot(); slot != std::numeric_limits<unsigned>().max())
					{
						store_resource(m_resources, shader, slot);
						m_resources[slot].m_removed = false;
						decrement();
						handle = slot;
					}
				}
				else
				{
					handle = increment();
					m_lookup.emplace(h, handle);
					store_resource(m_resources, shader);
				}
			}
			return handle;
		}

		//------------------------------------------------------------------------------------------------------------------------
		technique_t ctechnique_manager::create_embedded(stringview_t name, stringview_t vsdata, stringview_t psdata)
		{
			technique_t handle = invalid_handle_t;

			if (auto shader = raylib::LoadShaderFromMemory(vsdata, psdata); raylib::IsShaderReady(shader))
			{
				const auto h = algorithm::hash(name);

				if (fragmented())
				{
					if (const auto slot = fragmentation_slot(); slot != std::numeric_limits<unsigned>().max())
					{
						store_resource(m_resources, shader, slot);
						m_resources[slot].m_removed = false;
						decrement();
						handle = slot;
					}
				}
				else
				{
					handle = increment();
					m_lookup.emplace(h, handle);
					store_resource(m_resources, shader);
				}
			}
			return handle;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void ctechnique_manager::on_resource_unload(raylib::Shader& resource)
		{
			if (raylib::IsShaderReady(resource))
			{
				raylib::UnloadShader(resource);
			}
		}

		//------------------------------------------------------------------------------------------------------------------------
		material_t cmaterial_manager::create(stringview_t name, technique_t technique, blending_mode mode /*= blending_mode_alpha*/,
			blending_equation equation /*= blending_equation_blend_color*/, blending_factor src /*= blending_factor_src_color*/,
			blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
		{
			material_t handle = invalid_handle_t;

			CORE_ASSERT(algorithm::is_valid_handle(technique), "Invalid operation. Technique is not a valid handle");

			if (fragmented())
			{
				if (const auto slot = fragmentation_slot(); slot != std::numeric_limits<unsigned>().max())
				{
					store_resource(m_resources, std::move(cmaterial(technique, mode, equation, src, dst)), slot);
					m_resources[slot].m_removed = false;
					decrement();
					handle = slot;
				}
			}
			else
			{
				const auto h = algorithm::hash(name);

				handle = increment();
				m_lookup.emplace(h, handle);
				store_resource(m_resources, std::move(cmaterial(technique, mode, equation, src, dst)));
			}

			return handle;
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cmaterial_manager::on_resource_unload(cmaterial& resource)
		{
			resource.reset();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cmaterial_manager::init()
		{
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cmaterial_manager::create_default(technique_t technique, blending_mode mode /*= blending_mode_alpha*/,
			blending_equation equation /*= blending_equation_blend_color*/, blending_factor src /*= blending_factor_src_color*/,
			blending_factor dst /*= blending_factor_one_minus_src_alpha*/)
		{
			if (m_resources.empty())
			{
				CORE_ASSERT(algorithm::is_valid_handle(technique), "Invalid operation. Technique is not a valid handle");

				m_resources.emplace_back(std::move(cmaterial(technique, mode, equation, src, dst)));

				increment();

				return true;
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------------
		core::srect cspriteatlas_manager::subtexture(spriteatlas_t atlas, stringview_t name) const
		{
			auto& spriteatlas = m_resources[atlas].m_resource;

			return spriteatlas.subtexture(algorithm::hash(name));
		}

		//------------------------------------------------------------------------------------------------------------------------
		void cspriteatlas_manager::on_resource_unload(csprite_atlas& resource)
		{
			resource.reset();
		}

		//------------------------------------------------------------------------------------------------------------------------
		bool cspriteatlas_manager::init()
		{
			return true;
		}

	} //- internal

} //- sm