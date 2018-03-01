#include "Helper.h"

// define global variables
string DATAPATH      = "src/HodorFS/data/";
string TABLESCSV     = "tables.csv";
string DBCSV         = "databases.csv";
string STORAGECSV    = "storage.csv";

size_t PAGESIZE      = 1000;
size_t CHECKPERIOD   = 5;
size_t BUFFERSIZE    = 5000;

pthread_mutex_t Lock = PTHREAD_MUTEX_INITIALIZER;

void ToLower(string& data) {
	transform(data.begin(), data.end(), data.begin(), ::tolower);
}

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

string GetTime(string timestamp, string unit) {
	istringstream iss(timestamp);

	string month;
	string date;
	string time;
	string year;

	getline(iss, month, ' '); // get rid of week day
	getline(iss, month, ' ');
	getline(iss, date,  ' ');
	getline(iss, time,  ' ');
	getline(iss, year,  ' ');

	ToLower(unit);
	Month2Int(month);

	istringstream timestream(time);

	string hour;
	string minute;
	string second;

	getline(timestream, hour,  ':');
	getline(timestream, minute, ':');
	getline(timestream, second, ':');

	if (unit == "year") {
		return year;
	}
	else if (unit == "month") {
		return month + '/' + year;
	}
	else if (unit == "date") {
		return month + '/' + date + '/' + year;
	}
	else if (unit == "hour") {
		return month + '/' + date + '/' + year + '/' + hour;
	}
	else { // minute
		return month + '/' + date + '/' + year + '/' + hour + ':' + minute;
	}
}

void Month2Int(string & month) {
	ToLower(month);

	if (month == "jan")
		month = "01";
	else if (month == "feb")
		month = "02";
	else if (month == "mar")
		month = "03";
	else if (month == "apr")
		month = "04";
	else if (month == "may")
		month = "05";
	else if (month == "jun")
		month = "06";
	else if (month == "jul")
		month = "07";
	else if (month == "aug")
		month = "08";
	else if (month == "sep")
		month = "09";
	else if (month == "oct")
		month = "10";
	else if (month == "nov")
		month = "11";
	else
		month = "12";
}