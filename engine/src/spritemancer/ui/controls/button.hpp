#pragma once
#include "control.hpp"

namespace ui
{
	namespace detail
	{
	} //- detail

	//- usable as is class for a button. Can also be extended.
	//- Does return a verbose value to indicate which mouse button was pressed on it or
	//- a boolean value whether any button was pressed on it.
	//------------------------------------------------------------------------------------------------------------------------
	class cbutton : public detail::icontrol
	{
	public:
		cbutton() = default;
		~cbutton() = default;

		cbutton& id(stringview_t _id);
		cbutton& title(stringview_t _title);
		cbutton& tooltip(stringview_t _tooltip);
		cbutton& icon(stringview_t _icon);
		cbutton& image(stringview_t _texture);

	protected:
		detail::icontrol::click_result show_ui() override;
	};

} //- ui