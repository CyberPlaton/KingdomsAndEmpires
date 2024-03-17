#include "sdl_service.hpp"

namespace sdl
{
	sdl::csdl_service::sconfig csdl_service::s_cfg;

	//------------------------------------------------------------------------------------------------------------------------
	void csdl_service::configure(sconfig& cfg)
	{
		s_cfg = cfg;
	}

	//------------------------------------------------------------------------------------------------------------------------
	csdl_service::~csdl_service()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	bool csdl_service::on_start()
	{
		ASSERT(s_cfg.m_subsystems != subsystem_none, "Invalid operation. Did you configure SDL service?");

		if (SDL_Init(s_cfg.m_subsystems) == SDL_TRUE)
		{
			return true;
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void csdl_service::on_shutdown()
	{
		SDL_Quit();
	}

	//------------------------------------------------------------------------------------------------------------------------
	void csdl_service::on_update(float dt)
	{
		SDL_Event e;
		while (SDL_PollEvent(&e) == SDL_TRUE)
		{
			switch (e.type)
			{
				case 
			}
		}
	}

} //- sdl