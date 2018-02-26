#ifndef TABLES_H
#define TABLES_H

#include <string>
#include "BufferManager.h"

using namespace std;

class Database {
private: 
	string database;
	size_t dbsize;
	string date;

public:
	vector<Table*> tables;

	Database();
	string name();
	size_t size();
	string timestamp();
}

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
	string databse;
	string date;
	size_t tuples;
	size_t cols;

public:
	Table(string n, size_t s, size_t c);

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

#endif