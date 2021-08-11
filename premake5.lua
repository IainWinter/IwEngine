iwengdir  = path.getabsolute("IwEngine")
sndbxdir  = path.getabsolute("Sandbox")
edtordir  = path.getabsolute("Editor")
plugins_dir = path.getabsolute("Plugins") -- start adding _
exprtdir  = path.getabsolute("_export")
glewdir   = iwengdir .. "/extern/glew"
imguidir  = iwengdir .. "/extern/imgui"
assimpdir = iwengdir .. "/extern/assimp"
stbdir    = iwengdir .. "/extern/stb"
jsondir   = iwengdir .. "/extern/json"
fmoddir   = iwengdir .. "/extern/fmod"
glmdir    = iwengdir .. "/extern/glm"
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
	location (iwengdir .. blddir)

group "extern"
	include (glewdir)
	include (imguidir)
group ""

	os.execute ("cmake -S " .. assimpdir  .. " -B " .. assimpdir .. blddir)

	project "wLog"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/log/**.h",
			iwengdir .. srcdir .. "/log/**.cpp"
		}

		includedirs {
			iwengdir .. incdir
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWLOG_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

-- replacing with glm to finish school project on time, will comeback to this...
-- LOL glm is templated but hardcoded to max 4x4??????????????????????????????????????????????

	project "wMath"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/math/**.h",
			iwengdir .. srcdir .. "/math/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir
		}

		defines {
			--"IW_USE_REFLECTION"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWMATH_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wUtil"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/util/**.h",
			iwengdir .. srcdir .. "/util/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir
		}

		links {
			"wLog"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
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

	project "wAudio"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/audio/**.h",
			iwengdir .. srcdir .. "/audio/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir,
			fmoddir .. incdir
		}

		libdirs {
			fmoddir .. "/lib/%{cfg.platform}"
		}

		links {	
			"wLog",
			"fmod",
			"fmodstudio"
		}

		prebuildcommands {
			"xcopy /y /f \"" .. fmoddir .. "/bin/%{cfg.platform}/fmod.dll\" \""       .. iwengdir .. bindir .. "\"",
			"xcopy /y /f \"" .. fmoddir .. "/bin/%{cfg.platform}/fmodstudio.dll\" \"" .. iwengdir .. bindir .. "\"",
			"xcopy /y /f \"" .. fmoddir .. "/lib/%{cfg.platform}/fmod.lib\" \""       .. iwengdir .. libdir .. "\"",
			"xcopy /y /f \"" .. fmoddir .. "/lib/%{cfg.platform}/fmodstudio.lib\" \"" .. iwengdir .. libdir .. "\""
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWAUDIO_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wAsset"
		kind "None"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/asset/**.h"
		}

		includedirs {
			iwengdir  .. incdir
		}

		links {	
			"wLog",
			"wUtil"
		}

		filter "system:windows"
			cppdialect "C++17"
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

	project "wEvents"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/events/**.h",
			iwengdir .. srcdir .. "/events/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir
		}

		links {
			"wLog",
			"wUtil"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWEVENTS_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wCommon"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/common/**.h",
			iwengdir .. srcdir .. "/common/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir,
			glmdir .. incdir,
			voronoidir .. incdir
		}

		links {
			"wLog",
			--"wMath",
			"wUtil",
			"wEvents"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWCOMMON_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wEntity"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/entity/**.h",
			iwengdir .. srcdir .. "/entity/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir,
			--glmdir .. incdir
		}

		links {
			"wLog",
			--"wMath", -- builds fine without math, makes sense to me
			"wUtil",
			"wEvents"
		}

		defines {
			"IW_USE_EVENTS"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWENTITY_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wRenderer"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/renderer/**.h",
			iwengdir .. srcdir .. "/renderer/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir,
			glewdir   .. incdir,
			glmdir .. incdir
		}

		links {
			"wLog",
			--"wMath", -- only some 6 pipeline functions use matrix and could prob be passed by void/float*
			"GLEW",
			"opengl32.lib"
		}

		prebuildcommands {
			"xcopy /y /f \"" .. glewdir .. bindir .. "/GLEW.dll\" \"" .. iwengdir .. bindir .. "\"",
			"xcopy /y /f \"" .. glewdir .. libdir .. "/GLEW.lib\" \"" .. iwengdir .. libdir .. "\""
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWRENDERER_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wGraphics"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/graphics/**.h",
			iwengdir .. srcdir .. "/graphics/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir,
			glewdir   .. incdir,
			stbdir    .. incdir,
			assimpdir .. incdir,
			assimpdir .. blddir .. incdir,
			glmdir .. incdir
		}

		libdirs {
			assimpdir .. blddir .. "/code/%{cfg.buildcfg}"
		}

		links {
			"wLog",
			--"wMath",
			"wUtil",
			"wRenderer",
			"wEvents",
			"wCommon",
			"GLEW",
			"opengl32.lib",
			"assimp-vc140-mt"
		}

		prebuildcommands  {
			"xcopy /y /f \"" .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.dll\" \"" .. iwengdir .. bindir .. "\"",
			"xcopy /y /f \"" .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.lib\" \"" .. iwengdir .. libdir .. "\""
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWGRAPHICS_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wInput"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/input/**.h",
			iwengdir .. srcdir .. "/input/**.cpp",
			glmdir .. incdir
		}

		includedirs {
			iwengdir  .. incdir,
			glmdir .. incdir
		}

		links {
			"wLog",
			--"wMath",
			"wUtil",
			"wEvents",
			"wCommon"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWINPUT_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wPhysics"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/physics/**.h",
			iwengdir .. srcdir .. "/physics/**.cpp"
		}

		includedirs {
			iwengdir .. incdir,
			glmdir .. incdir
		}

		links {
			"wLog",
			--"wMath",
			"wCommon"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWPHYSICS_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

	project "wEngine"
		kind "SharedLib"
		language "C++"
		location  (iwengdir .. blddir)
		targetdir (iwengdir .. bindir)
		implibdir (iwengdir .. libdir)
		objdir    (iwengdir .. blddir)

		files {
			iwengdir .. incdir .. "/iw/engine/**.h",
			iwengdir .. srcdir .. "/engine/**.h",
			iwengdir .. srcdir .. "/engine/**.cpp"
		}

		includedirs {
			iwengdir .. incdir,
			iwengdir .. srcdir .. "/engine/Platform",
			imguidir .. incdir,
			glewdir  .. incdir,
			fmoddir  .. incdir,
			glmdir .. incdir
		}

		links {
			"wLog",
			--"wMath",
			"wUtil",
			"wAudio",
			"wEvents",
			"wCommon",
			"wEntity",
			"wGraphics",
			"wRenderer", -- only beacuse of MakeLayout
			"wInput",
			"wPhysics",
			"ImGui",
			"GLEW",
			"opengl32.lib"
		}

		postbuildcommands {
			"xcopy /y /f \""    .. iwengdir .. bindir .. "/*.dll\" \"" .. exprtdir .. bindir .. "/\"",
			"xcopy /y /f \""    .. iwengdir .. libdir .. "/*.lib\" \"" .. exprtdir .. libdir .. "/\"",
			"xcopy /y /f /e \"" .. iwengdir .. incdir .. "/*\" \""     .. exprtdir .. incdir .. "/\"",
			"xcopy /y /f /e \"" .. imguidir .. incdir .. "/*\" \""     .. exprtdir .. incdir .. "/\"",
			"xcopy /y /f /e \"" .. fmoddir  .. incdir .. "/*\" \""     .. exprtdir .. incdir .. "/\"",
			"xcopy /y /f    \"" .. jsondir            .. "/*.h\" \""   .. exprtdir .. incdir .. "/json/\"",

			"xcopy /y /f \"" .. imguidir .. libdir .. "/ImGui.lib\" \"" .. exprtdir .. libdir .. "\"",

			"xcopy /y /f /e \"" .. glmdir .. incdir .. "/*\" \""     .. exprtdir .. incdir .. "/\"",
		}

		defines {
			"IMGUI_IMPL_OPENGL_LOADER_GLEW",
			"IW_USE_EVENTS"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWENGINE_DLL",
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
			glmdir .. incdir
		}

		links {
			"wMath",
			"wUtil",
			"wGraphics",
			"wInput",
			"wLog",
			"wEntity",
			"wEngine",
			"wCommon",
			"wRenderer",
			"wPhysics"
			
		}

		postbuildcommands {
			"xcopy /y /f \"" .. sand_dir .. bindir .. "/*.dll\" \"" .. exprtdir .. bindir .. "/\"",
			"xcopy /y /f \"" .. sand_dir .. libdir .. "/*.lib\" \"" .. exprtdir .. libdir .. "/\"",
			"xcopy /y /f /e \"" .. sand_dir .. incdir .. "/*\" \""  .. exprtdir .. incdir .. "/plugins/\""
		}

		defines {
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IW_PLUGIN_SAND_DLL"
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
		kind "WindowedApp"
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
			jsondir,
			glmdir .. incdir
		}

		links {
			"wLog",
			--"wMath",
			"wUtil",
			"wAudio",
			"wEvents",
			"wCommon",
			"wEntity",
			"wGraphics",
			"wRenderer", -- only beacuse of MakeLayout
			"wInput",
			"wPhysics",
			"wEngine",
			"ImGui",
			"GLEW",
			"opengl32.lib"
		}

		prebuildcommands {
			"xcopy /y /f \"" .. iwengdir  .. bindir .. "/*.dll\" \"" .. edtordir .. bindir .. "/\"",
			"xcopy /y /f \"" .. iwengdir  .. libdir .. "/*.lib\" \"" .. edtordir .. libdir .. "/\""
		}

		defines {
		}

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

--postbuildcommands {
	--"xcopy /y /f \"" .. iwengdir  .. bindir .. "/IwEngine.dll\" \"" .. edtordir .. bindir .. "\"",
--}
