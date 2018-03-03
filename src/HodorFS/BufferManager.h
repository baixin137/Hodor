#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include "../Helper.h"
#include "NameSpace.h"

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

class Tuple {
public:
	Tuple(bool n, string s, string t);
	Tuple(bool n, int i,    string t);
	Tuple(bool n, double d, string t);
	Tuple(bool n,           string t);
	Tuple(                  string t);

	string timestamp();
	string type();

	bool isnull;
	string sval;
	int ival;
	double dval;
	string tupletype;

private:
	string time;
};

class Page {
protected:
	int page_num; // page number, 4 bytes, from 0 to 4,294,967,296
	string tablename; // the table it belongs to
	string attrname; // which attribute it stores
	string attrtype; // type of the attribute
	size_t attrs; // # of attributes stored in page

public:
	bool dirty; // the page is dirty of not
	int threads; // the buffer manager shouldn't flush a paged pinned by some threads
	vector<Tuple*> content;
	int slots; // number of empty slots in this page

	int number();
	int size();
	string table();
	string attribute();
	string type();
	int slot();
	bool isdirty();
	int pinned();

	void IncrementSize(size_t s);

	virtual void read(int p, string pn, vector<string> property) = 0;
	virtual void write() = 0;
	virtual void UpdateMeta(double val) = 0;
	// virtual void display() = 0;
};

// Text handles CHAR, DATE, TIME AND TIMESTAMP NOW
class TextPage : public Page {
public:
	TextPage();
	TextPage(int pn, string ta, string at, size_t attr);

	void read(int p, string pn, vector<string> property);
	void write();
	// void display();
	void UpdateMeta(double val);

};

class IntPage : public Page {
public:
	IntPage();
	IntPage(int pn, string ta, string at, size_t attr);

	void read(int p, string pn, vector<string> property);
	void write();
	// void display();

	int min();
	int max();
	double mean();

	// update min, max and mean, before inserted 
	void UpdateMeta(double val);
	
private:
	int minval;
	int maxval;
	double meanval;
};

class DoublePage : public Page {
public:
	DoublePage();
	DoublePage(int pn, string ta, string at, size_t attr);

	void read(int p, string pn, vector<string> property);
	void write();
	// void display();

	double min();
	double max();
	double mean();

	// update min, max and mean, before inserted 
	void UpdateMeta(double val);

private:
	double minval;
	double maxval;
	double meanval;
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
    void remove(int key); // remove from memory

    // void display();
};

class BufferManager {
private:
	LRUCache* buffer;

public:
	LRUCache* getbuffer();
	BufferManager(size_t capacity);
	void add(int pn, string type, string table, string attribute); // create a new page and add to buffer
	void fetch(int pn); // fetch page from disk
	void flush(int pn); // flush the page to disk and remove from memory
	void erase(int pn); // erase from memory and disk
	bool iscached(int pn); // return true if page is in memory
	Page* get(int pn); // get the page by page number
	void MoveTuple(PageSet* pnew, PageSet* pold, size_t line);
};

#endif