#include "kingdoms_module.hpp"
#include "kingdoms_layer_ui.hpp"
#include <engine.h>

RTTR_REGISTRATION
{
	using namespace kingdoms;

	REGISTER_LAYER(ckingdoms);
	REGISTER_LAYER(ckingdoms_layer_ui);
}

namespace kingdoms
{
	namespace
	{
		constexpr stringview_t C_RACE_FILEPATH = "/race";
		constexpr stringview_t C_MAPS_FILEPATH = "/maps";
		constexpr stringview_t C_TILESET_FILEPATH = "/maps/tileset";

		sgame_context S_CTX;

	} //- unnamed

	//------------------------------------------------------------------------------------------------------------------------
	bool ckingdoms::init()
	{
		bool result = true;

		//- Create virtual file paths for commonly accessed resources
		auto* vfs = engine::cengine::service<fs::cvirtual_filesystem>();

		const auto& fs = vfs->find_filesystem("/");
		const auto path = fs->base_path();

		auto races_fs = std::make_shared<io::cnative_filesystem>();
		auto maps_fs = std::make_shared<io::cnative_filesystem>();
		auto tilesets_fs = std::make_shared<io::cnative_filesystem>();

		const auto races_path = string_utils::join(path, C_RACE_FILEPATH.data());
		const auto maps_path = string_utils::join(path, C_MAPS_FILEPATH.data());
		const auto tilesets_path = string_utils::join(path, C_TILESET_FILEPATH.data());

		if (races_fs->init(races_path, C_RACE_FILEPATH) &&
			maps_fs->init(maps_path, C_MAPS_FILEPATH) &&
			tilesets_fs->init(tilesets_path, C_TILESET_FILEPATH))
		{
			vfs->add_filesystem(C_RACE_FILEPATH, races_fs);
			vfs->add_filesystem(C_MAPS_FILEPATH, maps_fs);
			vfs->add_filesystem(C_TILESET_FILEPATH, tilesets_fs);
		}
		else
		{
			result = false;
		}

		//- Create resource managers
		core::cservice_manager::emplace<cmap_manager>();

		return result;
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::shutdown()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::on_update(float dt)
	{
		S_CTX.m_command_system.on_update(S_CTX, dt);
	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::on_world_render()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::on_ui_render()
	{

	}

	//------------------------------------------------------------------------------------------------------------------------
	void ckingdoms::on_post_update(float dt)
	{

	}

} //- kingdoms