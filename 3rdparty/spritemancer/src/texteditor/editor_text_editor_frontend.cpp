#include "editor_text_editor_frontend.hpp"

namespace sm
{
	namespace textedit
	{
		namespace
		{
			//-------------------------------------------------------------------------------------------------------
			enum special_chars : uint8_t
			{
				special_chars_none = 0,
				special_chars_hidden,	//- character that is not shown
				special_chars_space,	//- a white space character
				special_chars_tab,		//- a tabulator character
			};

			//-------------------------------------------------------------------------------------------------------
			vec2_t string_size(const char* begin, const char* end = (const char*)0)
			{
				if (end == 0) { end = begin + strlen(begin); }

				const auto vec = ImGui::GetFont()->CalcTextSizeA(ImGui::GetFontSize(), MAX(float), -1.0f, begin, end);

				return { vec.x, vec.y };
			}

			//-------------------------------------------------------------------------------------------------------
			vec2_t character_size(char c)
			{
				return string_size(&c);
			}

			//-------------------------------------------------------------------------------------------------------
			ImU32 imgui_color(const core::scolor& color)
			{
				const auto c = color.normalize();

				return ImGui::ColorConvertFloat4ToU32({ c.r, c.g, c.b, c.a });
			}

			//-------------------------------------------------------------------------------------------------------
			vec2_t row_column_to_screen_space(unsigned row, unsigned column)
			{
				const auto screenpos = ImGui::GetCursorScreenPos();
				const auto fontsize = ImGui::GetFontSize();
				const auto x = screenpos.x;
				const auto y = screenpos.y;

				const auto row_x = x + column * fontsize;
				const auto row_y = y + row * fontsize;

				return {row_x, row_y};
			}

			//-------------------------------------------------------------------------------------------------------
			ivec2_t visible_row_range(unsigned total_row_count)
			{
				const auto zoom = 1.0f;
				const auto fontsize = ImGui::GetFontSize(); //- does include font window scale
				const auto screenpos = ImGui::GetCursorScreenPos();
				const auto row_number = glm::floor(ImGui::GetScrollY() / fontsize);
				const auto row_we_at = row_number == 0 ? 0 : static_cast<int>(0.5f + screenpos.y / (row_number));
				const auto rows_we_see = static_cast<unsigned>(0.5f + ImGui::GetWindowContentRegionMax().y / (fontsize * zoom));
				const auto rows_to_draw_down = glm::min(total_row_count, row_we_at + rows_we_see);
				const auto rows_to_draw_up = glm::max(0, static_cast<int>(row_we_at - rows_we_see));

				return { rows_to_draw_up, rows_to_draw_down };
			}

			//-------------------------------------------------------------------------------------------------------
			bool is_space(char c)
			{
				return c == ' ';
			}

			//-------------------------------------------------------------------------------------------------------
			bool is_tab(char c)
			{
				return c == '\t';
			}

			//-------------------------------------------------------------------------------------------------------
			void append_one_char(string_t& string, char c)
			{
				string = fmt::format("{}{}", string, c);
			}

			//-------------------------------------------------------------------------------------------------------
			unsigned utf8_character_length(char c)
			{
				if ((c & 0xFE) == 0xFC)
					return 6;
				if ((c & 0xFC) == 0xF8)
					return 5;
				if ((c & 0xF8) == 0xF0)
					return 4;
				else if ((c & 0xF0) == 0xE0)
					return 3;
				else if ((c & 0xE0) == 0xC0)
					return 2;
				return 1;
			}

		} //- unnamed

