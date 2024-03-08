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
	std::deque<ref_t<clayer_base>>& cdock_system::docks()
	{
		return m_docks;
	}

} //- editor