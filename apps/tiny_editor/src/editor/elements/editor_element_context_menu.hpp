#pragma once
#include <core.h>
#include <engine.h>
#include "../docks/editor_dock_base.hpp"

namespace editor::ui
{
	//- An extendable context menu, or popup in imgui terms, that is opened and allows for some actions to occurr.
	//- Employs factory pattern to create menues.
	//------------------------------------------------------------------------------------------------------------------------
	class ccontext_menu
	{
	public:
		using item_callback_t = std::function<void()>;

		ccontext_menu(stringview_t id, scontext& ctx, ImGuiWindowFlags flags = ImGuiWindowFlags_None);
		~ccontext_menu();

		ccontext_menu& begin(stringview_t label, const bool enabled = true);
		ccontext_menu& end();

		ccontext_menu& item(stringview_t label, bool* selected, item_callback_t&& callback, const bool enabled = true);

		void draw();

	private:
		enum entry_type : uint8_t
		{
			entry_type_none = 0,
			entry_type_menu,
			entry_type_item,
		};

		struct sentry
		{
			vector_t<ref_t<sentry>> m_entries;
			item_callback_t m_callback			= nullptr;
			ref_t<sentry> m_parent				= nullptr;
			stringview_t m_label;
			bool m_enabled						= false;
			bool* m_selected					= nullptr;
			entry_type m_type					= entry_type_none;
		};

		scontext& m_ctx;
		ref_t<sentry> m_root;
		ref_t<sentry> m_current;
		stringview_t m_id;
		ImGuiWindowFlags m_flags;

	private:
		void show_submenu(const vector_t<ref_t<sentry>>& entries);
	};

} //- editor::ui