		//-------------------------------------------------------------------------------------------------------
		sstyle sstyle::default_style()
		{
			static sstyle style;

			style.m_palette.resize((unsigned)color_palette_count);

			style.m_palette[color_palette_normal]						= imgui_color({ 150, 150, 150, 255 });
			style.m_palette[color_palette_none]							= style.m_palette[color_palette_normal];
			style.m_palette[color_palette_comment]						= imgui_color({ 8, 64, 62, 255 });
			style.m_palette[color_palette_multiline_comment]			= imgui_color({ 8, 64, 62, 255 });
			style.m_palette[color_palette_string]						= imgui_color({ 150, 43, 9, 255 });
			style.m_palette[color_palette_number]						= imgui_color({ 159, 193, 39, 255 });
			style.m_palette[color_palette_match]						= imgui_color({ 214, 213, 142, 255 });
			style.m_palette[color_palette_function]						= imgui_color({ 181, 113, 20, 255 });
			style.m_palette[color_palette_constant]						= imgui_color({ 255, 255, 255, 255 });
			style.m_palette[color_palette_field]						= imgui_color({ 219, 242, 39, 255 });
			style.m_palette[color_palette_primary_keyword]				= imgui_color({ 215, 5, 242, 255 });
			style.m_palette[color_palette_secondary_keyword]			= imgui_color({ 254, 96, 126, 255 });
			style.m_palette[color_palette_primary_special_character]	= imgui_color({ 250, 127, 8, 255 });
			style.m_palette[color_palette_secondary_special_character]	= imgui_color({ 255, 92, 83, 255 });
			style.m_palette[color_palette_line_number]					= imgui_color({ 159, 193, 39, 255 });
			style.m_palette[color_palette_background]					= imgui_color({ 255, 255, 255, 255 });
			style.m_palette[color_palette_cursor]						= imgui_color({ 255, 255, 255, 255 });
			style.m_palette[color_palette_selection]					= imgui_color({ 255, 255, 255, 255 });
			style.m_palette[color_palette_current_line_fill]			= imgui_color({ 255, 255, 255, 255 });
			style.m_palette[color_palette_current_line_edge]			= imgui_color({ 255, 255, 255, 255 });

			return style;
		}

		//-------------------------------------------------------------------------------------------------------
		unsigned sstyle::token_color(highlight_token highlight)
		{
			return palette_color((color_palette)highlight);
		}

		//-------------------------------------------------------------------------------------------------------
		unsigned sstyle::palette_color(color_palette i)
		{
			CORE_ASSERT((unsigned)i < (unsigned)color_palette_count, "Invalid operation. Index is out of bound");

			return m_palette[(unsigned)i];
		}

		//-------------------------------------------------------------------------------------------------------
		cfrontend::cfrontend(const ref_t<cbackend>& backend) :
			m_backend(backend), m_cursor({0, 0})
		{
		}

