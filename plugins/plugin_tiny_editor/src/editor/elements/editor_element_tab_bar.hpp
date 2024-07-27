#pragma once
#include <core.h>
#include <engine.h>
#include "../../config.hpp"

namespace editor::ui
{
	//- A tab bar consists of any number of horizontal columns and vertical rows. The individual slots can be defined
	//- visually and do something when interacted with.
	//------------------------------------------------------------------------------------------------------------------------
	class EDITOR_API ctab_bar final
	{
	public:
		struct sitem
		{
			stringview_t m_label;
			ImGuiTabItemFlags m_flags = ImGuiTabItemFlags_None;
			bool m_open = false;
		};

		ctab_bar(stringview_t id, ImGuiTabBarFlags flags = ImGuiTabBarFlags_None);
		~ctab_bar();

		ctab_bar& items(sitem* begin, unsigned count);
		unsigned draw();

	private:
		sitem* m_begin;
		stringview_t m_id;
		unsigned m_item_count;
		ImGuiTabBarFlags m_flags;
	};

} //- editor::ui