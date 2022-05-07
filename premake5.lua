iwengdir  = path.getabsolute("IwEngine")
sndbxdir  = path.getabsolute("Sandbox")
edtordir  = path.getabsolute("Editor")
plugins_dir = path.getabsolute("Plugins") -- start adding _
exprtdir  = path.getabsolute("_export")
assetdir  = path.getabsolute("_assets")
glewdir   = iwengdir .. "/extern/glew"
imguidir  = iwengdir .. "/extern/imgui"
assimpdir = iwengdir .. "/extern/assimp"
stbdir    = iwengdir .. "/extern/stb"
jsondir   = iwengdir .. "/extern/json"
fmoddir   = iwengdir .. "/extern/fmod"
glmdir    = iwengdir .. "/extern/glm"
asio_dir  = iwengdir .. "/extern/asio"
voronoidir= iwengdir .. "/extern/voronoi"

cfgname = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"
bindir  = "/bin/" .. cfgname
libdir  = "/lib/" .. cfgname
resdir  = "/assets"
blddir  = "/build"
incdir  = "/include"
srcdir  = "/src"

workspace "wEngine"
	configurations { "Debug", "Release" }
	platforms { "x64" } --"x32", 
	startproject "a_wEditor"

group "extern"
	include (glewdir)
	include (imguidir)
group ""

	project "wEverything"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/**.h",
			iwengdir .. srcdir .. "/**.h",
			iwengdir .. srcdir .. "/**.cpp"
		}

		includedirs {
			iwengdir .. incdir,
			iwengdir .. srcdir .. "/engine/Platform",
			imguidir .. incdir,
			glewdir  .. incdir,
			fmoddir  .. incdir,
			glmdir .. incdir,
			voronoidir .. incdir,
			stbdir    .. incdir,
			assimpdir .. incdir,
		}

		libdirs {
			assimpdir .. "/lib/%{cfg.platform}",
			fmoddir   .. "/lib/%{cfg.platform}"
		}

		links {
			"ImGui",
			"GLEW",
			"opengl32.lib",
			"assimp-vc140-mt",
			"fmod",
			"fmodstudio"
		}

-- export built binaries and header files to export folder

		postbuildcommands {
			"xcopy /y /f \""    .. iwengdir .. bindir .. "/*.dll\" \"" .. exprtdir .. bindir .. "/\"",
			"xcopy /y /f \""    .. iwengdir .. libdir .. "/*.lib\" \"" .. exprtdir .. libdir .. "/\"",
			"xcopy /y /f /e \"" .. iwengdir .. incdir .. "/*\" \""     .. exprtdir .. incdir .. "/\"",
			"xcopy /y /f /e \"" .. imguidir .. incdir .. "/*\" \""     .. exprtdir .. incdir .. "/\"",
			"xcopy /y /f /e \"" .. jsondir  .. incdir .. "/*\" \""     .. exprtdir .. incdir .. "/\"",

			"xcopy /y /f \"" .. imguidir    .. libdir .. "/ImGui.lib\" \"" .. exprtdir .. libdir .. "\"",

			"xcopy /y /f /e \"" .. glmdir   .. incdir .. "/*\" \""   .. exprtdir .. incdir .. "/\"",
			"xcopy /y /f /e \"" .. asio_dir .. incdir .. "/*\" \""   .. exprtdir .. incdir .. "/\"",

			"xcopy /y /f /e \"" .. fmoddir  .. incdir .. "/*\" \""                    .. exprtdir .. incdir .. "/\"",
			"xcopy /y /f \"" .. fmoddir .. "/bin/%{cfg.platform}/fmod.dll\" \""       .. exprtdir .. bindir .. "\"",
			"xcopy /y /f \"" .. fmoddir .. "/bin/%{cfg.platform}/fmodstudio.dll\" \"" .. exprtdir .. bindir .. "\"",

			
			"xcopy /y /f \""    .. glewdir .. libdir .. "/GLEW.lib\" \"" .. exprtdir .. libdir .. "\"",
			"xcopy /y /f \""    .. glewdir .. bindir .. "/GLEW.dll\" \"" .. exprtdir .. bindir .. "\"",
			"xcopy /y /f /e \"" .. glewdir .. incdir .. "/*\" \""        .. exprtdir .. incdir .. "/\""
		}

		defines {
			"IMGUI_IMPL_OPENGL_LOADER_GLEW",
			"IW_USE_EVENTS",
			"IW_ASSET_ROOT_PATH=\"" .. assetdir .. "/\""
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWENGINE_DLL", -- should rename to all follow the same _DLL macro
				"IWLOG_DLL",
				"IWMATH_DLL",
				"IWUTIL_DLL",
				"IWAUDIO_DLL",
				"IWEVENTS_DLL",
				"IWCOMMON_DLL",
				"IWENTITY_DLL",
				"IWRENDERER_DLL",
				"IWGRAPHICS_DLL",
				"IWINPUT_DLL",
				"IWPHYSICS_DLL",
				"IW_IMGUI"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"



group "Plugins"
	
	sand_dir = plugins_dir .. "/Sand"

	project "wSand"
		kind "SharedLib"
		language "C++"
		location  (sand_dir .. blddir)
		targetdir (sand_dir .. bindir)
		implibdir (sand_dir .. libdir)
		objdir    (sand_dir .. blddir)

		files {
			sand_dir  .. incdir .. "/**.h",
			sand_dir  .. srcdir .. "/**.h",
			sand_dir  .. srcdir .. "/**.cpp"
		}

		includedirs {
			sand_dir .. incdir,
			iwengdir .. incdir,
			glmdir .. incdir,
			fmoddir  .. incdir
		}

		links {
			"wEverything"
		}

		postbuildcommands {
			"xcopy /y /f \"" .. sand_dir .. bindir .. "/*.dll\" \"" .. exprtdir .. bindir .. "/\"",
			"xcopy /y /f \"" .. sand_dir .. libdir .. "/*.lib\" \"" .. exprtdir .. libdir .. "/\"",
			"xcopy /y /f /e \"" .. sand_dir .. incdir .. "/*\" \""  .. exprtdir .. incdir .. "/plugins/\""
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IW_PLUGIN_SAND_DLL",
				"IWUTIL_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"
group ""

group "Editor" -- might become it's own solution/repo
	project "wEditor"
		kind "ConsoleApp"
		language "C++"
		location  (edtordir .. blddir)
		targetdir (edtordir .. bindir)
		objdir    (edtordir .. blddir)

		files {
			edtordir .. incdir .. "/**.h",
			edtordir .. srcdir .. "/**.h",
			edtordir .. srcdir .. "/**.cpp"
		}

		includedirs {
			edtordir .. incdir,
			iwengdir .. incdir,
			imguidir .. incdir,
			fmoddir  .. incdir,
			jsondir  .. incdir,
			glmdir .. incdir
		}

		links {
			"wEverything",
			"ImGui",
			"GLEW",
			"opengl32.lib"
		}

--		prebuildcommands {
--			"xcopy /y /f \"" .. iwengdir  .. bindir .. "/*.dll\" \"" .. edtordir .. bindir .. "/\"",
--			"xcopy /y /f \"" .. iwengdir  .. libdir .. "/*.lib\" \"" .. edtordir .. libdir .. "/\""
--		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IW_IMGUI"  -- not sure if this needs to be here
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

group ""
