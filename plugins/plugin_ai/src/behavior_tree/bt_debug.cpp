#include "bt_debug.hpp"

namespace ai::bt::debug
{
	//------------------------------------------------------------------------------------------------------------------------
	cbehavior_tree_debugger::cbehavior_tree_debugger()
	{
		engine::cengine::service<engine::cevent_service>()->emplace_listener<snode_tick_event>(
			[this](const rttr::variant& var)
			{
				const auto& e = var.convert<snode_tick_event>();

				core::cscope_mutex m(m_mutex);

				//- quick emplace data for later conversion
			});

		engine::cengine::service<engine::cevent_service>()->emplace_listener<snode_attach_event>(
			[this](const rttr::variant& var)
			{
				const auto& e = var.convert<snode_attach_event>();

				core::cscope_mutex m(m_mutex);

				//- quick emplace data for later conversion
			});

		engine::cengine::service<engine::cevent_service>()->emplace_listener<sdecorator_attach_event>(
			[this](const rttr::variant& var)
			{
				const auto& e = var.convert<sdecorator_attach_event>();

				core::cscope_mutex m(m_mutex);

				//- quick emplace data for later conversion
			});

	}

} //- namespace ai::bt::debug