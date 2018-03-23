#include "Helper.h"

// define global variables
string DATAPATH      = "src/HodorFS/data/";
string TABLESCSV     = "tables.csv";
string DBCSV         = "databases.csv";
string STORAGECSV    = "storage.csv";

size_t PAGESIZE      = 1000;
size_t CHECKPERIOD   = 5;
size_t BUFFERSIZE    = 5000;
size_t BOXWIDTH      = 28;
size_t TUPLELOAD     = 1000;

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

string GetPageFile(int pn) {
	string page_name = to_string(pn);
	int zeros = 10 - page_name.size();

	for (size_t i = 0; i < zeros; i++) 
		page_name = '0' + page_name;
	page_name = DATAPATH + page_name + ".csv";
	
	return page_name;
}

string vtos(vector<string> v) {
	string s;
	for (string str : v) {
		s += (str + '/');
	}
	return s;
}

void PrintLine(size_t width, size_t cols) {
	size_t num = 1 + (width + 1) * cols;

	for (size_t i = 0; i < num; i++) {
		cout << '-';
	}
	cout << endl;
}

void PrintLineInner(size_t width, size_t cols) {
	for (size_t i = 0; i < cols; i++) {
		cout << '|';
		for (size_t j = 0; j < width; j++) {
			cout << '-';
		}
	}
	cout << '|' << endl;
}

template<class T1> 
// template<class T2>
void print(vector<T1>& v) {
	for (T1 elem : v) {
		cout << v << ", "
	}
	cout << endl;
}

// void print(unordered_map<T1, T2>& m) {
// 	for (auto it = m.begin(); it != m.end(); it++) {
// 		cout << "key: " << it->first << " val: " << it->second << endl;
// 	}
// }