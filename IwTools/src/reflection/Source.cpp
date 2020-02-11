#include "Source.h"

#define OUT out

// REFLECTED

void Reflected::PrintType(
    raw_ostream& out,
    const char* name,
    size_t size)
{
    OUT << "{ \"" << name << "\", " << size << "};\n";
}

// REFLECTED FIELD

ReflectedField::ReflectedField(
    const FieldDecl* field,
    ReflectedClass* parent)
    : m_field(field)
    , m_parent(parent)
{}

void ReflectedField::Print(
    raw_ostream& out) const
{
    bool integral;
    if (m_field->getType()->isArrayType()) {
        integral = m_field->getType()->getArrayElementTypeNoTypeQual()->isFundamentalType();
    }

    else {
        integral = m_field->getType()->isFundamentalType();
    }

    if (IsEnum() || integral) {
        OUT << "{\"" << Name() << "\", GetType(TypeTag<"   << TypeName() << ">()), offsetof(" << Parent()->Name() << ", " << Name() << ")};\n";
    }

    else {
        OUT << "{\"" << Name() << "\", GetClass(ClassTag<" << TypeName() << ">()), offsetof(" << Parent()->Name() << ", " << Name() << ")};\n";
    }
}

std::string ReflectedField::Name() const {
    return m_field->getName().str();
}

std::string ReflectedField::TypeName() const {
    if (IsEnum()) {
        return "int"; //get underlying type at somepoint
    }

    return m_field->getType().getAsString();
}

std::string ReflectedField::TypeNameTIfTemplate() const {
    if (m_field->getType()->isTemplateTypeParmType()) {
        return m_field->getTemplateParameterList(0)->getParam(0)->getName().str();
    }

    return TypeName();
}

bool ReflectedField::IsEnum() const {
    return  m_field->getType()->isEnumeralType();
}

// REFLECTED CLASS

const ReflectedClass* ReflectedField::Parent() const {
    return m_parent;
}

ReflectedClass::ReflectedClass(
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

void ReflectedClass::Generate(
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

    OUT <<
        "\t"  "template<size_t _s>"                                          "\n"
        "\t"      "const Class* GetClass(ClassTag<" << Name() << "[_s]>) {"  "\n"
        "\t\t"        "static Class c = ";                     PrintArray(out);

        for (size_t i = 0; i < m_fields.size(); i++) {
            OUT <<
        "\t\t"        "c.fields[" << i << "] = ";  m_fields[i].Print(out);
        }

        //for (size_t i = 0; i < m_functions.size(); i++) {
        //    std::cout <<
        //"\t\t\t"        "c.functions[" << i << "] = "; m_functions[i].Print(out);
        //}

    OUT <<
        "\t\t"        "return &c;"                                           "\n"
        "\t"      "}"                                                        "\n";

        m_writtenToDisk = true;
}

void ReflectedClass::Print(
    raw_ostream& out) const
{
    OUT << "Class(\"" << Name() << "\", sizeof(" << Name() << "), " << m_fields.size() << ");\n";
}

void ReflectedClass::PrintArray(
    raw_ostream& out) const
{
    OUT << "Class(\"" << Name() << "\"\"[]\", sizeof(" << Name() << "), " << m_fields.size() << ", _s);\n";
}

void ReflectedClass::AddField(
    const FieldDecl* field)
{
    m_fields.emplace_back(field, this);
}

std::string ReflectedClass::Name() const {
    return m_record->getQualifiedNameAsString();
}

size_t ReflectedClass::Size() const {
    return m_record->getASTContext()
        .getTypeSize(m_record->getTypeForDecl())
        / 8;
}

const std::vector<ReflectedField>& ReflectedClass::Fields() const {
    return m_fields;
}

const CXXRecordDecl* ReflectedClass::Record() const {
    return m_record;
}

std::string ReflectedClass::FilePath() const {
    return m_filename;
}

std::string ReflectedClass::OutPath() const {
    return m_outPath;
}

bool ReflectedClass::IsSubClass() const {
    return m_isSubClass;
}

bool ReflectedClass::HasBody() const {
    return m_hasBody;
}

bool ReflectedClass::HasWrittenToDisk() const {
    return m_writtenToDisk;
}

// CLASS FINDER

ClassFinder::ClassFinder(
    std::string outFolder)
    : m_outFolder(outFolder)
{}

void ClassFinder::run(
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

void ClassFinder::onStartOfTranmslationUnit() {}

void ClassFinder::onEndOfTranslationUnit() {
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
            std::cout << "ERROR GENERATING " << error;
            continue;
        }

        if (!c->HasWrittenToDisk()) {
            OUT <<
                "#pragma once"                                "\n\n"
                "#include \"iw/reflection/Reflect.h\""        "\n";

            for (const ReflectedField& field : c->Fields()) {
                for (ReflectedClass* other : toWrite) {
                    if (c == other) continue;
                        
                    if (other->Name() == field.TypeNameTIfTemplate()) {
            OUT << "#include \"" << other->OutPath() << "\""  "\n";
                    }
                }
            }

            OUT <<
                "#include \"" << c->FilePath() << "\""        "\n\n"
                "namespace iw {"                              "\n"
                "namespace detail {"                          "\n";
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

void ClassFinder::FoundRecord(
    const CXXRecordDecl* record,
    SourceManager* source)
{
    std::string filename = source->getFilename(record->getLocation()).str();
    std::string outpath  = filename;

    std::filesystem::path file = filename;
    std::filesystem::path base = m_outFolder;

    std::error_code ec;
    outpath = std::filesystem::relative(file, base, ec).string();

    while(true) {
        size_t relpos = outpath.find_first_of("..\\");
        if (relpos == 0) {
            outpath.erase(0, 3);
        }

        else {
            break;
        }
    }
        
    //outpath.erase(outpath.find_last_of('.'));  // if you want to change extension
    //outpath.append(".h");
        
    outpath = m_outFolder + outpath;

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

void ClassFinder::FoundField(
    const FieldDecl* field)
{
    if (field->isAnonymousStructOrUnion()) {
        return;
    }

    if (   field->getParent() != m_working.top()->Record()
        && !field->getParent()->isAnonymousStructOrUnion())
    {
        m_working.pop();
    }

    m_working.top()->AddField(field);

    std::cout << "Field matched" << std::endl;
    field->dump();
}

static llvm::cl::OptionCategory gToolCategory("iwreflection options");

int main(int argc, const char** argv) {
    CommonOptionsParser op(--argc, argv, gToolCategory);
    ClangTool tool(op.getCompilations(), op.getSourcePathList());

    ClassFinder classFinder(argv[argc]);
    MatchFinder finder;

    DeclarationMatcher classMatcher = cxxRecordDecl(decl().bind("id"), hasAttr(attr::Annotate));
    DeclarationMatcher propMatcher  = fieldDecl    (decl().bind("id"), hasAttr(attr::Annotate));
    //DeclarationMatcher funcMatcher  = functionDecl (decl().bind("id"), hasAttr(attr::Annotate));

    finder.addMatcher(classMatcher, &classFinder);
    finder.addMatcher(propMatcher,  &classFinder);
    //finder.addMatcher(funcMatcher,  &classFinder);

    return tool.run(newFrontendActionFactory(&finder).get());
}
