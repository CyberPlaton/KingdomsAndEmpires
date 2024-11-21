#pragma once
#include <engine.h>

namespace kingdoms
{
	namespace component
	{
		//- Component designating a player. In an active game can be several active players, they can be local, AI driven or
		//- over the network.
		//------------------------------------------------------------------------------------------------------------------------
		struct splayer final : public ecs::icomponent
		{
			DECLARE_COMPONENT(splayer);

			unsigned m_id = 0;

			RTTR_ENABLE(icomponent);
		};

	} //- component

} //- kingdoms