#include "plugin_kingdoms_and_empires_module.hpp"

void cgame::on_update(float dt)
{

}

void cgame::on_world_render()
{

}

void cgame::on_ui_render()
{

}

void cgame::on_post_update(float dt)
{

}


RTTR_REGISTRATION
{
	rttr::registration::class_<cgame>("cgame")
		.method("on_update",		&cgame::on_update)
		.method("on_world_render",	&cgame::on_world_render)
		.method("on_ui_render",		&cgame::on_ui_render)
		.method("on_post_update",	&cgame::on_post_update)
		;

	rttr::default_constructor<cgame>();
}