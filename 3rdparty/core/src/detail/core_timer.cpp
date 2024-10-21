#include "core_timer.hpp"

namespace core
{
	using namespace std::chrono_literals;

	//------------------------------------------------------------------------------------------------------------------------
	ctimer::ctimer(bool paused /*= true*/) :
		m_timepoint(0s)
	{
		if (!paused)
		{
			start();
		}
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ctimer::start()
	{
		m_timepoint = std::chrono::high_resolution_clock::now();
	}

	//------------------------------------------------------------------------------------------------------------------------
	bool ctimer::started() const
	{
		return m_timepoint.time_since_epoch().count() > 0;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimer::secs() const
	{
		static constexpr float C_TO_SEC = 1000.0f * 1000.0f;

		return microsecs() / C_TO_SEC;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimer::millisecs() const
	{
		static constexpr float C_TO_MILLISEC = 1000.0f;

		return microsecs() / C_TO_MILLISEC;
	}

	//------------------------------------------------------------------------------------------------------------------------
	float ctimer::microsecs() const
	{
		CORE_ASSERT(started(), "Invalid operation. Timer must be started before it can be used");

		auto now = std::chrono::high_resolution_clock::now();

		return SCAST(float, std::chrono::time_point_cast<std::chrono::microseconds>(now).time_since_epoch().count() -
			std::chrono::time_point_cast<std::chrono::microseconds>(m_timepoint).time_since_epoch().count());
	}

	//------------------------------------------------------------------------------------------------------------------------
	double ctimer::nanosecs() const
	{
		CORE_ASSERT(started(), "Invalid operation. Timer must be started before it can be used");

		auto now = std::chrono::high_resolution_clock::now();

		return SCAST(double, std::chrono::time_point_cast<std::chrono::nanoseconds>(now).time_since_epoch().count() -
			std::chrono::time_point_cast<std::chrono::nanoseconds>(m_timepoint).time_since_epoch().count());
	}

} //- core