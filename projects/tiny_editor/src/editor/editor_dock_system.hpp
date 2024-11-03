#pragma once
#include <core.h>
#include "docks/editor_main_menu_dock.hpp"
#include "docks/editor_dock_bottom_panel.hpp"
#include "docks/editor_dock_left_panel.hpp"
#include "docks/editor_dock_right_panel.hpp"
#include "docks/editor_entity_context_panel.hpp"

namespace editor
{
	//- The dock system is the central entry point into the editor. The update and ui render are started from here and
	//- propagate from base panels down to more basic ui entities
	//------------------------------------------------------------------------------------------------------------------------
	class cdock_system final
	{
	public:
		STATIC_INSTANCE_EX(cdock_system);

		static void on_update(float dt);
		static void on_world_render();
		static void on_ui_render();
		static void on_post_update(float dt);

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

		deque_t<layer_ref_t>& docks() { return m_docks; }

	private:
		scontext m_ctx; //- The real context structure distributed between context holders
		deque_t<layer_ref_t> m_docks;

	private:
		void update_context(float dt);
		inline const scontext& ctx() const { return m_ctx; }
		inline scontext& ctx() { return m_ctx; }

	};

	//------------------------------------------------------------------------------------------------------------------------
	template<class TDock>
	bool cdock_system::push_back()
	{
		static_assert(std::is_base_of_v<clayer_base, TDock>, "TDock must be derived from clayer_base class");

		auto dock = std::make_shared<TDock>(ctx());

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

		auto dock = std::make_shared<TDock>(ctx());

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

		auto dock = std::make_shared<TDock>(ctx(), std::forward<ARGS>(args)...);

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

		auto dock = std::make_shared<TDock>(ctx(), std::forward<ARGS>(args)...);

		if (dock->init())
		{
			m_docks.push_front(std::move(dock));

			return true;
		}
		return false;
	}

} //- editor