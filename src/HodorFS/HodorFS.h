#ifndef HFS_H
#define HFS_H

#include "Tables.h"
#include "BufferManager.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <unordered_set>

extern string DATAPATH;
extern string TABLESCSV;

using namespace std;

class FileManager {
public:
	unordered_map<string, Table*> tables; // this unordered map stores all the table names in the database
	unordered_map<string, TableStorage*> pages;

	// next page points to the next page number to use
	int nextpage;

	// when a used page is released, its page number got stored in emptypages
	// so when you want to assign a new page you should check the emptypages first
	// if it's empty, go to the next page
	unordered_set<int> emptypages;

	FileManager();

	void sort(string key, string table);

	void add(Table* table); // add a table
	// void insert(string tuple, string table);
	// void remove(vector<string> key, string table);
	// void update(vector<string> key, string table);

	void display_t();
	void display_p();
};

#endif