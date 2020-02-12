iwengdir  = path.getabsolute("IwEngine")
sndbxdir  = path.getabsolute("Sandbox")
edtordir  = path.getabsolute("Editor")
iwtoldir  = path.getabsolute("IwTools")
glewdir   = iwengdir .. "/extern/glew"
imguidir  = iwengdir .. "/extern/imgui"
assimpdir = iwengdir .. "/extern/assimp"
stbdir    = iwengdir .. "/extern/stb"
llvmdir   = iwengdir .. "/extern/llvm"

cfgname = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"
achname = "%{cfg.architecture}"
bindir  = "/bin/" .. cfgname
libdir  = "/lib/" .. cfgname
resdir  = "/assets"
blddir  = "/build"
incdir  = "/include"
srcdir  = "/src"

workspace "IwEngine"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }
	startproject "Editor"
	location (iwengdir .. blddir)

include (glewdir)
include (imguidir)
os.execute ("cmake -S " .. assimpdir    .. " -B " .. assimpdir .. blddir)

filter "platforms:x64" -- doesnt filter
	print("---------------------------- 64 ----------------------------")
	--os.execute ("cmake -S " .. llvmdir .. "/llvm -B " .. llvmdir .. blddir .. " -DLLVM_ENABLE_PROJECTS=clang -A x64 -Thost=x64 -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN=ON -DLLVM_TARGETS_TO_BUILD='X86'")

--filter "platforms:x32"
--	print("---------------------------- 32 ----------------------------")
--	os.execute ("cmake -S " .. llvmdir .. "/llvm -B " .. llvmdir .. blddir .. " -DLLVM_ENABLE_PROJECTS=clang -A Win32 -Thost=Win32 -DLLVM_TEMPORARILY_ALLOW_OLD_TOOLCHAIN=ON -DLLVM_TARGETS_TO_BUILD='X86'")


project "IwEngine"
	kind "SharedLib"
	language "C++"
	location  (iwengdir .. blddir)
	targetdir (iwengdir .. bindir)
	implibdir (iwengdir .. libdir)
	objdir    (iwengdir .. blddir)

	files {
		iwengdir .. incdir .. "/**.h",
		iwengdir .. srcdir .. "/**.h",
		iwengdir .. srcdir .. "/**.cpp"
	}

	includedirs {
		iwengdir  .. incdir,
		iwengdir  .. srcdir .. "/engine/Platform",
		iwtoldir  .. incdir,
		glewdir   .. incdir,
		imguidir  .. incdir,
		assimpdir .. incdir,
		assimpdir .. blddir .. incdir,
		stbdir .. incdir
	}

	libdirs {
		assimpdir .. blddir .. "/code/%{cfg.buildcfg}"
	}

	links {
		"GLEW",
		"ImGui",
		"assimp-vc140-mt",
		"opengl32.lib"
	}

	disablewarnings { 
		"26495",
		"26812",
		"4200"
	}

	defines {
		"IMGUI_IMPL_OPENGL_LOADER_GLEW",
		"IW_USE_REFLECTION"
	}

	filter "system:windows"
		cppdialect "C++17"
		systemversion "latest"
		defines {
			"IW_PLATFORM_WINDOWS",
			"IW_BUILD_DLL"
		}

	filter "configurations:Debug"
		defines "IW_DEBUG"
		runtime "Debug"
		symbols "On"

	filter "configurations:Release"
		defines "IW_RELEASE"
		runtime "Release"
		optimize "On"

