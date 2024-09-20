#include "editor_element_stack_system.hpp"

namespace editor
{
	namespace
	{
//------------------------------------------------------------------------------------------------------------------------
#define execute_elements(func, elements, idx, mode, ...)								\
switch (mode)																			\
{																						\
default:																				\
case celement_stack_system::mode_none:													\
case celement_stack_system::mode_plural:												\
{																						\
	algorithm::for_each(elements.begin(), elements.end(), [&](const auto& e)			\
		{																				\
			e->func(__VA_ARGS__);												\
		});																				\
	break;																				\
}																						\
case celement_stack_system::mode_singular:												\
{																						\
	elements[idx]->func(__VA_ARGS__);													\
	break;																				\
}																						\
}
	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	celement_stack_system::~celement_stack_system()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::set_active_at_index(unsigned idx)
	{
		m_mode = mode_singular;
		m_singular_index = idx;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::set_active_all()
	{
		m_mode = mode_plural;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::shutdown()
	{
		algorithm::for_each(m_elements.begin(), m_elements.end(), [this](const auto& e)
			{
				e->shutdown();
			});

		m_elements.clear();
		m_elements_lookup.clear();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::on_update(float dt)
	{
		//- Check for elements that are to be removed and remove them
		core::stack_t<const char*> to_remove;

		algorithm::for_each(m_elements.begin(), m_elements.end(), [&](const auto& e)
			{
				if (!e->active())
				{
					to_remove.push(e->id());
				}
			});

		while (!to_remove.empty())
		{
			auto id = to_remove.top(); to_remove.pop();

			pop(id);
		}

		//- Actually tick the update of UI elements
		execute_elements(on_update, m_elements, m_singular_index, m_mode, dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::on_world_render()
	{
		execute_elements(on_world_render, m_elements, m_singular_index, m_mode);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::on_ui_render()
	{
		execute_elements(on_ui_render, m_elements, m_singular_index, m_mode);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::on_post_update(float dt)
	{
		execute_elements(on_post_update, m_elements, m_singular_index, m_mode, dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void celement_stack_system::pop(const char* id)
	{
		if (const auto it = m_elements_lookup.find(id); it != m_elements_lookup.end())
		{
			auto idx = m_elements_lookup.at(id);

			m_elements[idx]->shutdown();

			//- Remove element from storage and lookup
			algorithm::erase_at_index(m_elements, idx);
			algorithm::erase_at(m_elements_lookup, it);
		}
	}

} //- editor