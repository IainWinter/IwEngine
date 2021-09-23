xcopy /y /f "./IwEngine/bin/Release.windows.x86_64/*.dll" "./_export/bin/Release.windows.x86_64/"
xcopy /y /f "./IwEngine/lib/Release.windows.x86_64/*.lib" "./_export/lib/Release.windows.x86_64/"
xcopy /y /f "./IwEngine/bin/Debug.windows.x86_64/*.dll" "./_export/bin/Debug.windows.x86_64/"
xcopy /y /f "./IwEngine/lib/Debug.windows.x86_64/*.lib" "./_export/lib/Debug.windows.x86_64/"

xcopy /y /f /e "./IwEngine/include/*" "./_export/include/"