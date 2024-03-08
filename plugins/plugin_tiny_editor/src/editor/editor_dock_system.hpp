#pragma once
#include <core.h>
#include "docks/editor_main_menu_dock.hpp"
#include "docks/editor_dock_bottom_panel.hpp"
#include "docks/editor_dock_left_panel.hpp"
#include "docks/editor_dock_right_panel.hpp"

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class cdock_system : public ccontext
	{
	public:
		STATIC_INSTANCE(cdock_system, s_cdock_system);

		template<class TDock>
		bool push_back();

		template<class TDock>
		bool push_front();

		template<class TDock, typename... ARGS>
		bool push_back(ARGS&&... args);

		template<class TDock, typename... ARGS>
		bool push_front(ARGS&&... args);

		void pop_back();

		void pop_front();

		static void on_update(float dt);
		static void on_world_render();
		static void on_ui_render();
		static void on_post_update(float dt);

		std::deque<ref_t<clayer_base>>& docks();

	private:
		std::deque<ref_t<clayer_base>> m_docks;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TDock>
	bool cdock_system::push_back()
	{
		static_assert(std::is_base_of_v<clayer_base, TDock>, "TDock must be derived from clayer_base class");

		auto dock = std::make_shared<TDock>(*this);

		if (dock->init())
		{
			m_docks.push_back(std::move(dock));

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TDock>
	bool cdock_system::push_front()
	{
		static_assert(std::is_base_of_v<clayer_base, TDock>, "TDock must be derived from clayer_base class");

		auto dock = std::make_shared<TDock>(*this);

		if (dock->init())
		{
			m_docks.push_front(std::move(dock));

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TDock, typename... ARGS>
	bool cdock_system::push_back(ARGS&&... args)
	{
		static_assert(std::is_base_of_v<clayer_base, TDock>, "TDock must be derived from clayer_base class");

		auto dock = std::make_shared<TDock>(*this, std::forward<ARGS>(args)...);

		if (dock->init())
		{
			m_docks.push_back(std::move(dock));

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<class TDock, typename... ARGS>
	bool cdock_system::push_front(ARGS&&... args)
	{
		static_assert(std::is_base_of_v<clayer_base, TDock>, "TDock must be derived from clayer_base class");

		auto dock = std::make_shared<TDock>(*this, std::forward<ARGS>(args)...);

		if (dock->init())
		{
			m_docks.push_front(std::move(dock));

			return true;
		}
		return false;
	}

} //- editor