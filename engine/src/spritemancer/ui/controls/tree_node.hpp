#pragma once
#include "control.hpp"

namespace ui
{
	//------------------------------------------------------------------------------------------------------------------------
	class ctreenode : public detail::icontrol
	{
	public:
		ctreenode() = default;
		~ctreenode() = default;

		ctreenode& id(stringview_t _id);
		ctreenode& title(stringview_t _title);
		ctreenode& tooltip(stringview_t _tooltip);
		ctreenode& flags(ImGuiTreeNodeFlags _flags);
		ctreenode& icon(stringview_t _icon);
		ctreenode& image(stringview_t _texture);

	protected:
		detail::icontrol::click_result show_ui() override;

	private:
		ImGuiTreeNodeFlags m_flags = 0;
	};

} //- ui