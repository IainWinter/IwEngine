project "ImGui"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	location "lib/build"
	targetdir "lib"

    files {
        "src/*.cpp"
    }

    includedirs {
        "include/imgui",
		"src",
		"../sdl/include/sdl"
    }

	filter "configurations:Debug"
		defines { "DEBUG" }
		symbols "On"

	filter "configurations:Release"
		defines { "NDEBUG" }
		optimize "On"

