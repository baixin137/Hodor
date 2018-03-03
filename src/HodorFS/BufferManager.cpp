#include "BufferManager.h"

Tuple::Tuple(bool n, string s, string t) {
	isnull = n;
	sval = s;
	time = t;
	tupletype = "TEXT";
}

Tuple::Tuple(bool n, int i, string t) {
	isnull = n;
	ival = i;
	time = t;
	tupletype = "INT";
}

Tuple::Tuple(bool n, double d, string t) {
	isnull = n;
	dval = d;
	time = t;
	tupletype = "DOUBLE";
}

Tuple::Tuple(bool n, string t) {
	isnull = n;
	time = t;
}

Tuple::Tuple(string t) {
	time = t;
}

string Tuple::timestamp() {
	return time;
}

string Tuple::type() {
	return tupletype;
}

int Page::number() {
	return page_num;
}

int Page::size() {
	return attrs;
}

string Page::table() {
	return tablename;
}

string Page::attribute() {
	return attrname;
}

string Page::type() {
	return attrtype;
}

int Page::slot() {
	return slots;
}

bool Page::isdirty() {
	return dirty;
}

int Page::pinned() {
	return threads;
}

void Page::IncrementSize(size_t s) {
	attrs += s;
}

TextPage::TextPage() {
	attrtype = "TEXT";
}

IntPage::IntPage() {
	attrtype = "INT";
}

DoublePage::DoublePage() {
	attrtype = "DOUBLE";
}

TextPage::TextPage(int pn, string ta, string at, size_t attr) {
	attrtype = "TEXT";
	page_num = pn;
	tablename = ta;
	attrname = at;
	slots = PAGESIZE;
	attrs = attr;
}

IntPage::IntPage(int pn, string ta, string at, size_t attr) {
	attrtype = "INT";
	page_num = pn;
	tablename = ta;
	attrname = at;
	slots = PAGESIZE;
	attrs = attr;
}

DoublePage::DoublePage(int pn, string ta, string at, size_t attr) {
	attrtype = "DOUBLE";
	page_num = pn;
	tablename = ta;
	attrname = at;
	slots = PAGESIZE;
	attr = attrs;
}

// BooleanPage::BooleanPage() {
// 	type = "BNOOLEAN";
// }

void TextPage::read(int pn, string page_name, vector<string> property) {
 	page_num = pn;
	tablename = property[0];
	attrname = property[1];
	slots = stoi(property[3]);
	attrs = stoi(property[4]);
	dirty = false; // fetched a new page, clean
	threads = 0; // new page, not pinned by any thread

	ifstream infile(page_name);

	if (!infile) {
		cerr << "Unable to find page " << page_name << "." << endl;
	}
	else {
		string token;

		getline(infile, token); // skip the header
		while (getline(infile, token)) {
			istringstream iss(token);
			string value;
			string t_stamp;

			getline(iss, value,   ',');
			getline(iss, t_stamp, ',');

			Tuple* tuple = new Tuple(t_stamp);

			if (value == "NULL") {
				tuple->isnull = true;
			}
			else {
				tuple->isnull = false;
				tuple->sval = value;
			}
			tuple->tupletype = "TEXT";
			content.push_back(tuple);
		}
	}
}

void IntPage::read(int pn, string page_name, vector<string> property) {
	page_num = pn;
	tablename = property[0];
	attrname = property[1];
	slots = stoi(property[3]);
	attrs = stoi(property[4]);
	minval = stoi(property[5]);
	maxval = stoi(property[6]);
	meanval = stod(property[7]);

	dirty = false; // fetched a new page, clean
	threads = 0; // new page, not pinned by any thread

	ifstream infile(page_name);

	if (!infile) {
		cerr << "Unable to find page " << page_name << "." << endl;
	}
	else {
		string token;

		getline(infile, token); // get rid of the header
		while (getline(infile, token)) {
			istringstream iss(token);
			string value;
			string t_stamp;

			getline(iss, value,   ',');
			getline(iss, t_stamp, ',');

			Tuple* tuple = new Tuple(t_stamp);
			if (value == "NULL") {
				tuple->isnull = true;
			}
			else {
				tuple->isnull = false;
				tuple->ival = stoi(value);
			}
			tuple->tupletype = "INT";
			content.push_back(tuple);
		}
	}
}

