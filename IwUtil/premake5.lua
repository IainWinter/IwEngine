project "IwUtil"
	kind "None"
	language "C++"

	files {
		"include/**.h"
	}

	includedirs {
		"include"
	}

	filter "system:windows"
		cppdialect "C++11"
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
