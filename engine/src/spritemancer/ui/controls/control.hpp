#pragma once
#include "../../sm_common.hpp"
#include "../../imgui_integration/imgui_integration.hpp"

namespace ui
{
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

		//------------------------------------------------------------------------------------------------------------------------
		class cindent
		{
		public:
			cindent(unsigned spaces);
			~cindent();
		};

	} //- scope

	namespace detail
	{
		//- Note: make sure that, when using show() explicitly to check for desired return value.
		//------------------------------------------------------------------------------------------------------------------------
		class icontrol
		{
		public:
			enum click_result : int8_t
			{
				click_result_none = -1,
				click_result_lmb = ImGuiMouseButton_Left,
				click_result_rmb = ImGuiMouseButton_Right,
				click_result_mmb = ImGuiMouseButton_Middle,
				click_result_lmb_repeated,
				click_result_rmb_repeated,
				click_result_mmb_repeated,
			};

			icontrol();
			virtual ~icontrol();

			click_result show();
			operator bool() { return show() != click_result_none; }

		protected:
			//- to be implemented by deriving control class
			virtual click_result show_ui();

			ImGuiID imgui_id() const;
			void set_image(texture_t handle);
			click_result mousebutton_state();

		protected:
			string_t m_id;
			string_t m_title;
			string_t m_tooltip;
			string_t m_icon;
			texture_t m_image;
			bool m_active;

		private:
			void show_tooltip();
		};

	} //- detail

	//- base class for implementing controls like drag control, string input control etc.
	//------------------------------------------------------------------------------------------------------------------------
	template<class TControlType>
	class ccontrol : public detail::icontrol
	{
	public:
		ccontrol();
		virtual ~ccontrol();

	protected:
		auto id(stringview_t _id)			{ m_id = _id; return _this(); }
		auto title(stringview_t _title)		{ m_title = _title; return _this(); }
		auto tooltip(stringview_t _tooltip) { m_tooltip = _tooltip; return _this(); }
		auto icon(stringview_t _icon)		{ m_icon = _icon; return _this(); }
		auto image(stringview_t _texture)	{ m_image = _texture; return _this(); }

	private:
		auto _this() { return *SCAST(TControlType, this); }
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TControlType>
	ui::ccontrol<TControlType>::~ccontrol()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TControlType>
	ui::ccontrol<TControlType>::ccontrol()
	{
	}

} //- ui
