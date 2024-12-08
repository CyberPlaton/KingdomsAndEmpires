#include "editor_text_editor.hpp"

namespace editor
{
	namespace textedit
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

			static const vector_t<string_t> C_TEXT_FILES_FILTER = { "lua", "LUA" };
			constexpr auto C_STRING_BUFFER_RESERVE_SIZE = 1024;
			constexpr stringview_t C_WINDOW_ID = "##editor_text_editor";
			constexpr stringview_t C_WINDOW_CHILD_ID = "##editor_text_editor_child";
			constexpr auto C_TEXT_EDITOR_FLAGS = ImGuiWindowFlags_MenuBar;
			constexpr auto C_TEXT_EDITOR_VIEWPORT_FLAGS = ImGuiWindowFlags_AlwaysVerticalScrollbar;
			constexpr vec2_t C_TEXT_EDITOR_VIEWPORT_SIZE = { 0.0f, 0.0f };
			constexpr auto C_TEXT_EDITOR_VIEWPORT_CHILD_FLAGS = ImGuiChildFlags_None;
			constexpr vec2_t C_SIZE_MIN = { 500, 400 };
			constexpr vec2_t C_SIZE_MAX = { 1000, 800 };
			static umap_t<string_t, unsigned> C_COLORS;

			//-------------------------------------------------------------------------------------------------------
			unsigned debug_color(const string_t& word)
			{
				static const auto color_rotation = []() -> unsigned
					{
						static auto S_COLOR = 0;

						auto color_index = (++S_COLOR) % core::common_color_neutral1000;

						if (color_index == core::common_color_transparent) { ++color_index; }

						auto c = core::scolor(core::common_color(color_index));

						c.m_a = 125;

						return c.abgr();
					};

				if (C_COLORS.find(word) == C_COLORS.end())
				{
					C_COLORS[word] = color_rotation();
				}

				return C_COLORS.at(word);
			}

