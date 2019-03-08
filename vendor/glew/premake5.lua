project "GLEW"
    kind "SharedLib"
    language "C++"
	targetdir "%{_CONFIG.vbin}/%{prj.name}"
	implibdir "%{_CONFIG.vlib}/%{prj.name}"
    objdir    "%{_CONFIG.obj}/%{prj.name}"

    includedirs {
        "include"
    }
        
    files { 
        "include/gl/*.h",
        "src/*.c"
    }
    
    excludes {
        "src/glewinfo.c",
        "src/visualinfo.c"
    }

	links {
		"opengl32.lib"
	}

    defines { 
      "WIN32",
      "WIN32_LEAN_AND_MEAN",
      "VC_EXTRALEAN",
	  "GLEW_BUILD"
    }
    
	filter "configurations:Debug"
		defines "DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "NDEBUG"
		runtime "Release"
		optimize "On"
