#include "clang-c/Index.h"

CXChildVisitResult visitor(
	CXCursor cursor, 
	CXCursor parent, 
	CXClientData client)
{

}

int main(int argc, char** argv)
{
	argc = 2;
	argv = new char* [2];
	argv[0] = nullptr;
	argv[1] = "C:/dev/IwEngine/IwTooling/reflector/src/test.ast";

	if (argc > 2)
	{
		return -1;
	}

	CXIndex index = clang_createIndex(0, 1);
	CXTranslationUnit tu = clang_createTranslationUnit(index, argv[1]);

	if (!tu)
	{
		return -2;
	}

	CXCursor cursor = clang_getTranslationUnitCursor(tu);

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);

	return 0;
}