void DoublePage::read(int pn, string page_name, vector<string> property) {
	page_num = pn;
	tablename = property[0];
	attrname = property[1];
	slots = stoi(property[3]);
	attrs = stoi(property[4]);
	minval = stod(property[5]);
	maxval = stod(property[6]);
	meanval = stod(property[7]);

	dirty = false; // fetched a new page, clean
	threads = 0; // new page, not pinned by any thread

	ifstream infile(page_name);

	if (!infile) {
		cerr << "Unable to find page " << page_name << "." << endl;
	}
	else {
		string token;

		getline(infile, token); // get rid of the header
		while (getline(infile, token)) {
			istringstream iss(token);
			string value;
			string t_stamp;

			getline(iss, value,   ',');
			getline(iss, t_stamp, ',');

			Tuple* tuple = new Tuple(t_stamp);
			if (value == "NULL") {
				tuple->isnull = true;
			}
			else {
				tuple->isnull = false;
				tuple->dval = stod(value);
			}
			tuple->tupletype = "DOUBLE";
			content.push_back(tuple);
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

void TextPage::write() {
	string page_name = GetPageFile(page_num);

	ofstream outfile;
	outfile.open(page_name);
	outfile << tablename << ',' << attrname << ','
			<< attrtype << ',' << slots << ','
			<< attrs << endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1) {
			if (!content[i]->isnull) {
				outfile << content[i]->sval << ","
						<< content[i]->timestamp() << endl;
			}
			else {
				outfile << "NULL" << ","
						<< content[i]->timestamp() << endl;
			}
		}
		else {
			if (!content[i]->isnull) {
				outfile << content[i]->sval << ","
						<< content[i]->timestamp();
			}
			else  {
				outfile << "NULL" << ","
						<< content[i]->timestamp();
			}
		}
	}
	outfile.close();
}

void IntPage::write() {
	string page_name = GetPageFile(page_num);

	ofstream outfile;
	outfile.open(page_name);
	outfile << tablename << ',' << attrname << ','
			<< attrtype << ',' << slots << ','
			<< attrs << ',' << minval << ','
			<< maxval << ',' << meanval << endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1) {
			if (!content[i]->isnull) {
				outfile << content[i]->ival << ","
						<< content[i]->timestamp() << endl;
			}
			else {
				outfile << "NULL" << ","
						<< content[i]->timestamp() << endl;
			}
		}
		else {
			if (!content[i]->isnull) {
				outfile << content[i]->ival << ","
						<< content[i]->timestamp();
			}
			else {
				outfile << "NULL" << ","
						<< content[i]->timestamp();
			}
		}
	}
	outfile.close();
}