project "IwReflection"
	kind "ConsoleApp"
	language "C++"
	location  (iwtoldir .. blddir)
	targetdir (iwtoldir .. bindir)
	objdir    (iwtoldir .. blddir)

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
		llvmdir  .. "/clang" .. incdir
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
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/libclang.lib\" \""            .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangAnalysis.lib\" \""       .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangAST.lib\" \""            .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangASTMatchers.lib\" \""    .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangBasic.lib\" \""          .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangDriver.lib\" \""         .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangEdit.lib\" \""           .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangFrontend.lib\" \""       .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangLex.lib\" \""            .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangParse.lib\" \""          .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangSema.lib\" \""           .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangSerialization.lib\" \""  .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/clangTooling.lib\" \""        .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMBinaryFormat.lib\" \""    .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMBitstreamReader.lib\" \"" .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMCore.lib\" \""            .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMDemangle.lib\" \""        .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMFrontendOpenMP.lib\" \""  .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMMC.lib\" \""              .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMMCParser.lib\" \""        .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMOption.lib\" \""          .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMProfileData.lib\" \""     .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMRemarks.lib\" \""         .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/lib/LLVMSupport.lib\" \""         .. iwtoldir .. bindir .. "\"",
			"xcopy /y /f \"" .. llvmdir .. blddir .. "/%{cfg.buildcfg}/bin/libclang.dll\" \""            .. iwtoldir .. bindir .. "\"",
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

project "Editor"
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
		iwtoldir .. incdir,
		imguidir .. incdir
	}

	links {
		"IwEngine",
		"ImGui",
		"opengl32.lib"
	}

	prebuildcommands  {
		"xcopy /y /f \""    .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.dll\" \"" .. edtordir .. bindir .. "\"",
		"xcopy /y /f \""    .. iwengdir  .. bindir .. "/IwEngine.dll\" \"" .. edtordir .. bindir .. "\"",
		"xcopy /y /f \""    .. glewdir   .. bindir .. "/GLEW.dll\" \""     .. edtordir .. bindir .. "\"",
		"xcopy /e /y /f /i \"" .. edtordir  .. resdir .. "\" \""           .. edtordir .. blddir .. resdir .. "\"",
		"xcopy /e /y /f /i \"" .. edtordir  .. resdir .. "\" \""           .. edtordir .. bindir .. resdir .. "\""
	}

	defines "IW_USE_REFLECTION"

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

--project "Sandbox"
--	kind "WindowedApp"
--	language "C++"
--	location  (sndbxdir .. blddir)
--	targetdir (sndbxdir .. bindir)
--	objdir    (sndbxdir .. blddir)
--
--	files {
--		sndbxdir .. incdir .. "/**.h",
--		sndbxdir .. srcdir .. "/**.h",
--		sndbxdir .. srcdir .. "/**.cpp"
--	}
--
--	includedirs {
--		sndbxdir .. incdir,
--		iwengdir .. incdir,
--		imguidir .. incdir
--	}
--
--	links {
--		"IwEngine",
--		"ImGui",
--		"opengl32.lib"
--	}
--
--	prebuildcommands  {
--		"xcopy /y /f \""    .. assimpdir .. blddir .. "/code/%{cfg.buildcfg}/assimp-vc140-mt.dll\" \"" .. sndbxdir .. bindir .. "\"",
--		"xcopy /y /f \""    .. iwengdir  .. bindir .. "/IwEngine.dll\" \"" .. sndbxdir .. bindir .. "\"",
--		"xcopy /y /f \""    .. glewdir   .. bindir .. "/GLEW.dll\" \""     .. sndbxdir .. bindir .. "\"",
--		"xcopy /e /y /f /i \"" .. sndbxdir  .. resdir .. "\" \""           .. sndbxdir .. blddir .. resdir .. "\"",
--		"xcopy /e /y /f /i \"" .. sndbxdir  .. resdir .. "\" \""           .. sndbxdir .. bindir .. resdir .. "\""
--	}
--
--	filter "system:windows"
--		cppdialect "C++17"
--		systemversion "latest"
--		defines "IW_PLATFORM_WINDOWS"
--
--	filter "configurations:Debug"
--		defines "IW_DEBUG"
--		runtime "Debug"
--		symbols "On"
--
--	filter "configurations:Release"
--		defines "IW_RELEASE"
--		runtime "Release"
--		optimize "On"
--
