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
			enum flags : uint32_t
			{
				flags_none = 0,
				flags_show_whitespaces = BIT(1),
			};

			unsigned to_color(highlight_token highlight);

			float m_whitespace_size = 1.0f;
			float m_tab_size = 4.0f;
			flags m_flags = flags_show_whitespaces;
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