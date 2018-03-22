#include "QueryParser.h"

Column::Column(Attribute* attr, FuncType t, string s) {
	attribute = attr;
	type = t;
	attrname = s;
}

Column::Column(Attribute* attr, string s) {
	attribute = attr;
	type = aFuncNA;
	attrname = s;
}

string Column::name() {
	return attrname;
}

Entry::Entry(size_t w) {
	width = w;
}

size_t Entry::size() {
	return width;
}

QueryResult::QueryResult() {}

bool QueryParser::ConditionMet(hsql::OperatorType op, int target, int condition) {
	if (op == hsql::kOpEquals) {
		if (target == condition) return true;
	}
	else if (op == hsql::kOpNotEquals) {
		if (target != condition) return true;
	}
	else if (op == hsql::kOpLess) {
		if (target < condition) return true;
	}
	else if (op == hsql::kOpLessEq) {
		if (target <= condition) return true;
	}
	else if (op == hsql::kOpGreater) {
		if (target > condition) return true;
	}
	else if (op == hsql::kOpGreaterEq) {
		if (target >= condition) return true;
	}
	else {
		cerr << "Error: Unsupported operation." << endl;
	}

	return false;
}

bool QueryParser::ConditionMet(hsql::OperatorType op, double target, double condition) {
	if (op == hsql::kOpEquals) {
		if (target == condition) return true;
	}
	else if (op == hsql::kOpNotEquals) {
		if (target != condition) return true;
	}
	else if (op == hsql::kOpLess) {
		if (target < condition) return true;
	}
	else if (op == hsql::kOpLessEq) {
		if (target <= condition) return true;
	}
	else if (op == hsql::kOpGreater) {
		if (target > condition) return true;
	}
	else if (op == hsql::kOpGreaterEq) {
		if (target >= condition) return true;
	}
	else {
		cerr << "Error: Unsupported operation." << endl;
	}

	return false;
}

bool QueryParser::ConditionMet(hsql::OperatorType op, string target, string condition) {
	if (op == hsql::kOpEquals) {
		if (target == condition) return true;
	}
	else if (op == hsql::kOpNotEquals) {
		if (target != condition) return true;
	}
	else {
		cerr << "Error: Unsupported operation." << endl;
	}

	return false;
}

// void Entry::print() {
// 	cout << '|';
// 	for (string item : attributeList)
// 		cout << left << setw(width) << setfill(' ') << item << '|';
// 	cout << endl;
// }

void QueryParser::filter(QueryResult* entries, hsql::OperatorType op, int val, 
						 Attribute* attr, unordered_map<string, Column*>& selectList,
						 vector<string>& selectOrder, unordered_map<string, Column*>& groupbyList,
						 vector<string>& totalList) {
	size_t num_pages = selectList[selectOrder[0]]->attribute->pages.size();
	size_t cols = totalList.size();

	for (size_t i = 0; i < num_pages; i++) {
		int page_num = selectList[selectOrder[0]]->attribute->page_order[i];
		if (!buffer->iscached(page_num))
			buffer->fetch(page_num);

		Page* p = buffer->get(page_num);
		size_t num_tuples = p->size();

		for (size_t j = 0; j < num_tuples; j++) {
			Entry* entry = new Entry(BOXWIDTH);
			bool satisfied = true;

			for (size_t k = 0; k < cols; k++) {
				int page_condnum = attr->page_order[i];
				if (!buffer->iscached(page_condnum))
					buffer->fetch(page_condnum);

				Page* page_cond = buffer->get(page_condnum);

				if (selectList.find(totalList[k]) != selectList.end()) {
					int page_num = selectList[totalList[k]]->attribute->page_order[i];
					if (!buffer->iscached(page_num))
						buffer->fetch(page_num);

					Page* page = buffer->get(page_num);

					int target = page_cond->content[j]->ival;
					if (ConditionMet(op, target, val)) {
						if (page->type() == "TEXT") {
							string text(page->content[j]->sval);
							entry->attributeList[page->attribute()] = text;
							// if the column is also in group by list, push it into the list
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = text;
						}
						else if (page->type() == "INT") {
							entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->ival);
						}
						else {
							entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->dval);
						}

						if (i == 0 && j == 0)
							entries->attrnames.push_back(page->attribute());
					}
					else
						satisfied = false;
				}
				else { // column in groupby list but not select list
					int page_num = groupbyList[totalList[k]]->attribute->page_order[i];
					if (!buffer->iscached(page_num))
						buffer->fetch(page_num);

					Page* page = buffer->get(page_num);

					int target = page_cond->content[j]->ival;
					if (ConditionMet(op, target, val)) {
						if (page->type() == "TEXT") {
							string text(page->content[j]->sval);
							entry->attributeGroupby[page->attribute()] = text;
						}
						else if (page->type() == "INT")
							entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->ival);
						else
							entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->dval);
					}
				}
			}
			if (satisfied)
				entries->item.push_back(entry);
		}
	}
	// for (size_t i = 0; i < entries.size(); i++) {
	// 	entries[i]->print();
	// 	if (i < entries.size() - 1)
	// 		PrintLineInner(BOXWIDTH, cols);
	// }
}

