#pragma once
#include "core_config.hpp"

namespace core
{
	//------------------------------------------------------------------------------------------------------------------------
	class ctimer final
	{
	public:
		static int64_t now() { return std::chrono::high_resolution_clock::now().time_since_epoch().count(); }

		ctimer(bool paused = true);
		~ctimer() = default;

		void start();
		bool started() const;
		float secs() const;
		float millisecs() const;
		float microsecs() const;
		double nanosecs() const;

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_timepoint;
	};

} //- core