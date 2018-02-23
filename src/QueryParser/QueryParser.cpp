#include "QueryParser.h"

QueryParser::QueryParser(FileManager* fs, BufferManager* bf) {
	filesystem = fs;
	buffer = bf;
}

void QueryParser::ParseCREATE(hsql::SQLStatement* statement) {
	const hsql::CreateStatement* create = (const hsql::CreateStatement*) statement;

	// traverse the columns
	string tablename(create->tableName);
	size_t cols = create->columns->size();

	Table* table = new Table(tablename, 0, cols);

	for (auto col : *(create->columns)) {
		string attr(col->name);

		string type;
		if (col->type == hsql::ColumnDefinition::INT)
			type = "INT";
		else if (col->type == hsql::ColumnDefinition::DOUBLE)
			type = "DOUBLE";
		else
			type = "STRING";

		Attribute* attribute = new Attribute(attr, type, tablename);
		table->attributes[attr] = (attribute);
		table->attr_order.push_back(attr);
	}
	filesystem->add(table);
}

void QueryParser::ParseINSERT(hsql::SQLStatement* statement) {
	const hsql::CreateStatement* insert = (const hsql::InsertStatement*) statement;
	string table(insert->tableName);

	// TODO: handle the error that data type doesn't match the correct order
	if (!insert->columns) { // attributes not specified
		PageSet* pset;

		bool pagesetfound = false;
		for (auto p : filesystem->pages[table].pageset) {
			if (p->slot() > 0) {
				pset = p;
				pagesetfound = true;
				break;
			}
		}

		if (!pagesetfound) { // allocate a new page
			// TODO
			int count = tables[table].size();
			PageSet* p = new PageSet(count);
			// check if there are released pages to be recycled
			for (auto attrname : filesystem->tables[table].attr_order) {
				Attribute* attr = filesystem->table[table].attributes[attrname];
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
					filesystem.nextpage++;
				}
				buffer->add(pnumber, attr->type(), attr->table(), attr->name());
			}
			pset = p;
			filesystem->pages[table].push_back(pset);
		}

		pset->slots -= 1;
		for (size_t i = 0; i < insert->values->size(); i++) {
			if (filesystem->pages.size() > 0) { // look for a page that can store this tuple
				int page_num = pset->pageset[i];
				Page* page2modify = buffer->getbuffer()->get(page_num);
				page2modify->dirty = true;
				page2modify->slots -= 1;

				if (val->type == hsql::kExprLiteralString) {
					string sval(insert->values[i]->name);
					Tuple* newtup = new Tuple(false, sval);
					page2modify->content.push_back(newtup);
				}
				else if (val->type == hsql::kExprLiteralInt) {
					int ival = insert->values[i]->ival;
					Tuple* newtup = new Tuple(false, ival);
					page2modify->content.push_back(newtup);
				}
				else if (val->type == hsql::kExprLiteralFloat) {
					double dval = insert->values[i]->fval;
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