#include "clang-c/Index.h"
#include <iostream>
#include <unordered_map>

#define DEF_STR_FUNC(func, get_func)                        \
	std::string iw_##func(CXCursor cursor)                  \
	{                                                       \
		CXString s = clang_get##func(##get_func##(cursor)); \
		std::string r = clang_getCString(s);                \
		clang_disposeString(s);                             \
		return r;                                           \
	}                                                       \

DEF_STR_FUNC(TypeSpelling, clang_getCursorType)
DEF_STR_FUNC(CursorKindSpelling, clang_getCursorKind)
DEF_STR_FUNC(CursorSpelling)

std::string iw_TypedefUnderlyingTypeSpelling(CXCursor cursor) {

	CXString s = clang_getTypeSpelling(clang_getTypedefDeclUnderlyingType(cursor));
	std::string r = clang_getCString(s);
	clang_disposeString(s);
	return r;
}

std::unordered_map<std::string, std::string> typedefs;
std::vector<CXCursorKind> whitelist;

std::string getName(const std::string& name)
{
	auto itr = typedefs.find(name);

	if (itr != typedefs.end())
	{
		return itr->second;
	}

	return name;
}

struct userdata {
	int TemplateArgsLeft;
	int Depth;
};

CXChildVisitResult visitor(
	CXCursor cursor,
	CXCursor parent,
	CXClientData client)
{
	userdata& user = *(userdata*)client;

	CXSourceLocation location = clang_getCursorLocation(cursor);

	if (clang_Location_isFromMainFile(location) == 0)
	{
		return CXChildVisit_Continue;
	}

	CXCursorKind kind = clang_getCursorKind(cursor);

	bool whitelisted = false;

	for (CXCursorKind& whitelised_record : whitelist)
	{
		if (kind == whitelised_record)
		{
			whitelisted = true;
			break;
		}
	}

	if (!whitelisted)
	{
		return CXChildVisit_Continue;
	}

	std::cout << std::string(user.Depth * 3, '-');

	switch (kind)
	{
		case CXCursor_TypedefDecl:
		case CXCursor_TypeAliasDecl:
		{
			auto itr = typedefs.emplace(iw_TypeSpelling(cursor), iw_TypedefUnderlyingTypeSpelling(cursor));
			std::cout << "type rename " << itr.first->second << " to " << itr.first->first << "\n";

			break;
		}
		case CXCursor_TemplateTypeParameter:
		case CXCursor_NonTypeTemplateParameter:
		{
			//clang_Cursor_getNumTemplateArguments(cursor);
			
			std::cout << getName(iw_CursorSpelling(cursor));

			user.TemplateArgsLeft--;
			if (user.TemplateArgsLeft == 0)
			{
				std::cout << ">";
			}

			break;
		}
		case CXCursor_ClassTemplate:
		{
			user.TemplateArgsLeft = clang_Cursor_getTemplate (cursor);
			std::cout << "record: " << getName(iw_CursorSpelling(cursor)) << "<"; 
			break;
		}
		case CXCursor_ClassDecl:
		case CXCursor_StructDecl:
		{
			std::cout << "record: " << getName(iw_CursorSpelling(cursor));
			break;
		}

		case CXCursor_CXXBaseSpecifier:
		{
			std::cout << "base: " << getName(iw_TypeSpelling(cursor));
			break;
		}
		
		case CXCursor_FieldDecl:
		{
			std::cout << "field: " << getName(iw_TypeSpelling(cursor)) << " " << iw_CursorSpelling(cursor);
			break;
		}
	}

	std::cout << "\n";

	//std::cout
	//	<< std::string(curLevel, '-')
	//	<< " "
	//	<< iw_CursorKindSpelling(cursor)
	//	<< " (" << iw_TypeSpelling(cursor) << " " << iw_CursorSpelling(cursor) << ")\n";

	user.Depth += 1;
	clang_visitChildren(cursor, visitor, &user);
	user.Depth -= 1;

	return CXChildVisit_Continue;
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

	whitelist = {
		CXCursor_TypedefDecl,
		CXCursor_TypeAliasDecl,
		CXCursor_ClassTemplate,
		CXCursor_ClassDecl,
		CXCursor_StructDecl,
		CXCursor_CXXBaseSpecifier,
		CXCursor_FieldDecl,
		CXCursor_TemplateTypeParameter,
		CXCursor_NonTypeTemplateParameter
	};

	CXIndex index = clang_createIndex(0, 1);
	CXTranslationUnit tu = clang_createTranslationUnit(index, argv[1]);

	if (!tu)
	{
		return -2;
	}

	CXCursor cursor = clang_getTranslationUnitCursor(tu);

	userdata data;
	data.Depth = 0;
	data.TemplateArgsLeft = 0;

	clang_visitChildren(cursor, visitor, &data);

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);

	return 0;
}
