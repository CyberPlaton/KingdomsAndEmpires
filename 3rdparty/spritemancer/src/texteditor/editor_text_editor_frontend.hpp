#pragma once
#include <core.h>
#include "../detail/sm_config.hpp"
#include "editor_text_editor_backend.hpp"

namespace sm
{
	namespace textedit
	{
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
			ref_t<cbackend> m_backend;
			coordinate_t m_cursor;		//- position where we write the next character

		private:
			void prepare();
			void show_main_menu();
			void show_main_window();
			void finish();

			void draw_text(float x, float y, const core::scolor& color,
				const char* begin, const char* end = (const char*)0);
			void draw_char(float x, float y, const core::scolor& color, char c);
		};


	} //- textedit

} //- sm