#include "editor_entity_context_panel.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool centity_context_panel::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_panel::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_panel::on_ui_render()
	{
		if (!m_context_menu && ctx().m_inspected_entity != core::cuuid::C_INVALID_UUID)
		{
			auto e = ecs::cworld_manager::instance().active().em().entity(ctx().m_inspected_entity);

			m_context_menu = std::make_shared<centity_context_menu>(e, ctx());
		}

		if (m_context_menu){ m_context_menu->on_ui_render(); }
	}

} //- editor