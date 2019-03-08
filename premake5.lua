workspace "IwEngine"
	configurations { "Debug", "Release" }
	platforms { "x32", "x64" }

_CONFIG = {}
_CONFIG["prefix"] = "x:/"
_CONFIG["outname"] = "%{cfg.buildcfg}.%{cfg.system}.%{cfg.architecture}"
_CONFIG["bin"] = _CONFIG["prefix"] .. "bin/" .. _CONFIG["outname"]
_CONFIG["lib"] = _CONFIG["prefix"] .. "lib/" .. _CONFIG["outname"]
_CONFIG["obj"] = _CONFIG["prefix"] .. "obj/" .. _CONFIG["outname"]
_CONFIG["vbin"] = _CONFIG["prefix"] .. "bin/" .. _CONFIG["outname"]
_CONFIG["vlib"] = _CONFIG["prefix"] .. "lib/" .. _CONFIG["outname"]

vendor_include_dirs = {}
vendor_include_dirs["imgui"] = "vendor/imgui"
vendor_include_dirs["glew"] = "vendor/glew/include"

include "vendor/glew"
include "vendor/imgui"

include "IwUtil"
include "IwEvents"
include "IwLog"
include "IwMath"
include "IwInput"
include "IwEcs"
include "IwEngine"

include "Sandbox"
