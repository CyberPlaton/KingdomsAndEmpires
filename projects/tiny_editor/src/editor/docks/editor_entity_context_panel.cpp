#include "editor_entity_context_panel.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool centity_context_panel::init()
	{
// 		if (auto context_menu = m_elements_stack.push<centity_context_menu>(ctx()); context_menu && context_menu->init())
// 		{
// 			return true;
// 		}
//		return false;
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_panel::shutdown()
	{
		m_elements_stack.shutdown();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_panel::on_ui_render()
	{
		m_elements_stack.on_ui_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_panel::on_world_render()
	{
		m_elements_stack.on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_panel::on_update(float dt)
	{
		m_elements_stack.on_update(dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_context_panel::on_post_update(float dt)
	{
		m_elements_stack.on_post_update(dt);
	}

} //- editor