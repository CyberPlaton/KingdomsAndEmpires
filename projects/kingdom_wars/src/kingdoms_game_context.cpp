#include "kingdoms_game_context.hpp"
#include "kingdoms/actions/kingdoms_action.hpp"

namespace kingdoms
{
	//------------------------------------------------------------------------------------------------------------------------
	void ccommand_system::on_update(sgame_context& ctx, float dt)
	{
		while (!m_command_queue.empty())
		{
			auto& var = m_command_queue.front(); m_command_queue.pop();

			if (var.can_convert<iaction>())
			{
				auto& a = var.get_value<iaction>();

				a.do_execute(ctx, dt);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ccommand_system::push(rttr::type action_type)
	{
		if (action_type.is_valid())
		{
			m_command_queue.push(action_type.create({}));

			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ccommand_system::push(stringview_t action_type_name)
	{
		return push(rttr::type::get_by_name(action_type_name.data()));
	}

} //- kingdoms