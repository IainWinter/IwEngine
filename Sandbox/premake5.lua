project "Sandbox"
	kind "WindowedApp"
	language "C++"
	targetdir "%{_CONFIG.bin}/%{prj.name}"
	implibdir "%{_CONFIG.lib}/%{prj.name}"
    objdir    "%{_CONFIG.obj}/%{prj.name}"

	files {
		"src/**.cpp"
	}

	includedirs {
		"%{wks.location}/IwEngine/include",
		"%{wks.location}/IwEvents/include",
		"%{wks.location}/IwLog/include"
	}

	links {
		"IwEngine"
	}

	filter "system:windows"
		cppdialect "C++11"
		systemversion "latest"

	filter "configurations:Debug"
		defines "IW_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "IW_RELEASE"
		runtime "Release"
		optimize "On"
