#include "ecs_world_manager.hpp"

namespace ecs
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		ecs_os_thread_t thread_new(ecs_os_thread_callback_t callback, void* arg)
		{
			std::thread* thread = new std::thread{ callback,arg };
			return reinterpret_cast<ecs_os_thread_t>(thread);

// 			auto* thread_service = core::cservice_manager::find<engine::cthread_service>();
// 
// 			auto future = thread_service->push_foreground_job("", [=]()
// 				{
// 					callback(arg);
// 				});
// 
// 			//return reinterpret_cast<ecs_os_thread_t>(new std::future<void>(std::move(future)));
// 			return reinterpret_cast<ecs_os_thread_t>(&future);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void* thread_join(ecs_os_thread_t thread)
		{
			void* arg = nullptr;
			std::thread* thr = reinterpret_cast<std::thread*>(thread);
			thr->join();
			delete thr;
			return arg;

// 			if (auto future = reinterpret_cast<std::future<void>*>(thread); future)
// 			{
// 				future->wait();
// 			}
// 			return nullptr;
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_manager::create(world::sconfig cfg, const bool make_active /*= false*/)
	{
		if (!cfg.m_name.empty())
		{
			auto h = algorithm::hash(cfg.m_name);

			if (m_worlds.find(h) == m_worlds.end())
			{
				m_worlds.emplace(h, cfg);

				if (make_active)
				{
					m_current = h;
				}

				return true;
			}
		}
		else
		{
			log_error("Trying to create a world with an empty name is not allowed!");
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_manager::set_active(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_worlds.find(h) != m_worlds.end())
		{
			m_current = h;
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_manager::tick(float dt)
	{
		if (m_current != 0)
		{
			active().tick(dt);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	const cworld& cworld_manager::active() const
	{
		ASSERT(m_current != 0, "Active world was not set for world manager!");

		return m_worlds.at(m_current);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld& cworld_manager::active()
	{
		ASSERT(m_current != 0, "Active world was not set for world manager!");

		return m_worlds.at(m_current);
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld_manager::cworld_manager()
	{
		init_ecs_os_api();
	}

	//------------------------------------------------------------------------------------------------------------------------
	cworld_manager::~cworld_manager()
	{
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_manager::init_ecs_os_api()
	{
		ecs_os_set_api_defaults();
		ecs_os_api_t api = ecs_os_api;

		api.thread_new_ = thread_new;
		api.thread_join_ = thread_join;

		ecs_os_set_api(&api);
	}

} //- ecs