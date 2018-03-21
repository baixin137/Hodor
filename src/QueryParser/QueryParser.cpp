#include "QueryParser.h"

Entry::Entry(size_t w) {
	width = w;
}

size_t Entry::size() {
	return width;
}

// void Entry::print() {
// 	cout << '|';
// 	for (string item : attributeList)
// 		cout << left << setw(width) << setfill(' ') << item << '|';
// 	cout << endl;
// }

QueryResult::QueryResult() {}

template <class T> void QueryParser::filter(QueryResult* entries, hsql::OperatorType op, T val, Attribute* attr, vector<Attribute*> selectList) {
	size_t num_pages = selectList[0]->pages.size();
	size_t cols = selectList.size();

	for (size_t i = 0; i < num_pages; i++) {
		int page_num = selectList[0]->page_order[i];
		if (!buffer->iscached(page_num)) {
			buffer->fetch(page_num);
		}
		Page* p = buffer->get(page_num);
		size_t num_tuples = p->size();

		for (size_t j = 0; j < num_tuples; j++) {
			Entry* entry = new Entry(BOXWIDTH);
			bool satisfied = true;

			for (size_t k = 0; k < cols; k++) {
				int page_condnum = attr->page_order[i];
				if (!buffer->iscached(page_condnum)) {
					buffer->fetch(page_condnum);
				}
				Page* page_cond = buffer->get(page_condnum);

				int page_num = selectList[k]->page_order[i];
				if (!buffer->iscached(page_num)) {
					buffer->fetch(page_num);
				}
				Page* page = buffer->get(page_num);
				if (op == hsql::kOpEquals) {
					if (page_cond->type() == "INT") {
						if (page_cond->content[j]->ival == val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else if (page_cond->type() == "DOUBLE") {
						if (page_cond->content[j]->dval == val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else {
						cerr << "Error: Invalid condition, text on the right side of operator.";
						return;
					}
				}
				else if (op == hsql::kOpNotEquals) {
					if (page_cond->type() == "INT") {
						if (page_cond->content[j]->ival != val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else if (page_cond->type() == "DOUBLE") {
						if (page_cond->content[j]->dval != val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else {
						cerr << "Error: Invalid condition, text on the right side of operator.";
						return;
					}
				}
				else if (op == hsql::kOpLess) {
					if (page_cond->type() == "INT") {
						if (page_cond->content[j]->ival < val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else if (page_cond->type() == "DOUBLE") {
						if (page_cond->content[j]->dval < val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else {
						cerr << "Error: Invalid condition, text on the right side of operator.";
						return;
					}
				}
				else if (op == hsql::kOpLessEq) {
					if (page_cond->type() == "INT") {
						if (page_cond->content[j]->ival <= val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else if (page_cond->type() == "DOUBLE") {
						if (page_cond->content[j]->dval <= val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else {
						cerr << "Error: Invalid condition, text on the right side of operator.";
						return;
					}
				}
				else if (op == hsql::kOpGreater) {
					if (page_cond->type() == "INT") {
						if (page_cond->content[j]->ival > val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else if (page_cond->type() == "DOUBLE") {
						if (page_cond->content[j]->dval > val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else {
						cerr << "Error: Invalid condition, text on the right side of operator.";
						return;
					}
				}
				else if (op == hsql::kOpGreaterEq) {
					if (page_cond->type() == "INT") {
						if (page_cond->content[j]->ival >= val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else if (page_cond->type() == "DOUBLE") {
						if (page_cond->content[j]->dval >= val) {
							if (page->type() == "TEXT") {
								string text(page->content[j]->sval);
								entry->attributeList[page->attribute()] = text;
							}
							else if (page->type() == "INT")
								entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
							else
								entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

							if (i == 0 && j == 0) {
								entries->attrnames.push_back(page->attribute());
							}
						}
						else satisfied = false;
					}
					else {
						cerr << "Error: Invalid condition, text on the right side of operator.";
						return;
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
	vector<Attribute*> groupbyList;

	if (groupby) {
		for (auto column : *select->groupBy->columns) {
			if (column->type == hsql::kExprStar) {
				for (size_t i = 0; i < fromTable->attr_order.size() - 1; i++)
					groupbyList.push_back(fromTable->attributes[fromTable->attr_order[i+1]]);
				break;
			}
			string col(column->name);

			if (fromTable->attributes.find(col) == fromTable->attributes.end()) {
				cout << "Error: table <" << TableName(t_name) << "> only has attributes: " << endl;
				for (string a : fromTable->attr_order) {
					cout << '<' << a << '>' << " ";
				}
				cout << endl;
				return;
			}
			groupbyList.push_back(fromTable->attributes[col]);
		}
	}

	// select list
	vector<Attribute*> selectList;

	for (auto column : *select->selectList) {
		if (column->type == hsql::kExprStar) {
			for (size_t i = 0; i < fromTable->attr_order.size() - 1; i++)
				selectList.push_back(fromTable->attributes[fromTable->attr_order[i+1]]);
			break;
		}
		string col(column->name);

		if (fromTable->attributes.find(col) == fromTable->attributes.end()) {
			cout << "Error: table <" << TableName(t_name) << "> only has attributes: " << endl;
			for (string a : fromTable->attr_order) {
				cout << '<' << a << '>' << " ";
			}
			cout << endl;
			return;
		}
		selectList.push_back(fromTable->attributes[col]);
	}

	size_t num_pages = selectList[0]->pages.size();
	size_t cols = selectList.size();

	// filter first
	if (select->whereClause) {
		// parse operand on the left side
		string condition_left(select->whereClause->expr->name);
		Attribute* condition_col = fromTable->attributes[condition_left];

		// parse the operand on the right side
		if (select->whereClause->expr2->type == hsql::kExprLiteralFloat) {
			double condition_right = select->whereClause->expr2->fval;

			filter(entries, select->whereClause->opType, condition_right, condition_col, selectList);
		}
		else if (select->whereClause->expr2->type == hsql::kExprLiteralInt) {
			int condition_right = select->whereClause->expr2->ival;

			filter(entries, select->whereClause->opType, condition_right, condition_col, selectList);
			// PrintLine(BOXWIDTH, cols);
		}
		else {
			cerr << "Error: Unsupport condition." << endl;
			return;
		}
	}
	else {
		for (size_t i = 0; i < num_pages; i++) {
			int page_num = selectList[0]->page_order[i];
			if (!buffer->iscached(page_num)) {
				buffer->fetch(page_num);
			}
			Page* p = buffer->get(page_num);
			size_t num_tuples = p->size();

			for (size_t j = 0; j < num_tuples; j++) {
				Entry* entry = new Entry(BOXWIDTH);

				for (size_t k = 0; k < cols; k++) {
					int page_num = selectList[k]->page_order[i];
					if (!buffer->iscached(page_num)) {
						buffer->fetch(page_num);
					}
					Page* page = buffer->get(page_num);

					if (page->type() == "TEXT") {
						string text(page->content[j]->sval);
						entry->attributeList[page->attribute()] = text;
					}
					else if (page->type() == "INT")
						entry->attributeList[page->attribute()] = to_string(page->content[j]->ival);
					else
						entry->attributeList[page->attribute()] = to_string(page->content[j]->dval);

					if (i == 0 && j == 0) {
						entries->attrnames.push_back(page->attribute());
					}
				}
			}
		}
	}

	// group
	if (groupby) {
		// reorder the columns, put groupby columns together
		set<string> glist;
		vector<string> neworder;
		for (auto col : groupbyList) {
			glist.insert(col->name());
			neworder.push_back(col->name());
		}

		for (auto col : selectList) {
			if (glist.find(col->name()) == glist.end()) {
				neworder.push_back(col->name());
			}
		}

		entries->attrnames = neworder;

		for (Entry* entry : entries->item) {
			vector<string> groupkey;
			vector<string> groupval;
			for (string attr : entries->attrnames) {
				if (glist.find(attr) != glist.end())
					groupkey.push_back(entry->attributeList[attr]);
				else
					groupval.push_back(entry->attributeList[attr]);
			}
			entries->groups[vtos(groupkey)].push_back(groupval);
		}
		// print group here
		for (auto it = entries->groups.begin(); it != entries->groups.end(); it++) {
			cout << "Key is: " << it->first << endl;
			cout << "Value is: " << endl;
			for (auto i : it->second) {
				for (auto itj = i.begin(); itj != i.end(); itj++) {
					cout << *itj << " ";
				}
				cout << endl;
			}
		}
	}
}