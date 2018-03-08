#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "../Helper.h"
#include "../HodorFS/HodorFS.h"
#include "../third_party/hyrise_sqlparser/src/SQLParser.h"

using namespace std;

class QueryParser {
private:
	FileManager* filesystem;
	BufferManager* buffer;

public:
	QueryParser(FileManager* fs, BufferManager* bf);

	// parse incoming CREATE statement
	void ParseCREATE(const hsql::SQLStatement* statement);
	// parse incoming INSERT statement
	void ParseINSERT(const hsql::SQLStatement* statement);
	// parse incoming SELECT statement
	void ParseSELECT(const hsql::SQLStatement* statement);
};

#endif