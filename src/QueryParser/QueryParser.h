#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <iomanip>

#include "../Helper.h"
#include "../HodorFS/HodorFS.h"
#include "../third_party/hyrise_sqlparser/src/SQLParser.h"

using namespace std;

class Entry { // each Entry stores a tuple to print
private:
	size_t width; // width of each box
	size_t num; // num of attributes to print

public:
	Entry(size_t w);

	size_t size();
	// void print(); // print this entry

	unordered_map<string, string> attributeList;
};

class QueryResult {
public:
	QueryResult();
	vector<Entry*> item;
	vector<string> attrnames;
	unordered_map<string, vector<vector<string>>> groups;
};

class QueryParser {
private:

	FileManager* filesystem;
	BufferManager* buffer;

	template <class T> void filter(QueryResult* entries, hsql::OperatorType op, T val, Attribute* attr, vector<Attribute*> selectList);

public:
	QueryParser(FileManager* fs, BufferManager* bf);

	template<typename T> void printElement(T t, const int& width);

	// parse incoming CREATE statement
	void ParseCREATE(const hsql::SQLStatement* statement);
	// parse incoming INSERT statement
	void ParseINSERT(const hsql::SQLStatement* statement);
	// parse incoming SELECT statement
	void ParseSELECT(const hsql::SQLStatement* statement);
};

#endif