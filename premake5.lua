include "scripts/utility.lua"

-- Postponed thirdparty:
-- "slang"
-- Postponed plugins:
-- "plugin_kingdoms_core", "plugin_race_human", "plugin_kingdoms_and_empires"
-- Postponed projects:
-- "kingdoms_and_empires"
-- Postponed exes:
-- "engine_launcher", "spritemancer_testbed", "unittest_launcher"

ENGINE_NAME		= "kokoro"
VERBOSE			= true
SCRIPTS_DIR		= "<undefined>"
VENDOR_DIR		= "<undefined>"
WORKSPACE_DIR	= "<undefined>"
THIRDPARTY_DIR	= "3rdparty"
PLUGINS_DIR		= "plugins"
PROJECTS_DIR	= "projects"
THIRDPARTY = {"spdlog", "core", "glm", "argparse", "nlohmann", "mimalloc", "imgui", "pfd",
			  "magic_enum", "taskflow", "flecs", "LuaBridge", "Lua", "rttr", "raylib",
			  "Tracy", "spritemancer", "tree_sitter", "unittest", "hwinfo", "asio"}

PLUGINS = {"plugin_module_example", "plugin_camera_system",
			"plugin_status_effects", "plugin_ai", "plugin_rttr_example"}

PROJECTS = {"tiny_editor", "kingdom_wars"}
ACTIVE_PROJECT = "kingdom_wars"

EXES = {}
PLATFORM = "<undefined>"
OUTDIR = "%{cfg.buildcfg}-%{cfg.system}"

workspace("Workspace")
	startproject(ACTIVE_PROJECT)
	architecture "x86_64"
	configurations{"debug", "hybrid", "release"}
	language "C++"
	cppdialect "C++17"
	systemversion "latest"
	staticruntime "Off"
	flags{"MultiProcessorCompile"}

	-- determine available platforms
	if os.host() == "linux" then
		PLATFORM = "linux"
		system "linux"
	elseif os.host() == "windows" then
		PLATFORM = "windows"
		system "windows"
	elseif os.host() == "macosx" then
		PLATFORM = "macosx"
		system "macosx"
	else
		print("Unrecognoized or unsupported platform: " .. os.host())
		return
	end

	-- setup variables
	WORKSPACE_DIR = os.getcwd()
	VENDOR_DIR = path.join(WORKSPACE_DIR, "vendor")
	SCRIPTS_DIR = path.join(WORKSPACE_DIR, "scripts")

	-- create 3rdparty dependency projects
	print("Loading 3rdparty projects...")
	group "3rdparty"
		for ii = 1, #THIRDPARTY do
			p = THIRDPARTY[ii]

			load_project(p, path.join("3rdparty", p))
		end
	group ""

	-- create plugin projects
	print("Loading plugin projects...")
	group "plugins"
		for ii = 1, #PLUGINS do
			p = PLUGINS[ii]

			load_project(p, path.join("plugins", p))
		end
	group ""

	-- create engine project
	print("Loading engine project...")
	group "engine"
		load_project("engine", "engine")
	group ""

	-- create projects
	print("Loading projects...")
	group "projects"
		for ii = 1, #PROJECTS do
			p = PROJECTS[ii]

			load_project(p, path.join("projects", p))
		end
	group""

	-- create executable
	print("Loading app projects...")
	group "app"
		for ii = 1, #EXES do
			p = EXES[ii]

			load_project(p, path.join("apps", p))
		end
	group ""