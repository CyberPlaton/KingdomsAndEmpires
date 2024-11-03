#pragma once
#include <core.h>
#include "docks/editor_dock_base.hpp"

namespace editor
{
	//- The element stack contains an array of UI elements. It is designed to allow quick removal and creation of UI elements.
	//- Intended to be used as one for each panel. Moreover it reflects the API of a layer.
	//------------------------------------------------------------------------------------------------------------------------
	class celement_stack_system final
	{
	public:
		using element_lookup_t = umap_t<const char*, unsigned>;
		using elements_t = vector_t<layer_ref_t>;

		enum mode : uint8_t
		{
			mode_none = 0,
			mode_singular,	//- Only one element from stack is updated
			mode_plural,	//- All elements from stack are updated
		};

		celement_stack_system() = default;
		~celement_stack_system();

		void shutdown();
		void on_update(float dt);
		void on_world_render();
		void on_ui_render();
		void on_post_update(float dt);

		template<typename T, typename... ARGS>
		layer_ref_t push(ARGS&&... args);

		void pop(const char* id);

		void set_active_at_index(unsigned idx);
		void set_active_all();

	private:
		elements_t m_elements;
		element_lookup_t m_elements_lookup;
		unsigned m_singular_index = MAX(unsigned);
		mode m_mode = mode_plural;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T, typename... ARGS>
	layer_ref_t editor::celement_stack_system::push(ARGS&&... args)
	{
		layer_ref_t element = nullptr;

		if (element = detail::create_ui_element<T>(std::forward<ARGS>(args)...); element && element->init())
		{
			auto idx = m_elements.size();

			m_elements.push_back(element);

			m_elements_lookup[element->id()] = idx;
		}

		return element;
	}

} //- editor