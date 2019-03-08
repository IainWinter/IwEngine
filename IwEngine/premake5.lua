project "IwEngine"
	kind "SharedLib"
	language "C++"
	targetdir "%{_CONFIG.bin}/%{prj.name}"
	implibdir "%{_CONFIG.lib}/%{prj.name}"
    objdir    "%{_CONFIG.obj}/%{prj.name}"

	files {
		"include/**.h",
		"src/**.cpp"
	}

	includedirs {
		"include",
		"%{wks.location}/IwEvents/include",
		"%{wks.location}/IwLog/include",
		"%{vendor_include_dirs.glew}",
		"%{vendor_include_dirs.imgui}"
	}

	links {
		"IwLog",
		"GLEW",
		"ImGui",
		"opengl32.lib"
	}

	filter "system:windows"
		cppdialect "C++11"
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