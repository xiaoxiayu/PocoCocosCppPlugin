#ifndef _XX_AUTOTEST_DUMP_H
#define _XX_AUTOTEST_DUMP_H

#include "json/document.h"
#include "json/writer.h"

std::string DumpFilter(const char* id, rapidjson::Value* filter);

std::string Dump(const char* id);

#endif // _XX_AUTOTEST_DUMP_H