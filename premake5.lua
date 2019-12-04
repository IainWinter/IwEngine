iwengdir  = path.getabsolute("IwEngine")
sndbxdir  = path.getabsolute("Sandbox")
edtordir  = path.getabsolute("Editor")
glewdir   = iwengdir .. "/extern/glew"
imguidir  = iwengdir .. "/extern/imgui"
assimpdir = iwengdir .. "/extern/assimp"
stbdir = iwengdir .. "/extern/stb"

cfgname = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"
bindir  = "/bin/" .. cfgname
libdir  = "/lib/" .. cfgname
resdir  = "/assets"
blddir  = "/build"
incdir  = "/include"
srcdir  = "/src"

workspace "IwEngine"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	startproject "Editor"
	location (iwengdir .. blddir)

include (glewdir)
include (imguidir)
os.execute ("cmake -S " .. assimpdir .. " -B " .. assimpdir .. blddir)

project "IwEngine"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/**.h",
		iwengdir .. srcdir .. "/**.h",
		iwengdir .. srcdir .. "/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir,
		iwengdir  .. srcdir .. "/engine/Platform",
		glewdir   .. incdir,
		imguidir  .. incdir,
		assimpdir .. incdir,
		assimpdir .. blddir .. incdir,
		stbdir .. incdir
	}

	libdirs {
		assimpdir .. blddir .. "/code/%{cfg.buildcfg}"
	}

	links {
		"GLEW",
		"ImGui",
		"assimp-vc140-mt",
		"opengl32.lib"
	}

	disablewarnings { 
		"26495",
		"26812",
		"4200"
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

project "Editor"
	kind "WindowedApp"
	language "C++"
	location  (edtordir .. blddir)
	targetdir (edtordir .. bindir)
	objdir    (edtordir .. blddir)

	files {
		edtordir .. incdir .. "/**.h",
		edtordir .. srcdir .. "/**.h",
		edtordir .. srcdir .. "/**.cpp"
	}

	includedirs {
		edtordir .. incdir,
		iwengdir .. incdir,
		imguidir .. incdir
	}

	links {
		"IwEngine",
		"ImGui",
		"opengl32.lib"
	}

	prebuildcommands  {
		"xcopy /y /f \""    .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.dll\" \"" .. edtordir .. bindir .. "\"",
		"xcopy /y /f \""    .. iwengdir  .. bindir .. "/IwEngine.dll\" \"" .. edtordir .. bindir .. "\"",
		"xcopy /y /f \""    .. glewdir   .. bindir .. "/GLEW.dll\" \""     .. edtordir .. bindir .. "\"",
		"xcopy /e /y /f /i \"" .. edtordir  .. resdir .. "\" \""           .. edtordir .. blddir .. resdir .. "\"",
		"xcopy /e /y /f /i \"" .. edtordir  .. resdir .. "\" \""           .. edtordir .. bindir .. resdir .. "\""
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines "IW_PLATFORM_WINDOWS"

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
		sndbxdir .. incdir .. "/**.h",
		sndbxdir .. srcdir .. "/**.h",
		sndbxdir .. srcdir .. "/**.cpp"
	}

	includedirs {
		sndbxdir .. incdir,
		iwengdir .. incdir,
		imguidir .. incdir
	}

	links {
		"IwEngine",
		"ImGui",
		"opengl32.lib"
	}

	prebuildcommands  {
		"xcopy /y /f \""    .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.dll\" \"" .. sndbxdir .. bindir .. "\"",
		"xcopy /y /f \""    .. iwengdir  .. bindir .. "/IwEngine.dll\" \"" .. sndbxdir .. bindir .. "\"",
		"xcopy /y /f \""    .. glewdir   .. bindir .. "/GLEW.dll\" \""     .. sndbxdir .. bindir .. "\"",
		"xcopy /e /y /f /i \"" .. sndbxdir  .. resdir .. "\" \""           .. sndbxdir .. blddir .. resdir .. "\"",
		"xcopy /e /y /f /i \"" .. sndbxdir  .. resdir .. "\" \""           .. sndbxdir .. bindir .. resdir .. "\""
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines "IW_PLATFORM_WINDOWS"

	filter "configurations:Debug"
		defines "IW_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "IW_RELEASE"
		runtime "Release"
		optimize "On"
