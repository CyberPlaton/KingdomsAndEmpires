#include "editor_text_editor.hpp"

namespace sm
{
	namespace textedit
	{
		//-------------------------------------------------------------------------------------------------------
		ctext_editor::ctext_editor()
		{
			m_backend = std::make_shared<cbackend>();
			m_frontend = std::make_shared<cfrontend>(m_backend);
		}

		//-------------------------------------------------------------------------------------------------------
		ctext_editor::~ctext_editor()
		{
		}

		//-------------------------------------------------------------------------------------------------------
		bool ctext_editor::init(language_type lang, const string_t& source)
		{
			return m_backend->init(lang, source) && m_frontend->init();
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::shutdown()
		{
			m_frontend->shutdown();
			m_backend->shutdown();
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::draw()
		{
			m_frontend->draw();
		}

		//-------------------------------------------------------------------------------------------------------
		void ctext_editor::update()
		{
			m_backend->update();
		}

	} //- textedit

} //- sm