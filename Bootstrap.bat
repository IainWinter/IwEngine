@echo off

if not exist .\_download\ (
	mkdir .\_download
)

if not exist .\IwEngine\extern\fmod\ (
	echo Downloading FMod libs
	
	mkdir .\IwEngine\extern\fmod
	.\Tools\curl.exe -sS https://winter.dev/engine/fmod_win86.zip -o .\_download\fmod_win86.zip
	.\Tools\tar.exe -x -f .\_download\fmod_win86.zip -C .\IwEngine\extern\fmod
) else (
	echo FMod libs already installed
)

if not exist .\IwEngine\extern\assimp\ (
	echo Downloading Assimp libs

	mkdir .\IwEngine\extern\assimp
	.\Tools\curl.exe -sS https://winter.dev/engine/assimp_win86.zip -o .\_download\assimp_win86.zip
	.\Tools\tar.exe -x -f .\_download\assimp_win86.zip -C .\IwEngine\extern\assimp
) else (
	echo Assimp libs already installed
)

if exist .\_download\ (
	rmdir /S /Q .\_download
)

echo Done