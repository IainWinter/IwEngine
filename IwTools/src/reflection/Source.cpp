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
#include <iostream>
#include <vector>
#include <stack>

#define OUT out

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
        size_t size)
    {
        OUT << "{ \"" << name << "\", " << size << "};\n";
    }
};

class ReflectedField
    : public Reflected
{
private:
    const FieldDecl* m_field;

public:
    ReflectedField(
        const FieldDecl* field)
        : m_field(field)
    {}

    void Print(
        raw_ostream& out)
    {
        if (m_field->getType()->isFundamentalType()) {
            OUT << "{\"" << Name() << "\", GetType<" << TypeName() << ">(), " << Offset() << "};\n";
        }

        else {
            OUT << "{\"" << Name() << "\", GetClass(ClassTag<" << TypeName() << ">()), " << Offset() << "};\n";
        }
    }

    std::string Name() {
        return m_field->getName().str();
    }

    std::string TypeName() {
        return m_field->getType().getAsString();
    }

    size_t Offset() {
        return m_field->getASTContext()
            .getASTRecordLayout(m_field->getParent())
            .getFieldOffset(m_field->getFieldIndex())
            / 8;
    }
};

class ReflectedFunction
    : public Reflected
{
private:
    const FunctionDecl* m_function;

public:
    ReflectedFunction(
        const FunctionDecl* function)
        : m_function(function)
    {}

    const char* Name() {
        return m_function->getName().data();
    }

    const char* ReturnTypeName() {
        return m_function->getReturnType().getAsString().data();
    }

    size_t ParamCount() {
        return m_function->getNumParams();
    }
};

class ReflectedClass
    : public Reflected
{
private:
    const CXXRecordDecl* m_record;
    std::string m_filename;
    std::string m_outPath;

    std::vector<ReflectedField>    m_fields;
    std::vector<ReflectedFunction> m_functions;

    bool m_isSubClass;
    bool m_hasBody;
    bool m_writtenToDisk;

public:
    ReflectedClass(
        const CXXRecordDecl* record,
        std::string filename,
        std::string outPath,
        bool subclass,
        bool hasBody)
        : m_record(record)
        , m_filename(filename)
        , m_outPath(outPath)
        , m_isSubClass(subclass)
        , m_hasBody(hasBody)
        , m_writtenToDisk(false)
    {}

    void Generate(
        raw_ostream& out)
    {
        OUT <<
            "\t"  "template<>"                                               "\n"
            "\t"      "const Class* GetClass(ClassTag<" << Name() << ">) {"  "\n"
            "\t\t"        "static Class c = ";                     Print(out);
                
            for (size_t i = 0; i < m_fields.size(); i++) {
                OUT <<
            "\t\t"        "c.fields[" << i << "] = ";  m_fields[i].Print(out);
            }

            //for (size_t i = 0; i < m_functions.size(); i++) {
            //    std::cout <<
            //"\t\t\t"        "c.functions[" << i << "] = "; m_functions[i].Print(out);
            //}

            OUT <<
            "\t\t"        "return &c;"                                       "\n"
            "\t"      "}"                                                    "\n";

            m_writtenToDisk = true;
    }

    void Print(
        raw_ostream& out)
    {
        OUT << "Class(\"" << Name() << "\", " << Size() << ", " << m_fields.size() << ");\n";
    }

    void AddField(
        const FieldDecl* field)
    {
        m_fields.emplace_back(field);
    }

    void AddFunction(
        const FunctionDecl* function)
    {
        m_functions.emplace_back(function);
    }

    std::string Name() {
        return m_record->getQualifiedNameAsString();
    }

    size_t Size() {
        return m_record->getASTContext()
            .getTypeSize(m_record->getTypeForDecl())
            / 8;
    }

    const CXXRecordDecl* Record() {
        return m_record;
    }

    std::string FilePath() {
        return m_filename;
    }

    std::string OutPath() {
        return m_outPath;
    }

    bool IsSubClass() {
        return m_isSubClass;
    }

    bool HasBody() {
        return m_hasBody;
    }

    bool HasWrittenToDisk() {
        return m_writtenToDisk;
    }
};

