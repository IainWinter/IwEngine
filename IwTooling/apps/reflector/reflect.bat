
.\bin\clang++.exe -emit-ast reflect_me.cpp -I "C:\dev\IwEngine\_export\include"
move .\reflect_me.ast .\_reflect_me.ast
.\bin\wReflector.exe _reflect_me.ast > _reflect_me.json
.\bin\generateReflection.exe _reflect_me.json > reflect_out.cpp
#del .\_reflect_me.ast
#del .\_reflect_me.json