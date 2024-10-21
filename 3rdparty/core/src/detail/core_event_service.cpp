#include "core_event_service.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	cevent_service::~cevent_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool cevent_service::on_start()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void cevent_service::on_shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void cevent_service::on_update(float)
	{
		core::cscope_mutex m(m_mutex);

		for (auto& pair : m_queue)
		{
			//- An event might note have any listeners, so we check here
			if (auto it = m_listeners.find(pair.first); it != m_listeners.end())
			{
				const auto& listeners = it->second;
				auto& queue = pair.second;

				while (!queue.empty())
				{
					const auto& e = queue.front();

					//- delegate the event to all intersted listeners and dispose of it after
					for (const auto& l : listeners)
					{
						l(e);
					}

					queue.pop();
				}
			}
		}
	}

} //- core