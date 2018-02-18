#ifndef HFS_H
#define HFS_H

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

using namespace std;

class FileManager {
private:
	unordered_map<string, vector<string>> tables; // this unordered map stores all the table names in the database
	unordered_map<string, vector<pair<int, vector<int>>>> pages;

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