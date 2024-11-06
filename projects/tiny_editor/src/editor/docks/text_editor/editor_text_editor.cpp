#include "editor_text_editor.hpp"
#include "../../elements/editor_element_window.hpp"

namespace editor
{
	namespace
	{
		constexpr stringview_t C_SOURCE =
			"int main()						\n"
			"{								\n"
			"	printf(\"Hello World\");	\n"
			"	return 0;					\n"
			"};								\n"
			;

		static const vector_t<string_t> C_TEXT_FILES_FILTER = {"lua", "LUA"};
		constexpr auto C_STRING_BUFFER_RESERVE_SIZE = 1024;
		constexpr stringview_t C_WINDOW_ID			= "##editor_text_editor";
		constexpr stringview_t C_WINDOW_CHILD_ID	= "##editor_text_editor_child";
		constexpr auto C_TEXT_EDITOR_FLAGS			= ImGuiWindowFlags_MenuBar;
		constexpr auto C_TEXT_EDITOR_VIEWPORT_FLAGS = ImGuiWindowFlags_AlwaysVerticalScrollbar;
		constexpr vec2_t C_TEXT_EDITOR_VIEWPORT_SIZE = { 0.0f, 0.0f};
		constexpr auto C_TEXT_EDITOR_VIEWPORT_CHILD_FLAGS = ImGuiChildFlags_None;
		constexpr vec2_t C_SIZE_MIN					= { 500, 400 };
		constexpr vec2_t C_SIZE_MAX					= { 1000, 800 };

	} //- unnamed

	//-------------------------------------------------------------------------------------------------------
	bool ctext_editor::init()
	{
		m_source_buffer.reserve(C_STRING_BUFFER_RESERVE_SIZE);
		return true;
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::shutdown()
	{
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::on_update(float dt)
	{
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::on_ui_render()
	{
		show_main_window();
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::show_main_menu()
	{
		if (const auto menubar_scope = imgui::cmenubar_scope())
		{
			if (const auto file_scope = imgui::cmenu_scope(ICON_FA_FILE))
			{
				if (const auto open_file_scope = imgui::cmenu_item("Open"))
				{
					if (const auto selected = pfd::open_file("Open File", core::cfilesystem::cwd().view(), {"*"}).result(); !selected.empty())
					{
						m_selected_text_file = selected.front();
					}
				}
			}
			if (const auto file_scope = imgui::cmenu_scope(ICON_FA_GEAR))
			{
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::show_main_window()
	{
		imgui::cid_scope id_scope(C_WINDOW_ID.data());

		const auto position = ImGui::GetContentRegionAvail() * 0.5f;
		const auto title = m_loaded_text_file.empty() ? "Text Editor" : fs::cfileinfo(m_loaded_text_file).filename().string();

		if (const auto window_scope = imgui::cwindow_scope(title, &active(), C_TEXT_EDITOR_FLAGS, vec2_t(position.x, position.y), C_SIZE_MIN, C_SIZE_MAX))
		{
			show_main_menu();
			show_text_editor();
		}
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::show_text_editor()
	{
		if (const auto child_scope = imgui::cchild_scope(C_WINDOW_CHILD_ID, C_TEXT_EDITOR_VIEWPORT_SIZE,
			C_TEXT_EDITOR_VIEWPORT_CHILD_FLAGS, C_TEXT_EDITOR_VIEWPORT_FLAGS))
		{
			update_common();

			std::istringstream stream(m_source_buffer.c_str());
			string_t line;

			while (std::getline(stream, line))
			{
				ImGui::TextUnformatted(line.data());
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::load_from_file(const fs::cfileinfo& info)
	{
		if (const auto memory = fs::load_text_from_file(info.path()); memory && !memory->empty())
		{
			m_loaded_text_file = info.path();

			//- Copy source file text to buffer
			m_source_buffer.clear();
			m_source_buffer.write(memory->data());

			//- Setup line by line view
			std::stringstream stream(m_source_buffer.c_str());
			string_t text_line;

			while (std::getline(stream, text_line, '\n'))
			{
				m_data.m_source_lines.emplace_back(text_line);
			}
		}
		else
		{
			log_error(fmt::format("Could not open source file for text editing at '{}'", info.path()));
		}
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::save_to_file(const fs::cfileinfo& info)
	{
		if (info.exists())
		{
			//- Copy edited data to source buffer
			m_source_buffer.clear();
			m_source_buffer.write(fmt::format("{}", fmt::join(m_data.m_source_lines, "\n")));

			if (const auto status = fs::save_text_to_file(info.path(), m_source_buffer.c_str()); status != core::file_io_status_success)
			{
				log_error(fmt::format("Could not write edited text to source file '{}'", info.path()));
			}
		}
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::update_common()
	{
		const auto p = ImGui::GetWindowPos();
		const auto s = ImGui::GetWindowSize();
		const auto m = ImGui::GetMousePos();
		const auto c = ImGui::GetCursorScreenPos();

		//- Do update variables
		m_data.m_imgui_data.m_window_position = { p.x, p.y };
		m_data.m_imgui_data.m_window_size = { s.x, s.y };
		m_data.m_imgui_data.m_mouse_position = { m.x, m.y };
		m_data.m_imgui_data.m_cursor_position = { c.x, c.y };

		//- Do update source text
		if (dirty())
		{
		}

		if (!m_selected_text_file.empty())
		{
			load_from_file({ m_selected_text_file });

			m_selected_text_file.clear();
		}
	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::update_key_input()
	{

	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::update_mouse_input()
	{

	}

	//-------------------------------------------------------------------------------------------------------
	void ctext_editor::update_shortcuts()
	{

	}

	//-------------------------------------------------------------------------------------------------------
	ivec2_t ctext_editor::mouse_to_line_column() const
	{
		return { 0, 0 };
	}

} //- editor