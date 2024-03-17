#include "ecs_world_manager.hpp"
#include "../services/thread_service.hpp"

namespace ecs
{
	namespace
	{
		//------------------------------------------------------------------------------------------------------------------------
		ecs_os_thread_t thread_new(ecs_os_thread_callback_t callback, void* arg)
		{
			auto* thread_service = core::cservice_manager::find<engine::cthread_service>();

			auto future = thread_service->push_foreground_job("", [=]()
				{
					callback(arg);
				});

			return reinterpret_cast<ecs_os_thread_t>(new std::future<void>(std::move(future)));
			//return reinterpret_cast<ecs_os_thread_t>(&future);
		}

		//------------------------------------------------------------------------------------------------------------------------
		void* thread_join(ecs_os_thread_t thread)
		{
			if (auto future = reinterpret_cast<std::future<void>*>(thread); future)
			{
				future->wait();
			}
			return nullptr;
		}

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool cworld_manager::create(stringview_t name)
	{
		auto h = algorithm::hash(name);

		if (m_worlds.find(h) == m_worlds.end())
		{
			m_worlds.emplace(h, name);
			m_current = h;
			return true;
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
	void cworld_manager::tick(float dt, system_running_phase p)
	{
		if (m_current != 0)
		{
			active().tick(dt, p);
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cworld_manager::prepare(const core::srect& area)
	{
		if (m_current != 0)
		{
			active().prepare(area);
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