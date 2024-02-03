#pragma once
#include <config.hpp>
#include "stb_integration/stb_integration.hpp"

namespace sm
{

	//- Simple way of retrieving a custom set fps/frametime delta time or current applications actual fps/frametime
	//------------------------------------------------------------------------------------------------------------------------
	class ENGINE_API ctimestep
	{
	public:
		static float real_frametime();
		static unsigned real_fps();

		ctimestep(unsigned target_fps = 0);
		~ctimestep() = default;

		float custom_frametime() const;
		unsigned custom_fps() const;

	private:
		unsigned m_fps;
	};

} //- sm