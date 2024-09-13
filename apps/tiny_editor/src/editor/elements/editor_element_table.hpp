#pragma once
#include <core.h>
#include <engine.h>
#include <spritemancer.h>

namespace editor::ui
{
	//- Class wrapping the ImGui table API and allows convenient definition and construction of a table.
	//------------------------------------------------------------------------------------------------------------------------
	class ctable final
	{
	public:
		enum options
		{
			options_none								= 0,
			options_resizable							= ImGuiTableFlags_Resizable,
			options_reorderable							= ImGuiTableFlags_Reorderable,
			options_hideable							= ImGuiTableFlags_Hideable,
			options_sortable							= ImGuiTableFlags_Sortable,
			options_inner_horizontal_borders			= ImGuiTableFlags_BordersInnerH,
			options_inner_vertical_borders				= ImGuiTableFlags_BordersInnerV,
			options_outer_horizontal_borders			= ImGuiTableFlags_BordersOuterH,
			options_outer_vertical_borders				= ImGuiTableFlags_BordersOuterV,
			options_sizing_fixed_fit					= ImGuiTableFlags_SizingFixedFit,
			options_sizing_fixed_same					= ImGuiTableFlags_SizingFixedSame,
			options_sizing_stretch_proportional_width	= ImGuiTableFlags_SizingStretchProp,
			options_sizing_stretch_same_width			= ImGuiTableFlags_SizingStretchSame,
		};

		ctable(stringview_t id = {}, bool* enabled = nullptr, ImGuiTableFlags flags = ImGuiTableFlags_None);
		~ctable() = default;

		ctable& size(const vec2_t& value);
		ctable& tooltip(stringview_t text, const bool show = true);
		ctable& option(options op, const bool value);

		bool draw();

	private:
		string_t m_id;
		stringview_t m_tooltip;
		stringview_t m_hint;
		ImVec2 m_size;
		ImGuiTableFlags m_flags;
		bool m_show_tooltip;
		bool m_enabled;
		bool m_result = false;
	};

} //- editor::ui