@echo off
..\..\extern\llvm\bin\clang++.exe -emit-ast test.h
rename test.h.gch test.ast
