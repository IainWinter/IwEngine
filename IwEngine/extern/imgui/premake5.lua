project "ImGui"
    kind "StaticLib"
    language "C++"
    location  (blddir)
	targetdir (libdir)
	objdir    (blddir)

    files {
        "include/imgui/*.h",
        "src/*.h",
        "src/imgui.cpp",
        "src/imgui_draw.cpp"
    }

    includedirs {
        "include"
    }

	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "On"
