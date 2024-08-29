include (path.join(SCRIPTS_DIR, "utility.lua"))

function spritemancer()
	add_target_static_library("spritemancer",
					{},
					{"spritemancer_EXPORTS"},
					{},
					{"spdlog", "core", "mimalloc", "hwinfo", "rttr", "imgui", "bx", "bimg", "bgfx", "glfw"},
					"c++",
					{}, -- plugin_headeronly_deps
					{"nlohmann", "glm", "magic_enum", "taskflow", "Tracy", "argparse", "asio"}, -- thirdparty_headeronly_deps
					{"src", "../rttr/rttr/src", "../asio/include/asio/asio/include"} -- additional_includes
					)
end
spritemancer()
