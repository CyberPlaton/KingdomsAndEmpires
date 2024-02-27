#pragma once
#include <core.h>
#include <engine.h> 

namespace editor
{
	//------------------------------------------------------------------------------------------------------------------------
	class clayer_base
	{
	public:
		clayer_base() = default;
		virtual ~clayer_base() {}

		virtual bool init() {return false;}
		virtual void shutdown() {}

		virtual void on_update(float dt) {}
		virtual void on_world_render() {}
		virtual void on_ui_render() {}
		virtual void on_post_update(float dt) {}

	private:

	};

} //- editor