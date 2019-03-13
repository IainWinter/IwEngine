project "ImGui"
    kind "StaticLib"
    language "C++"
    location  (imguidir .. blddir)
	targetdir (imguidir .. libdir)
	objdir    (imguidir .. blddir)

    files {
        "include/imgui/*.h",
        "src/*.h",
        "src/*.cpp"
    }

    excludes {
        "src/imgui_demo.cpp"
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