			//-------------------------------------------------------------------------------------------------------
			float string_length(const char* string)
			{
				return ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, string, nullptr, nullptr).x;
			}

			//-------------------------------------------------------------------------------------------------------
			float string_length(char c)
			{
				string_t s; s.push_back(c); return string_length(s.c_str());
			}

		} //- unnamed

		//-------------------------------------------------------------------------------------------------------
		bool ctext_editor::init()
		{
			m_backend.init();
			m_data.m_current_string.reserve(1024);

			//- Initialize default color style
			//- Text editor source code colors
			m_style.m_colors[color_palette_none]								= core::scolor(core::common_color_neutral800).abgr();
			m_style.m_colors[color_palette_text_keyword]						= core::scolor(core::common_color_blue_dark).abgr();
			m_style.m_colors[color_palette_text_number]							= core::scolor(core::common_color_green).abgr();
			m_style.m_colors[color_palette_text_string]							= core::scolor(core::common_color_orange_light).abgr();
			m_style.m_colors[color_palette_text_literal]						= core::scolor(core::common_color_neutral800).abgr();
			m_style.m_colors[color_palette_text_punctuation]					= core::scolor(core::common_color_yellow_light).abgr();
			m_style.m_colors[color_palette_text_preprocessor]					= core::scolor(core::common_color_neutral300).abgr();
			m_style.m_colors[color_palette_text_identifier]						= core::scolor(core::common_color_orange).abgr();
			m_style.m_colors[color_palette_text_resolved_identifier]			= core::scolor(core::common_color_orange_light).abgr();
			m_style.m_colors[color_palette_text_comment]						= core::scolor(core::common_color_green_dark).abgr();
			m_style.m_colors[color_palette_text_multiline_comment]				= core::scolor(core::common_color_green_dark).abgr();
			//- Text editor colors
			m_style.m_colors[color_palette_editor_background]					= core::scolor(core::common_color_neutral100).abgr();
			m_style.m_colors[color_palette_editor_cursor]						= core::scolor(core::common_color_neutral1000).abgr();
			m_style.m_colors[color_palette_editor_selection]					= core::scolor(core::common_color_blue_light).abgr();
			m_style.m_colors[color_palette_editor_error_marker]					= core::scolor(core::common_color_red_dark).abgr();
			m_style.m_colors[color_palette_editor_line_number]					= core::scolor(core::common_color_green_light).abgr();
			m_style.m_colors[color_palette_editor_current_line_fill]			= core::scolor(core::common_color_blue_light).abgr();
			m_style.m_colors[color_palette_editor_current_line_fill_inactive]	= core::scolor(core::common_color_blue_dark).abgr();
			m_style.m_colors[color_palette_editor_current_line_outline]			= core::scolor(core::common_color_neutral500).abgr();

			return true;
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::shutdown()
		{
			m_backend.shutdown();
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
						if (const auto selected = pfd::open_file("Open File", core::cfilesystem::cwd().view(), { "*" }).result(); !selected.empty())
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
			//- Show reference text editor
			m_text_editor_reference.Render("Reference Text Editor", ImVec2(0.0f, 0.0f), true);

			//- Reset render data
			m_data.m_previous_color = color_palette_none;
			m_data.m_current_string.clear();
			m_data.m_current_string_length = 0.0f;
			m_data.m_current_longest_line = 0.0f;
			m_data.m_character_advance = { 0.0f, 0.0f };
			m_data.m_space_size = 0.0f;

			const auto position = ImGui::GetContentRegionAvail() * 0.5f;

			if (const auto window_scope = imgui::cwindow_scope(C_WINDOW_ID, &active(), C_TEXT_EDITOR_FLAGS, vec2_t(position.x, position.y), C_SIZE_MIN, C_SIZE_MAX))
			{
				show_main_menu();

				if (const auto child_scope = imgui::cchild_scope(C_WINDOW_CHILD_ID, C_TEXT_EDITOR_VIEWPORT_SIZE,
					C_TEXT_EDITOR_VIEWPORT_CHILD_FLAGS, C_TEXT_EDITOR_VIEWPORT_FLAGS))
				{
					update();

					auto& draw_list = *ImGui::GetWindowDrawList();
					auto scroll_x = ImGui::GetScrollX();
					auto scroll_y = ImGui::GetScrollY();
					const auto cursor_screen_position = ImGui::GetCursorScreenPos();

					//- Select range of lines to be displayed
					unsigned from_line = static_cast<unsigned>(std::floor(scroll_y / m_data.m_character_advance.y));
					unsigned to_line = m_backend.line_count();

					log_critical(fmt::format("Processing lines {}-{}", from_line, to_line));

					for (auto line_idx = from_line; line_idx < to_line; ++line_idx)
					{
						const auto& line = m_backend.line_at(line_idx);

						m_data.m_current_longest_line = std::max(m_data.m_current_longest_line, line_length(line));

						auto line_start_screen_position = ImVec2(cursor_screen_position.x,
							cursor_screen_position.y + line_idx * m_data.m_character_advance.y);

						auto text_screen_position = ImVec2(line_start_screen_position.x,
							line_start_screen_position.y);

						auto text_buffer_offset = ImVec2(0.0f, 0.0f);

						for (auto glyph_idx = 0; glyph_idx < line.size();)
						{
							const auto& glyph = line[glyph_idx];

							//- Check if we encountered a new word or the end of the current word.
							if ((glyph.m_color != m_data.m_previous_color ||
								glyph.m_char == '\t' ||
								glyph.m_char == ' ') && !m_data.m_current_string.empty())
							{
								const ImVec2 new_offset(text_screen_position.x + text_buffer_offset.x, text_screen_position.y + text_buffer_offset.y);
								draw_list.AddText(new_offset, m_style.m_colors.at(glyph.m_color), m_data.m_current_string.c_str());

								//- Debug show text rectangle
								{
									const auto width = string_length(m_data.m_current_string.c_str());
									const auto height = m_data.m_character_advance.y;

									draw_list.AddRectFilled(new_offset, ImVec2(new_offset.x + width, new_offset.y + height), debug_color(m_data.m_current_string));
								}

								m_data.m_current_string_length = string_length(m_data.m_current_string.c_str());
								m_data.m_current_string.clear();
								text_buffer_offset.x += m_data.m_current_string_length;
							}

							switch (glyph.m_char)
							{
							case '\t':
							{
								const auto previous_position_x = text_buffer_offset.x;

								const auto tab_length = m_data.m_tab_size * m_data.m_space_size;

								text_buffer_offset.x = (1.0f +
									std::floor((1.0f + text_buffer_offset.x) / (tab_length))) * tab_length;

								const auto& s = m_data.m_font_size;
								const auto x1 = text_screen_position.x + previous_position_x + 1.0f;
								const auto x2 = text_screen_position.x + text_buffer_offset.x - 1.0f;
								const auto y = text_screen_position.y + text_buffer_offset.y + s * 0.5f;
								const ImVec2 p1(x1, y);
								const ImVec2 p2(x2, y);
								const ImVec2 p3(x2 - s * 0.2f, y - s * 0.2f);
								const ImVec2 p4(x2 - s * 0.2f, y + s * 0.2f);
								draw_list.AddLine(p1, p2, m_style.m_colors.at(color_palette_text_punctuation));
								draw_list.AddLine(p2, p3, m_style.m_colors.at(color_palette_text_punctuation));
								draw_list.AddLine(p2, p4, m_style.m_colors.at(color_palette_text_punctuation));

								++glyph_idx;
								break;
							}
							case ' ':
							{
								const auto& s = m_data.m_font_size;
								const auto x = text_screen_position.x + text_buffer_offset.x + m_data.m_space_size * 0.5f;
								const auto y = text_screen_position.y + text_buffer_offset.y + s * 0.5f;
								draw_list.AddCircleFilled(ImVec2(x, y), 1.5f, m_style.m_colors.at(color_palette_text_punctuation), 4);

								text_buffer_offset.x += m_data.m_space_size;

								++glyph_idx;
								break;
							}
							default:
							{
								auto length = glyph.m_utf8_char_length;
								while (length-- > 0)
								{
									m_data.m_current_string.push_back(line[glyph_idx++].m_char);
								}
								break;
							}
							}

							m_data.m_previous_color = glyph.m_color;
						}

						if (!m_data.m_current_string.empty())
						{
							const ImVec2 new_offset(text_screen_position.x, text_screen_position.y);
							draw_list.AddText(new_offset, m_style.m_colors.at(m_data.m_previous_color), m_data.m_current_string.c_str());

							m_data.m_current_string.clear();
						}
					}

					ImGui::Dummy(ImVec2(m_data.m_current_longest_line, m_backend.line_count() * m_data.m_character_advance.y));
				}
			}
		}

		//-------------------------------------------------------------------------------------------------------
		float ctext_editor::line_length(const text_line_t& line)
		{
			string_t s;

			for (const auto& glyph : line)
			{
				s.push_back(glyph.m_char);
			}

			return string_length(s.c_str());
		}

		//-------------------------------------------------------------------------------------------------------
		float ctext_editor::text_length(char c) const
		{
			return string_length(c);
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::load_from_file(const fs::cfileinfo& info)
		{
			if (const auto memory = fs::load_text_from_file(info.path()); memory && !memory->empty())
			{
				m_loaded_text_file = info.path();

				//- Copy source file text to buffer
				m_backend.set_source((const char*)memory->data());
				m_text_editor_reference.SetText((const char*)memory->data());
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
				if (const auto status = fs::save_text_to_file(info.path(), m_backend.source().data()); status != core::file_io_status_success)
				{
					log_error(fmt::format("Could not write edited text to source file '{}'", info.path()));
				}
			}
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::update()
		{
			//- update common variables
			{
				const auto p = ImGui::GetWindowPos();
				const auto s = ImGui::GetWindowSize();
				const auto m = ImGui::GetMousePos();
				const auto c = ImGui::GetCursorScreenPos();
				const auto focused = ImGui::IsWindowFocused();
				const auto font_size = ImGui::GetFontSize();

				m_data.m_viewport.m_x = p.x;
				m_data.m_viewport.m_y = p.y;
				m_data.m_viewport.m_w = s.x;
				m_data.m_viewport.m_h = s.y;
				m_data.m_mouse_position = { m.x, m.y };
				m_data.m_cursor_position = { c.x, c.y };
				m_data.m_window_focused = focused;
				m_data.m_font_size = font_size;
				m_data.m_character_advance = vec2_t(m_data.m_font_size, ImGui::GetTextLineHeightWithSpacing() * m_data.m_line_spacing);
				m_data.m_space_size = string_length(' ');
			}

			//- Do update source text with new received actions input
			if (handle_io() && dirty())
			{
			}

			if (!m_selected_text_file.empty())
			{
				load_from_file({ m_selected_text_file });

				m_selected_text_file.clear();
			}

			m_backend.update();
		}

		//-------------------------------------------------------------------------------------------------------
		bool ctext_editor::handle_io()
		{
			return false;
		}

	} //- textedit

} //- editor