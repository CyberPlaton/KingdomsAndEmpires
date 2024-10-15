#pragma once
#include "ecs_world_context_holder.hpp"
#include "ecs_entity.hpp"
#include "ecs_singleton.hpp"

namespace ecs
{
	namespace system
	{
		using system_flags_t = int;

		//------------------------------------------------------------------------------------------------------------------------
		enum system_flag : uint8_t
		{
			system_flag_none = 0,
			system_flag_multithreaded,
			system_flag_immediate,
		};

		//------------------------------------------------------------------------------------------------------------------------
		struct sconfig final
		{
			flecs::world& m_world;
			string_t m_name;
			vector_t<string_t> m_run_after;
			vector_t<string_t> m_run_before;
			system_flags_t m_flags = 0;
		};

		//- Function responsible for creating a system for a world with given configuration, matching components and function to execute.
		//------------------------------------------------------------------------------------------------------------------------
		template<typename TCallback, typename... TComps>
		void create_system(const sconfig& cfg, TCallback callback)
		{
			CORE_ASSERT(!(algorithm::bit_check(cfg.m_flags, system_flag_multithreaded) &&
				algorithm::bit_check(cfg.m_flags, system_flag_immediate)), "Invalid operation! A system cannot be multithreaded and immediate at the same time!");

			auto builder = cfg.m_world.system<TComps...>(cfg.m_name.c_str());

			//- Set options that are required before system entity creation
			{
				if (algorithm::bit_check(cfg.m_flags, system_flag_multithreaded))
				{
					builder.multi_threaded();
				}
				if (algorithm::bit_check(cfg.m_flags, system_flag_immediate))
				{
					builder.immediate();
				}
			}

			auto system = builder.each<TComps...>(callback);

			//- Set options that are required after system entity creation
			{
				for (const auto& after :cfg.m_run_after)
				{
					if (auto e = cfg.m_world.lookup(after.c_str()); e.is_valid())
					{
						system.add(flecs::Phase).depends_on(e);
					}
					else
					{
						log_error(fmt::format("Dependency (run after) system '{}' for system '{}' could not be found!",
							after, cfg.m_name));
					}
				}

				for (const auto& before : cfg.m_run_before)
				{
					if (auto e = cfg.m_world.lookup(before.c_str()); e.is_valid())
					{
						e.add(flecs::Phase).depends_on(system);
					}
					else
					{
						log_error(fmt::format("Dependent (run before) system '{}' for system '{}' could not be found!",
							after, cfg.m_name));
					}
				}
			}

			//- TODO: store system object along with information in system manager for later use
		}

		//- Function responsible for creating a system for a world with given configuration, without matching any components
		//- and function to execute.
		//- A task is similar to a normal system, only that it does not match any components and thus no entities.
		//- If entities are required they can be retrieved through the world or a query.
		//- The function itself is executed as is, with only delta time provided.
		//------------------------------------------------------------------------------------------------------------------------
		template<typename TCallback>
		void create_task(const sconfig& cfg, TCallback callback)
		{
			CORE_ASSERT(!(algorithm::bit_check(cfg.m_flags, system_flag_multithreaded) &&
				algorithm::bit_check(cfg.m_flags, system_flag_immediate)), "Invalid operation! A system cannot be multithreaded and immediate at the same time!");

			auto builder = cfg.m_world.system(cfg.m_name.c_str());

			//- Set options that are required before system entity creation
			{
				if (algorithm::bit_check(cfg.m_flags, system_flag_multithreaded))
				{
					builder.multi_threaded();
				}
				if (algorithm::bit_check(cfg.m_flags, system_flag_immediate))
				{
					builder.immediate();
				}
			}

			auto task = builder.run([&](flecs::iter& it)
				{
					callback(it.delta_time());
				});

			//- Set options that are required after system entity creation
			{
				for (const auto& after : cfg.m_run_after)
				{
					if (auto e = cfg.m_world.lookup(after.c_str()); e.is_valid())
					{
						task.add(flecs::Phase).depends_on(e);
					}
					else
					{
						log_error(fmt::format("Dependency (run after) system '{}' for system '{}' could not be found!",
							after, cfg.m_name));
					}
				}

				for (const auto& before : cfg.m_run_before)
				{
					if (auto e = cfg.m_world.lookup(before.c_str()); e.is_valid())
					{
						e.add(flecs::Phase).depends_on(systasktem);
					}
					else
					{
						log_error(fmt::format("Dependent (run before) system '{}' for system '{}' could not be found!",
							after, cfg.m_name));
					}
				}
			}
		}

	} //- system

	struct smy_system final
	{
		smy_system(flecs::world& world)
		{
			ecs::system::sconfig cfg{ world };

			cfg.m_run_after = { "A", "B" };
			cfg.m_run_before = { "C", "D", "E" };
			cfg.m_name = "Name";
			cfg.m_flags = ecs::system::system_flag_multithreaded;

			ecs::system::create_system(cfg, nullptr);
		}
	};

} //- ecs