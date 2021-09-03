xcopy /y /f "C:/dev/IwEngine/IwEngine/bin/Release.windows.x86_64/*.dll" "C:/dev/IwEngine/_export/bin/Release.windows.x86_64/"
xcopy /y /f "C:/dev/IwEngine/IwEngine/lib/Release.windows.x86_64/*.lib" "C:/dev/IwEngine/_export/lib/Release.windows.x86_64/"
xcopy /y /f "C:/dev/IwEngine/IwEngine/bin/Debug.windows.x86_64/*.dll" "C:/dev/IwEngine/_export/bin/Debug.windows.x86_64/"
xcopy /y /f "C:/dev/IwEngine/IwEngine/lib/Debug.windows.x86_64/*.lib" "C:/dev/IwEngine/_export/lib/Debug.windows.x86_64/"

xcopy /y /f /e "C:/dev/IwEngine/IwEngine/include/*" "C:/dev/IwEngine/_export/include/"