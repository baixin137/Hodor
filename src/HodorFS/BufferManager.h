#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <list>
#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <unordered_map>

#include "HodorFS.h"

using namespace std;

class Tuple {
public:
	Tuple(bool n, string s);
	Tuple(bool n, int i);
	Tuple(bool n, double d);
	Tuple(bool n);

	bool isnull;
	string sval;
	int ival;
	double dval;
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

// String handles CHAR, DATE, TIME AND TIMESTAMP NOW
class StringPage : public Page {
public:
	StringPage();
	void read(int p, string pn, vector<string> property);
	void write();
	void display();
};

class IntPage : public Page {
public:
	IntPage();
	void read(int p, string pn, vector<string> property);
	void write();
	void display();
};

class DoublePage : public Page {
public:
	DoublePage();
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
private:
    size_t capacity;
    unordered_map<int,  list<pair<int, Page*>>::iterator> map; //map_iter->first: key, m_map_iter->second: list iterator;
    list<pair<int, Page*>> linkedlist; //linkedlist_iter->first: key, linkedlist_iter->second: value;

public:
    LRUCache(size_t capacity);
    Page* get(int key); // get page from cache
    void set(int key, Page* value); // set/insert page in cache
    void (int key);

    // TODO: we should flush every dirty page to disk like every 1 minute

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