#pragma once
#include "control.hpp"

namespace ui
{
	class cbutton;

	namespace detail
	{
		//------------------------------------------------------------------------------------------------------------------------
		class ibutton
		{
			friend class cbutton;
		public:
			ibutton();
			virtual ~ibutton() {};

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

	//- usable base class for a button. Can also be extended.
	//------------------------------------------------------------------------------------------------------------------------
	class cbutton : public detail::ibutton
	{
	public:
		cbutton() = default;
		~cbutton() {};

		cbutton& id(stringview_t _id);
		cbutton& title(stringview_t _title);
		cbutton& tooltip(stringview_t _tooltip);
		cbutton& icon(stringview_t _icon);

	protected:
		bool show_ui() override;
	};

} //- ui