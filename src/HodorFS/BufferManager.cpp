#include "BufferManager.h"

string DATAPATH = "src/HodorFS/data/";
string TABLESCSV = "tables.csv";
int PAGESIZE = 1000;
pthread_mutex_t Lock = PTHREAD_MUTEX_INITIALIZER;
int CHECKPERIOD = 20;

Tuple::Tuple(bool n, string s) {
	isnull = n;
	sval = s;
}

Tuple::Tuple(bool n, int i) {
	isnull = n;
	ival = i;
}

Tuple::Tuple(bool n, double d) {
	isnull = n;
	dval = d;
}

Tuple::Tuple(bool n) {
	isnull = n;
}

Tuple::Tuple() {}

int Page::getnum() {
	return page_num;
}

string Page::gettable() {
	return table;
}

string Page::getattr() {
	return attribute;
}

string Page::gettype() {
	return type;
}

int Page::getslots() {
	return slots;
}

bool Page::isdirty() {
	return dirty;
}

int Page::getpinned() {
	return pinned;
}

StringPage::StringPage() {
	type = "STRING";
}

IntPage::IntPage() {
	type = "INT";
}

DoublePage::DoublePage() {
	type = "DOUBLE";
}

StringPage::StringPage(int pn, string ta, string at) {
	type = "STRING";
	page_num = pn;
	table = ta;
	attribute = at;
	slots = PAGESIZE;
}

IntPage::IntPage(int pn, string ta, string at) {
	type = "INT";
	page_num = pn;
	table = ta;
	attribute = at;
	slots = PAGESIZE;
}

DoublePage::DoublePage(int pn, string ta, string at) {
	type = "DOUBLE";
	page_num = pn;
	table = ta;
	attribute = at;
	slots = PAGESIZE;
}

// BooleanPage::BooleanPage() {
// 	type = "BNOOLEAN";
// }

void StringPage::read(int pn, string page_name, vector<string> property) {
 	page_num = pn;
	table = property[0];
	attribute = property[1];
	slots = stoi(property[3]);
	dirty = false; // fetched a new page, clean
	pinned = 0; // new page, not pinned by any thread

	ifstream infile(page_name);

	if (!infile) {
		cerr << "Unable to find page " << page_name << "." << endl;
	}
	else {
		string token;

		getline(infile, token); // get rid of the header
		while (getline(infile, token)) {
			Tuple* tuple = new Tuple();

			if (token == "NULL") {
				tuple->isnull = true;
				content.push_back(tuple);
			}
			else {
				tuple->isnull = false;
				tuple->sval = token;
				content.push_back(tuple);
			}
		}
	}
}

void IntPage::read(int pn, string page_name, vector<string> property) {
	page_num = pn;
	table = property[0];
	attribute = property[1];
	slots = stoi(property[3]);

	dirty = false; // fetched a new page, clean
	pinned = 0; // new page, not pinned by any thread

	ifstream infile(page_name);

	if (!infile) {
		cerr << "Unable to find page " << page_name << "." << endl;
	}
	else {
		string token;

		getline(infile, token); // get rid of the header
		while (getline(infile, token)) {
			Tuple* tuple = new Tuple();
			if (token == "NULL") {
				tuple->isnull = true;
				content.push_back(tuple);
			}
			else {
				tuple->isnull = false;
				tuple->ival = stoi(token);
				content.push_back(tuple);
			}
		}
	}
}

void DoublePage::read(int pn, string page_name, vector<string> property) {
	page_num = pn;
	table = property[0];
	attribute = property[1];
	slots = stoi(property[3]);

	dirty = false; // fetched a new page, clean
	pinned = 0; // new page, not pinned by any thread

	ifstream infile(page_name);

	if (!infile) {
		cerr << "Unable to find page " << page_name << "." << endl;
	}
	else {
		string token;

		getline(infile, token); // get rid of the header
		while (getline(infile, token)) {
			Tuple* tuple = new Tuple();
			if (token == "NULL") {
				tuple->isnull = true;
				content.push_back(tuple);
			}
			else {
				tuple->isnull = false;
				tuple->dval = stod(token);
				content.push_back(tuple);
			}
		}
	}
}

// void BooleanPage::read(int pn, string page_name, vector<string> property) {
// 	page_num = pn;
// 	table = property[0];
// 	attribute = property[1];
// 	slots = stoi(property[3]);

// 	dirty = false; // fetched a new page, clean
// 	pinned = 0; // new page, not pinned by any thread