		//-------------------------------------------------------------------------------------------------------
		cfrontend::~cfrontend()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		bool cfrontend::init()
		{
			m_style = sstyle::default_style();

			return true;
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::shutdown()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw()
		{
			prepare();

			if (ImGui::BeginChild("##container"))
			{
				show_main_window();

				ImGui::EndChild();
			}

			finish();
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::prepare()
		{
			//- If styling has changed, reset required variables
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::show_main_menu()
		{

		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::show_main_window()
		{
			m_backend->update();

			const auto row_range = visible_row_range(m_backend->row_count());

			//- rows to draw start such that our cursor is always in the middle
			for (auto i = row_range.x; i < row_range.y; ++i)
			{
				show_row(i);
			}
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::show_row(unsigned r)
		{
			const auto draw_list = ImGui::GetWindowDrawList();
			const auto fontsize = ImGui::GetFontSize();
			const auto& source = m_backend->row_at(r);
			string_t line; line.reserve(128);
			unsigned color = m_style.token_color(highlight_token_normal);
			vec2_t line_buffer_offset = {0.0f, 0.0f};
			const auto screenpos = row_column_to_screen_space(r, 1);
			const auto spacesize = character_size(' ').x;
			const auto tabsize = m_style.m_tab_size;

			//- iterate over all characters
			for (auto i = 0u; i < source.m_text.length();)
			{
				auto c = source.m_text[i];
				const auto char_color = m_style.token_color(source.m_highlight[i]);

				//- draw accumulated word if anything changed
				if ((char_color != color || is_space(c) || is_tab(c)) && !line.empty())
				{
					//- display accumulated text
					const auto x = screenpos.x + line_buffer_offset.x;
					const auto y = screenpos.y + line_buffer_offset.y;

					draw_text(x, y, color, line.data());

					const auto text_size = string_size(line.data());

					line_buffer_offset.x += text_size.x;

					line.clear();
				}

				color = char_color;

				//- handle whitespaces and special characters
				switch (c)
				{
				case '\t':
				{
					const auto previuos_x = line_buffer_offset.x;
					line_buffer_offset.x = (1.0f + glm::floor((1.0f + line_buffer_offset.x) / (tabsize * spacesize))) * (tabsize * spacesize);

					if (algorithm::bit_check(m_style.m_flags, sstyle::flags_show_whitespaces))
					{
						const auto x1 = screenpos.x + previuos_x + 1.0f;
						const auto x2 = screenpos.x + line_buffer_offset.x - 1.0f;
						const auto y = screenpos.y + line_buffer_offset.y + fontsize * 0.5f;
						const ImVec2 p1(x1, y);
						const ImVec2 p2(x2, y);
						const ImVec2 p3(x2 - fontsize * 0.2f, y - fontsize * 0.2f);
						const ImVec2 p4(x2 - fontsize * 0.2f, y + fontsize * 0.2f);
						draw_list->AddLine(p1, p2, 0xffeeffff);
						draw_list->AddLine(p2, p3, 0xffeeffff);
						draw_list->AddLine(p2, p4, 0xffeeffff);
					}

					++i;
					break;
				}
				case ' ':
				{
					if (algorithm::bit_check(m_style.m_flags, sstyle::flags_show_whitespaces))
					{
						const auto x = screenpos.x + line_buffer_offset.x + spacesize * 0.5f;
						const auto y = screenpos.y + line_buffer_offset.y + fontsize * 0.5f;
						draw_list->AddCircleFilled(ImVec2(x, y), 1.5f, 0x77ffbbff, 4);
					}

					line_buffer_offset.x += spacesize;
					++i;
					break;
				}
				//- non-whitespace character
				default:
				{
					auto length = utf8_character_length(c);

					while(length-- > 0)
					{
						append_one_char(line, source.m_text[i++]);
					}

					break;
				}
				}
			}

			//- handle case where we are at line end but have to draw last word in buffer
			if (!line.empty())
			{
				const auto x = screenpos.x + line_buffer_offset.x;
				const auto y = screenpos.y + line_buffer_offset.y;

				draw_list->AddText({x, y}, color, line.data());
			}
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::finish()
		{

		}

		//- Draw a text at screen space position x and y
		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw_text(float x, float y, unsigned color,
			const char* begin, const char* end /*= (const char*)0*/)
		{
			const auto draw_list = ImGui::GetWindowDrawList();

			draw_list->AddText({x, y}, color, begin, end);
		}

		//- Draw a text at given row and column
		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw_text(unsigned row, unsigned column, unsigned color,
			const char* begin, const char* end /*= (const char*)0*/)
		{
			const auto position = row_column_to_screen_space(row, column);

			draw_text(position.x, position.y, color, begin, end);
		}

		//- Draw a single character at screen space position x and y
		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw_char(float x, float y, unsigned color, char c)
		{
			const char* start = &c;
			const char* end = start + sizeof(char);

			draw_text(x, y, color, start, end);
		}

		//- Draw a single character at given row and column
		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw_char(unsigned row, unsigned column, unsigned color, char c)
		{
			const char* start = &c;
			const char* end = start + sizeof(char);

			draw_text(row, column, color, start, end);
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw_whitespace(unsigned row, unsigned column, unsigned color)
		{
			const auto draw_list = ImGui::GetWindowDrawList();

			const auto position = row_column_to_screen_space(row, column);

			draw_list->AddCircleFilled({position.x, position.y}, 1.5f, color, 4);
		}

		//-------------------------------------------------------------------------------------------------------
		void cfrontend::draw_tab(unsigned row, unsigned column, unsigned color)
		{
			const auto draw_list = ImGui::GetWindowDrawList();
			const auto fontsize = ImGui::GetFontSize();
			const auto position = row_column_to_screen_space(row, column);

			vec2_t from = {0.0f, 0.0f }, to = { 0.0f, 0.0f };

			//- draw three lines to depict an arrow pointing right, akin to VS2022 tab arrows
			//- center line
			from = { position.x, position.y};
			to = { position.x + fontsize, position.y };
			draw_list->AddLine({from.x, from.y}, {to.x, to.y}, color);

			//- right to top backwards
			from = { position.x + fontsize, position.y };
			to = { (position.x + fontsize - position.x) / 2.0f, position.y - fontsize };
			draw_list->AddLine({ from.x, from.y }, { to.x, to.y }, color);

			//- right to bottom backwards
			from = { position.x + fontsize, position.y };
			to = { (position.x + fontsize - position.x) / 2.0f, position.y + fontsize };
			draw_list->AddLine({ from.x, from.y }, { to.x, to.y }, color);
		}

	} //- textedit

} //- sm