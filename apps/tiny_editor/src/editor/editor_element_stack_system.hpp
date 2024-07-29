#pragma once
#include <core.h>
#include "docks/editor_dock_base.hpp"

namespace editor
{
	//- The element stack contains an array of UI elements. It is designed to allow quick removal and creation of UI elements.
	//- Intended to be used as one for each panel. Moreover it reflects the API of a layer.
	//-
	//- TODO: decide whether we update always all elements or theres a concept of current active one...
	//------------------------------------------------------------------------------------------------------------------------
	class celement_stack_system final
	{
	public:
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

	private:
		umap_t<const char*, layer_ref_t> m_elements;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename T, typename... ARGS>
	layer_ref_t editor::celement_stack_system::push(ARGS&&... args)
	{
		layer_ref_t element = nullptr;

		if (element = detail::create_ui_element<T>(std::forward<ARGS>(args)...); element && element->init())
		{
			m_elements[element->id()] = element;
		}

		return element;
	}

} //- editor