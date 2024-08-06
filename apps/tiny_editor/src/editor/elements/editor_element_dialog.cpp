#include "editor_element_dialog.hpp"

namespace editor::ui
{
	namespace
	{
		constexpr ImGuiWindowFlags C_DIALOG_FLAGS = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;
		constexpr unsigned C_DIALOG_WIDTH = 350;
		constexpr unsigned C_DIALOG_HEIGHT = 300;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cdialog::cdialog(stringview_t id /*= {}*/, bool* open /*= nullptr*/, ImGuiWindowFlags flags /*= ImGuiWindowFlags_None*/) :
		m_id(id), m_open(open), m_flags(flags == ImGuiWindowFlags_None ? C_DIALOG_FLAGS : flags)
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cdialog::~cdialog()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cdialog& cdialog::title(stringview_t text)
	{
		m_title = text;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cdialog& cdialog::icon(const char* text)
	{
		m_icon = text;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cdialog& cdialog::callback(dialog_display_callback_t&& cb)
	{
		m_callback = cb;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cdialog& cdialog::confirm_button(stringview_t text, dialog_button_callback_t&& on_clicked)
	{
		m_confirm_button_text = text;
		m_confirm_button = std::move(on_clicked);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cdialog& cdialog::cancel_button(stringview_t text, dialog_button_callback_t&& on_clicked)
	{
		m_cancel_button_text = text;
		m_cancel_button = std::move(on_clicked);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cdialog& cdialog::tooltip(stringview_t text)
	{
		m_tooltip = text;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cdialog::draw()
	{
		if (m_id.empty())
		{
			m_id = fmt::format("{} {}", m_icon, m_title);
		}

		CORE_ASSERT(!m_id.empty(), "Invalid operation. Identifier cannot be empty, either define an Id or a Title with/without Icon!");

		imgui::cid_scope id_scope(m_id.data());

		if (const auto window_scope = imgui::cwindow_scope(fmt::format("{} {}", m_icon, m_title), m_open, m_flags,
			{0, 0}, { C_DIALOG_WIDTH, C_DIALOG_HEIGHT }))
		{
			if (!m_tooltip.empty())
			{
				imgui::cui::help_marker_no_question_mark(m_tooltip.data(), imgui::tooltip_hovering_delay_long);
			}

			ImGui::BeginGroup();
			m_callback();
			ImGui::EndGroup();

			if (ImGui::SmallButton(m_confirm_button_text.data()))
			{
				m_confirm_button();
				m_result = true;
			}

			ImGui::SameLine(C_DIALOG_WIDTH * 0.5f);

			if (ImGui::SmallButton(m_cancel_button_text.data()))
			{
				m_cancel_button();
				m_result = false;
			}
		}
		return m_result;
	}

} //- editor::ui