void QueryParser::filter(QueryResult* entries, hsql::OperatorType op, double val, 
						 Attribute* attr, unordered_map<string, Column*>& selectList,
						 vector<string>& selectOrder, unordered_map<string, Column*>& groupbyList,
						 vector<string>& totalList) {
	size_t num_pages = selectList[selectOrder[0]]->attribute->pages.size();
	size_t cols = totalList.size();

	for (size_t i = 0; i < num_pages; i++) {
		int page_num = selectList[selectOrder[0]]->attribute->page_order[i];
		if (!buffer->iscached(page_num))
			buffer->fetch(page_num);

		Page* p = buffer->get(page_num);
		size_t num_tuples = p->size();

		for (size_t j = 0; j < num_tuples; j++) {
			Entry* entry = new Entry(BOXWIDTH);
			bool satisfied = true;

			for (size_t k = 0; k < cols; k++) {
				int page_condnum = attr->page_order[i];
				if (!buffer->iscached(page_condnum))
					buffer->fetch(page_condnum);

				Page* page_cond = buffer->get(page_condnum);

				if (selectList.find(totalList[k]) != selectList.end()) {
					int page_num = selectList[totalList[k]]->attribute->page_order[i];
					if (!buffer->iscached(page_num))
						buffer->fetch(page_num);

					Page* page = buffer->get(page_num);

					double target = page_cond->content[j]->dval;
					if (ConditionMet(op, target, val)) {
						if (page->type() == "TEXT") {
							string text(page->content[j]->sval);
							entry->attributeList[page->attribute()] = text;
							// if the column is also in group by list, push it into the list
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = text;

						}
						else if (page->type() == "INT") {
							entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->ival);

						}
						else {
							entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->dval);

						}

						if (i == 0 && j == 0)
							entries->attrnames.push_back(page->attribute());
					}
					else
						satisfied = false;
				}
				else { // column in groupby list but not select list
					int page_num = groupbyList[totalList[k]]->attribute->page_order[i];
					if (!buffer->iscached(page_num))
						buffer->fetch(page_num);

					Page* page = buffer->get(page_num);

					double target = page_cond->content[j]->dval;
					if (ConditionMet(op, target, val)) {
						if (page->type() == "TEXT") {
							string text(page->content[j]->sval);
							entry->attributeGroupby[page->attribute()] = text;
						}
						else if (page->type() == "INT")
							entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->ival);
						else
							entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->dval);
					}
				}
			}
			if (satisfied)
				entries->item.push_back(entry);
		}
	}
	// for (size_t i = 0; i < entries.size(); i++) {
	// 	entries[i]->print();
	// 	if (i < entries.size() - 1)
	// 		PrintLineInner(BOXWIDTH, cols);
	// }
}

