include "scripts/utility.lua"

VERBOSE = true
SCRIPTS_DIR = "<undefined>"
VENDOR_DIR = "<undefined>"
WORKSPACE_DIR = "<undefined>"
THIRDPARTY = {"sokol", "spdlog", "core", "cereal", "glm", "EASTL",
"magic_enum", "taskflow", "mimalloc", "flecs", "LuaBridge", "Lua"}
PLUGINS = {"test", "test2", "plugin_logging", "plugin_module_example"}
EXES = {"engine_launcher"}
PLATFORM = "<undefined>"
OUTDIR = "%{cfg.buildcfg}-%{cfg.system}"

workspace("KingdomsAndEmpires")
	startproject("engine_launcher")
	architecture "x86_64"
	configurations{"debug", "release"}
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

	-- create executable
	print("Loading app projects...")
	group "app"
		for ii = 1, #EXES do
			p = EXES[ii]

			load_project(p, path.join("apps", p))
		end
	group ""