// 	ifstream infile(page_name);

// 	if (!infile) {
// 		cerr << "Unable to find page " << page_name << "." << endl;
// 	}
// 	else {
// 		string line;

// 		getline(infile, line); // get rid of the header
// 		while (getline(infile, line)) {
// 			if (line == "0") 
// 				content.push_back(false);
// 			else if (line == "NULL")
// 				content.push_back(false);
// 			else
// 				content.push_back(true);
// 		}
// 	}
// }

void StringPage::write() {
	string page_name = to_string(page_num);
	int zeros = 10 - page_name.size();

	for (size_t i = 0; i < zeros; i++) 
		page_name = '0' + page_name;
	page_name = DATAPATH + page_name + ".csv";

	ofstream outfile;
	outfile.open(page_name);
	outfile << table << ',' << attribute << ','
			<< type << ',' << to_string(slots)
			<< endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1) {
			if (!content[i]->isnull)
				outfile << content[i]->sval << endl;
			else
				outfile << "NULL" << endl;
		}
		else {
			if (!content[i]->isnull)
				outfile << content[i]->sval;
			else 
				outfile << "NULL";
		}
	}
	outfile.close();
}

void IntPage::write() {
	string page_name = to_string(page_num);
	int zeros = 10 - page_name.size();

	for (size_t i = 0; i < zeros; i++) 
		page_name = '0' + page_name;
	page_name = DATAPATH + page_name + ".csv";

	ofstream outfile;
	outfile.open(page_name);
	outfile << table << ',' << attribute << ','
			<< type << ',' << to_string(slots)
			<< endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1) {
			if (!content[i]->isnull)
				outfile << content[i]->ival << endl;
			else
				outfile << "NULL" <<endl;
		}
		else {
			if (!content[i]->isnull)
				outfile << content[i]->ival;
			else 
				outfile << "NULL";
		}
	}
	outfile.close();
}

void DoublePage::write() {
		string page_name = to_string(page_num);
	int zeros = 10 - page_name.size();

	for (size_t i = 0; i < zeros; i++) 
		page_name = '0' + page_name;
	page_name = DATAPATH + page_name + ".csv";

	ofstream outfile;
	outfile.open(page_name);
	outfile << table << ',' << attribute << ','
			<< type << ',' << to_string(slots)
			<< endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1) {
			if (!content[i]->isnull)
				outfile << content[i]->dval << endl;
			else
				outfile << "NULL" << endl;
		}
		else
			if (!content[i]->isnull)
				outfile << content[i]->dval;
			else
				outfile << "NULL";
	}
	outfile.close();
}

// void BooleanPage::write(string pn) {
// 	ofstream outfile;
// 	outfile.open(pn);
// 	outfile << table << ',' << attribute << ','
// 			<< type << ',' << to_string(slots)
// 			<< endl;

// 	for (size_t i = 0; i < content.size(); i++) {
// 		if (i != content.size() - 1)
// 			outfile << content[i] << endl;
// 		else
// 			outfile << content[i];
// 	}
// 	outfile.close();
// }

void StringPage::display() {
	cout << "Page number: " << page_num << endl;
	cout << "Table: " << table << endl;;
	cout << "Attribute: " << attribute << endl;
	cout << "Type: " << type << endl;
	cout << "Empty slots: " << slots << endl;
	string d = dirty ? "true" : "false";
	cout << "Dirty: " << d << endl;

	for (auto c : content) 
		cout << c->sval << endl;

	cout << endl;
}

void IntPage::display() {
	cout << "Page number: " << page_num << endl;
	cout << "Table: " << table << endl;;
	cout << "Attribute: " << attribute << endl;
	cout << "Type: " << type << endl;
	cout << "Empty slots: " << slots << endl;
	string d = dirty ? "true" : "false";
	cout << "Dirty: " << d << endl;

	for (auto c : content) 
		cout << c->ival << endl;

	cout << endl;
}

void DoublePage::display() {
	cout << "Page number: " << page_num << endl;
	cout << "Table: " << table << endl;;
	cout << "Attribute: " << attribute << endl;
	cout << "Type: " << type << endl;
	cout << "Empty slots: " << slots << endl;
	string d = dirty ? "true" : "false";
	cout << "Dirty: " << d << endl;

	for (auto c : content) 
		cout << c->dval << endl;

	cout << endl;
}

