@echo off

if not exist .\_download\ (
	mkdir .\_download
)

if not exist .\IwEngine\extern\fmod\bin\ (
	echo Downloading FMod libs

	.\Tools\curl.exe -sS https://winter.dev/engine/fmod_libs.zip -o .\_download\fmod_libs.zip
	.\Tools\tar.exe -x -f .\_download\fmod_libs.zip -C .\IwEngine\extern\fmod
) else (
	echo FMod libs already installed
)

if not exist .\IwEngine\extern\assimp\bin\ (
	echo Downloading Assimp libs

	.\Tools\curl.exe -sS https://winter.dev/engine/assimp_libs.zip -o .\_download\assimp_libs.zip
	.\Tools\tar.exe -x -f .\_download\assimp_libs.zip -C .\IwEngine\extern\assimp
) else (
	echo Assimp libs already installed
)

if exist .\_download\ (
	rmdir /S /Q .\_download
)

echo Done