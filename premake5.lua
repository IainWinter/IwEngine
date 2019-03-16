iwengdir = path.getabsolute("IwEngine")
sndbxdir = path.getabsolute("Sandbox")
glewdir  = iwengdir .. "/extern/glew"
imguidir = iwengdir .. "/extern/imgui"

cfgname = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"
bindir  = "/bin/" .. cfgname
libdir  = "/lib/" .. cfgname
blddir  = "/build"

workspace "IwEngine"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	startproject "Sandbox"
	location (iwengdir .. blddir)

include (glewdir)
include (imguidir)

project "IwEngine"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. "/include/**.h",
		iwengdir .. "/src/**.h",
		iwengdir .. "/src/**.cpp"
	}

	includedirs {
		iwengdir .. "/include",
		iwengdir .. "/src/engine/Platform",
		glewdir  .. "/include",
		imguidir .. "/include"
	}

	links {
		"GLEW",
		"ImGui",
		"opengl32.lib"
	}

	disablewarnings { 
		"26495" 
	}

	defines {
		"IMGUI_IMPL_OPENGL_LOADER_GLEW"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines {
			"IW_PLATFORM_WINDOWS",
			"IW_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "IW_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "IW_RELEASE"
		runtime "Release"
		optimize "On"

project "Sandbox"
	kind "WindowedApp"
	language "C++"
	location  (sndbxdir .. blddir)
	targetdir (sndbxdir .. bindir)
	objdir    (sndbxdir .. blddir)

	files {
		sndbxdir .. "/src/**.cpp"
	}

	includedirs {
		iwengdir .. "/include"
	}

	links {
		"IwEngine",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines "IW_PLATFORM_WINDOWS"

		prebuildcommands {
			"xcopy /q /y /f \"" .. iwengdir .. bindir .. "/IwEngine.dll\" \"" .. sndbxdir .. bindir .. "\"",
			"xcopy /q /y /f \"" .. glewdir  .. bindir .. "/GLEW.dll\" \""     .. sndbxdir .. bindir .. "\""
		}

	filter "configurations:Debug"
		defines "IW_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "IW_RELEASE"
		runtime "Release"
		optimize "On"
