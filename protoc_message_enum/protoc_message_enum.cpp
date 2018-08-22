// protoc_message_enum.cpp: 定义控制台应用程序的入口点。
//

#include "pch.h"
#include "File.hpp"

#include <iostream>
#include <sstream>
#include "google/protobuf/compiler/importer.h"
#include <google/protobuf/compiler/code_generator.h>

using namespace std;
using namespace google::protobuf::compiler;

struct ProtoErrorCollector : google::protobuf::compiler::MultiFileErrorCollector {
	void AddError(const string& filename, int line, int column, const string& message)
	{
		printf("error:filename=%s,line=%d,column=%d,message=%s", filename.c_str(), line, column, message.c_str());
	}
};


int main()
{
	DiskSourceTree tree;
	tree.MapPath("", ".");
	tree.Open("proto.proto");
	ProtoErrorCollector ec;
	Importer import(&tree, &ec);
	//import.Import("proto.proto");
	auto pool = import.pool();
	const google::protobuf::FileDescriptor* fd = pool->FindFileByName("proto.proto");
	evt::File file("MessageEnum.java", evt::File::normal);
	file.writeUTF8("package " + fd->package() + ";", false).writeUTF8("\nclass MessageEnum {\n\n");
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
	return 0;
}

