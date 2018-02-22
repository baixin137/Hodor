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

extern string DATAPATH;

using namespace std;

class FileManager {
private:
	unordered_map<string, Table*> tables; // this unordered map stores all the table names in the database
	unordered_map<string, TableStorage*> pages;

public:
	FileManager();

	void sort(string key, string table);
	// void insert(string tuple, string table);
	// void remove(vector<string> key, string table);
	// void update(vector<string> key, string table);

	void display_t();
	void display_p();
};

#endif