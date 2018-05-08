#ifndef HFS_H
#define HFS_H

#include "NameSpace.h"
#include "BufferManager.h"
#include "../Helper.h"

#include <vector>
#include <string>
#include <csignal>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <unordered_map>
#include <unordered_set>

using namespace std;

class FileManager {
public:
	unordered_map<string, Table*> tables; // this unordered map stores all the table names in the database
	unordered_map<string, TableStorage*> pages; // table name / pages storing these tables
	unordered_map<string, Database*> users;

	Database* user;

	// next page points to the next page number to use
	int nextpage;

	// when a used page is released, its page number got stored in emptypages
	// so when you want to assign a new page you should check the emptypages first
	// if it's empty, go to the next page
	unordered_set<int> emptypages;

	FileManager();

	void sort(string key, string table);

	void add(Table* table); // add a table, takes in database::table
	void create(Table* table, string tname); // create a new table from old table, with the same attributes
	void remove(string tname, BufferManager* buffer); // remove a table from database, database::table
	// void insert(string tuple, string table);
	// void remove(vector<string> key, string table);
	// void update(vector<string> key, string table);
	void FlushPages(BufferManager* buffer); // flush dirty pages when you need to

	PageSet* FindPageSet(string table, BufferManager* buffer);
	size_t FindPage(); // find next avalable page to store new data

	void display();
};

class AutoSave { // periodically save data to disk
public:
	AutoSave(BufferManager* bf, FileManager* fs);

	// Returns true if the thread was successfully started
	bool StartInternalThread();

	// Will not return until the internal thread has exited.
	void WaitForInternalThreadToExit()
	{
		(void) pthread_join(_thread, NULL);
	}
	void FlushBuffer();

private:
	static void * FlushBufferFunc(void * This) {((AutoSave *)This)->FlushBuffer(); return NULL;}

	pthread_t _thread;
	BufferManager* buffer;
	FileManager* filesystem;
};

#endif