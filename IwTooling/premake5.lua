reflector_dir = path.getabsolute("reflector")
extern_dir = path.getabsolute("extern")
llvm_dir = extern_dir .. "/llvm"

cfgname = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"
bin_dir  = "/bin/" .. cfgname
lib_dir  = "/lib/" .. cfgname
bld_dir  = "/build"
inc_dir  = "/include"
src_dir  = "/src"

workspace "wTools"
	configurations { "Debug", "Release" }
	platforms { "x64" }
	startproject "wReflector"

	project "wReflector"
		kind "ConsoleApp"
		language "C++"
		location  (reflector_dir .. bld_dir)
		targetdir (reflector_dir .. bin_dir)
		implibdir (reflector_dir .. lib_dir)
		objdir    (reflector_dir .. bld_dir)

		files {
			reflector_dir .. src_dir .. "/**.h",
			reflector_dir .. src_dir .. "/**.cpp"
		}

		includedirs {
			llvm_dir .. inc_dir
		}

		libdirs {
			llvm_dir .. "/lib"
		}

		links {
			"libclang"
		}

		postbuildcommands {
			"xcopy /y /f \"" .. llvm_dir  .. "/bin/libclang.dll\" \"" .. reflector_dir .. bin_dir .. "/\""
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"