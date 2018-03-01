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
	filesystem->user->IncrementSize(1);
}

void QueryParser::ParseINSERT(const hsql::SQLStatement* statement) {
	const hsql::InsertStatement* insert = (const hsql::InsertStatement*) statement;
	string table(insert->tableName);
	table = filesystem->user->name() + "::" + table;

	filesystem->tables[table]->IncrementSize(1);

	// TODO: handle the error that data type doesn't match the correct order
	if (!insert->columns) { // attributes not specified
		// find or create a pageset to store this tuple
		PageSet* pset = filesystem->FindPageSet(table, buffer); 		string t_stamp = addTimeStamp();

		pset->slots -= 1;

		cout << "pageset created, creating pages;" << endl;

		// insert time stamp page
		int t_pagenum = pset->pageset[0];

		LRUCache* cache = buffer->getbuffer();
		if (cache->map.find(t_pagenum) == cache->map.end())
			buffer->fetch(t_pagenum);

		Page* tstamp_page = buffer->getbuffer()->get(t_pagenum);
		tstamp_page->dirty = true;
		tstamp_page->slots -= 1;

		cout << "Timestamp created" << endl;

		Tuple* attr_ts = new Tuple(false, t_stamp);
		tstamp_page->content.push_back(attr_ts);

		for (size_t i = 0; i < insert->values->size(); i++) {
			auto val = (*insert->values)[i];

			if (filesystem->pages.size() > 0) { // look for a page that can store this tuple
				int page_num = pset->pageset[i + 1];

				if (cache->map.find(page_num) == cache->map.end())
					buffer->fetch(page_num);

				Page* page2modify = cache->get(page_num);
				page2modify->dirty = true;
				page2modify->slots -= 1;

				if (val->type == hsql::kExprLiteralString) {
					string sval(val->name);
					Tuple* newtup = new Tuple(false, sval, t_stamp);
					page2modify->content.push_back(newtup);
				}
				else if (val->type == hsql::kExprLiteralInt) {
					int ival = val->ival;
					Tuple* newtup = new Tuple(false, ival, t_stamp);
					page2modify->content.push_back(newtup);
				}
				else if (val->type == hsql::kExprLiteralFloat) {
					double dval = val->fval;
					Tuple* newtup = new Tuple(false, dval, t_stamp);
					page2modify->content.push_back(newtup);
				}
				else { // it's null
					Tuple* newtup = new Tuple(true, t_stamp);
					page2modify->content.push_back(newtup);
				}
			}
		}
	}
}