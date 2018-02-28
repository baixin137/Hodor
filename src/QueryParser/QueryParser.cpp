#include "QueryParser.h"

QueryParser::QueryParser(FileManager* fs, BufferManager* bf) {
	filesystem = fs;
	buffer = bf;
}

void QueryParser::ParseCREATE(const hsql::SQLStatement* statement) {
	const hsql::CreateStatement* create = (const hsql::CreateStatement*) statement;

	// traverse the columns
	string tablename(create->tableName);
	string user = filesystem->user->name();
	string timestamp = addTimeStamp();
	size_t cols = create->columns->size() + 1; // plus timestamp

	Table* table = new Table(tablename, user, timestamp,0, cols);

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
		table->attr_order.push_back(attr);
	}
	filesystem->add(table);
}

void QueryParser::ParseINSERT(const hsql::SQLStatement* statement) {
	const hsql::InsertStatement* insert = (const hsql::InsertStatement*) statement;
	string table(insert->tableName);
	table = filesystem->user->name() + "::" + table;

	// TODO: handle the error that data type doesn't match the correct order
	if (!insert->columns) { // attributes not specified
		PageSet* pset;
		string t_stamp = addTimeStamp();

		cout << "Time is: " << t_stamp << endl;

		bool pagesetfound = false;

		if (filesystem->pages.find(table) == filesystem->pages.end()) {
			cerr << "Table not found in " << filesystem->user->name() << "." << endl;
		}
		else {
			for (auto p : filesystem->pages[table]->pageset) {
				if (p->slot() > 0) {
					pset = p;
					pagesetfound = true;
					break;
				}
			}
			if (!pagesetfound) { // allocate a new page
				cout << "Pageset not found" << endl;
				int count = filesystem->tables[table]->size();

				PageSet* p = new PageSet(PAGESIZE);
				// check if there are released pages to be recycled

				cout << "# of cols is: " << filesystem->tables[table]->attr_order.size() << endl;
				for (auto attrname : filesystem->tables[table]->attr_order) {
					Attribute* attr = filesystem->tables[table]->attributes[attrname];
					int pnumber;

					if (filesystem->emptypages.size() > 0) {
						auto iter = filesystem->emptypages.begin();
						pnumber = *iter;
						p->pageset.push_back(pnumber);
						filesystem->emptypages.erase(iter);
					}
					else {
						pnumber = filesystem->nextpage;
						p->pageset.push_back(pnumber);
						filesystem->nextpage++;
					}
					buffer->add(pnumber, attr->type(), attr->table(), attr->name());
				}
				pset = p;
				filesystem->pages[table]->pageset.push_back(pset);
			}

			pset->slots -= 1;

			cout << "pageset created, creating pages;" << endl;

			// insert time stamp page
			int t_pagenum = pset->pageset[0];
			Page* tstamp_page = buffer->getbuffer()->get(t_pagenum);
			tstamp_page->dirty = true;
			tstamp_page->slots -= 1;

			Tuple* attr_ts = new Tuple(false, t_stamp);
			tstamp_page->content.push_back(attr_ts);

			for (size_t i = 0; i < insert->values->size(); i++) {
				auto val = (*insert->values)[i];

				if (filesystem->pages.size() > 0) { // look for a page that can store this tuple
					int page_num = pset->pageset[i + 1];
					Page* page2modify = buffer->getbuffer()->get(page_num);
					page2modify->dirty = true;
					page2modify->slots -= 1;

					if (val->type == hsql::kExprLiteralString) {
						string sval(val->name);
						Tuple* newtup = new Tuple(false, sval);
						page2modify->content.push_back(newtup);
					}
					else if (val->type == hsql::kExprLiteralInt) {
						int ival = val->ival;
						Tuple* newtup = new Tuple(false, ival);
						page2modify->content.push_back(newtup);
					}
					else if (val->type == hsql::kExprLiteralFloat) {
						double dval = val->fval;
						Tuple* newtup = new Tuple(false, dval);
						page2modify->content.push_back(newtup);
					}
					else { // it's null
						Tuple* newtup = new Tuple(true);
						page2modify->content.push_back(newtup);
					}
				}
			}
		}
	}
}