class ClassFinder
    : public MatchFinder::MatchCallback
{
    std::vector<ReflectedClass*> m_classes;
    std::stack <ReflectedClass*> m_working;

    virtual void run(
        const MatchFinder::MatchResult& result)
    {
        //const FunctionDecl* function = result.Nodes.getNodeAs<FunctionDecl>("id");
        //if (function) {
        //    return FoundFunction(function);
        //}

        const FieldDecl* field = result.Nodes.getNodeAs<FieldDecl>("id");
        if (field) {
            return FoundField(field);
        }

        const CXXRecordDecl* record = result.Nodes.getNodeAs<CXXRecordDecl>("id");
        if (record) {
            return FoundRecord(record, result.SourceManager);
        }
    }

    virtual void onStartOfTranmslationUnit() {

    }

    virtual void onEndOfTranslationUnit() {
        std::vector<ReflectedClass*> toWrite;

        for (int i = 0; i < m_classes.size(); i++) {
            ReflectedClass* c = m_classes[i];
            if (c->HasBody()) {
                toWrite.push_back(c);
            }
        }

        for (int i = 0; i < toWrite.size(); i++) {
            for (int j = 0; i < toWrite.size(); j++) {
                if (i == j) break;

                ReflectedClass* a = toWrite[i];
                ReflectedClass* b = toWrite[j];

                if (a->Name() == b->Name()) {
                    delete a;
                    toWrite.erase(toWrite.begin() + i);
                }
            }
        }

        const int FIRST = toWrite.size() - 1;
        const int LAST  = 0;

        for (int i = FIRST; i >= LAST; i--) {
            sys::fs::OpenFlags flag = sys::fs::OpenFlags::OF_None;

            ReflectedClass* c = toWrite[i];

            if (c->HasWrittenToDisk()) {
                flag = sys::fs::OpenFlags::OF_Append;
            }

            std::error_code error;
            raw_fd_ostream out(c->OutPath(), error, flag);

            if (error) {
                std::cout << "ERROR GENERATING " << c->OutPath();
                continue;
            }

            if (!c->HasWrittenToDisk()) {
                OUT <<
                    "#pragma once"                          "\n"
                    "#include \"iw/reflection/Type.h\""     "\n"
                    "#include \"" << c->FilePath() << "\""  "\n\n"
                    "namespace iw {"                        "\n"
                    "namespace detail {"                    "\n";
            }

            c->Generate(out);

            if (c->IsSubClass()) {
                OUT << "\n";
            }
        }

        for (ReflectedClass* c : toWrite) {
            std::error_code error;
            raw_fd_ostream out(c->OutPath(), error, sys::fs::OpenFlags::OF_Append);

            if (error) {
                std::cout << "ERROR GENERATING " << c->OutPath();
                continue;
            }

            if (!c->IsSubClass()) {
                OUT << "}\n}\n";
            }

            delete c;
        }

        std::cout << "Done";
    }

    void FoundRecord(
        const CXXRecordDecl* record,
        SourceManager* source)
    {
        std::string filename = source->getFilename(record->getLocation()).str();
        std::string outpath  = filename;

        outpath.erase(outpath.find_last_of('.'));
        outpath.append(".reflect.h");

        ReflectedClass* c = new ReflectedClass(
            record, 
            filename,
            outpath, 
            record->getParent()->isRecord(), 
            record->isThisDeclarationADefinition()
        );

        m_classes.emplace_back(c);
        m_working.push(c);

        std::cout << "Record matched" << std::endl;
        record->dump();
    }

    void FoundField(
        const FieldDecl* field)
    {
        if (field->getParent() != m_working.top()->Record()) {
            m_working.pop();
        }

        m_working.top()->AddField(field);

        std::cout << "Field matched" << std::endl;
        field->dump();
    }

    void FoundFunction(
        const FunctionDecl* function)
    {
        if (function->getParent() != m_working.top()->Record()) {
            m_working.pop();
        }

        if (function->getPreviousDecl() != nullptr) {
            return;
        }

        m_working.top()->AddFunction(function);

        std::cout << "Function matched" << std::endl;
        function->dump();
    }
};

static llvm::cl::OptionCategory gToolCategory("iwreflection options");

int main(int argc, const char** argv) {
    CommonOptionsParser op(argc, argv, gToolCategory);
    ClangTool tool(op.getCompilations(), op.getSourcePathList());

    ClassFinder classFinder;
    MatchFinder finder;

    DeclarationMatcher classMatcher = cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));
    DeclarationMatcher propMatcher  = fieldDecl    (decl().bind("id"), hasAttr(attr::Annotate));
    //DeclarationMatcher funcMatcher  = functionDecl (decl().bind("id"), hasAttr(attr::Annotate));

    finder.addMatcher(classMatcher, &classFinder);
    finder.addMatcher(propMatcher,  &classFinder);
    //finder.addMatcher(funcMatcher,  &classFinder);

    return tool.run(newFrontendActionFactory(&finder).get());
}
