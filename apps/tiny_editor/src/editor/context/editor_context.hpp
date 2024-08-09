#pragma once
#include <core.h>
#include <engine.h>

namespace editor
{
	//- Class contains basic data useful almost everywhere. Can also be used to synchronize between systems.
	//------------------------------------------------------------------------------------------------------------------------
	struct scontext : private core::cnon_copyable
	{
		unsigned m_window_width		= 0;
		unsigned m_window_height	= 0;
		float m_mainmenu_height		= 0.0f;
		bool m_docks_enabled		= true;
		bool m_imgui_demo			= false;
		bool m_implot_demo			= false;
	};

	//- Access to context.
	//------------------------------------------------------------------------------------------------------------------------
	class ccontext_holder
	{
	public:
		ccontext_holder(scontext& ctx) : m_ctx(ctx) {}
		virtual ~ccontext_holder() {}

	protected:
		inline const scontext& ctx() const {return m_ctx;}
		inline scontext& ctx() {return m_ctx;}

	private:
		scontext& m_ctx;
	};

} //- editor