
#include "xx_dump.h"
#include "xx_node.h"

#include "CCDirector.h"


std::string Dump(const char* id) {
	XXCCDirector* director = XXCCDirector::funcDirectorGetInstance();
	XXCCScene* scene = director->getRunningScene();

	if (scene == NULL)
		return "";

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);

	writer.StartObject();

	writer.Key("jsonrpc");
	writer.Double(2.0);
	writer.Key("id");
	writer.String(id);

	writer.Key("result");
	writer.StartObject();

	XXNode scene_node(scene);
	
	scene_node.setDumpString(writer);

	writer.EndObject();
	writer.EndObject();

	std::string dumpString = buffer.GetString();

	return dumpString;
}
