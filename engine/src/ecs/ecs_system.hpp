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

		//- Marks the system as available for threading
		void multithreaded()
		{
			m_builder.multi_threaded();
		}

		//- Changes made by this system are visible immediately. Normally,
		//- any structural changes are inserted into a command buffer and synced
		//- before next tick. Enabling this makes the system single threaded.
		void immediate()
		{
			m_builder.no_readonly();
		}

		void run_on(system_running_phase p)
		{
			m_builder.kind(emplace_phase(p));
		}

		void run_after(flecs::entity e)
		{
			m_builder.kind(e);
		}

		void run_after(stringview_t name)
		{
			run_after(world().lookup(name));
		}

		template<typename TCallable>
		void build(TCallable&& func)
		{
			m_self = m_builder.each(func);
		}

	private:
		flecs::system m_self;
		flecs::system_builder<TComps...> m_builder;

	private:
		flecs::entity emplace_phase(system_running_phase p)
		{
			const auto* phases = world().template get<ssystem_phases>();

			switch (p)
			{
			case system_running_phase::system_running_phase_on_update:
			{
				return phases->m_phases[ssystem_phases::C_ON_UPDATE];
			}
			case system_running_phase::system_running_phase_on_world_render:
			{
				return phases->m_phases[ssystem_phases::C_ON_WORLD_RENDER];
			}
			case system_running_phase::system_running_phase_on_ui_render:
			{
				return phases->m_phases[ssystem_phases::C_ON_UI_RENDER];
			}
			case system_running_phase::system_running_phase_on_post_update:
			{
				return phases->m_phases[ssystem_phases::C_ON_POST_UPDATE];
			}
			default:
			case system_running_phase::system_running_phase_none:
			{
				CORE_ASSERT(false, "Invalid operation. Phase is not valid");
				break;
			}
			}
			return {};
		}
	};

} //- ecs