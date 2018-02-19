#include "BufferManager.h"

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

BooleanPage::BooleanPage() {
	type = "BNOOLEAN";
}

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
		string line;

		getline(infile, line); // get rid of the header
		while (getline(infile, line)) {
			if (line == "NULL")
				content.push_back("");
			else
				content.push_back(line);
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
		string line;

		getline(infile, line); // get rid of the header
		while (getline(infile, line)) {
			if (line == "NULL")
				content.push_back(0);
			else
				content.push_back(stoi(line));
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
		string line;

		getline(infile, line); // get rid of the header
		while (getline(infile, line)) {
			if (line == "NULL")
				content.push_back(0.0);
			else
				content.push_back(stod(line));
		}
	}
}

void BooleanPage::read(int pn, string page_name, vector<string> property) {
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
		string line;

		getline(infile, line); // get rid of the header
		while (getline(infile, line)) {
			if (line == "0") 
				content.push_back(false);
			else if (line == "NULL")
				content.push_back(false);
			else
				content.push_back(true);
		}
	}
}

void StringPage::write(string pn) {
	ofstream outfile;
	outfile.open(pn);
	outfile << table << ',' << attribute << ','
			<< type << ',' << to_string(slots)
			<< endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1)
			outfile << content[i] << endl;
		else
			outfile << content[i];
	}

	outfile.close();
}

void IntPage::write(string pn) {
	ofstream outfile;
	outfile.open(pn);
	outfile << table << ',' << attribute << ','
			<< type << ',' << to_string(slots)
			<< endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1)
			outfile << content[i] << endl;
		else
			outfile << content[i];
	}

	outfile.close();
}

void DoublePage::write(string pn) {
	ofstream outfile;
	outfile.open(pn);
	outfile << table << ',' << attribute << ','
			<< type << ',' << to_string(slots)
			<< endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1)
			outfile << content[i] << endl;
		else
			outfile << content[i];
	}

	outfile.close();
}

void BooleanPage::write(string pn) {
	ofstream outfile;
	outfile.open(pn);
	outfile << table << ',' << attribute << ','
			<< type << ',' << to_string(slots)
			<< endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1)
			outfile << content[i] << endl;
		else
			outfile << content[i];
	}

	outfile.close();
}

void StringPage::display() {
	cout << "Page number: " << page_num << endl;
	cout << "Table: " << table << endl;;
	cout << "Attribute: " << attribute << endl;
	cout << "Type: " << type << endl;
	cout << "Empty slots: " << slots << endl;
	string d = dirty ? "true" : "false";
	cout << "Dirty: " << d << endl;

	for (auto c : content) 
		cout << c << endl;

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
		cout << c << endl;

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
		cout << c << endl;

	cout << endl;
}

void BooleanPage::display() {
	cout << "Page number: " << page_num << endl;
	cout << "Table: " << table << endl;;
	cout << "Attribute: " << attribute << endl;
	cout << "Type: " << type << endl;
	cout << "Empty slots: " << slots << endl;
	string d = dirty ? "true" : "false";
	cout << "Dirty: " << d << endl;

	for (auto c : content) {
		if (c)
			cout << "True" << endl;
		else
			cout << "False" << endl;
	}

	cout << endl;
}

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
    if (map.size() == capacity) //reached capacity
    {
       int key_to_del = linkedlist.back().first; 
       linkedlist.pop_back();            //remove node in list;
       map.erase(key_to_del);      //remove key in map
    }
    linkedlist.emplace_front(key, value);  //create new node in list
    map[key] = linkedlist.begin();       //create correspondence between key and node
}

void LRUCache::display() {
	cout << "Buffer size: " << map.size() << endl << endl;

	for (auto it = map.begin(); it != map.end(); it++) {
		it->second->second->display();
	}
}

BufferManager::BufferManager(size_t c) {
	buffer = new LRUCache(c);
}

void BufferManager::fetch(int pn) {
	string page_name = to_string(pn);
	int zeros = 10 - page_name.size();

	for (size_t i = 0; i < zeros; i++) 
		page_name = '0' + page_name;
	page_name = "data/" + page_name + ".csv";

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

		if (property[2] == "STRING")
			page = new StringPage();
		else if (property[2] == "INT")
			page = new IntPage();
		else if (property[2] == "DOUBLE")
			page = new DoublePage();
		else
			page = new BooleanPage();

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

	if (!page->isdirty()) return; // no need to modify the file in memory
	else {
		string page_name = to_string(pn);
		int zeros = 10 - page_name.size();

		for (size_t i = 0; i < zeros; i++) 
			page_name = '0' + page_name;
		page_name = "data/" + page_name + ".csv";

		if (!remove(page_name.c_str())) { // remove old file from disk
			cerr << "Failed to remove file " << page_name << endl;
		}
		else { // flush data from memory to disk
			page->write(page_name);
		}
	}
}