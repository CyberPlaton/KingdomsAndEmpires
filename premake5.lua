include "scripts/utility.lua"

VERBOSE = true
SCRIPTS_DIR = "<undefined>"
VENDOR_DIR = "<undefined>"
WORKSPACE_DIR = "<undefined>"
THIRDPARTY = {"box2d", "sokol"}
PLUGINS = {"test", "test2"}
EXES = {"engine"}
CFG = "<undefined>"
SYS = "<undefined>"
ARCH = "<undefined>"
OUTDIR = "<undefined>"

workspace("KingdomsAndEmpires")
	startproject("KingdomsAndEmpires")
	architecture "x86_64"
	configurations{"debug", "release"}
	platforms{"windows", "linux", "macosx"}
	language "C++"
	cppdialect "C++17"
	systemversion "latest"
	staticruntime "Off"
	flags{"MultiProcessorCompile"}

	-- setup variables
	WORKSPACE_DIR = os.getcwd()
	VENDOR_DIR = path.join(WORKSPACE_DIR, "vendor")
	SCRIPTS_DIR = path.join(WORKSPACE_DIR, "scripts")
	CFG = "%{cfg.buildcfg}"
	SYS = "%{cfg.system}"
	ARCH = "%{cfg.architecture}"
	OUTDIR = SYS .. "-" .. CFG .. "-" .. ARCH

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

	-- create executable
	print("Loading app projects...")
	group "app"
		for ii = 1, #EXES do
			p = EXES[ii]

			load_project(p, path.join("apps", p))
		end
	group ""