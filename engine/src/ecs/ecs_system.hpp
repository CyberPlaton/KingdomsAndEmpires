#pragma once
#include "ecs_world_context_holder.hpp"
#include "ecs_singleton.hpp"

namespace ecs
{
	//------------------------------------------------------------------------------------------------------------------------
	enum system_running_phase
	{
		system_running_phase_none = 0,
		system_running_phase_on_update,
		system_running_phase_on_world_render,
		system_running_phase_on_ui_render,
		system_running_phase_on_post_update, //- Note: fixed dt. After all rendering is done.
	};

	//------------------------------------------------------------------------------------------------------------------------
	class isystem
	{
	public:
		virtual flecs::entity self() const = 0;
		virtual stringview_t name() const = 0;
	};

	//- system interface. Note that flecs::system is basically an entity,
	//- we take advantage of this by assigning a name to it.
	//-
	//- when defining used components do
	//- class cexample : public csystem<ecs::stransform, ecs::sidentifier>
	//-
	//- what really matters for runtime is the 'build' function:
	//- you can define const ecs::stransform& there as well as ecs::stransform&
	//- and those as what will be passed to the function.
	//-
	//- TCallable system function should be as in system_function_prototype_t, i.e.
	//- entity first and then your used components as described above.
	//-
	//- Note: it is possible to force a system to run on more than one phase, i.e.
	//- run_on(system_running_phase_on_update | system_running_phase_on_ui_render);
	//- just make sure you know what you are doing.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	class csystem : public isystem,
		public iworld_context_holder
	{
	public:
		using system_function_prototype_t = std::function<void(flecs::entity, TComps...)>;

		csystem(flecs::world& world, const std::string& name) :
			iworld_context_holder(world),
			m_builder(world.system<TComps...>(name.c_str()))
		{
			m_builder.run([](flecs::iter_t* it) {
					while (ecs_iter_next(it)) {
						it->callback(it);
					}
				});
		}
		~csystem() = default;

		flecs::entity self() const override final { return m_self; }
		stringview_t name() const override final { return m_self.name().c_str(); }

	protected:
		template<class TSystem>
		void depends_on(flecs::world& world)
		{
			//- Note: registers a system we depend on AFTER we are registered to flecs
			TSystem system(world);
		}

		void kind(const flecs::entity& e)
		{
			m_builder.kind(e);
		}

		void kind(const std::string& kind_entity_name)
		{
			//- TODO: should we not have by default kind(flecs::OnUpdate) ?
			//- and how does this all intertwine with out custom pipeline in engine for ecs world.
			kind(world().lookup(kind_entity_name.c_str()));
		}

		void run_on(int bitwise_phase)
		{
			if (algorithm::bit_on(bitwise_phase, system_running_phase_on_update))
			{
				m_builder.kind<ssystem_phases::OnUpdate>();
				m_phase |= system_running_phase_on_update;
			}
			if (algorithm::bit_on(bitwise_phase, system_running_phase_on_world_render))
			{
				m_builder.kind<ssystem_phases::OnWorldRender>();
				m_phase |= system_running_phase_on_world_render;
			}
			if (algorithm::bit_on(bitwise_phase, system_running_phase_on_ui_render))
			{
				m_builder.kind<ssystem_phases::OnUiRender>();
				m_phase |= system_running_phase_on_ui_render;
			}
			if (algorithm::bit_on(bitwise_phase, system_running_phase_on_post_update))
			{
				m_builder.kind<ssystem_phases::OnPostUpdate>();
				m_phase |= system_running_phase_on_post_update;
			}
		}

		template<typename TCallable>
		void build(TCallable&& func)
		{
			if (m_phase == 0)
			{
				run_on(system_running_phase_on_update);
			}
			m_self = m_builder.each(func);
		}

	private:
		flecs::system m_self;
		flecs::system_builder<TComps...> m_builder;
		int m_phase = 0;
	};

} //- ecs