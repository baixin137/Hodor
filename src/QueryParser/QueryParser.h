#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include "../HodorFS/HodorFS/HodorFS.H"
#include "../third_party/hyrise_sqlparser/src/SQLParser.h"

using namespace std;

class QueryParser {
private:
	FileManager* filesystem;
	BufferManager* buffer;

public:
	QueryParser(FileManager* fs, BufferManager* bf);

	// parse incoming CREATE statement
	void ParseCREATE(hsql::SQLStatement* statement);
	// parse incoming INSERT statement
	void ParseINSERT(hsql::SQLStatement* statement);
	// parse incoming SELECT statement
	void ParseSELECT(hsql::SQLStatement* statement);
};

#endif