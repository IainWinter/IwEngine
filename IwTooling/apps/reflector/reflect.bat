
.\bin\clang++.exe -emit-ast reflect_me.cpp -I "C:\dev\IwEngine\_export\include" -DIW_USE_REFLECTION -DIW_PLATFORM_WINDOWS
move .\reflect_me.ast .\_reflect_me.ast
.\bin\wReflector.exe _reflect_me.ast > _reflect_me.json
.\bin\generateReflection.exe _reflect_me.json > reflect_out.h
del .\_reflect_me.ast
del .\_reflect_me.json