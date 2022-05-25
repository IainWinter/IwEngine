project "Hitbox"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	location "lib/build"
	targetdir "lib"

	files { 
		"src/Hitbox.cpp"
	}

	includedirs {
		"include",
		"../glm/include"
	}

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"