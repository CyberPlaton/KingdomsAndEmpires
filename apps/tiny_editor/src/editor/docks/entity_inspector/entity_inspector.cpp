#include "entity_inspector.hpp"

namespace editor
{
	namespace
	{
		constexpr stringview_t C_CHILD_PANEL_ID = "##editor_entity_inspector";
		constexpr auto C_ENTITY_INSPECTOR_CHILD_FLAGS = ImGuiChildFlags_Border;
		constexpr auto C_ENTITY_INSPECTOR_FLAGS = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuBar;

	} //- unnamed

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
			if (ctx().m_inspected_entity_uuid != core::cuuid::C_INVALID_UUID)
			{
				if (imgui::cchild_scope scope(C_CHILD_PANEL_ID.data(), { 0.0f, 0.0f }, C_ENTITY_INSPECTOR_CHILD_FLAGS, C_ENTITY_INSPECTOR_FLAGS); scope)
				{
					show_menubar();

					if (m_table
						.options(ui::ctable::options_borders_all)
						.begin(2,
							{
								{ImGuiTableColumnFlags_WidthStretch, 0.9f},
								{ImGuiTableColumnFlags_WidthStretch, 0.1f}
							}))
					{
						show_inspected_entity();

						m_table.end();
					}
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_inspector::show_menubar()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void centity_inspector::show_inspected_entity()
	{
		const auto& world = ecs::cworld_manager::instance().active();
		const auto& e = world.em().entity(ctx().m_inspected_entity_uuid);

		for (const auto& c : world.cm().all(e))
		{
			if (const auto type = rttr::type::get_by_name(c); type.is_valid())
			{
				m_table.begin_next_row();

				//- Show component UI and icons for settings and state changes
				m_table.begin_column(0);

				if (const auto method = type.get_method(ecs::detail::C_COMPONENT_SHOW_UI_FUNC_NAME); method.is_valid())
				{
					if (ImGui::CollapsingHeader(c.data()))
					{
						//- Show actual component UI
						method.invoke(e);
					}
				}

				m_table.begin_column(1);
			}
		}
	}

} //- editor