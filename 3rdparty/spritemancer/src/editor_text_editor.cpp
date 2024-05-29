#include "editor_text_editor.hpp"

namespace sm
{
	namespace textedit
	{
		//-------------------------------------------------------------------------------------------------------
		ctext_editor::ctext_editor()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		ctext_editor::~ctext_editor()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::draw()
		{
			prepare();

			ImGui::SetNextWindowSize({ 500, 500 }, ImGuiCond_Appearing);
			ImGui::SetNextWindowPos({ 0, 0, }, ImGuiCond_Appearing);

			ImGui::Begin(m_title.c_str(), &m_showing, m_flags);

			ImGui::BeginChild("##container", { 0, 0 }, true, m_child_flags);
			show_main_window();
			ImGui::EndChild();
			ImGui::End();
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::set_language(language_type language)
		{
			switch (language)
			{
			case language_type_lua:
			{
				m_language = tree_sitter_lua();
				break;
			}
			case language_type_cpp:
			{
				m_language = tree_sitter_cpp();
				break;
			}
			case language_type_none:
			default:
			{
				//- no need to free memory, it is statically stack allocated
				m_language = nullptr;
				m_language_type = language_type_none;
				break;
			}
			}
			if (m_language_type != language_type_none)
			{
				retrieve_language_symbols(m_language);
			}
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::parse(const string_t& source)
		{
			m_source.assign(source.begin(), source.end());
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::parse()
		{
			//- assume tokenize function complete and source non-empty
			std::istringstream stream(m_source);
			token_type_t type = 0;
			token_type_palette_t color = 0;
			coordinate_t coords = { 0, 0 };
			std::string token;

			// 			for(std::string line; std::getline(stream, line);)
			// 			{
			// 				//- check if need to resize the line buffer
			// 				if (coords.y >= m_lines.size())
			// 				{
			// 					m_lines.resize(m_lines.size() == 0 ? 2 : m_lines.size() * 2);
			// 					m_lines_tokenized.resize(m_lines_tokenized.size() == 0 ? 2 : m_lines_tokenized.size() * 2);
			// 				}
			// 
			// 				//- assure line is not empty and there is something to parse
			// 				if (line.empty())
			// 				{
			// 					coords.y++;
			// 					continue;
			// 				}
			// 
			// 				coords.x = 0;
			// 				const char* begin = &line.front();
			// 				const char* end = begin + line.length();
			// 				const char* line_end = end;
			// 				for(auto start = begin; start != line_end;)
			// 				{
			// 					const char* token_begin = nullptr;
			// 					const char* token_end = nullptr;
			// 					bool tokenized = false;
			// 
			// 					//- find next token
			// 					if (m_language.m_tokenize(start, line_end, token_begin, token_end, token, type))
			// 					{
			// 						//- assign color to glyphs belonging to the token
			// 						tokenized = true;
			// 					}
			// 					
			// 					if (!tokenized)
			// 					{
			// 						//- unexpected character, tokenize to default
			// 						token.assign(start, start + 1);
			// 						log_info(fmt::format("Unexpected Token: Text: [{}], Type: [-], Coords: [{}:{}]", token, coords.x, coords.y).c_str());
			// 
			// 						m_lines_tokenized[coords.y].m_line.emplace_back(token, coordinate_t{ coords.x / token.length(), coords.y },
			// 							(token_type_palette_t)0xffff0000, (token_type_t)0);
			// 
			// 						for (auto ii = 0; ii < token.length(); ++ii)
			// 						{
			// 							Byte c = token[ii];
			// 							m_lines[coords.y].m_line.emplace_back(c, coordinate_t{ coords.x + ii, coords.y }, 0xffff0000, 0);
			// 						}
			// 
			// 						coords.x += token.length();
			// 						start += token.length();
			// 					}
			// 					else
			// 					{
			// 						auto token_length = token_end - token_begin;
			// 
			// 						log_info(fmt::format("Token: Text: [{}], Type: [{}], Coords: [{}:{}]", token, type, coords.x, coords.y).c_str());
			// 
			// 						m_lines_tokenized[coords.y].m_line.emplace_back(token, coordinate_t{ coords.x / token_length, coords.y }, m_language.m_token_type_colors[type], type);
			// 
			// 						for (auto ii = 0; ii < token_length; ++ii)
			// 						{
			// 							Byte c = token[ii];
			// 							m_lines[coords.y].m_line.emplace_back(c, coordinate_t{ coords.x + ii, coords.y }, m_language.m_token_type_colors[type], type);
			// 						}
			// 
			// 						coords.x += token_length;
			// 						start += token_length;
			// 					}
			// 				}
			// 				coords.y++;
			// 			}
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::prepare()
		{
			m_title = "Editor";
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::show_main_menu()
		{
			ImGui::BeginMenuBar();
			if (ImGui::BeginMenu("File"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::show_main_window()
		{
			auto window_size = ImGui::GetContentRegionAvail();
			auto* draw_list = ImGui::GetWindowDrawList();
			auto cursor_screen_position = ImGui::GetCursorScreenPos();
			auto scroll_x = ImGui::GetScrollX();
			auto scroll_y = ImGui::GetScrollY();
			const auto font_size = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "#", nullptr, nullptr).x;
			const auto char_advance = ImVec2(font_size, ImGui::GetTextLineHeightWithSpacing() * 1.0f);
			const auto text_start = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), FLT_MAX, -1.0f, "###", nullptr, nullptr).x + 1.0f;
			unsigned line_number = glm::floor(scroll_y / char_advance.y);
			ImVec2 glyph_offset = { 0.0f, 0.0f };

			//- compute required data
			const auto line_start_screen_position = ImVec2(cursor_screen_position.x, cursor_screen_position.y + line_number * char_advance.y);
			const auto text_screen_position = ImVec2(line_start_screen_position.x + text_start, line_start_screen_position.y);

			for (const auto& pair : m_language_symbol_names)
			{
				ImGui::Text(fmt::format("{}:{}\n", pair.first, pair.second).c_str());

				// 				const ImVec2 p = { text_screen_position.x + glyph_offset.x, text_screen_position.y + glyph_offset.y };
				// 				draw_list->AddText(p, 0xffffff99, fmt::format("{}:{}\n", pair.first, pair.second).c_str());
				// 
				// 				glyph_offset.y += char_advance.y;
				// 				glyph_offset.x = 0.0f;
			}

			// 			ImGui::Dummy({ window_size.x, m_language_symbol_names.size() * char_advance.y });

			// 			for (auto yy = 0; yy < m_lines.size(); ++yy) //- iterate over lines
			// 			{
			// 				const auto& line = m_lines[yy];
			// 
			// 				for (auto xx = 0; xx < line.m_line.size(); ++xx) //- iterate over columns
			// 				{
			// 					const auto& token = line.m_line[xx];
			// 
			// 					const ImVec2 p = { text_screen_position.x + glyph_offset.x, text_screen_position.y + glyph_offset.y };
			// 					draw_list->AddText(p, token.m_color, fmt::format("Empty").c_str());
			// 
			// 
			// 					glyph_offset.x += char_advance.x;
			// 				}
			// 				glyph_offset.y += char_advance.y;
			// 				glyph_offset.x = 0.0f;
			// 			}
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::retrieve_language_symbols(const TSLanguage* language)
		{
			m_language_symbol_names.clear();
			for (auto ii = 0; ii < language->symbol_count; ++ii)
			{
				auto symbol = language->public_symbol_map[ii];
				m_language_symbol_names[symbol] = ts_language_symbol_name(language, symbol);
			}
		}

	} //- textedit

} //- sm