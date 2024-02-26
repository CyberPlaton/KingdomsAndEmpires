#pragma once
#include <core.h>

namespace engine
{
	//- This is a dummy layer to show how one should be defined. While creating one you don´t have to inherit from it.
	//- Note: if you don´t reflect a function then it will not be registered and executed,
	//- thus you can limit your layer to update only if you only define that function.
	//------------------------------------------------------------------------------------------------------------------------
	struct slayer final
	{
	public:
		static constexpr std::string_view C_LAYER_UPDATE_FUNC_NAME = "on_update";
		static constexpr std::string_view C_LAYER_WORLD_RENDER_FUNC_NAME = "on_world_render";
		static constexpr std::string_view C_LAYER_UI_RENDER_FUNC_NAME = "on_ui_render";
		static constexpr std::string_view C_LAYER_POST_UPDATE_FUNC_NAME = "on_post_update";

		static void on_update(float){}
		static void on_world_render(){}
		static void on_ui_render(){}
		static void on_post_update(float) {}

		RTTR_ENABLE();
	};

	//------------------------------------------------------------------------------------------------------------------------
	REFLECT_INLINE(slayer)
	{
		rttr::registration::class_<slayer>("slayer")
			.method("on_update",		&slayer::on_update)
			.method("on_world_render",	&slayer::on_world_render)
			.method("on_ui_render",		&slayer::on_ui_render)
			.method("on_ui_render",		&slayer::on_ui_render)
			.method("on_post_update",	&slayer::on_post_update)
			;

		rttr::default_constructor<slayer>();
	}

} //- engine