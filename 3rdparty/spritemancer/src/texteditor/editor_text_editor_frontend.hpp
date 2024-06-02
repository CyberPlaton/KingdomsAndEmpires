#pragma once
#include <core.h>
#include "../detail/sm_config.hpp"
#include "editor_text_editor_backend.hpp"

namespace sm
{
	namespace textedit
	{
		//-------------------------------------------------------------------------------------------------------
		struct sstyle
		{
			static sstyle default_style();

			enum color_palette : uint8_t
			{
				//- color palette for source code highlighting
				//- Note: must be in sync with highlight_token enum.
				color_palette_none = 0,
				color_palette_normal,
				color_palette_comment,
				color_palette_multiline_comment,
				color_palette_string,
				color_palette_number,
				color_palette_match,
				color_palette_function,
				color_palette_constant,
				color_palette_field,
				color_palette_primary_keyword,
				color_palette_secondary_keyword,
				color_palette_primary_special_character,
				color_palette_secondary_special_character,

				//- color palette for UI and editor elements
				color_palette_line_number,
				color_palette_background,
				color_palette_cursor,
				color_palette_selection,
				color_palette_current_line_fill,
				color_palette_current_line_edge,

				color_palette_count = color_palette_current_line_edge + 1,
			};

			enum flags : uint32_t
			{
				flags_none = 0,
				flags_show_whitespaces = BIT(1),
			};

			unsigned token_color(highlight_token highlight);
			unsigned palette_color(color_palette i);

			vector_t<unsigned> m_palette;
			float m_whitespace_size = 1.0f;
			float m_tab_size = 4.0f;
			int m_flags = flags_show_whitespaces;
		};

		//- Frontend of a text editor. Responsible for displaying the source in a ImGui window,
		//- handling and translating user input to meaningful actions within the frontend and backend.
		//-
		//- Does not create a window, must be contained within a parent scope. Note that handling
		//- keyboard and mouse input is done in draw() because of immediate mode ImGui.
		//-------------------------------------------------------------------------------------------------------
		class cfrontend
		{
		public:
			cfrontend(const ref_t<cbackend>& backend);
			~cfrontend();

			bool init();
			void shutdown();

			void draw();

		private:
			sstyle m_style;
			ref_t<cbackend> m_backend;
			coordinate_t m_cursor;		//- position where we write the next character

		private:
			void prepare();
			void show_main_menu();
			void show_main_window();
			void show_row(unsigned r);
			void finish();

			void draw_text(unsigned row, unsigned column, unsigned color,
				const char* begin, const char* end = (const char*)0);

			void draw_char(unsigned row, unsigned column, unsigned color, char c);

			void draw_text(float x, float y, unsigned color,
				const char* begin, const char* end = (const char*)0);

			void draw_char(float x, float y, unsigned color, char c);

			void draw_whitespace(unsigned row, unsigned column, unsigned color);
			void draw_tab(unsigned row, unsigned column, unsigned color);
		};


	} //- textedit

} //- sm