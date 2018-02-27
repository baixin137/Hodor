#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "../Helper.h"

#include <list>
#include <ctime>
#include <chrono>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <pthread.h>
#include <unordered_map>

using namespace std;

extern string DATAPATH;
extern string TABLESCSV;
extern string DBCSV;
extern int PAGESIZE;
extern pthread_mutex_t Lock;
extern int CHECKPERIOD;

class Tuple {
public:
	Tuple(bool n, string s);
	Tuple(bool n, int i);
	Tuple(bool n, double d);
	Tuple(bool n);
	Tuple();

	string timestamp();

	bool isnull;
	string sval;
	int ival;
	double dval;

private:
	string time;
};

class Page {
protected:
	int page_num; // page number, 4 bytes, from 0 to 4,294,967,296
	string table; // the table it belongs to
	string attribute; // which attribute it stores
	string type; // type of the attribute

public:
	bool dirty; // the page is dirty of not
	int pinned; // the buffer manager shouldn't flush a paged pinned by some threads
	vector<Tuple*> content;
	int slots; // number of empty slots in this page

	int getnum();
	string gettable();
	string getattr();
	string gettype();
	int getslots();
	bool isdirty();
	int getpinned();

	virtual void read(int p, string pn, vector<string> property) = 0;
	virtual void write() = 0;
	virtual void display() = 0;
};

// Text handles CHAR, DATE, TIME AND TIMESTAMP NOW
class TextPage : public Page {
public:
	TextPage();
	TextPage(int pn, string ta, string at);

	void read(int p, string pn, vector<string> property);
	void write();
	void display();
};

class IntPage : public Page {
public:
	IntPage();
	IntPage(int pn, string ta, string at);

	void read(int p, string pn, vector<string> property);
	void write();
	void display();
};

class DoublePage : public Page {
public:
	DoublePage();
	DoublePage(int pn, string ta, string at);

	void read(int p, string pn, vector<string> property);
	void write();
	void display();
};

// class BooleanPage : public Page {
// public:
// 	BooleanPage();
// 	void read(int p, string pn, vector<string> property);
// 	void write(string pn);
// 	void display();
// };

// Least recently used cache
class LRUCache {
public:
    size_t capacity;
    unordered_map<int,  list<pair<int, Page*>>::iterator> map; //map_iter->first: key, m_map_iter->second: list iterator;
    list<pair<int, Page*>> linkedlist; //linkedlist_iter->first: key, linkedlist_iter->second: value;
    LRUCache(size_t capacity);
    Page* get(int key); // get page from cache
    void set(int key, Page* value); // set/insert page in cache
    void remove(int key);

    void display();
};

class BufferManager {
private:
	LRUCache* buffer;

public:
	LRUCache* getbuffer();
	BufferManager(size_t capacity);
	void add(int pn, string type, string table, string attribute); // create a new page and add to buffer
	void fetch(int pn); // fetch page from disk
	void flush(int pn); //flush the page to disk and remove from memory
};

#endif