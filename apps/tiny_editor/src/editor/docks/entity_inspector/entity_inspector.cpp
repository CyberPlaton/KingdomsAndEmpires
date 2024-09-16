#include "entity_inspector.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool centity_inspector::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_inspector::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_inspector::on_ui_render()
	{
		if (ecs::cworld_manager::instance().has_active())
		{
			const auto& world = ecs::cworld_manager::instance().active();

			if (ctx().m_inspected_entity_uuid != core::cuuid::C_INVALID_UUID)
			{
				const auto& e = world.em().entity(ctx().m_inspected_entity_uuid);

				for (const auto& c : world.cm().all(e))
				{
					if (const auto type = rttr::type::get_by_name(c); type.is_valid())
					{
						if (const auto method = type.get_method(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME); method.is_valid())
						{
							if (ImGui::CollapsingHeader(c.data()))
							{
								//- Show actual component UI
								method.invoke(e);
							}
						}
					}
				}

				ImGui::Text(e.name());
			}
		}
	}

} //- editor