#pragma once
#include <core.h>

namespace engine
{
	class cengine;

	//- This is a dummy layer to show how one should be defined. While creating one you must not inherit from it.
	//- Remember to define all functions as shown below and reflect them using the macro REGISTER_LAYER().
	//------------------------------------------------------------------------------------------------------------------------
	struct slayer final
	{
		static constexpr std::string_view C_LAYER_UPDATE_FUNC_NAME		= "on_update";
		static constexpr std::string_view C_LAYER_WORLD_RENDER_FUNC_NAME= "on_world_render";
		static constexpr std::string_view C_LAYER_UI_RENDER_FUNC_NAME	= "on_ui_render";
		static constexpr std::string_view C_LAYER_POST_UPDATE_FUNC_NAME = "on_post_update";
		static constexpr std::string_view C_LAYER_INIT_FUNC_NAME		= "init";
		static constexpr std::string_view C_LAYER_SHUTDOWN_FUNC_NAME	= "shutdown";
		static constexpr array_t<std::string_view, 6> C_LAYER_FUNC_NAMES= { C_LAYER_UPDATE_FUNC_NAME, C_LAYER_WORLD_RENDER_FUNC_NAME, C_LAYER_UI_RENDER_FUNC_NAME,
																			C_LAYER_POST_UPDATE_FUNC_NAME, C_LAYER_INIT_FUNC_NAME, C_LAYER_SHUTDOWN_FUNC_NAME };

		static bool init()					{return false;}
		static void shutdown()				{}
		static void on_update(float)		{}
		static void on_world_render()		{}
		static void on_ui_render()			{}
		static void on_post_update(float)	{}
	};

	//- Class containing array of layers that are currently present. Responsible for any layer management and execution.
	//------------------------------------------------------------------------------------------------------------------------
	class clayers final
	{
		friend class cengine;
	public:
		clayers() = default;
		~clayers() = default;

		void init();
		void shutdown();
		void on_update(float dt);
		void on_world_render();
		void on_ui_render();
		void on_post_update(float dt);

		void emplace_new_layer(stringview_t name, rttr::method update, rttr::method world_render, rttr::method ui_render,
			rttr::method post_update, rttr::method on_init, rttr::method on_shutdown);

	private:
		struct slayer_data
		{
			slayer_data(stringview_t name, rttr::method update, rttr::method world_render, rttr::method ui_render,
				rttr::method post_update, rttr::method on_init, rttr::method on_shutdown);
			~slayer_data() = default;

			string_t m_name;
			rttr::method m_update;
			rttr::method m_world_render;
			rttr::method m_ui_render;
			rttr::method m_post_update;
			rttr::method m_init;
			rttr::method m_shutdown;
		};

		vector_t<slayer_data> m_layers;
	};

} //- engine

//- Use macro to reflect your layer, the layer functions must be declared and implemented
//------------------------------------------------------------------------------------------------------------------------
#define REGISTER_LAYER(layer) \
REFLECT_INLINE(layer) \
{ \
	rttr::cregistrator<layer>(STRINGIFY(layer)) \
		.meth(engine::slayer::C_LAYER_UPDATE_FUNC_NAME,			&layer::on_update) \
		.meth(engine::slayer::C_LAYER_WORLD_RENDER_FUNC_NAME,	&layer::on_world_render) \
		.meth(engine::slayer::C_LAYER_UI_RENDER_FUNC_NAME,		&layer::on_ui_render) \
		.meth(engine::slayer::C_LAYER_POST_UPDATE_FUNC_NAME,	&layer::on_post_update) \
		.meth(engine::slayer::C_LAYER_INIT_FUNC_NAME,			&layer::init) \
		.meth(engine::slayer::C_LAYER_SHUTDOWN_FUNC_NAME,		&layer::shutdown) \
		; \
}