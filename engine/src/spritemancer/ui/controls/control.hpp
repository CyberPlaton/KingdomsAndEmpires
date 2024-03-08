#pragma once
#include "../../sm_common.hpp"
#include "../../imgui_integration/imgui_integration.hpp"

namespace ui
{
	class ccontrol;

	namespace scope
	{
		//------------------------------------------------------------------------------------------------------------------------
		class cwrap_text
		{
		public:
			cwrap_text(size_t text_length);
			cwrap_text(const std::string& text);
			~cwrap_text();
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cdisabled
		{
		public:
			cdisabled();
			~cdisabled();
		};

		//------------------------------------------------------------------------------------------------------------------------
		template<typename TVariableType>
		class cstyle_variable
		{
		public:
			cstyle_variable(ImGuiStyleVar var, TVariableType value)
			{
				ImGui::PushStyleVar(var, value);
			}
			~cstyle_variable()
			{
				ImGui::PopStyleVar();
			}
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cstyle_color
		{
		public:
			cstyle_color(ImGuiCol var, core::scolor color);
			~cstyle_color();
		};

		//------------------------------------------------------------------------------------------------------------------------
		class cid
		{
		public:
			cid(ImGuiID id);
			~cid();
		};

	} //- scope

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		class icontrol
		{
			friend class ccontrol;
		public:
			icontrol();
			virtual ~icontrol() {};

			bool show();
			operator bool() { return show(); }

		protected:
			virtual bool show_ui() = 0;

		private:
			std::string m_id;
			std::string m_title;
			std::string m_tooltip;
			std::string m_icon;
			bool m_active;

		private:
			void show_tooltip();
		};

	} //- detail

	//- base class for implementing controls like drag control, string input control etc.
	//------------------------------------------------------------------------------------------------------------------------
	class ccontrol : public detail::icontrol
	{
	public:
		virtual ~ccontrol() {}

	protected:
		decltype(auto) id(stringview_t _id) { m_id = _id; return *this; }
		decltype(auto) title(stringview_t _title) { m_title = _title; return *this; }
		decltype(auto) tooltip(stringview_t _tooltip) { m_tooltip = _tooltip; return *this; }
		decltype(auto) icon(stringview_t _icon) { m_icon = _icon; return *this; }
	};

} //- ui