#pragma once
#include "sm_config.hpp"
#include "sm_mesh.hpp"

namespace sm
{
	//------------------------------------------------------------------------------------------------------------------------
	struct sposition_color_texcoord_vertex final
	{
		static void init();

		float x = 0.0f, y = 0.0f, z = 0.0f;
		float u = 0.0f, v = 0.0f;
		unsigned abgr = 0;
	};

	//------------------------------------------------------------------------------------------------------------------------
	class cspritebatch final : public detail::cmesh<sposition_color_texcoord_vertex>
	{
	public:
		cspritebatch();
		~cspritebatch() = default;

		void begin();
		void draw();
		void end();

	private:
		texture_handle_t m_previous_texture = MAX(texture_handle_t);
	};

} //- sm