void DoublePage::write() {
	string page_name = GetPageFile(page_num);

	ofstream outfile;
	outfile.open(page_name);
	outfile << tablename << ',' << attrname << ','
			<< attrtype << ',' << slots << ','
			<< attrs << ',' << minval << ','
			<< maxval << ',' << meanval << endl;

	for (size_t i = 0; i < content.size(); i++) {
		if (i != content.size() - 1) {
			if (!content[i]->isnull) {
				outfile << content[i]->dval << ","
						<< content[i]->timestamp() << endl;
			}
			else {
				outfile << "NULL" << ","
						<< content[i]->timestamp() << endl;
			}
		}
		else
			if (!content[i]->isnull) {
				outfile << content[i]->dval << ","
						<< content[i]->timestamp();
			}
			else
				outfile << "NULL" << ","
						<< content[i]->timestamp();
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

// void TextPage::display() {
// 	cout << "Page number: " << page_num << endl;
// 	cout << "Table: " << table << endl;;
// 	cout << "Attribute: " << attribute << endl;
// 	cout << "Type: " << type << endl;
// 	cout << "Empty slots: " << slots << endl;
// 	string d = dirty ? "true" : "false";
// 	cout << "Dirty: " << d << endl;

// 	for (auto c : content) 
// 		cout << c->sval << endl;

// 	cout << endl;
// }

// void IntPage::display() {
// 	cout << "Page number: " << page_num << endl;
// 	cout << "Table: " << table << endl;;
// 	cout << "Attribute: " << attribute << endl;
// 	cout << "Type: " << type << endl;
// 	cout << "Empty slots: " << slots << endl;
// 	string d = dirty ? "true" : "false";
// 	cout << "Dirty: " << d << endl;

// 	for (auto c : content) 
// 		cout << c->ival << endl;

// 	cout << endl;
// }

// void DoublePage::display() {
// 	cout << "Page number: " << page_num << endl;
// 	cout << "Table: " << table << endl;;
// 	cout << "Attribute: " << attribute << endl;
// 	cout << "Type: " << type << endl;
// 	cout << "Empty slots: " << slots << endl;
// 	string d = dirty ? "true" : "false";
// 	cout << "Dirty: " << d << endl;

// 	for (auto c : content) 
// 		cout << c->dval << endl;

// 	cout << endl;
// }

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

int IntPage::min() {return minval;}
double DoublePage::min() {return minval;}

int IntPage::max() {return maxval;}
double DoublePage::max() {return maxval;}

double IntPage::mean() {return meanval;}
double DoublePage::mean() {return meanval;}

void TextPage::UpdateMeta(double val) {}

void IntPage::UpdateMeta(double val) {
	val = int(val);

	if (attrs == 0)
		minval = val;
	else 
		minval = val < minval ? val : minval;

	maxval = val > maxval ? val : maxval;
	if (attrs == 0)
		meanval = double(val);
	else
		meanval = (meanval * attrs + double(val)) / (attrs + 1);
}

void DoublePage::UpdateMeta(double val) {
	if (attrs == 0)
		minval = val;
	else
		minval = val < minval ? val : minval;
	maxval = val > maxval ? val : maxval;
	if (attrs == 0)
		meanval = val;
	else
		meanval = (meanval * attrs + val) / (attrs + 1);
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

// void LRUCache::display() {
// 	cout << "Buffer size: " << map.size() << endl << endl;

// 	for (auto it = linkedlist.begin(); it != linkedlist.end(); it++) {
// 		it->second->display();
// 	}
// }

BufferManager::BufferManager(size_t c) {
	buffer = new LRUCache(c);
}

LRUCache* BufferManager::getbuffer() {
	return buffer;
}

void BufferManager::add(int pn, string type, string table, string attribute) {
	Page* page;

	if (type == "INT") {
		page = new IntPage(pn, table, attribute, 0);
	}
	else if (type == "DOUBLE") {
		page = new DoublePage(pn, table, attribute, 0);
	}
	else { // is string
		page = new TextPage(pn, table, attribute, 0);
	}

	buffer->set(pn, page);
}

void BufferManager::fetch(int pn) {
	string page_name = GetPageFile(pn);

	ifstream infile(page_name);
	if (!infile) {
		cerr << "Page not found from disk." << endl;
	}
	else {
		string header;
		getline(infile, header);

		istringstream iss(header);
		vector<string> property; // table, attribute, type, slots, size

		for (size_t i = 0; i < 5; i++) {
			string p;
			getline(iss, p, ',');
			property.push_back(p);
		}
		if (property[2] == "INT" || property[2] == "DOUBLE") {
			for (size_t i = 0; i < 3; i++) {
				string p;
				getline(iss, p, ',');
				property.push_back(p);
			}
		}

		Page* page;

		if (property[2] == "INT")
			page = new IntPage();
		else if (property[2] == "DOUBLE")
			page = new DoublePage();
		else
			page = new TextPage();

		page->read(pn, page_name, property); // read page from disk

		buffer->set(pn, page); // put page into cache
	}
}

void BufferManager::flush(int pn) {
	Page* page = buffer->get(pn);
	if (!page) return;

	if (page->pinned() > 0) {
		string thread = page->pinned() > 1 ? "threads" : "thread";
		cerr << "Page pinned by " << page->pinned() << " " << thread << ", cannot be flushed." << endl;
		return;
	}

	if (page->isdirty()) return; // no need to modify the file in memory
	else {
		string page_name = GetPageFile(pn);

		if (remove(page_name.c_str())) { // remove old file from disk
			cerr << "Failed to remove file " << page_name << endl;
		}
		else { // flush data from memory to disk
			page->write();
		}

	buffer->remove(pn);
	}
}

void BufferManager::erase(int pn) {
	string path = GetPageFile(pn);
	if (remove(path.c_str()) != 0) {
		cerr << "Unable to delete file: " << path << endl;
	}
	buffer->remove(pn);
}

bool BufferManager::iscached(int pn) {
	return (buffer->map.find(pn) != buffer->map.end());
}

Page* BufferManager::get(int pn) {
	return buffer->get(pn);
}


void BufferManager::MoveTuple(PageSet* pnew, PageSet* pold, size_t line) {
	pnew->slots -= 1;
	for (size_t i = 0; i < pold->pageset.size(); i++) {
		Page* page_old = get(pold->pageset[i]);
		Page* page_new = get(pnew->pageset[i]);

		Tuple* tuple_old = page_old->content[line];
		page_new->content.push_back(tuple_old);

		if (tuple_old->type() == "INT") {
			cout << "updating int meta" << endl;
			page_new->UpdateMeta(tuple_old->ival);
		}
		else if (tuple_old->type() == "DOUBLE") {
			cout << "updating float meta" << endl;
			page_new->UpdateMeta(tuple_old->dval);
		}

		page_new->IncrementSize(1);
		page_new->dirty = true;
		page_new->slots -= 1;
	}
	cout << "Tuples moved from old pageset successfully" << endl;
}