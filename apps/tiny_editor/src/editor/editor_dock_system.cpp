#include "editor_dock_system.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	void cdock_system::pop_back()
	{
		const auto& dock = m_docks.back();

		dock->shutdown();

		m_docks.pop_back();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cdock_system::pop_front()
	{
		auto dock = m_docks.front();

		dock->shutdown();

		m_docks.pop_front();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cdock_system::on_update(float dt)
	{
		//- Set central imgui and other contexts
		ImGui::SetCurrentContext(imgui::imgui_ctx());

		//- Update central context data where known and wanted
		cdock_system::instance().update_context(dt);

		for (auto& d : cdock_system::instance().docks())
		{
			d->on_update(dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cdock_system::on_world_render()
	{
		for (auto& d : cdock_system::instance().docks())
		{
			d->on_world_render();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cdock_system::on_ui_render()
	{
		for (auto& d : cdock_system::instance().docks())
		{
			d->on_ui_render();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cdock_system::on_post_update(float dt)
	{
		for (auto& d : cdock_system::instance().docks())
		{
			d->on_post_update(dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	deque_t<layer_ref_t>& cdock_system::docks()
	{
		return m_docks;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cdock_system::update_context(float dt)
	{
		auto& context = ctx();

		//- As of now there is only one main window, so just query directly
		context.m_window_width = raylib::GetScreenWidth();
		context.m_window_height = raylib::GetScreenHeight();
	}

} //- editor