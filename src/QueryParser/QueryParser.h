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

struct Compare {
    bool operator()(const pair<string, Tuple*> &left, const pair<string ,Tuple*> &right) {
        return left.first < right.first;
    }
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
	QueryResult(bool printtable);
	bool printtable;
	vector<Entry*> item;
	vector<string> attrnames;
	set<string> attrset;
	unordered_map<string, pair<vector<string>, size_t>> groups;

	template<typename T> void PrintElement(T t, const int& width);
	void PrintGroup(vector<string>& order, vector<string>& attrorder, 
					unordered_map<string, string>& attributes);
	void PrintAll(vector<string>& attrs);

	void AddAttribute(string attr);
};

class QueryParser {
private:

	FileManager* filesystem;
	BufferManager* buffer;

	Attribute* FindAttribute(hsql::Expr* expr);

	// TODO: figure out how to parse conditions with TEXT data type
	string ConditionType(vector<string>& cols, Table* table, hsql::Expr* expr); // return INT, DOUBLE or TEXT
	// int ConditionParser(Expr* left, Expr* right);

	bool ConditionMet(hsql::OperatorType op, int target, int condition);
	bool ConditionMet(hsql::OperatorType op, double target, double condition);
	bool ConditionMet(hsql::OperatorType op, string target, string condition);

	int    ParseExprINT(   unordered_map<string, int>& map,    hsql::Expr* expr);
	double ParseExprDOUBLE(unordered_map<string, double>& map, hsql::Expr* expr);

	void FilterInt(vector<string>& leftList, vector<string>& rightList, Table* table, 
								unordered_map<string, Column*>& selectList, hsql::OperatorType op, 
								vector<string>& totalList, size_t i, size_t j, size_t k, Entry* entry,
								hsql::Expr* left, hsql::Expr* right, unordered_map<string, Column*>& groupbyList,
								QueryResult* entries, bool& satisfied);
	void FilterDouble(vector<string>& leftList, vector<string>& rightList, Table* table, 
								   unordered_map<string, Column*>& selectList, hsql::OperatorType op, 
							   	   vector<string>& totalList, size_t i, size_t j, size_t k, Entry* entry,
							   	   hsql::Expr* left, hsql::Expr* right, unordered_map<string, Column*>& groupbyList,
							   	   QueryResult* entries, bool& satisfied);

	void filter(QueryResult* entries, hsql::Expr* left, hsql::Expr* right, string type, 
				hsql::OperatorType op, unordered_map<string, Column*>& selectList, Table* table, 
				vector<string>& selectOrder, unordered_map<string, Column*>& groupbyList, 
				vector<string>& totalList, vector<string>& leftList, vector<string>& rightList);
	void filter(QueryResult* entries, hsql::OperatorType op, string val, Attribute* attr, 
				unordered_map<string, Column*>& selectList, vector<string>& selectOrder,
				unordered_map<string, Column*>& groupbyList, vector<string>& totalList);

	void SortPage(Page* page, vector<string>& order);
	void MergeSort(Table* table, Attribute* attr_cond);

	string GetVal(Attribute* attribute, size_t n); // get the val in the nth line of this column
	
	void AddtoJoinedTable(Table* JoinedTable, size_t n_left, size_t n_right, vector<Attribute*>& attr_left, 
						  vector<Attribute*>& attr_right, vector<string>& selectList, string tname);
	void AddtoJoinedTable(Table* JoinedTable, size_t n, vector<Attribute*>& attr, vector<string>& selectList, string tname);

	// void AddtoJoinedTable(size_t i, size_t j, size_t jj, vector<Attribute*>& attr_left, vector<Attribute*>& attr_right,
	// 					  size_t cols_left, size_t cols_right, string tname, vector<string>& selectList);

	Table* JoinTable(hsql::TableRef* fromTable, const hsql::SelectStatement* select, bool& temporary);

	Table* NestedLoopJoin(Table* left, Table* right, hsql::Expr* condition, const hsql::SelectStatement* select);
	Table* MergeSortJoin(Table* left, Table* right, hsql::Expr* condition, const hsql::SelectStatement* select);

	void InnerJoin(Table* JoinedTable, Table* left, Table* right, vector<Attribute*>& attr_left, vector<Attribute*>& attr_right,
				   vector<string>& selectList, string tname, Attribute* join_left, Attribute* join_right);
	void FullJoin(Table* JoinedTable, Table* left, Table* right, vector<Attribute*>& attr_left, vector<Attribute*>& attr_right,
				  vector<string>& selectList, string tname, Attribute* join_left, Attribute* join_right);
	void LeftJoin(Table* JoinedTable, Table* left, Table* right, vector<Attribute*>& attr_left, vector<Attribute*>& attr_right,
				  vector<string>& selectList, string tname, Attribute* join_left, Attribute* join_right);
	void CrossJoin(Table* JoinedTable, Table* left, Table* right, vector<Attribute*>& attr_left, vector<Attribute*>& attr_right,
				   vector<string>& selectList, string tname, Attribute* join_left, Attribute* join_right);
	void NaturalJoin(Table* JoinedTable, Table* left, Table* right, vector<Attribute*>& attr_left, vector<Attribute*>& attr_right,
					 vector<string>& selectList, string tname, Attribute* join_left, Attribute* join_right);

public:
	QueryParser(FileManager* fs, BufferManager* bf);

	template<typename T> void PrintElement(T t, const int& width);

	// parse incoming CREATE statement
	void ParseCREATE(const hsql::SQLStatement* statement);
	// parse incoming INSERT statement
	void ParseINSERT(const hsql::SQLStatement* statement);
	// parse incoming SELECT statement
	Table* ParseSELECT(const hsql::SQLStatement* statement, bool printtable);
};

#endif