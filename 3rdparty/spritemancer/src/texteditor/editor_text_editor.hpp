#pragma once
#include "editor_text_editor_backend.hpp"
#include "editor_text_editor_frontend.hpp"

namespace sm
{
	namespace textedit
	{
		//- Text editor for one piece of source code.
		//-------------------------------------------------------------------------------------------------------
		class ctext_editor
		{
		public:
			ctext_editor();
			~ctext_editor();

			bool init(language_type lang, const string_t& source);
			void shutdown();

			void update();
			void draw();

			//- visual representation

			//- text editor functionality

			//- utility

		private:
			ref_t<cbackend> m_backend;
			ref_t<cfrontend> m_frontend;
		};

	} //- textedit

} //- sm