#include "clang/AST/AST.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecordLayout.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/CommandLine.h"

using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace clang::ast_matchers;
using namespace llvm;

#include "iw/reflection/Type.h"
#include <filesystem>
#include <iostream>
#include <vector>
#include <stack>

class Reflected {
public:
	//void PrintParam(
	//    raw_ostream& out,
	//    const char* name,
	//    const char* typeName)
	//{
	//    out << "{ " << name << ", GetType<" << typeName << ">()}";
	//}

	//void Print(
	//    raw_ostream& out)
	//{
	//    out << "{ GetType<" << TypeName() << ">(), " << Offset() << "};\n";
	//}

	void PrintType(
		raw_ostream& out,
		const char* name,
		size_t size);
};

class ReflectedClass;

class ReflectedField
	: public Reflected
{
private:
	const FieldDecl* m_field;
	ReflectedClass* m_parent;

public:
	ReflectedField(
		const FieldDecl* field,
		ReflectedClass* parent);

	void Print(
		raw_ostream& out) const;

	std::string Name() const;

	std::string TypeName() const;

	std::string TypeNameTIfTemplate() const;

	bool IsEnum() const;

	const ReflectedClass* Parent() const;
};

//class ReflectedFunction
//	: public Reflected
//{
//private:
//	const FunctionDecl* m_function;
//
//public:
//	ReflectedFunction(
//		const FunctionDecl* function);
//
//	const char* Name() const;
//
//	const char* ReturnTypeName() const;
//
//	size_t ParamCount() const;
//};

class ReflectedClass
	: public Reflected
{
private:
	const CXXRecordDecl* m_record;
	std::string m_filename;
	std::string m_outPath;

	std::vector<ReflectedField>    m_fields;
	//std::vector<ReflectedFunction> m_functions;

	bool m_isSubClass;
	bool m_hasBody;
	bool m_writtenToDisk;

public:
	ReflectedClass(
		const CXXRecordDecl* record,
		std::string filename,
		std::string outPath,
		bool subclass,
		bool hasBody);

	void Generate(
		raw_ostream& out);

	void Print(
		raw_ostream& out) const;

	void PrintArray(
		raw_ostream& out) const;

	void AddField(
		const FieldDecl* field);

	//void AddFunction(
	//	const FunctionDecl* function);

	std::string Name() const;

	size_t Size() const;

	const std::vector<ReflectedField>& Fields() const;

	const CXXRecordDecl* Record() const;

	std::string FilePath() const;

	std::string OutPath() const;

	bool IsSubClass() const;

	bool HasBody() const;

	bool HasWrittenToDisk() const;
};

class ClassFinder
	: public MatchFinder::MatchCallback
{
	std::vector<ReflectedClass*> m_classes;
	std::stack <ReflectedClass*> m_working;

	std::string m_outFolder;

public:
	ClassFinder(
		std::string outFolder);

private:
	virtual void run(
		const MatchFinder::MatchResult& result);

	virtual void onStartOfTranmslationUnit();

	virtual void onEndOfTranslationUnit();

	void FoundRecord(
		const CXXRecordDecl* record,
		SourceManager* source);

	void FoundField(
		const FieldDecl* field);

	void FoundFunction(
		const FunctionDecl* function);
};
