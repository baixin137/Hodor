#include "QueryParser.h"

QueryParser::QueryParser(FileManager* fs, BufferManager* bf) {
	filesystem = fs;
	buffer = bf;
}

template <typename T> void QueryParser::printElement(T elem, const int& width) {
	cout << left << setw(width) << setfill(' ') << elem << '|';
}

void QueryParser::ParseCREATE(const hsql::SQLStatement* statement) {
	const hsql::CreateStatement* create = (const hsql::CreateStatement*) statement;

	// traverse the columns
	string tablename(create->tableName);
	tablename = filesystem->user->name() + "::" + tablename;
	string user = filesystem->user->name();
	string timestamp = addTimeStamp();
	size_t cols = create->columns->size() + 1; // plus timestamp

	Table* table = new Table(tablename, user, timestamp,0, cols);

	// cout << "Creating new table: " << tablename << endl;

	// add timestamp to table
	Attribute* t_stamp = new Attribute("TimeStamp", "TEXT", tablename);
	table->attributes["TimeStamp"] = t_stamp;
	table->attr_order.push_back("TimeStamp");

	for (auto col : *(create->columns)) {
		string attr(col->name);

		string type;
		if (col->type == hsql::ColumnDefinition::INT)
			type = "INT";
		else if (col->type == hsql::ColumnDefinition::DOUBLE)
			type = "DOUBLE";
		else
			type = "TEXT";

		Attribute* attribute = new Attribute(attr, type, tablename);
		table->attributes[attr] = attribute;
		// cout << "Adding column " << attr << " to table" << endl;
		table->attr_order.push_back(attr);
	}
	filesystem->add(table);
	filesystem->user->IncrementSize(1);
}

void QueryParser::ParseINSERT(const hsql::SQLStatement* statement) {
	const hsql::InsertStatement* insert = (const hsql::InsertStatement*) statement;
	string table(insert->tableName);
	table = filesystem->user->name() + "::" + table;
	// cout << "Ready to insert to table: " << table << endl;

	if (filesystem->tables.find(table) == filesystem->tables.end()) {
		cerr << "Trying to insert to non-existing table" << endl;
		return;
	}

	filesystem->tables[table]->IncrementSize(1);

	// cout << "Table size incremented" << endl;

	filesystem->tables[table]->IncrementSize(1);

	// TODO: handle the error that data type doesn't match the correct order
	if (!insert->columns) { // attributes not specified
		// find or create a pageset to store this tuple
		PageSet* pset = filesystem->FindPageSet(table, buffer);
		string t_stamp = addTimeStamp();

		// add pages to attributes
		for (size_t i = 0; i < pset->pageset.size(); i++) {
			string attrname = filesystem->tables[table]->attr_order[i];
			Attribute* attribute = filesystem->tables[table]->attributes[attrname];
			if (attribute->pages.find(pset->pageset[i]) != attribute->pages.end())
				break;
			else {
				attribute->pages.insert(pset->pageset[i]);
				attribute->page_order.push_back(pset->pageset[i]);
			}
		}

		pset->slots -= 1;

		// cout << "pageset created, creating pages;" << endl;

		// insert time stamp page
		int t_pagenum = pset->pageset[0];

		LRUCache* cache = buffer->getbuffer();
		if (cache->map.find(t_pagenum) == cache->map.end())
			buffer->fetch(t_pagenum);

		Page* tstamp_page = buffer->getbuffer()->get(t_pagenum);
		tstamp_page->dirty = true;
		tstamp_page->slots -= 1;

		// cout << "Timestamp created" << endl;

		Tuple* attr_ts = new Tuple(false, t_stamp, t_stamp);
		tstamp_page->content.push_back(attr_ts);
		tstamp_page->IncrementSize(1);

		for (size_t i = 0; i < insert->values->size(); i++) {
			auto val = (*insert->values)[i];

			if (filesystem->pages.size() > 0) { // look for a page that can store this tuple
				int page_num = pset->pageset[i + 1];

				if (cache->map.find(page_num) == cache->map.end())
					buffer->fetch(page_num);

				Page* page2modify = cache->get(page_num);
				page2modify->dirty = true;
				page2modify->slots -= 1;

				Tuple* newtup;

				if (val->type == hsql::kExprLiteralString) {
					string sval(val->name);
					newtup = new Tuple(false, sval, t_stamp);
					// cout << "New tuple: " << sval << " type: " << newtup->type() << endl;
				}
				else if (val->type == hsql::kExprLiteralInt) {
					int ival = val->ival;
					newtup = new Tuple(false, ival, t_stamp);
				}
				else if (val->type == hsql::kExprLiteralFloat) {
					double dval = val->fval;
					newtup = new Tuple(false, dval, t_stamp);
				}
				else { // it's null
					newtup = new Tuple(true, t_stamp);
				}
				page2modify->content.push_back(newtup);

				if (page2modify->type() == "INT")
					page2modify->UpdateMeta(val->ival);
				else if (page2modify->type() == "DOUBLE")
					page2modify->UpdateMeta(val->fval);

				page2modify->IncrementSize(1);
			}
		}
	}
}

void QueryParser::ParseSELECT(const hsql::SQLStatement* statement) {
	const hsql::SelectStatement* select = (const hsql::SelectStatement*) statement;

	string t_name(select->fromTable->name);
	t_name = filesystem->user->name() + "::" + t_name;
	// cout << "table name is: " << t_name << endl;
	Table* fromTable = filesystem->tables[t_name];

	vector<Attribute*> selectList;
	for (auto column : *select->selectList) {
		string col(column->name);

		if (fromTable->attributes.find(col) == fromTable->attributes.end()) {
			cout << "Error: table <" << TableName(t_name) << "> only has attributes: " << endl;
			for (auto a : fromTable->attr_order) {
				cout << '<' << a << '>' << " ";
			}
			cout << endl;
			return;
		}
		selectList.push_back(fromTable->attributes[col]);
	}

	size_t num_pages = selectList[0]->pages.size();
	size_t cols = selectList.size();

	PrintLine(BOXWIDTH, cols);

	for (size_t i = 0; i < num_pages; i++) {
		int page_num = selectList[0]->page_order[i];
		if (!buffer->iscached(page_num)) {
			buffer->fetch(page_num);
		}
		Page* p = buffer->get(page_num);
		size_t num_tuples = p->size();
		// cout << 1 << endl;
		for (size_t j = 0; j < num_tuples; j++) {
			// cout << 2 << endl;
			cout << '|';
			for (size_t k = 0; k < cols; k++) {
				// cout << 3 << endl;
				int page_num = selectList[k]->page_order[i];
				if (!buffer->iscached(page_num)) {
					buffer->fetch(page_num);
				}
				Page* page = buffer->get(page_num);

				if (page->type() == "TEXT")
					printElement(page->content[j]->sval, BOXWIDTH);
				else if (page->type() == "INT")
					printElement(page->content[j]->ival, BOXWIDTH);
				else
					printElement(page->content[j]->dval, BOXWIDTH);
			}
			cout << endl;
			if (j < num_tuples - 1)
				PrintLineInner(BOXWIDTH, cols);
			else
				PrintLine(BOXWIDTH, cols);
		}
	}
}