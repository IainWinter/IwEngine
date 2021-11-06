@echo off

if not exist .\_download\ (
	mkdir .\_download
)

if not exist .\extern\llvm\ (
	echo Downloading LLVM libs

	mkdir .\extern\llvm
	..\Tools\curl.exe -sS https://winter.dev/engine/llvm_win64.zip -o .\_download\llvm_win64.zip
	..\Tools\tar.exe -x -f .\_download\llvm_win64.zip -C .\extern\llvm
) else (
	echo LLVM libs already installed
)

if exist .\_download\ (
	rmdir /S /Q .\_download
)

echo Done