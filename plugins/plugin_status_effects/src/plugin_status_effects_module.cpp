#include "plugin_status_effects_module.hpp"

namespace effects
{
	//------------------------------------------------------------------------------------------------------------------------
	bool sexample_effect::on_apply(flecs::entity e)
	{
		if (e.has<ecs::ssprite>())
		{
			auto* c = e.get_mut<ecs::ssprite>();

			m_material_handle = sm::ctx().sm().get(m_material.c_str());
			m_texture_handle = sm::ctx().tm().get(m_texture.c_str());

			if (algorithm::is_valid_handle(m_material_handle) && algorithm::is_valid_handle(m_texture_handle))
			{
				//- Here we only apply the effect to our entity because of limitations.
				auto* c = e.get_mut<ecs::ssprite>();

				m_index = c->m_materials.size();
				c->m_materials.emplace_back(core::smaterial_pair{m_texture_handle, m_material_handle});

				return true;
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool sexample_effect::on_update(flecs::entity e)
	{
		if (--m_turns < 0)
		{
			return false;
		}

		//- do something...

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void sexample_effect::on_remove(flecs::entity e)
	{
		if (e.has<ecs::ssprite>() && algorithm::is_valid_handle(m_material_handle) && algorithm::is_valid_handle(m_texture_handle))
		{
			auto* c = e.get_mut<ecs::ssprite>();

			algorithm::erase_at_index(c->m_materials, m_index);
		}
	}

} //- effects