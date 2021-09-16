#include "clang-c/Index.h"

#include <iostream>
#include <unordered_map>

#include "record.h"

#include "iw/util/reflection/serialization/JsonSerializer.h"

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
	int Depth;

	std::vector<record> Records;
	record* CurrentRecord;
	bool IsRecordDecl;

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

	if (clang_Location_isInSystemHeader(location) != 0)
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
		return CXChildVisit_Recurse;
	}

	userdata& user = *(userdata*)client;

	switch (kind)
	{
		case CXCursor_TypedefDecl:
		case CXCursor_TypeAliasDecl:
		{
			typedefs.emplace(iw_TypeSpelling(cursor), iw_TypedefUnderlyingTypeSpelling(cursor));
			break;
		}
		case CXCursor_TemplateTypeParameter:
		{
			if (user.IsRecordDecl)
			{
				user.CurrentRecord->TemplateArgs.emplace_back(iw_CursorSpelling(cursor), "typename");
			}
			break;
		}
		case CXCursor_NonTypeTemplateParameter:
		{
			if (user.IsRecordDecl)
			{
				user.CurrentRecord->TemplateArgs.emplace_back(iw_CursorSpelling(cursor), iw_TypeSpelling(cursor));
			}
			break;
		}
		case CXCursor_ClassTemplate:
		case CXCursor_ClassDecl:
		case CXCursor_StructDecl:
		{
			user.NewRecord(getName(iw_CursorSpelling(cursor)));
			user.IsRecordDecl = true;
			break;
		}
		case CXCursor_CXXBaseSpecifier:
		{
			user.CurrentRecord->Bases.push_back(getName(iw_TypeSpelling(cursor)));
			break;
		}
		case CXCursor_FieldDecl:
		{
			user.CurrentRecord->Fields.emplace_back(iw_CursorSpelling(cursor), getName(iw_TypeSpelling(cursor)));
			break;
		}
		case CXCursor_FunctionTemplate:
		case CXCursor_FunctionDecl:
		{
			user.IsRecordDecl = false;
			break;
		}
		case CXCursor_AnnotateAttr:
		{
			user.CurrentRecord->IncludeInOutput = true;

			std::cout << iw_CursorSpelling(cursor);
			break;
		}
	}

	user.Depth += 1;
	clang_visitChildren(cursor, visitor, &user);
	user.Depth -= 1;

	return CXChildVisit_Continue;
}

void print_comma(std::ostream& stream, int index, int count)
{
	if (index < count - 1)
	{
		stream << ",";
	}
}

int main(int argc, char** argv)
{
	// validate inputs

	argc = 2;
	argv = new char*[2];
	argv[1] = "C:/dev/IwEngine/IwTooling/apps/reflector/_reflect_me.ast";

	if (argc != 2) {
		std::cout << "please specify <source.ast>";
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
		CXCursor_NonTypeTemplateParameter,
		CXCursor_FunctionTemplate,
		CXCursor_FunctionDecl,
		CXCursor_AnnotateAttr
	};

	CXIndex index = clang_createIndex(0, 1);
	CXTranslationUnit tu = clang_createTranslationUnit(index, argv[1]);

	if (!tu)
	{
		std::cout << "failed to load ast";
		return -2;
	}

	CXCursor cursor = clang_getTranslationUnitCursor(tu);

	userdata data;
	data.Depth = 0;
	data.CurrentRecord = 0;

	clang_visitChildren(cursor, visitor, &data);

	// print json

	std::ostream& ss = std::cout;

	ss << "{";
	ss << "\"records\":[";

	int record_count = data.Records.size();
	for (int i = 0; i < record_count; i++)
	{
		const record& record = data.Records.at(i);

		if (!record.IncludeInOutput) continue;

		ss << "{";

		ss << "\"type_name\":" << "\"" << record.GetNameWithTArgs() << "\"";

		if (record.TemplateArgs.size() > 0)
		{
			ss << ",\"targs\":[";

			int targ_count = record.TemplateArgs.size();
			for (int t = 0; t < targ_count; t++)
			{
				const auto& [targ_name, targ_type_name] = record.TemplateArgs.at(t);

				ss << "{";
				
				ss << "\"name\":"      << "\"" << targ_name      << "\",";
				ss << "\"type_name\":" << "\"" << targ_type_name << "\"";

				ss << "}";
				print_comma(ss, t, targ_count);
			}

			ss << "]";
		}

		if (record.Fields.size() > 0)
		{
			ss << ",\"fields\":[";

			int field_count = record.Fields.size();
			for (int f = 0; f < field_count; f++)
			{
				const auto& [field_name, field_type_name] = record.Fields.at(f);

				ss << "{";
				
				ss << "\"name\":"      << "\"" << field_name      << "\",";
				ss << "\"type_name\":" << "\"" << field_type_name << "\"";

				ss << "}";
				print_comma(ss, f, field_count);
			}

			ss << "]";
		}

		if (record.Bases.size() > 0)
		{
			ss << ",\"bases\":[";

			int base_count = record.Bases.size();
			for (int b = 0; b < base_count; b++)
			{
				const std::string& base_type_name = record.Bases.at(b);
				ss << "\"" << base_type_name << "\"";
				print_comma(ss, b, base_count);
			}

			ss << "]";
		}


		ss << "}";
		print_comma(ss, i, record_count);
	}

	ss << "]";
	ss << "}";

	/*
	
	{
		"records": [
			{
				"type_name": "vector2",
				"fields": [
					{
						"name": "x",
						"type_name": "float"
					},
					{
						"name": "y",
						"type_name": "float"
					}
				],
				"bases": [
					"vec<2, float>"
				]
			},
			{
				"type_name": "vec<2, float>",
				"fields": [
					{
						"name": "x",
						"type_name": "float"
					},
					{
						"name": "y",
						"type_name": "float"
					}
				],
				"bases": [
					"vec<2, float>"
				]
			}
		]
	}
	*/

	clang_disposeTranslationUnit(tu);
	clang_disposeIndex(index);

	return 0;
}
