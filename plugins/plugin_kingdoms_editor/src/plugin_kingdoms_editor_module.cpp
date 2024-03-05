#include "plugin_kingdoms_editor_module.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	bool ceditor::init()
	{
		auto& docks = cdock_system::instance();

		bool result = true;

		result &= docks.push_back<cmain_menu>();
		result &= docks.push_back<cbottom_panel>();
		result &= docks.push_back<cleft_panel>();
		result &= docks.push_back<cright_panel>();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::shutdown()
	{
		auto& docks = cdock_system::instance();

		while (!docks.docks().empty())
		{
			docks.pop_back();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_update(float dt)
	{
		cdock_system::on_update(dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_world_render()
	{
		cdock_system::on_world_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_ui_render()
	{
		cdock_system::on_ui_render();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ceditor::on_post_update(float dt)
	{
		cdock_system::on_post_update(dt);
	}

} //- editor