// void BooleanPage::display() {
// 	cout << "Page number: " << page_num << endl;
// 	cout << "Table: " << table << endl;;
// 	cout << "Attribute: " << attribute << endl;
// 	cout << "Type: " << type << endl;
// 	cout << "Empty slots: " << slots << endl;
// 	string d = dirty ? "true" : "false";
// 	cout << "Dirty: " << d << endl;

// 	for (auto c : content) {
// 		if (c)
// 			cout << "True" << endl;
// 		else
// 			cout << "False" << endl;
// 	}

// 	cout << endl;
// }

LRUCache::LRUCache(size_t c) {
	capacity = c;
}

Page* LRUCache::get(int key) {
	auto found_iter = map.find(key);
    if (found_iter == map.end()) //key doesn't exist
        return nullptr;
    linkedlist.splice(linkedlist.begin(), linkedlist, found_iter->second); //move the node corresponding to key to front
    return found_iter->second->second; 
}

void LRUCache::set(int key, Page* value) {
	auto found_iter = map.find(key);
    if (found_iter != map.end()) //key exists
    {
        linkedlist.splice(linkedlist.begin(), linkedlist, found_iter->second); //move the node corresponding to key to front
        found_iter->second->second = value;                        //update value of the node
        return;
    }
    if (map.size() == capacity) { //reached capacity
       int key_to_del = linkedlist.back().first; 
       // flush to disk
       Page* p_to_flush = linkedlist.back().second;
       p_to_flush->write();

       linkedlist.pop_back();            //remove node in list;
       map.erase(key_to_del);      //remove key in map
    }
    linkedlist.emplace_front(key, value);  //create new node in list
    map[key] = linkedlist.begin();       //create correspondence between key and node
}

void LRUCache::remove(int key) {
	auto found_iter = map.find(key);
	if (found_iter != map.end()) {
		// flush to disk
		found_iter->second->second->write();
		linkedlist.erase(found_iter->second);
		map.erase(found_iter);
	}
}

void LRUCache::display() {
	cout << "Buffer size: " << map.size() << endl << endl;

	for (auto it = linkedlist.begin(); it != linkedlist.end(); it++) {
		it->second->display();
	}
}

BufferManager::BufferManager(size_t c) {
	buffer = new LRUCache(c);
}

LRUCache* BufferManager::getbuffer() {
	return buffer;
}

void BufferManager::add(int pn, string type, string table, string attribute) {
	Page* page;

	if (type == "INT") {
		page = new IntPage(pn, table, attribute);
	}
	else if (type == "DOUBLE") {
		page = new DoublePage(pn, table, attribute);
	}
	else { // is string
		page = new StringPage(pn, table, attribute);
	}

	buffer->set(pn, page);
}

void BufferManager::fetch(int pn) {
	string page_name = to_string(pn);
	int zeros = 10 - page_name.size();

	for (size_t i = 0; i < zeros; i++) 
		page_name = '0' + page_name;
	page_name = DATAPATH + page_name + ".csv";

	ifstream infile(page_name);
	if (!infile) {
		cerr << "Page not found from disk." << endl;
	}
	else {
		string header;
		getline(infile, header);

		istringstream iss(header);
		vector<string> property; // table, attribute, type, slots

		for (size_t i = 0; i < 4; i++) {
			string p;
			getline(iss, p, ',');
			property.push_back(p);
		}

		Page* page;

		if (property[2] == "INT")
			page = new IntPage();
		else if (property[2] == "DOUBLE")
			page = new DoublePage();
		else
			page = new StringPage();

		page->read(pn, page_name, property); // read page from disk

		buffer->set(pn, page); // put page into cache
	}
}

void BufferManager::flush(int pn) {
	Page* page = buffer->get(pn);
	if (!page) return;

	if (page->getpinned() > 0) {
		string thread = page->getpinned() > 1 ? "threads" : "thread";
		cerr << "Page pinned by " << page->getpinned() << " " << thread << ", cannot be flushed." << endl;
		return;
	}

	if (page->isdirty()) return; // no need to modify the file in memory
	else {
		string page_name = to_string(pn);
		int zeros = 10 - page_name.size();

		for (size_t i = 0; i < zeros; i++) 
			page_name = '0' + page_name;
		page_name = DATAPATH + page_name + ".csv";

		if (remove(page_name.c_str())) { // remove old file from disk
			cerr << "Failed to remove file " << page_name << endl;
		}
		else { // flush data from memory to disk
			page->write();
		}

	buffer->remove(pn);
	}
}