void QueryParser::filter(QueryResult* entries, hsql::OperatorType op, string val, 
						 Attribute* attr, unordered_map<string, Column*>& selectList,
						 vector<string>& selectOrder, unordered_map<string, Column*>& groupbyList,
						 vector<string>& totalList) {
	size_t num_pages = selectList[selectOrder[0]]->attribute->pages.size();
	size_t cols = totalList.size();

	for (size_t i = 0; i < num_pages; i++) {
		int page_num = selectList[selectOrder[0]]->attribute->page_order[i];
		if (!buffer->iscached(page_num))
			buffer->fetch(page_num);

		Page* p = buffer->get(page_num);
		size_t num_tuples = p->size();

		for (size_t j = 0; j < num_tuples; j++) {
			Entry* entry = new Entry(BOXWIDTH);
			bool satisfied = true;

			for (size_t k = 0; k < cols; k++) {
				int page_condnum = attr->page_order[i];
				if (!buffer->iscached(page_condnum))
					buffer->fetch(page_condnum);

				Page* page_cond = buffer->get(page_condnum);

				if (selectList.find(totalList[k]) != selectList.end()) {
					int page_num = selectList[totalList[k]]->attribute->page_order[i];
					if (!buffer->iscached(page_num))
						buffer->fetch(page_num);

					Page* page = buffer->get(page_num);

					string target = page_cond->content[j]->sval;
					if (ConditionMet(op, target, val)) {
						if (page->type() == "TEXT") {
							string text(page->content[j]->sval);
							entry->attributeList[page->attribute()] = text;
							// if the column is also in group by list, push it into the list
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = text;
						}
						else if (page->type() == "INT") {
							entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->ival);
						}
						else {
							entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);
							if (groupbyList.find(totalList[k]) != groupbyList.end())
								entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->dval);
						}

						if (i == 0 && j == 0)
							entries->attrnames.push_back(page->attribute());
					}
					else
						satisfied = false;
				}
				else { // column in groupby list but not select list
					int page_num = groupbyList[totalList[k]]->attribute->page_order[i];
					if (!buffer->iscached(page_num)) {
						buffer->fetch(page_num);
					}
					Page* page = buffer->get(page_num);

					string target = page_cond->content[j]->sval;
					if (ConditionMet(op, target, val)) {
						if (page->type() == "TEXT") {
							string text(page->content[j]->sval);
							entry->attributeGroupby[page->attribute()] = text;
						}
						else if (page->type() == "INT")
							entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->ival);
						else
							entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->dval);
					}
				}
			}
			if (satisfied) {
				entries->item.push_back(entry);
			}
		}
	}
	// for (size_t i = 0; i < entries.size(); i++) {
	// 	entries[i]->print();
	// 	if (i < entries.size() - 1)
	// 		PrintLineInner(BOXWIDTH, cols);
	// }
}
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

	QueryResult* entries = new QueryResult();

	// cout << "check group" << endl;
	// group by list
	bool groupby;
	if (select->groupBy != nullptr) {
		groupby = true;
		// cout << "Group by found" << endl;
	}
	else {
		groupby = false;
		// cout << "No group by found" << endl;
	}
	unordered_map<string, Column*> groupbyList;
	unordered_map<string, Column*> selectList;
	vector<string> totalList;
	vector<string> selectOrder;

	// create groupby list
	if (groupby) {
		for (auto column : *select->groupBy->columns) {
			if (column->type == hsql::kExprStar) { // there is a star in group by
				for (size_t i = 0; i < fromTable->attr_order.size() - 1; i++) {
					Column* col_group = new Column(fromTable->attributes[fromTable->attr_order[i+1]], fromTable->attr_order[i+1]);
					groupbyList[fromTable->attr_order[i+1]] = col_group;
					totalList.push_back(fromTable->attr_order[i+1]);
				}
				break;
			}
			else {
				string col(column->name);

				// if there is no such attribute in given table
				if (fromTable->attributes.find(col) == fromTable->attributes.end()) {
					cout << "Error: table <" << TableName(t_name) << "> only has attributes: " << endl;
					for (string a : fromTable->attr_order) {
						cout << '<' << a << '>' << " ";
					}
					cout << endl;
					return;
				}
				else {
					Column* col_group = new Column(fromTable->attributes[col], col);
					groupbyList[col] = col_group;
					totalList.push_back(col);
				}
			}
		}
	}

	// create select list
	for (auto column : *select->selectList) {
		if (column->type == hsql::kExprStar) {
			// add every column to the select list
			for (size_t i = 0; i < fromTable->attr_order.size() - 1; i++) {
				Column* col_select = new Column(fromTable->attributes[fromTable->attr_order[i+1]], fromTable->attr_order[i+1]);
				selectList[fromTable->attr_order[i+1]] = col_select;
				totalList.push_back(fromTable->attr_order[i+1]);
				selectOrder.push_back(fromTable->attr_order[i+1]);
			}
			break;
		}
		else {
			if (column->type == hsql::kExprFunctionRef) { // aggregated
				string func(column->name);

				for (auto expr : *column->exprList) {
					string col(expr->name);
					Column* col_select = new Column(fromTable->attributes[col], col);
					if (func == "AVG") {
						col_select->type = aFuncAVG;
					}
					else if (func == "COUNT") {
						col_select->type = aFuncCOUNT;
					}
					else if (func == "MIN") {
						col_select->type = aFuncMIN;
					}
					else if (func == "MAX") {
						col_select->type = aFuncMAX;
					}
					else if (func == "SUM") {
						col_select->type = aFuncSUM;
					}
					else if (func == "NA") {
						col_select->type = aFuncNA;
					}
					else{
						cerr << "Error: Unsupported aggregation function \"" << func << "\"." << endl;
						return; 
					}
					selectList[col] = col_select;
					totalList.push_back(col);
					selectOrder.push_back(col);
				}
			}
			else { // regular column
				string col(column->name);

				if (fromTable->attributes.find(col) == fromTable->attributes.end()) {
					cout << "Error: table <" << TableName(t_name) << "> only has attributes: " << endl;
					for (string a : fromTable->attr_order) {
						cout << '<' << a << '>' << " ";
					}
					cout << endl;
					return;
				}
				Column* col_select = new Column(fromTable->attributes[col], col);
				selectList[col] = col_select;
				totalList.push_back(col);
				selectOrder.push_back(col);
			}

		}
	}

	size_t num_pages = selectList[selectOrder[0]]->attribute->pages.size();
	size_t cols = totalList.size();

	// filter first
	if (select->whereClause) {
		// parse operand on the left side
		string condition_left(select->whereClause->expr->name);
		Attribute* condition_col = fromTable->attributes[condition_left];

		// parse the operand on the right side
		if (select->whereClause->expr2->type == hsql::kExprLiteralFloat) {
			double condition_right = select->whereClause->expr2->fval;

			filter(entries, select->whereClause->opType, condition_right, 
				   condition_col, selectList, selectOrder, groupbyList, totalList);
		}
		else if (select->whereClause->expr2->type == hsql::kExprLiteralInt) {
			int condition_right = select->whereClause->expr2->ival;

			filter(entries, select->whereClause->opType, condition_right, 
				   condition_col, selectList, selectOrder, groupbyList, totalList);
			// PrintLine(BOXWIDTH, cols);
		}
		else if (select->whereClause->expr2->type == hsql::kExprLiteralString) {
			string condition_right(select->whereClause->expr2->name);
			filter(entries, select->whereClause->opType, condition_right, 
				   condition_col, selectList, selectOrder, groupbyList, totalList);
		}
		else {
			cerr << "Error: Unsupport condition." << endl;
			return;
		}
	}
	else {
		for (size_t i = 0; i < num_pages; i++) {
			int page_num = selectList[selectOrder[0]]->attribute->page_order[i];
			if (!buffer->iscached(page_num)) {
				buffer->fetch(page_num);
			}
			Page* p = buffer->get(page_num);
			size_t num_tuples = p->size();

			for (size_t j = 0; j < num_tuples; j++) {
				Entry* entry = new Entry(BOXWIDTH);

				for (size_t k = 0; k < cols; k++) {
					if (selectList.find(totalList[k]) != selectList.end()) {
						int page_num = selectList[totalList[k]]->attribute->page_order[i];
						if (!buffer->iscached(page_num)) {
							buffer->fetch(page_num);
						}
						Page* page = buffer->get(page_num);

						if (page->type() == "TEXT") {
							string text(page->content[j]->sval);
							entry->attributeList[page->attribute()] = text;
							if (groupbyList.find(totalList[k]) != groupbyList.end()) {
								entry->attributeGroupby[page->attribute()] = text;
							}
						}
						else if (page->type() == "INT") {
							entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							if (groupbyList.find(totalList[k]) != groupbyList.end()) {
								entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->ival);
							}
						}
						else {
							entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);
							if (groupbyList.find(totalList[k]) != groupbyList.end()) {
								entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->dval);
							}
						}
						if (i == 0 && j == 0) {
							entries->attrnames.push_back(page->attribute());
						}
					}
					else if (groupbyList.find(totalList[k]) != groupbyList.end()) {
						int page_num = groupbyList[totalList[k]]->attribute->page_order[i];
						if (!buffer->iscached(page_num)) {
							buffer->fetch(page_num);
						}
						Page* page = buffer->get(page_num);

						if (page->type() == "TEXT") {
							string text(page->content[j]->sval);
							entry->attributeGroupby[page->attribute()] = text;
						}
						else if (page->type() == "INT")
							entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->ival);
						else
							entry->attributeGroupby[page->attribute()] = to_string(page->content[j]->dval);

						// if (i == 0 && j == 0) {
						// 	entries->attrnames.push_back(page->attribute());
						// }
					}
					else {
						cerr << "I don't think this is possible." << endl;
						return;
					}
				}
			}
		}
	}

	// group
	set<string> glist;

	// cols in the selectList but not groupbyList
	// they need to be processed by aggregation functions
	vector<string> cols_aggregated;

	if (groupby) {
		// reorder the columns, put groupby columns together
		vector<string> neworder;
		for (auto it = groupbyList.begin(); it != groupbyList.end(); it++) {
			glist.insert(it->first);
			neworder.push_back(it->first);
		}

		for (auto col : selectOrder) {
			if (glist.find(col) == glist.end()) {
				neworder.push_back(col);
				cols_aggregated.push_back(col);
			}
		}

		entries->attrnames = neworder;

		// group entries by given keys
		for (Entry* entry : entries->item) {
			vector<string> groupkey;
			vector<string> groupval;
			for (string attr : entries->attrnames) {
				// cout << "attr is: " << attr;
				if (glist.find(attr) != glist.end()) {
					// cout << " in groupby!" << endl;
					groupkey.push_back(entry->attributeGroupby[attr]);
					// cout << "pushed: " << entry->attributeGroupby[attr] << endl;
				}
				else {
					// cout << " not in groupby!" << endl;
					groupval.push_back(entry->attributeList[attr]);
					// cout << "pushed: " << entry->attributeList[attr] << endl;
				}
			}
			string key = vtos(groupkey);
			// cout << "Key is: " << key << endl;

			if (entries->groups.find(key) == entries->groups.end()) { // if this is a new key
				// cout << "Key is new: " << key << endl;
				//store it in entries
				entries->groups[key] = make_pair(groupval, 1);
				for (size_t i = 0; i < cols_aggregated.size(); i++) {
					if (selectList[cols_aggregated[i]]->type == aFuncCOUNT)
						entries->groups[key].first[i] = "1";
				}
			}
			else { // update the entry according to the aggregate
				// cout << "Key is old: " << key << endl;
				for (size_t i = 0; i < cols_aggregated.size(); i++) {
					entries->groups[key].second++;
					if (selectList[cols_aggregated[i]]->attribute->type() == "INT") {
						if (selectList[cols_aggregated[i]]->type == aFuncAVG) {
							int temp = stoi(entries->groups[key].first[i]) * (entries->groups[key].second - 1);
							temp += stoi(groupval[i]);
							temp /= entries->groups[key].second;
							entries->groups[key].first[i] = to_string(temp);

						}
						else if (selectList[cols_aggregated[i]]->type == aFuncCOUNT) {
							entries->groups[key].first[i] = to_string(entries->groups[key].second);
						}
						else if (selectList[cols_aggregated[i]]->type == aFuncMIN) {
							entries->groups[key].first[i] = to_string(min(stoi(entries->groups[key].first[i]), stoi(groupval[i])));
						}
						else if (selectList[cols_aggregated[i]]->type == aFuncMAX) {
							entries->groups[key].first[i] = to_string(max(stoi(entries->groups[key].first[i]), stoi(groupval[i])));
						}
						else if (selectList[cols_aggregated[i]]->type == aFuncSUM) {
							int temp = stoi(entries->groups[key].first[i]) + stoi(groupval[i]);
							entries->groups[key].first[i] = to_string(temp);
						}
						else if (selectList[cols_aggregated[i]]->type == aFuncNA) {
							cerr << "Error: Aggregation function not designated to this column." << endl;
							return;
						}
						else {
							cerr << "Error: Unsupported aggregation functions." << endl;
							return;
						}
					}
					else if (selectList[cols_aggregated[i]]->attribute->type() == "DOUBLE") {
						if (selectList[cols_aggregated[i]]->type == aFuncAVG) {
							int temp = stod(entries->groups[key].first[i]) * (entries->groups[key].second - 1);
							temp += stod(groupval[i]);
							temp /= entries->groups[key].second;
							entries->groups[key].first[i] = to_string(temp);

						}
						else if (selectList[cols_aggregated[i]]->type == aFuncCOUNT) {
							entries->groups[key].first[i] = to_string(entries->groups[key].second);
						}
						else if (selectList[cols_aggregated[i]]->type == aFuncMIN) {
							entries->groups[key].first[i] = to_string(min(stod(entries->groups[key].first[i]), stod(groupval[i])));
						}
						else if (selectList[cols_aggregated[i]]->type == aFuncMAX) {
							entries->groups[key].first[i] = to_string(max(stod(entries->groups[key].first[i]), stod(groupval[i])));
						}
						else if (selectList[cols_aggregated[i]]->type == aFuncSUM) {
							int temp = stod(entries->groups[key].first[i]) + stod(groupval[i]);
							entries->groups[key].first[i] = to_string(temp);
						}
						else if (selectList[cols_aggregated[i]]->type == aFuncNA) {
							cerr << "Error: Aggregation function not designated to this column." << endl;
							return;
						}
						else {
							cerr << "Error: Unsupported aggregation functions." << endl;
							return;
						}
					}
					else if (selectList[cols_aggregated[i]]->attribute->type() == "TEXT") {
						if (selectList[cols_aggregated[i]]->type == aFuncCOUNT) {
							entries->groups[key].first[i] = to_string(entries->groups[key].second);
						}
						else {
							cerr << "Error: Unsupported aggregation function for TEXT data type." << endl;
							return;
						}
					}
					else {
						cerr << "Error: Unsupported data type." << endl;
						return;
					}
				}
			}
		}
	for (auto it : entries->groups) {
			cout << "Key is: " << it.first << endl;
			cout << "Values are: ";
			for (string val : it.second.first) {
				cout << val << " ";
			}
			cout << endl << endl;
		}

		// print group here
		// for (auto it = entries->groups.begin(); it != entries->groups.end(); it++) {
		// 	cout << "Key is: " << it->first << endl;
		// 	cout << "Value is: " << endl;
		// 	for (auto i : it->second) {
		// 		for (auto itj = i.begin(); itj != i.end(); itj++) {
		// 			cout << *itj << " ";
		// 		}
		// 		cout << endl;
		// 	}
		// }
	}
}