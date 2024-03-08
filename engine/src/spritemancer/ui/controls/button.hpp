#pragma once
#include "control.hpp"

namespace ui
{
	namespace detail
	{
	} //- detail

	//- usable base class for a button. Can also be extended.
	//------------------------------------------------------------------------------------------------------------------------
	class cbutton : public detail::icontrol
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