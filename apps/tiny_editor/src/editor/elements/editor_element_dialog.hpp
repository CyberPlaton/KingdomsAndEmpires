#pragma once
#include <core.h>
#include <engine.h>
#include <spritemancer.h>

namespace editor::ui
{
	//- Class responsible for creating smaller dialog windows between user and editor.
	//------------------------------------------------------------------------------------------------------------------------
	class cdialog final
	{
	public:
		using dialog_display_callback_t = core::cfunction<void()>;
		using dialog_button_callback_t = core::cfunction<void()>;

		cdialog(bool* open, stringview_t id = {}, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
		~cdialog();

		cdialog& title(stringview_t text);
		cdialog& icon(const char* text);
		cdialog& tooltip(stringview_t text);
		cdialog& callback(dialog_display_callback_t&& cb);
		cdialog& confirm_button(stringview_t text, dialog_button_callback_t&& on_clicked);
		cdialog& cancel_button(stringview_t text, dialog_button_callback_t&& on_clicked);

		bool draw();

	private:
		dialog_display_callback_t m_callback;
		dialog_button_callback_t m_confirm_button;
		dialog_button_callback_t m_cancel_button;
		string_t m_id;
		stringview_t m_title;
		stringview_t m_tooltip;
		stringview_t m_icon;
		stringview_t m_confirm_button_text;
		stringview_t m_cancel_button_text;
		ImGuiWindowFlags m_flags;
		bool* m_open;
		bool m_result = false;
	};

} //- editor::ui