#include "HodorFS.h"
#include "Tables.h"

string DATAPATH = "src/HodorFS/data/";
string TABLESCSV = "tables.csv";

FileManager::FileManager() {
	// fetch the table names from the table file
	// use the tables hash table to store which table has which attributes

	// each file in tables.csv is a table and looks like this:
	// <table name>,<number of tuples>,<number of attributes>,<attr1>,...,<attr1 type>,...
	ifstream table_attr(DATAPATH + TABLESCSV);
	if (!table_attr) {
		cerr << "Unable to find table information." << endl;
	}
	else {
		string line;

		while (getline(table_attr, line)) {
			istringstream iss(line);

			string table;
			string tuples;
			string cols;

		 	getline(iss, table, ',');
		 	getline(iss, tuples, ',');
		 	getline(iss, cols, ',');

		 	tables[table] = new Table(table, stoi(tuples), stoi(cols));

		 	string attr;
		 	vector<string> attrs;
		 	for (size_t i = 0; i  < stoi(cols); i++) {
		 		getline(iss, attr, ',');
		 		attrs.push_back(attr);
		 	}

		 	string attrtype;
		 	for (size_t i = 0; i < stoi(cols); i++) {
		 		getline(iss, attrtype, ',');
		 		Attribute* attr_n = new Attribute(attrs[i], attrtype, table);
		 		tables[table]->attributes.push_back(attr_n);
		 	}
		}

		// cout << "begin" << endl;
		// use pages to store which tables are stored in which pages
		for (auto it = tables.begin(); it != tables.end(); it++) {
			string t_name = it->first;

			TableStorage* ts = new TableStorage(t_name);
			pages[t_name] = ts;

			string directory = DATAPATH + t_name + ".csv";
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
					PageSet* ps = new PageSet(stoi(slots));

					while (getline(iss, pnumber, ',')) {
						ps->pageset.push_back(stoi(pnumber));
					}

					pages[t_name]->pages.push_back(ps);
				}
			}
		}
	}
}

void FileManager::addtable(Table* table) {
	if (tables.find(table->name()) != tables.end()) {
		cout << "Table already exists." << endl;
		return;
	}

	string tablemeta = DATAPATH + TABLESCSV;

	ofstream outfile;
	outfile.open(tablemeta, ios_base::app);
	outfile << table->name() << "," << table->size() << "," << table->columns() << ",";

	for (size_t i = 0; i < table->columns(); i++) {
		outfile << table->attributes[i]->name() << ",";
	}
	for (size_t i = 0; i < table->columns(); i++) {
		if (i != table->columns() - 1)
			outfile << table->attributes[i]->type() << ",";
		else 
			outfile << table->attributes[i]->type() << endl;
	}
	tables[table->name()] = table;
}

void FileManager::display_t() {
	for (auto it = tables.begin(); it != tables.end(); it++) {
		cout << it->first << ", ";
		for (auto attr : it->second->attributes) {
			cout << attr->name() << ", ";
		}
		cout << endl;
	}
}

void FileManager::display_p() {
	for (auto it = pages.begin(); it != pages.end(); it++) {
		cout << "Table: " << it->first << endl;
		for (auto t : it->second->pages) {
			cout << "Slots: " << t->slot() << ", ";
			for (auto k : t->pageset) 
				cout << k << ", ";
		}
		cout << endl;
	}
	cout << endl;
}

Attribute::Attribute(string n, string t, string ta) {
	attrname  = n;
	attrtype  = t;
	tablename = ta;
}

string Attribute::name() {
	return attrname;
}

string Attribute::type() {
	return attrtype;
}

string Attribute::table() {
	return tablename;
}

Table::Table(string n, size_t s, size_t c) {
	tablename = n;
	tuples = s;
	cols = c;
}

string Table::name() {
	return tablename;
}

size_t Table::size() {
	return tuples;
}

size_t Table::columns() {
	return cols;
}

PageSet::PageSet(size_t s) {
	emptyslots = s;
}

size_t PageSet::slot() {
	return emptyslots;
}

TableStorage::TableStorage(string t) {
	tablename = t;
}

string TableStorage::table() {
	return tablename;
}