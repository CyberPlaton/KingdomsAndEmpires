#pragma once
#include <core.h>
#include <spritemancer.h>

namespace editor::ui
{
	//- Class responsible for creating and showing a basic window.
	//------------------------------------------------------------------------------------------------------------------------
	class cwindow final
	{
	public:
		using window_display_callback_t = core::cfunction<bool()>;

		cwindow(bool* open, stringview_t id = {}, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
		~cwindow();

		cwindow& title(stringview_t text);
		cwindow& icon(const char* text);
		cwindow& tooltip(stringview_t text);
		cwindow& callback(window_display_callback_t&& cb);

		bool draw();

	private:
		window_display_callback_t m_callback;
		string_t m_id;
		stringview_t m_title;
		stringview_t m_tooltip;
		stringview_t m_icon;
		ImGuiWindowFlags m_flags;
		bool* m_open;
		bool m_result = false;
	};

} //- editor::ui