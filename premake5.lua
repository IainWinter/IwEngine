iwengdir  = path.getabsolute("IwEngine")
sndbxdir  = path.getabsolute("Sandbox")
edtordir  = path.getabsolute("Editor")
iwtoldir  = path.getabsolute("IwTools")
glewdir   = iwengdir .. "/extern/glew"
imguidir  = iwengdir .. "/extern/imgui"
assimpdir = iwengdir .. "/extern/assimp"
stbdir    = iwengdir .. "/extern/stb"
llvmdir   = iwengdir .. "/extern/llvm"
jsondir   = iwengdir .. "/extern/json"
fmoddir   = iwengdir .. "/extern/fmod"

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
os.execute ("cmake -S " .. assimpdir    .. " -B " .. assimpdir .. blddir)

filter "platforms:x64" -- doesnt filter
	print("---------------------------- 64 ----------------------------")
	--os.execute ("cmake -S " .. llvmdir .. "/llvm -B " .. llvmdir .. blddir .. " -DLLVM_ENABLE_PROJECTS=clang -A x64 -Thost=x64 -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN=ON -DLLVM_TARGETS_TO_BUILD='X86'")

--filter "platforms:x32"
--	print("---------------------------- 32 ----------------------------")
--	os.execute ("cmake -S " .. llvmdir .. "/llvm -B " .. llvmdir .. blddir .. " -DLLVM_ENABLE_PROJECTS=clang -A Win32 -Thost=Win32 -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN=ON -DLLVM_TARGETS_TO_BUILD='X86'")

project "wMath"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/math/**.h",
		iwengdir .. srcdir .. "/math/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir
	}

	defines {
		--"IW_USE_REFLECTION"
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

project "wLog"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/log/**.h",
		iwengdir .. srcdir .. "/log/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir
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

project "wUtil"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/util/**.h",
		iwengdir .. srcdir .. "/util/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir
	}

	links {
		"wLog"
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

project "wAudio"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/audio/**.h",
		iwengdir .. srcdir .. "/audio/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir,
		fmoddir .. incdir
	}

	libdirs {
		fmoddir .. "/lib/%{cfg.platform}"
	}

	links {	
		"wLog",
		"fmod",
		"fmodstudio"
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

project "wAsset"
	kind "None"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/asset/**.h"
	}

	includedirs {
		iwengdir  .. incdir
	}

	links {	
		"wLog",
		"wUtil"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines {
			"IW_PLATFORM_WINDOWS"
		}

	filter "configurations:Debug"
		defines "IW_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "IW_RELEASE"
		runtime "Release"
		optimize "On"

project "wEvents"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/events/**.h",
		iwengdir .. srcdir .. "/events/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir
	}

	links {
		"wLog",
		"wUtil"
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

project "wCommon"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/events/**.h",
		iwengdir .. srcdir .. "/events/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir
	}

	links {
		"wEvents",
		"wUtil",
		"wMath"
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

project "wEntity"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/events/**.h",
		iwengdir .. srcdir .. "/events/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir
	}

	links {
		"wEvents",
		"wLog",
		"wUtil",
		"wMath"
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

project "wRenderer"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/renderer/**.h",
		iwengdir .. srcdir .. "/renderer/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir,
		glewdir   .. incdir,
	}

	links {
		"wLog",
		"wMath",
		"GLEW",
		"opengl32.lib"
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

project "wGraphics"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/iw/graphics/**.h",
		iwengdir .. srcdir .. "/graphics/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir,
		glewdir   .. incdir,
		assimpdir .. incdir,
		assimpdir .. blddir .. incdir,
		stbdir .. incdir,
	}

	libdirs {
		assimpdir .. blddir .. "/code/%{cfg.buildcfg}",
	}

	links {
		"wLog",
		"wUtil",
		"wMath",
		"wCommon",
		"wRenderer",
		"GLEW",
		"assimp-vc140-mt",
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

--postbuildcommands {
	--"xcopy /y /f \"" .. iwengdir  .. bindir .. "/IwEngine.dll\" \"" .. edtordir .. bindir .. "\"",
--}
