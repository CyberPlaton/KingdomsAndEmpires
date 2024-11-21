#pragma once
#include <core.h>

namespace kingdoms
{
	class icommand_system;
	struct sgame_context;

	//- Command system interface. Responsible for gathering user commands to be executed and alter the game state in a
	//- distinct way. E.g. entities Attacking other entities is a command type.
	//------------------------------------------------------------------------------------------------------------------------
	class ccommand_system
	{
	public:
		ccommand_system() = default;
		virtual ~ccommand_system() = default;

		void on_update(sgame_context& ctx, float dt);

		bool push(rttr::type action_type);
		bool push(stringview_t action_type_name);
		template<typename TAction>
		bool push();

	protected:
		queue_t<rttr::variant> m_command_queue;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TAction>
	bool ccommand_system::push()
	{
		return push(rttr::type::get<TAction>());
	}

	//------------------------------------------------------------------------------------------------------------------------
	struct sgame_context final
	{
		core::cuuid m_selected_entity;	//- The entity with which the action is performed
		core::cuuid m_target_entity;	//- The entity at which the action is targeted

		ccommand_system m_command_system;
	};

} //- kingdoms