#ifndef HELPER_H
#define HELPER_H

#include <ctime>
#include <string>
#include <chrono>

using namespace std;

string addTimeStamp();

string TableName(string name); // return table name from database::table
string DatabaseName(string name); // return database name from database:;table

#endif