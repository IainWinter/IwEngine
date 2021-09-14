#include "clang-c/Index.h"

#include <iostream>
#include <unordered_map>
#include <sstream>


struct record {
	std::string Name;
	std::vector<std::string> Bases;
	std::vector<std::pair<std::string, std::string>> Members;
};

#include "iw/util/reflection/Reflect.h"

IW_BEGIN_REFLECT

inline const Class* GetClass(ClassTag<record>)
{
	static Class c = Class("record", sizeof(record), 3);
	c.fields[0] = { "name",   GetType(TypeTag<std::string>()),                                      offsetof(record, Name)    };
	c.fields[1] = { "bases",  GetType(TypeTag<std::vector<std::string>>()),                         offsetof(record, Bases)   };
	c.fields[2] = { "fields", GetType(TypeTag<std::vector<std::pair<std::string, std::string>>>()), offsetof(record, Members) };

	return &c;
}

template<size_t _s>
inline const Class* GetClass(ClassTag<record[_s]>)
{
	static Class c = Class("record[]", sizeof(record[_s]), 3);
	c.fields[0] = { "name",   GetType(TypeTag<std::string>()),                                      offsetof(record, Name) };
	c.fields[1] = { "bases",  GetType(TypeTag<std::vector<std::string>>()),                         offsetof(record, Bases) };
	c.fields[2] = { "fields", GetType(TypeTag<std::vector<std::pair<std::string, std::string>>>()), offsetof(record, Members) };

	return &c;
}

IW_END_REFLECT

#include "iw/util/reflection/serialization/JsonSerializer.h"
#include "iw/reflected/string.h"
#include "iw/reflected/vector.h"
#include "iw/reflected/pair.h"

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
	std::string TemplatedRecord;
	CXCursorKind Last;
	int Depth;

	std::vector<record> Records;
	record* CurrentRecord;

	void NewRecord(std::string name)
	{
		record& record = Records.emplace_back();
		record.Name = name;

		CurrentRecord = &record;
	}
};

CXChildVisitResult visitor(
	CXCursor cursor,
	CXCursor parent,
	CXClientData client)
{
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

	userdata& user = *(userdata*)client;

	if (   !(kind      == CXCursor_TemplateTypeParameter || kind      == CXCursor_NonTypeTemplateParameter)
		 && (user.Last == CXCursor_TemplateTypeParameter || user.Last == CXCursor_NonTypeTemplateParameter))
	{
		user.TemplatedRecord[user.TemplatedRecord.size() - 2] = '>';
		user.TemplatedRecord.pop_back();

		user.NewRecord(user.TemplatedRecord);

		std::cout << user.CurrentRecord->Name << "\n";
	}

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
			user.TemplatedRecord += getName(iw_CursorSpelling(cursor)) + ", ";
			break;
		}
		case CXCursor_ClassTemplate:
		{
			user.TemplatedRecord = getName(iw_CursorSpelling(cursor)) + "<";
			break;
		}
		case CXCursor_ClassDecl:
		case CXCursor_StructDecl:
		{
			user.NewRecord(getName(iw_CursorSpelling(cursor)));

			std::cout << user.CurrentRecord->Name << "\n";
			break;
		}

		case CXCursor_CXXBaseSpecifier:
		{
			user.CurrentRecord->Bases.push_back(getName(iw_TypeSpelling(cursor)));

			std::cout << "`- " << user.CurrentRecord->Bases.back() << "\n";
			break;
		}
		
		case CXCursor_FieldDecl:
		{
			user.CurrentRecord->Members.emplace_back(iw_CursorSpelling(cursor), getName(iw_TypeSpelling(cursor)));

			std::cout << "`- " << user.CurrentRecord->Members.back().second << " " << user.CurrentRecord->Members.back().first << "\n";
			break;
		}
	}

	//std::cout
	//	<< std::string(user.Depth, '-')
	//	<< " "
	//	<< iw_CursorKindSpelling(cursor)
	//	<< " (" << iw_TypeSpelling(cursor) << " " << iw_CursorSpelling(cursor) << ")\n";

	user.Last = kind;
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
	data.CurrentRecord = 0;

	clang_visitChildren(cursor, visitor, &data);

	{
		iw::JsonSerializer out("./test.json");
		out.Write(data.Records);
	}

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);

	return 0;
}
