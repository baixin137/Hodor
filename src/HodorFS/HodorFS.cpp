#include "HodorFS.h"

FileManager::FileManager() {
	user = nullptr; // current user initialized as null

	ifstream empty_pages(DATAPATH + "availablepages.csv");
	if (!empty_pages) {
		ofstream apages(DATAPATH + "availablepages.csv");
		apages << "0" << endl;
		apages.close();
	}
	else {
		string line;
		getline(empty_pages, line);
		nextpage = stoi(line); // store next page

		getline(empty_pages, line);
		istringstream ps(line);
		string p;

		while (getline(ps, p, ',')) {
			emptypages.insert(stoi(p));
		}
	}

	// fetch the table names from the table file
	// use the tables hash table to store which table has which attributes

	// each file in tables.csv is a table and looks like this:
	// <table name>,<username>,<timestamp>,<number of tuples>,<number of attributes>,<attr1>,...,<attr1 type>,...
	ifstream table_attr(DATAPATH + TABLESCSV);
	if (!table_attr) {
		cerr << "Unable to find table information." << endl;
	}
	else {
		string line;

		while (getline(table_attr, line)) {
			istringstream iss(line);

			string table;
			string user;
			string ts;
			string tuples;
			string cols;

		 	getline(iss, table,  ',');
		 	getline(iss, user,   ',');
		 	getline(iss, ts,     ',');
		 	getline(iss, tuples, ',');
		 	getline(iss, cols,   ',');

		 	table = user + "::" + table;

		 	tables[table] = new Table(table, user, ts, stoi(tuples), stoi(cols));

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
		 		tables[table]->attributes[attrs[i]] = (attr_n);
		 		tables[table]->attr_order.push_back(attrs[i]);
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

					pages[t_name]->pageset.push_back(ps);
				}
			}
		}
	}

	// read meta info of databases
	string path_DB = DATAPATH + DBCSV;

	// the meta information should look like:
	// <database1>,<size>,<timestamp>,<table1>,<table2>...
	ifstream infile_DB(path_DB);
	if (!infile_DB) {
		ofstream Database(path_DB);
		Database.close();
	}
	else {
		string line;

		while (getline(infile_DB, line)) {
			istringstream iss;
			string db_name;
			string db_size;
			string db_time;

			getline(iss, db_name, ',');
			getline(iss, db_size, ',');
			getline(iss, db_time, ',');

			Database* newDB = new Database(db_name, stoi(db_size), db_time);

			string t_name;
			while (getline(iss, t_name, ',')) {
				newDB->tables.push_back(tables[db_name + "::" + t_name]);
			}
			users[db_name] = newDB;
		}
	}
}

void FileManager::add(Table* table) {
	if (tables.find(table->name()) != tables.end()) {
		cout << "Table already exists." << endl;
		return;
	}

	// add table metainfo into disk
	string tablemeta = DATAPATH + TABLESCSV;

	ofstream outfile;
	outfile.open(tablemeta, ios_base::app);
	outfile << table->name() << "," << table->size() << "," << table->columns() << ",";

	for (size_t i = 0; i < table->columns(); i++) {
		outfile << table->attributes[table->attr_order[i]]->name() << ",";
	}
	for (size_t i = 0; i < table->columns(); i++) {
		if (i != table->columns() - 1)
			outfile << table->attributes[table->attr_order[i]]->type() << ",";
		else 
			outfile << table->attributes[table->attr_order[i]]->type() << endl;
	}

	// store table in memeory
	tables[table->name()] = table;

	// the table is just created and no data is in it
	// so the pages doesn't have this table's information yet
}

void FileManager::display_t() {
	for (auto it = tables.begin(); it != tables.end(); it++) {
		cout << it->first << ", ";
		for (auto attr : it->second->attr_order) {
			cout << attr << ", ";
		}
		cout << endl;
	}
}

void FileManager::display_p() {
	for (auto it = pages.begin(); it != pages.end(); it++) {
		cout << "Table: " << it->first << endl;
		for (auto t : it->second->pageset) {
			cout << "Slots: " << t->slot() << ", ";
			for (auto k : t->pageset) 
				cout << k << ", ";
		}
		cout << endl;
	}
	cout << endl;
}

Database::Database(string nm, size_t s, string time) { // when first start, read meta information from disk
	database = nm;
	dbsize = s;
	date = time;
}

string Database::name() { // return the name of current database
	return database;
}

size_t Database::size() { // return the number of tables stored in this database
	return dbsize;
}

string Database::timestamp() {
	return date;
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

Table::Table(string n, string user, string ts, size_t s, size_t c) {
	tablename = n;
	database = user;
	date = ts;
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
	slots = s;
}

size_t PageSet::slot() {
	return slots;
}

TableStorage::TableStorage(string t) {
	tablename = t;
}

string TableStorage::table() {
	return tablename;
}

AutoSave::AutoSave(BufferManager* bf) {
	buffer = bf;
}

bool AutoSave::StartInternalThread() {
	return (pthread_create(&_thread, NULL, FlushBufferFunc, this) == 0);

}

void AutoSave::FlushBuffer() {
	LRUCache* cache = buffer->getbuffer();

	while (true) {
		sleep(CHECKPERIOD);

		cout << "Ready to flush." << endl;

		for (auto it = cache->linkedlist.begin(); it != cache->linkedlist.end(); it++) {
			Page* page = it->second;
			if (page->dirty) {
				page->write();
				page->dirty = false;

				cout << "Flushing page: " << page->getnum() << endl;
			}
		}
	}
}