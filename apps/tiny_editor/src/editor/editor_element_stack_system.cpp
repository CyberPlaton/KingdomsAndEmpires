#include "editor_element_stack_system.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	celement_stack_system::~celement_stack_system()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::shutdown()
	{
		algorithm::for_each(m_elements.begin(), m_elements.end(), [this](const auto& pair)
			{
				pair.second->shutdown();
			});

		m_elements.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::on_update(float dt)
	{
		//- Check for elements that are to be removed and remove them
		stack_t<ImGuiID> to_remove;

		algorithm::for_each(m_elements.begin(), m_elements.end(), [](const auto& pair)
			{
				if (!pair.second->active())
				{
					to_remove.push(pair.second->id());
				}
			});

		while (!to_remove.empty())
		{
			auto id = to_remove.top(); to_remove.pop();

			pop(id);
		}

		//- Update the actual UI elements
		algorithm::for_each(m_elements.begin(), m_elements.end(), [dt = dt](const auto& pair)
			{
				pair.second->on_update(dt);
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::on_world_render()
	{
		algorithm::for_each(m_elements.begin(), m_elements.end(), [](const auto& pair)
			{
				pair.second->on_world_render();
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::on_ui_render()
	{
		algorithm::for_each(m_elements.begin(), m_elements.end(), [](const auto& pair)
			{
				pair.second->on_ui_render();
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::on_post_update(float dt)
	{
		algorithm::for_each(m_elements.begin(), m_elements.end(), [dt = dt](const auto& pair)
			{
				pair.second->on_post_update(dt);
			});
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::pop(const char* id)
	{
		pop(ImGui::GetID(id));
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::pop(ImGuiID id)
	{
		auto it = m_elements.find(id);

		if (it != m_elements.end())
		{
			it->second->shutdown();

			m_elements.erase(it);
		}
	}

} //- editor