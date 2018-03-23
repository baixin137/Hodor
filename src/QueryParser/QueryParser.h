#ifndef QUERYPARSER_H
#define QUERYPARSER_H

#include <iomanip>

#include "../Helper.h"
#include "../HodorFS/HodorFS.h"
#include "../third_party/hyrise_sqlparser/src/SQLParser.h"

using namespace std;

enum FuncType { // aggregation function types
	aFuncAVG,
	aFuncCOUNT,
	aFuncMIN,
	aFuncMAX,
	aFuncSUM,
	aFuncNA
};

class Column { // select columns
private:
	string attrname;

public:
	Column(Attribute* attr, FuncType t, string s);
	Column(Attribute* attr, string s);
	string name();
	
	Attribute* attribute;
	FuncType type;
};

class Entry { // each Entry stores a tuple to print
private:
	size_t width; // width of each box
	size_t num; // num of attributes to print

public:
	Entry(size_t w);

	size_t size();
	// void print(); // print this entry

	// everything in the select list
	unordered_map<string, string> attributeList; // attrname : attrvalue
	// everything in the groupby list
	unordered_map<string, string> attributeGroupby; // attrname : attrvalue
};

class QueryResult {
public:
	QueryResult();
	vector<Entry*> item;
	vector<string> attrnames;
	set<string> attrset;
	unordered_map<string, pair<vector<string>, size_t>> groups;

	template<typename T> void PrintElement(T t, const int& width);
	void PrintGroup(vector<string>& order, vector<string>& attrorder, unordered_map<string, string>& attributes);
	void PrintAll();

	void AddAttribute(string attr);
};

class QueryParser {
private:

	FileManager* filesystem;
	BufferManager* buffer;

	bool ConditionMet(hsql::OperatorType op, int target, int condition);
	bool ConditionMet(hsql::OperatorType op, double target, double condition);
	bool ConditionMet(hsql::OperatorType op, string target, string condition);

	void filter(QueryResult* entries, hsql::OperatorType op, int val, Attribute* attr, 
				unordered_map<string, Column*>& selectList, vector<string>& selectOrder,
				unordered_map<string, Column*>& groupbyList, vector<string>& totalList);
	void filter(QueryResult* entries, hsql::OperatorType op, double val, Attribute* attr, 
				unordered_map<string, Column*>& selectList, vector<string>& selectOrder,
				unordered_map<string, Column*>& groupbyList, vector<string>& totalList);
	void filter(QueryResult* entries, hsql::OperatorType op, string val, Attribute* attr, 
				unordered_map<string, Column*>& selectList, vector<string>& selectOrder,
				unordered_map<string, Column*>& groupbyList, vector<string>& totalList);

public:
	QueryParser(FileManager* fs, BufferManager* bf);

	template<typename T> void PrintElement(T t, const int& width);

	// parse incoming CREATE statement
	void ParseCREATE(const hsql::SQLStatement* statement);
	// parse incoming INSERT statement
	void ParseINSERT(const hsql::SQLStatement* statement);
	// parse incoming SELECT statement
	void ParseSELECT(const hsql::SQLStatement* statement);
};

#endif