#ifndef NAMESPACE_H
#define NAMESPACE_H

#include <string>
#include "BufferManager.h"

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

	vector<string> attr_order;
	unordered_map<string, Attribute*> attributes;
};

class PageSet {
public:
	PageSet(size_t s);

	size_t slots;
	size_t slot();
	vector<int> pageset;

};

class TableStorage {
private:
	string tablename;

public:
	TableStorage(string t);

	string table();
	vector<PageSet*> pageset;
};

class Database {
private: 
	string database;
	size_t dbsize;
	string date;

public:
	vector<string> table_names;
	unordered_map<string, Table*> tables;

	Database(string username, size_t s, string t);
	string name();
	size_t size();
	string timestamp();
};

#endif