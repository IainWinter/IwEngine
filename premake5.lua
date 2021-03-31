iwengdir  = path.getabsolute("IwEngine")
sndbxdir  = path.getabsolute("Sandbox")
edtordir  = path.getabsolute("Editor")
iwtoldir  = path.getabsolute("IwTools")
sandgamedir  = path.getabsolute("Games/SandGame")
glewdir   = iwengdir .. "/extern/glew"
imguidir  = iwengdir .. "/extern/imgui"
assimpdir = iwengdir .. "/extern/assimp"
stbdir    = iwengdir .. "/extern/stb"
llvmdir   = iwengdir .. "/extern/llvm"
jsondir   = iwengdir .. "/extern/json"
fmoddir   = iwengdir .. "/extern/fmod"

cfgname = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"
bindir  = "/bin/" .. cfgname
libdir  = "/lib/" .. cfgname
resdir  = "/assets"
blddir  = "/build"
incdir  = "/include"
srcdir  = "/src"

workspace "wEngine"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	startproject "a_wEditor"
	location (iwengdir .. blddir)

	include (glewdir) --temp and annoying
	include (imguidir)

	os.execute ("cmake -S " .. assimpdir  .. " -B " .. assimpdir .. blddir)

	--filter "platforms:x64" -- doesnt filter
		--print("---------------------------- 64 ----------------------------")
		--os.execute ("cmake -S " .. llvmdir .. "/llvm -B " .. llvmdir .. blddir .. " -DLLVM_ENABLE_PROJECTS=clang -A x64 -Thost=x64 -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN=ON -DLLVM_TARGETS_TO_BUILD='X86'")

	--filter "platforms:x32"
	--	print("---------------------------- 32 ----------------------------")
	--	os.execute ("cmake -S " .. llvmdir .. "/llvm -B " .. llvmdir .. blddir .. " -DLLVM_ENABLE_PROJECTS=clang -A Win32 -Thost=Win32 -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN=ON -DLLVM_TARGETS_TO_BUILD='X86'")

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
			iwengdir  .. incdir,
			iwtoldir .. incdir
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
			iwengdir  .. incdir,
			iwtoldir .. incdir
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
			iwengdir  .. incdir,
			iwtoldir .. incdir
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
			fmoddir .. incdir,
			iwtoldir .. incdir
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
			iwengdir  .. incdir,
			iwtoldir .. incdir
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
			iwengdir  .. incdir,
			iwtoldir .. incdir
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
			iwtoldir .. incdir
		}

		links {
			"wLog",
			"wMath",
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
			iwtoldir .. incdir
		}

		links {
			"wLog",
			"wMath",
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
			iwtoldir .. incdir
		}

		links {
			"wLog",
			"wMath",
			"GLEW",
			"opengl32.lib"
		}

		prebuildcommands {
			"xcopy /y /f \"" .. glewdir .. bindir .. "/GLEW.dll\" \"" .. edtordir .. bindir .. "\""
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
			iwtoldir .. incdir
		}

		libdirs {
			assimpdir .. blddir .. "/code/%{cfg.buildcfg}"
		}

		links {
			"wLog",
			"wMath",
			"wUtil",
			"wRenderer",
			"wEvents",
			"wCommon",
			"GLEW",
			"opengl32.lib",
			"assimp-vc140-mt"
		}

		prebuildcommands  {
			"xcopy /y /f \"" .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.dll\" \"" .. iwengdir .. bindir .. "\""
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
			iwengdir .. srcdir .. "/input/**.cpp"
		}

		includedirs {
			iwengdir  .. incdir,
			iwtoldir .. incdir
		}

		links {
			"wLog",
			"wMath",
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
			iwengdir  .. incdir,
			iwtoldir .. incdir
		}

		links {
			"wLog",
			"wMath",
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
			iwengdir  .. incdir,
			iwengdir  .. srcdir .. "/engine/Platform",
			imguidir  .. incdir,
			glewdir   .. incdir,
			fmoddir   .. incdir,
			iwtoldir .. incdir
		}

		links {
			"wLog",
			"wMath",
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

		defines {
			"IMGUI_IMPL_OPENGL_LOADER_GLEW",
			"IW_USE_EVENTS"
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines {
				"IW_PLATFORM_WINDOWS",
				"IWENGINE_DLL"
			}

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"
	
	project "a_wEditor"
		kind "SharedLib"
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
			iwtoldir .. incdir,
			imguidir .. incdir,
			fmoddir  .. incdir,
			iwtoldir .. incdir,
			jsondir
		}

		links {
			"wLog",
			"wMath",
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

		prebuildcommands  {
			--"xcopy /y /f \"" .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.dll\" \"" .. edtordir .. bindir .. "\"",
			--"xcopy /y /f \"" .. fmoddir   ..           "/bin/%{cfg.platform}/fmod.dll\" \""             .. edtordir .. bindir .. "\"",
			--"xcopy /y /f \"" .. fmoddir   ..           "/bin/%{cfg.platform}/fmodstudio.dll\" \""       .. edtordir .. bindir .. "\"",
			"xcopy /y /f \"" .. iwengdir  .. bindir .. "/*.dll\" \""                             .. edtordir .. bindir .. "\""--,
			--"xcopy /e /y /f /i \"" .. edtordir  .. resdir .. "\" \"" .. edtordir .. blddir .. resdir .. "\"",
		}

		defines {
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

	project "a_SandGame"
		kind "WindowedApp"
		language "C++"
		location  (sandgamedir .. blddir)
		targetdir (sandgamedir .. bindir)
		objdir    (sandgamedir .. blddir)

		files {
			sandgamedir .. incdir .. "/**.h",
			sandgamedir .. srcdir .. "/**.h",
			sandgamedir .. srcdir .. "/**.cpp"
		}

		includedirs {
			sandgamedir .. incdir,
			iwengdir .. incdir,
			iwtoldir .. incdir,
			imguidir .. incdir,
			fmoddir  .. incdir,
			iwtoldir .. incdir,
			jsondir
		}

		links {
			"wLog",
			"wMath",
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

		prebuildcommands  {
			--"xcopy /y /f \"" .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.dll\" \"" .. sandgamedir .. bindir .. "\"",
			--"xcopy /y /f \"" .. fmoddir   ..           "/bin/%{cfg.platform}/fmod.dll\" \""             .. sandgamedir .. bindir .. "\"",
			--"xcopy /y /f \"" .. fmoddir   ..           "/bin/%{cfg.platform}/fmodstudio.dll\" \""       .. sandgamedir .. bindir .. "\"",
			"xcopy /y /f \"" .. iwengdir  .. bindir .. "/*.dll\" \""                             .. edtordir .. bindir .. "\""--,
			--"xcopy /e /y /f /i \"" .. sandgamedir  .. resdir .. "\" \"" .. sandgamedir .. blddir .. resdir .. "\"",
		}

		defines {
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

	project "z_wReflector"
		kind "ConsoleApp"
		language "C++"
		location  (iwtoldir .. blddir .. "/reflection")
		targetdir (iwtoldir .. bindir .. "/reflection")
		objdir    (iwtoldir .. blddir .. "/reflection")

		files {
			iwtoldir .. incdir .. "/iw/reflection/**.h",
			iwtoldir .. srcdir .. "/reflection/**.h",
			iwtoldir .. srcdir .. "/reflection/**.cpp"
		}

		includedirs {
			iwtoldir .. incdir,
			llvmdir  .. blddir .. "/tools/clang" .. incdir,
			llvmdir  .. blddir .. incdir,
			llvmdir  .. "/llvm"  .. incdir,
			llvmdir  .. "/clang" .. incdir,
			jsondir
		}

		libdirs {
			llvmdir .. blddir .. "/%{cfg.buildcfg}/lib",
			llvmdir .. blddir .. "/%{cfg.buildcfg}/bin",
		}

		links {
			"version.lib",
			"libclang.lib",
			"clangAnalysis.lib",
			--"clangARCMigrate.lib",
			"clangAST.lib",
			"clangASTMatchers.lib",
			"clangBasic.lib",
			--"clangCodeGen.lib",
			--"clangCrossTU.lib",
			--"clangDependencyScanning.lib",
			--"clangDirectoryWatcher.lib",
			"clangDriver.lib",
			--"clangDynamicASTMatchers.lib",
			"clangEdit.lib",
			--"clangFormat.lib",
			"clangFrontend.lib",
			--"clangFrontendTool.lib",
			--"clangHandleCXX.lib",
			--"clangHandleLLVM.lib",
			--"clangIndex.lib",
			"clangLex.lib",
			"clangParse.lib",
			--"clangRewrite.lib",
			--"clangRewriteFrontend.lib",
			"clangSema.lib",
			"clangSerialization.lib",
			--"clangStaticAnalyzerCheckers.lib",
			--"clangStaticAnalyzerCore.lib",
			--"clangStaticAnalyzerFrontend.lib",
			"clangTooling.lib",
			--"clangToolingASTDiff.lib",
			--"clangToolingCore.lib",
			--"clangToolingInclusions.lib",
			--"clangToolingRefactoring.lib",
			--"clangToolingSyntax.lib",
			--"clangTransformer.lib",
			--"LLVMAggressiveInstCombine.lib",
			--"LLVMAnalysis.lib",
			--"LLVMAsmPrinter.lib",
			"LLVMBinaryFormat.lib",
			--"LLVMBitReader.lib",
			"LLVMBitstreamReader.lib",
			--"LLVMBitWriter.lib",
			--"LLVMCFGuard.lib",
			--"LLVMCodeGen.lib",
			"LLVMCore.lib",
			--"LLVMDebugInfoCodeView.lib",
			--"LLVMDebugInfoDWARF.lib",
			--"LLVMDebugInfoMSF.lib",
			"LLVMDemangle.lib",
			"LLVMFrontendOpenMP.lib",
			--"LLVMGlobalISel.lib",
			--"LLVMInstCombine.lib",
			"LLVMMC.lib",
			--"LLVMMCDisassembler.lib",
			"LLVMMCParser.lib",
			--"LLVMObject.lib",
			"LLVMOption.lib",
			"LLVMProfileData.lib",
			"LLVMRemarks.lib",
			--"LLVMScalarOpts.lib",
			--"LLVMSelectionDAG.lib",
			"LLVMSupport.lib",
			--"LLVMTableGen.lib",
			--"LLVMTableGenGlobalISel.lib",
			--"LLVMTarget.lib",
			--"LLVMTextAPI.lib",
			--"LLVMTransformUtils.lib",
			--"LLVMX86AsmParser.lib",
			--"LLVMX86CodeGen.lib",
			--"LLVMX86Desc.lib",
			--"LLVMX86Disassembler.lib",
			--"LLVMX86Info.lib",
			--"LLVMX86Utils.lib"
		}

		postbuildcommands  {
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/libclang.lib\" \""            .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangAnalysis.lib\" \""       .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangAST.lib\" \""            .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangASTMatchers.lib\" \""    .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangBasic.lib\" \""          .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangDriver.lib\" \""         .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangEdit.lib\" \""           .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangFrontend.lib\" \""       .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangLex.lib\" \""            .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangParse.lib\" \""          .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangSema.lib\" \""           .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangSerialization.lib\" \""  .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangTooling.lib\" \""        .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMBinaryFormat.lib\" \""    .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMBitstreamReader.lib\" \"" .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMCore.lib\" \""            .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMDemangle.lib\" \""        .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMFrontendOpenMP.lib\" \""  .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMMC.lib\" \""              .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMMCParser.lib\" \""        .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMOption.lib\" \""          .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMProfileData.lib\" \""     .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMRemarks.lib\" \""         .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMSupport.lib\" \""         .. iwtoldir .. bindir .. "/reflection\"",
				"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/bin/libclang.dll\" \""            .. iwtoldir .. bindir .. "/reflection\"",
		}

		filter "system:windows"
			cppdialect "C++17"
			systemversion "latest"
			defines "IW_PLATFORM_WINDOWS"

		filter "configurations:Debug"
			defines "IW_DEBUG"
			runtime "Debug"
			symbols "On"

		filter "configurations:Release"
			defines "IW_RELEASE"
			runtime "Release"
			optimize "On"

--postbuildcommands {
	--"xcopy /y /f \"" .. iwengdir  .. bindir .. "/IwEngine.dll\" \"" .. edtordir .. bindir .. "\"",
--}
