#include "editor_element_window.hpp"

namespace editor::ui
{
	namespace
	{
		constexpr ImGuiWindowFlags C_WINDOW_FLAGS = ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoDocking;
		constexpr unsigned C_WINDOW_WIDTH = 500;
		constexpr unsigned C_WINDOW_HEIGHT = 400;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	cwindow::cwindow(bool* open, stringview_t id /*= {}*/, ImGuiWindowFlags flags /*= ImGuiWindowFlags_None*/) :
		m_id(id), m_open(open), m_flags(flags)
	{
		CORE_ASSERT(open, "Invalid operation");

		m_flags = m_flags == ImGuiWindowFlags_None ? C_WINDOW_FLAGS : flags;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow::~cwindow()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow& cwindow::title(stringview_t text)
	{
		m_title = text;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow& cwindow::icon(const char* text)
	{
		m_icon = text;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow& cwindow::tooltip(stringview_t text)
	{
		m_tooltip = text;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	cwindow& cwindow::callback(window_display_callback_t&& cb)
	{
		m_callback = std::move(cb);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cwindow::draw()
	{
		if (m_id.empty())
		{
			m_id = fmt::format("{} {}", m_icon, m_title);
		}

		CORE_ASSERT(!m_id.empty(), "Invalid operation. Identifier cannot be empty, either define an Id or a Title with/without Icon!");

		imgui::cid_scope id_scope(m_id.data());

		if (const auto window_scope = imgui::cwindow_scope(fmt::format("{} {}", m_icon, m_title.data()),
			m_open, m_flags, { 0, 0 }, { C_WINDOW_WIDTH, C_WINDOW_HEIGHT }))
		{
			ImGui::BeginGroup();
			m_result = m_callback();
			ImGui::EndGroup();
		}

		return m_result;
	}

} //- editor::ui