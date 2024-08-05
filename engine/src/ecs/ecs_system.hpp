#pragma once
#include "ecs_world_context_holder.hpp"
#include "ecs_entity.hpp"
#include "ecs_singleton.hpp"

namespace ecs
{
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
	//- and those are what will be passed to the function.
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
		}
		virtual ~csystem() {};

		flecs::entity self() const override final { return m_self; }
		stringview_t name() const override final { return m_self.name().c_str(); }

	protected:
		//- Marks the system as available for threading. Note that a system using ImGui UI
		//- is not eligible for multithreading.
		//- Note: must be called BEFORE 'build'
		void multithreaded();

		//- Changes made by this system are visible immediately. Normally,
		//- any structural changes are inserted into a command buffer and synced
		//- before next tick. Enabling this makes the system single threaded.
		//- Note: Structural changes are for example adding or removing components from an entity.
		//- Also, must be called BEFORE 'build'
		void immediate();

		//- Specifies a component to explicitly exclude, meaning an entity having it will be ignored,
		//- can be more than one component
		//- Note: must be called BEFORE 'build'
		template<typename... TComponent>
		void exclude();

		template<typename TCallable>
		void build(TCallable&& func);

		//- Specifies after which already existing system to run in order.
		//- Note: the other system must have the same 'run_on' value set,
		//- otherwise we wont run at all.
		//- Also, this must be called AFTER 'build', otherwise its a crash...
		void run_after(flecs::entity e);
		void run_after(const flecs::entity_t e);
		void run_after(stringview_t name);

	private:
		flecs::system m_self;
		flecs::system_builder<TComps...> m_builder;
		bool m_multithreaded = false;
	};

	//- Excludes one or more components from matching table, i.e. retrieves entities that
	//- do not have those components.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	template<typename... TComponent>
	void csystem<TComps...>::exclude()
	{
		m_builder.template without<TComponent...>();
	}

	//- Marks the system as available for threading. Note that a system using ImGui UI
	//- is not eligible for multithreading.
	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::multithreaded()
	{
		m_builder.multi_threaded();
		m_multithreaded = true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::immediate()
	{
		m_builder.immediate();
		m_multithreaded = false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::run_after(flecs::entity e)
	{
		CORE_ASSERT(e.is_valid(), "Invalid operation. Specified system does not exist!");

		m_self.add(flecs::Phase).depends_on(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::run_after(const flecs::entity_t e)
	{
		m_self.add(flecs::Phase).depends_on(e);
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	void csystem<TComps...>::run_after(stringview_t name)
	{
		run_after(world().lookup(name.data()));
	}

	//------------------------------------------------------------------------------------------------------------------------
	template<typename... TComps>
	template<typename TCallable>
	void csystem<TComps...>::build(TCallable&& func)
	{
		m_self = m_builder.each(func);
	}

	//- A task is similar to a normal system, only that it does not match any components and thus no entities.
	//- If entities are required they can be retrieved through the world or a query.
	//- The function itself is executed as is, with only delta time provided.
	//------------------------------------------------------------------------------------------------------------------------
	class ctask : public isystem,
		public iworld_context_holder
	{
	public:
		using system_function_prototype_t = std::function<void(float)>;

		ctask(flecs::world& world, const string_t& name) :
			iworld_context_holder(world),
			m_builder(world.system(name.data()))
		{
		}
		virtual ~ctask() {};

		flecs::entity self() const override final { return m_self; }
		stringview_t name() const override final { return m_self.name().c_str(); }

		template<typename TCallable>
		void build(TCallable&& callback);

		void run_after(flecs::entity e);
		void run_after(const flecs::entity_t e);
		void run_after(stringview_t name);

	private:
		flecs::system m_self;
		flecs::system_builder<> m_builder;
	};

	//------------------------------------------------------------------------------------------------------------------------
	template<typename TCallable>
	void ctask::build(TCallable&& callback)
	{
		m_self = m_builder.run([&](flecs::iter& it)
			{
				callback(it.delta_time());
			});
	}

} //- ecs