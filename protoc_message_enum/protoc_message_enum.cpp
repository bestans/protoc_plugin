// protoc_message_enum.cpp: 定义控制台应用程序的入口点。
//

#include "pch.h"
#include "File.hpp"

#include <iostream>
#include <sstream>
#include "google/protobuf/compiler/importer.h"
#include <google/protobuf/compiler/code_generator.h>
#include <google/protobuf/compiler/plugin.h>
#include <google/protobuf/testing/file.h>
#include <google/protobuf/stubs/strutil.h>
#include <google/protobuf/compiler/java/java_helpers.h>
#include <google/protobuf/io/zero_copy_stream.h>

using namespace std;
using namespace google::protobuf;
using namespace google::protobuf::compiler;

struct ProtoErrorCollector : google::protobuf::compiler::MultiFileErrorCollector {
	void AddError(const string& filename, int line, int column, const string& message)
	{
		printf("error:filename=%s,line=%d,column=%d,message=%s", filename.c_str(), line, column, message.c_str());
	}
};

class MyCodeGenerator : public CodeGenerator {
private:
	mutable int init = false;
	mutable io::ZeroCopyOutputStream* stream = nullptr;
	mutable io::Printer* printer = nullptr;
	mutable int index = 1;

public:
	void initFile(const string& package, const string& parameter, GeneratorContext* generator_context) const {
		if (init) return;

		init = true;
		string package_dir = java::JavaPackageToDir(package);
		string java_name = package_dir + parameter + ".java";
		stream = generator_context->Open(java_name);
		printer = new io::Printer(stream, '$');

		std::stringstream ss;
		ss << "package " << package << ";\n\nclass " << parameter << " {\n";
		printer->Print(ss.str().c_str());
	}

	bool Generate(const FileDescriptor* file, const string& parameter, GeneratorContext* generator_context, string* error) const {

		initFile(file->package(), parameter, generator_context);

		auto filename = file->name();
		auto names = Split(filename, ".");
		if ((int)names.size() > 0) {
			filename = names[0];
		}
		std::stringstream ss;
		for (int i = 0; i < file->message_type_count(); ++i) {
			auto msg = file->message_type(i);
			ss << "	" << filename << "_" << msg->name() << " = " << index++ << ",\n";
		}

		printer->Print(ss.str().c_str());

		return true;
	}
	virtual bool GenerateAll(const std::vector<const FileDescriptor*>& files,
		const string& parameter,
		GeneratorContext* generator_context,
		string* error) const {
		if (!CodeGenerator::GenerateAll(files, parameter, generator_context, error))
			return false;

		if (printer) {
			printer->Print("}\n");
		}
		delete printer;
		delete stream;
		return true;
	}
};
void test1() {

	evt::File test("111111.java", evt::File::normal);
	test.write("arg\n", false);

	DiskSourceTree tree;
	tree.MapPath("", ".");
	tree.Open("config/proto.proto");
	ProtoErrorCollector ec;
	Importer import(&tree, &ec);
	//import.Import("proto.proto");
	auto pool = import.pool();
	const google::protobuf::FileDescriptor* fd = pool->FindFileByName("proto.proto");
	evt::File file("MessageEnum.java", evt::File::normal);
	auto s = "package " + fd->package() + ";\n";
	cout << s << endl << fd->name();
	file.write(s, false).writeUTF8("\nclass MessageEnum {\n");
	{//if (fd->options().has_java_package()) {
		cout << fd->package() << endl;
	}
	int index = 1;
	std::stringstream str;
	for (int i = 0; i < fd->message_type_count(); ++i) {
		auto msg = fd->message_type(i);
		std::cout << msg->name() << std::endl;
		str << "	" << msg->name() << " = " << index++ << ",\n";
	}
	str << "}\n";
	file.writeUTF8(str.str());
	system("PAUSE");
}
int main(int argc, char* argv[])
{
	MyCodeGenerator generator;
	return google::protobuf::compiler::PluginMain(argc, argv, &generator);
	return 0;
}

