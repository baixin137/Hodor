#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <set>
#include <string>
#include <vector>
#include <unordered_map>

#include "../Helper.h"

using namespace std;

class Attribute {
private:
	string attrname;
	string attrtype;
	string tablename;

public:
	Attribute(string n, string t, string ta);

	string name();
	string type();
	string table();

	set<int> pages; // store which pages are this attribute stored
	vector<int> page_order;
};

class Table {
private:
	string tablename;
	string database;
	string date;
	size_t tuples;
	size_t cols;

public:
	Table(string n, string user, string ts, size_t s, size_t c);

	string name();
	string user();
	string timestamp();
	size_t size();
	size_t columns();

	void IncrementSize(size_t s);

	vector<string> attr_order;
	unordered_map<string, Attribute*> attributes;
};

class PageSet {
public:
	PageSet(size_t s);

	size_t slot();
	size_t slots;
	vector<int> pageset;
};

class TableStorage { // stores which page sets a table has
private:
	string tablename; // database::table
	string databasename;

public:
	TableStorage(string t, string db);

	string table();
	string database();
	vector<PageSet*> pageset;
};

class Database {
private: 
	string database; // database name
	size_t dbsize; // number of tables in it
	string date; // time stamp

public:
	set<string> table_names;
	unordered_map<string, Table*> tables;

	Database(string username, size_t s, string t);
	
	void IncrementSize(size_t s);
	string name();
	size_t size();
	string timestamp();
};

#endif