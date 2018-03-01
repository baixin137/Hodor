#include "HodorFS.h"

FileManager::FileManager() {
	user = nullptr; // current user initialized as null

	// read available pages
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

	cout << "avalable pages read." << endl;

	// fetch the table names from the table file
	// use the tables hash table to store which table has which attributes

	// each file in tables.csv is a table and looks like this:
	// <table name>,<username>,<timestamp>,<number of tuples>,<number of attributes>,<attr1>,...,<attr1 type>,...
	ifstream table_attr(DATAPATH + TABLESCSV);
	if (!table_attr) {
		ofstream outfile(DATAPATH + TABLESCSV);
	}
	else {
		string line;

		while (getline(table_attr, line)) {
			istringstream iss(line);

			string table;
			string username;
			string ts;
			string tuples;
			string cols;

		 	getline(iss, table,      ',');
		 	getline(iss, username,   ',');
		 	getline(iss, ts,         ',');
		 	getline(iss, tuples,     ',');
		 	getline(iss, cols,       ',');

		 	table = username + "::" + table;

		 	tables[table] = new Table(table, username, ts, stoi(tuples), stoi(cols));

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

		// use pages to store which tables are stored in which pages
		for (auto it = tables.begin(); it != tables.end(); it++) {
			string t_name = it->first;

			TableStorage* ts = new TableStorage(t_name, DatabaseName(t_name));
			pages[t_name] = ts;

			string directory = DATAPATH + t_name + ".csv";
			ifstream table_loc(directory);

			if (table_loc) {
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

	cout << "Tables read." << endl;

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
			istringstream iss(line);
			string db_name;
			string db_size;
			string db_time;

			getline(iss, db_name, ',');
			getline(iss, db_size, ',');
			getline(iss, db_time, ',');

			// cout << "database name is: " << db_name << endl;
			// cout << "database size is: " << db_size << endl;
			// cout << "database created at: " << db_time << endl;

			Database* newDB = new Database(db_name, stoi(db_size), db_time);

			string t_name;
			while (getline(iss, t_name, ',')) {
				newDB->tables[t_name] = (tables[db_name + "::" + t_name]);
				newDB->table_names.push_back(t_name);
			}
			users[db_name] = newDB;
		}
	}

	cout << "Databases read." << endl;

	// read pagestorage
	string storage_path = (DATAPATH + STORAGECSV);
	ifstream pstorage(storage_path);

	if (!pstorage) {
		ofstream outfile(storage_path);
		outfile.close();
	}
	else {
		string line;
		while (getline(pstorage, line)) {
			istringstream iss(line);

			string db_table;
			getline(iss, db_table, ',');
			string db_name = DatabaseName(db_table);
			string tb_name = TableName(db_table);

			if (pages.find(db_table) == pages.end()) {
				pages[db_table] = new TableStorage(db_table, db_name);
			}

			string slots;
			getline(iss, slots, ',');

			PageSet* pageset = new PageSet(stoi(slots));

			string p_num;
			while (getline(iss, p_num, ',')) {
				pageset->pageset.push_back(stoi(p_num));
			}

			pages[db_table]->pageset.push_back(pageset);
		}

	}
}

void FileManager::add(Table* table) {
	string t_name = user->name() + "::" + table->name();

	if (tables.find(t_name) != tables.end()) {
		cout << "Table already exists." << endl;
		return;
	}

	// add table metainfo into disk
	string tablemeta = DATAPATH + TABLESCSV;

	ofstream outfile;
	outfile.open(tablemeta, ios_base::app);
	outfile << t_name             << "," 
			<< table->user()      << ","
			<< table->timestamp() << ","
			<< table->size()      << "," 
			<< table->columns()   << ",";

	for (size_t i = 0; i < table->columns(); i++) {
		outfile << table->attributes[table->attr_order[i]]->name() << ",";
	}
	for (size_t i = 0; i < table->columns(); i++) {
		// cout << "type is: " << table->attributes[table->attr_order[i]]->type() << endl;
		if (i != table->columns() - 1)
			outfile << table->attributes[table->attr_order[i]]->type() << ",";
		else 
			outfile << table->attributes[table->attr_order[i]]->type() << endl;
	}

	// store table in memeory
	tables[t_name] = table;
	user->table_names.push_back(table->name());
	user->tables[table->name()] = table;

	TableStorage* storage = new TableStorage(t_name, user->name());
	pages[t_name] = storage;

	// the table is just created and no data is in it
	// so the pages doesn't have this table's information yet
}
void FileManager::create(Table* table, string tname) {
	// traverse the columns
	string username = user->name();
	string timestamp = addTimeStamp();
	size_t cols = table->columns();

	Table* table_new = new Table(tname, username, timestamp, 0, cols);
	table_new->attr_order = table->attr_order;
	table_new->attributes = table->attributes;

	add(table);
	user->IncrementSize(1);
}

void FileManager::remove(string tname) {

}

PageSet* FileManager::FindPageSet(string table, BufferManager* buffer) {
	PageSet* pset;
	string t_stamp = addTimeStamp();

	bool pagesetfound = false;

	if (pages.find(table) == pages.end()) {
		cerr << "Table not found in " << user->name() << "." << endl;
	}
	else { // table found
		for (auto p : pages[table]->pageset) {
			if (p->slot() > 0) {
				cout << "Slots left: " << p->slot() << endl;
				pset = p;
				pagesetfound = true;
				break;
			}
		}
		if (!pagesetfound) { // allocate a new page
			// cout << "Pageset not found" << endl;
			// int count = tables[table]->size();

			PageSet* p = new PageSet(PAGESIZE);
			// check if there are released pages to be recycled

			// cout << "# of cols is: " << tables[table]->attr_order.size() << endl;
			for (auto attrname : tables[table]->attr_order) {
				Attribute* attr = tables[table]->attributes[attrname];
				int pnumber;

				if (emptypages.size() > 0) {
					auto iter = emptypages.begin();
					pnumber = *iter;
					p->pageset.push_back(pnumber);
					emptypages.erase(iter);
				}
				else {
					pnumber = nextpage;
					p->pageset.push_back(pnumber);
					nextpage++;
				}
				buffer->add(pnumber, attr->type(), attr->table(), attr->name());
			}
			pset = p;
			pages[table]->pageset.push_back(pset);
		}
		pset->slots -= 1;
	}
	return pset;
}

// void FileManager::display_t() {
// 	for (auto it = tables.begin(); it != tables.end(); it++) {
// 		cout << it->first << ", ";
// 		for (auto attr : it->second->attr_order) {
// 			cout << attr << ", ";
// 		}
// 		cout << endl;
// 	}
// }

// void FileManager::display_p() {
// 	for (auto it = pages.begin(); it != pages.end(); it++) {
// 		cout << "Table: " << it->first << endl;
// 		for (auto t : it->second->pageset) {
// 			cout << "Slots: " << t->slot() << ", ";
// 			for (auto k : t->pageset) 
// 				cout << k << ", ";
// 		}
// 		cout << endl;
// 	}
// 	cout << endl;
// }

Database::Database(string nm, size_t s, string time) { // when first start, read meta information from disk
	database = nm;
	dbsize = s;
	date = time;
}

void Database::IncrementSize(size_t s) {
	dbsize += 1;
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

string Table::user() {
	return database;
}

string Table::timestamp() {
	return date;
}

size_t Table::size() {
	return tuples;
}

size_t Table::columns() {
	return cols;
}

void Table::IncrementSize(size_t s) {
	tuples += s;
}

PageSet::PageSet(size_t s) {
	slots = s;
}

size_t PageSet::slot() {
	return slots;
}

TableStorage::TableStorage(string t, string db) {
	tablename = t;
	databasename = db;
}

string TableStorage::table() {
	return tablename;
}

AutoSave::AutoSave(BufferManager* bf, FileManager* fs) {
	buffer = bf;
	filesystem = fs;
}

bool AutoSave::StartInternalThread() {
	return (pthread_create(&_thread, NULL, FlushBufferFunc, this) == 0);

}

void AutoSave::FlushBuffer() {
	LRUCache* cache = buffer->getbuffer();

	while (true) {
		sleep(CHECKPERIOD);

		// cout << "Ready to flush." << endl;

		// flush available pages
		string page_path = DATAPATH + "availablepages.csv";
		ofstream apages(page_path);

		apages << filesystem->nextpage << endl;
		for (int recycled_page : filesystem->emptypages) {
			apages << recycled_page << ",";
		}

		// flush dirty pages
		for (auto it = cache->linkedlist.begin(); it != cache->linkedlist.end(); it++) {
			Page* page = it->second;
			if (page->dirty) {
				page->write();
				page->dirty = false;

				// cout << "Flushing page: " << page->getnum() << endl;
			}
		}

		// flush table information
		ofstream tables_info(DATAPATH + TABLESCSV);

		for (auto db = filesystem->users.begin(); db != filesystem->users.end(); db++) {
			for (auto tb = db->second->tables.begin(); tb != db->second->tables.end(); tb++) {
				tables_info << tb->first          << "," // table name
					   << db->first               << "," // user name
					   << tb->second->timestamp() << "," // time stamp
					   << tb->second->size()      << "," // table size
					   << tb->second->columns()   << ","; // table attributes

				// write table name
				for (auto attr = tb->second->attr_order.begin(); attr != tb->second->attr_order.end(); attr++) {
					tables_info << *attr << ",";
				}

				// write table type
				for (size_t i = 0; i < tb->second->attr_order.size(); i++) {
					if (i != tb->second->attr_order.size() - 1)
						tables_info << tb->second->attributes[tb->second->attr_order[i]]->type() << ",";
					else
						tables_info << tb->second->attributes[tb->second->attr_order[i]]->type() << endl;
				}
				// cout << "table " << tb->first << " flushed" << endl;
			}
		}
		tables_info.close();

		// flush users information
		ofstream db_info(DATAPATH + DBCSV);

		for (auto db = filesystem->users.begin(); db != filesystem->users.end(); db++) {
			// cout << "Flush database: " << db->first << " to disk..." << endl;

			db_info << db->first               << ","
					<< db->second->size()      << ","
					<< db->second->timestamp() << ",";

			if (!db->second->table_names.size())
				db_info << endl;

			for (size_t i = 0; i < db->second->table_names.size(); i++) {
				if (i != db->second->table_names.size() - 1)
					db_info << db->second->table_names[i] << ",";
				else 
					db_info << db->second->table_names[i] << endl;
			}
			// cout << "database " << db->first << " flushed" << endl;
		}
		db_info.close();

		// flush which table has which pages meta information
		ofstream pagestorage(DATAPATH + STORAGECSV);

		for (auto table_pages = filesystem->pages.begin(); table_pages != filesystem->pages.end(); table_pages++) {
			for (auto pset : table_pages->second->pageset) {
				pagestorage << table_pages->first << ","
							// << filesystem->tables[table_pages->first]->columns() << ","
							<< pset->slot() << ",";

				for (auto page : pset->pageset) {
					pagestorage << page << ",";
				}
				pagestorage << endl;
			}
		}
		pagestorage.close();
	}
}