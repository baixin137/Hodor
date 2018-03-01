#ifndef HELPER_H
#define HELPER_H

#include <ctime>
#include <string>
#include <chrono>
#include <sstream>

using namespace std;

void ToLower(string& data);

string addTimeStamp();

string TableName(string name); // return table name from database::table
string DatabaseName(string name); // return database name from database:;table

// return year/month/date/hour/minute from timestamp
// time stamp look like: Wed Feb 28 17:38:50 2018
string GetTime(string timestamp, string unit);
void Month2Int(string& month); // convert month to 1-12


#endif