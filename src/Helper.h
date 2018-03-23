#ifndef HELPER_H
#define HELPER_H

#include <ctime>
#include <vector>
#include <string>
#include <chrono>
#include <sstream>
#include <iostream>
#include <unordered_set>

using namespace std;

extern string DATAPATH;
extern string TABLESCSV;
extern string DBCSV;
extern string STORAGECSV;

extern size_t PAGESIZE;
extern size_t CHECKPERIOD;
extern size_t BUFFERSIZE;
extern size_t BOXWIDTH;
extern size_t TUPLELOAD;

extern pthread_mutex_t Lock;

void ToLower(string& data);

string addTimeStamp();

string TableName(string name); // return table name from database::table
string DatabaseName(string name); // return database name from database:;table

// return year/month/date/hour/minute from timestamp
// time stamp look like: Wed Feb 28 17:38:50 2018
string GetTime(string timestamp, string unit);
void Month2Int(string& month); // convert month to 1-12
string GetPageFile(int pn);
string vtos(vector<string> v);

void PrintLine(size_t width, size_t cols);
void PrintLineInner(size_t width, size_t cols);

#endif