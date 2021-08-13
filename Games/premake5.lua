name = "SpaceSand"

iwengdir  = path.getabsolute("../_export")
gamedir  = path.getabsolute(name)

cfgname = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"
bindir  = "/bin/" .. cfgname
libdir  = "/lib/" .. cfgname
blddir  = "/build"
incdir  = "/include"
srcdir  = "/src"

workspace (name)
	configurations { "Debug", "Release" }
	platforms { "x64" }
	startproject (name)
	location (gamedir .. blddir)

	project (name)
		--kind "SharedLib"
		kind "WindowedApp"
		language "C++"
		location  (gamedir .. blddir)
		targetdir (gamedir .. bindir)
		objdir    (gamedir .. blddir)

		files {
			gamedir .. incdir .. "/**.h",
			gamedir .. srcdir .. "/**.h",
			gamedir .. srcdir .. "/**.cpp"
		}

		includedirs {
			gamedir  .. incdir,
			iwengdir .. incdir
		}

		libdirs {
			iwengdir .. libdir
		}

		links {
			"wLog",
			"wMath",
			"wUtil",
			"wAudio",
			"wEvents",
			"wCommon",
			"wEntity",
			"wGraphics",
			"wRenderer", -- only beacuse of MakeLayout
			"wInput",
			"wPhysics",
			"wEngine",
			"ImGui",
			"GLEW",
			"opengl32.lib",
			"wSand"
		}

		prebuildcommands {
			"xcopy /y /f \""    .. iwengdir .. bindir .. "/*.dll\" \"" .. gamedir .. bindir .. "/\""
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IW_IMGUI"  -- not sure if this needs to be here
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"