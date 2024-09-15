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

				ImGui::Text(e.name());
			}
		}
	}

} //- editor