#include "HodorFS.h"

FileManager::FileManager() {
	// fetch the table names from the table file
	// use the tables hash table to store which table has which attributes
	ifstream table_attr("data/tables.csv");
	if (!table_attr) {
		cerr << "Unable to find table information." << endl;
	}
	else {
		string line;

		while (getline(table_attr, line)) {
			istringstream iss(line);

			string table;
			string attribute;
			vector<string> attributes;

		 	getline(iss, table, ',');
			while (getline(iss, attribute, ',')) {
				attributes.push_back(attribute);
			}

			tables[table] = attributes;
		}

		// cout << "begin" << endl;
		// use pages to store which tables are stored in which pages
		for (auto it = tables.begin(); it != tables.end(); it++) {
			string t_name = it->first;
			string directory = "data/" + t_name + ".csv";
			ifstream table_loc(directory);
			if (!table_loc) {
				cerr << "Unable to find page information of table " << t_name << "." << endl;
			}
			else {
				string pages_attr;
				while (getline(table_loc, pages_attr)) {
					istringstream iss(pages_attr);
					string pnumber;
					vector<int> page_num;
					string slots;

					getline(iss, slots, ',');
					while (getline(iss, pnumber, ',')) {
						page_num.push_back(stoi(pnumber));
					}

					pair<int, vector<int>> page_info = make_pair(stoi(slots), page_num);
					pages[t_name].push_back(page_info);
				}
			}
		}
		// cout << "end" << endl;

		// use the pages hash table to store which tables are stored in which pages
	}
}

void FileManager::display_t() {
	for (auto it = tables.begin(); it != tables.end(); it++) {
		cout << it->first << ", ";
		for (auto attr : it->second) {
			cout << attr << ", ";
		}
		cout << endl;
	}
}

void FileManager::display_p() {
	for (auto it = pages.begin(); it != pages.end(); it++) {
		cout << "Table: " << it->first << endl;
		for (auto t : it->second) {
			cout << "Slots: " << t.first << ", ";
			for (auto a : t.second) {
				cout << a << ", ";
			}
			cout << endl;
		}
		cout << endl;
	}
}

// FileManager::sort(string key, string table) {

// }

// FileManager::insert(string tuple, string table) {
// 	istringstream iss(tuple);

// 	for (string attribute : tables[table]) {
// 		string val;
// 		string filename = table + "_" + attribute + ".cvs";
// 		ofstream outfile;
// 		outfile.open(filename, ios_base::app);

// 	 iss >> val;
// 		outfile << '\n' << val << ",";
// 	}
// }

// FileManager::remove(vector<string> key, string table) {

// }

// FileManager::update(vector<string> key, string table) {

// }