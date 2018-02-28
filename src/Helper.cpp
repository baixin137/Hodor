#include "Helper.h"

string addTimeStamp() {
	auto curr_time = chrono::system_clock::now();
	time_t t = chrono::system_clock::to_time_t(curr_time);

	string timestamp(ctime(&t));
	timestamp.erase(remove(timestamp.begin(), timestamp.end(), '\n'), timestamp.end());

	return timestamp;
}

string TableName(string name) {
	string delimiter = "::";

	return name.substr(name.find(delimiter) + 2);
}

string DatabaseName(string name) {
	string delimiter = "::";

	return name.substr(0, name.find(delimiter));
}