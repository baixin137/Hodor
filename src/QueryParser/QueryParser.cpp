#include "QueryParser.h"

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
	// cout << "insert!" << endl;
	const hsql::InsertStatement* insert = (const hsql::InsertStatement*) statement;
	string table(insert->tableName);
	table = filesystem->user->name() + "::" + table;
	// cout << "Ready to insert to table: " << table << endl;

	if (filesystem->tables.find(table) == filesystem->tables.end()) {
		cerr << "Error: Trying to insert to non-existing table" << endl;
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

	// create table
	Table* fromTable = JoinTable(select->fromTable, select);
	if (!fromTable) {
		cerr << "Error: Table join failed." << endl;
		return;
	}

	string t_name = fromTable->name();

	// if (select->fromTable->type == hsql::kTableName) {
	// 	string t_name(select->fromTable->name);
	// 	t_name = filesystem->user->name() + "::" + t_name;
	// 	// cout << "table name is: " << t_name << endl;
	// 	fromTable = filesystem->tables[t_name];
	// }
	// else if (select->fromTable->type == hsql::kTableJoin) {
	// 	// do join
	// 	fromTable = JoinTable(select->fromTable);
	// }

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
	unordered_map<string, string> attributeOrder;

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
				if (groupbyList.find(fromTable->attr_order[i+1]) == groupbyList.end())
					totalList.push_back(fromTable->attr_order[i+1]);
				selectOrder.push_back(fromTable->attr_order[i+1]);
				attributeOrder[fromTable->attr_order[i+1]] = "";

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
						attributeOrder[col] = "AVG";
					}
					else if (func == "COUNT") {
						col_select->type = aFuncCOUNT;
						attributeOrder[col] = "COUNT";
					}
					else if (func == "MIN") {
						col_select->type = aFuncMIN;
						attributeOrder[col] = "MIN";
					}
					else if (func == "MAX") {
						col_select->type = aFuncMAX;
						attributeOrder[col] = "MAX";
					}
					else if (func == "SUM") {
						col_select->type = aFuncSUM;
						attributeOrder[col] = "SUM";
					}
					else if (func == "NA") {
						col_select->type = aFuncNA;
					}
					else{
						cerr << "Error: Unsupported aggregation function \"" << func << "\"." << endl;
						return; 
					}
					selectList[col] = col_select;
					selectOrder.push_back(col);
					if (groupbyList.find(col) != groupbyList.end()) {
						groupbyList[col] = col_select;
					}
					else {
						totalList.push_back(col);
					}
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
				if (groupbyList.find(col) == groupbyList.end())
					totalList.push_back(col);
				selectOrder.push_back(col);
			}

		}
	}

	size_t num_pages = selectList[selectOrder[0]]->attribute->pages.size();
	size_t cols = totalList.size();

	if (select->whereClause) {
		// parse operand on the left side
		// string condition_left(select->whereClause->expr->name)
		hsql::Expr* left = select->whereClause->expr;
		hsql::Expr* right = select->whereClause->expr2;

		vector<string> ConditionLeftList ;
		vector<string> ConditionRightList;
		string left_type  = ConditionType(ConditionLeftList , fromTable, left );
		string right_type = ConditionType(ConditionRightList, fromTable, right);

		if (left_type != right_type) {
			if (left_type == "DOUBLE" || right_type == "DOUBLE") 
				left_type = "DOUBLE";
			else { // one is text and the other is not
				cerr << "Error: Invalid comparison between text and numerical." << endl;
				return;
			}
		}
		if (left_type == "TEXT") {
			string condition_left(select->whereClause->expr->name);
			Attribute* condition_col = fromTable->attributes[condition_left];

			string condition_right(select->whereClause->expr2->name);
			filter(entries, select->whereClause->opType, condition_right,
				   condition_col, selectList, selectOrder, groupbyList, totalList);
		}
		else
			filter(entries, left, right, left_type, select->whereClause->opType, selectList, fromTable, 
			   selectOrder, groupbyList, totalList, ConditionLeftList, ConditionRightList);
	}
	else {
		for (size_t i = 0; i < num_pages; i++) {
			int page_num = selectList[selectOrder[0]]->attribute->page_order[i];
			if (!buffer->iscached(page_num)) {
				cout << "fetching page " << page_num << " from disk!" << endl;
				buffer->fetch(page_num);
			}
			Page* p = buffer->get(page_num);
			size_t num_tuples = p->size();

			for (size_t j = 0; j < num_tuples; j++) {
				Entry* entry = new Entry(BOXWIDTH);

				for (size_t k = 0; k < cols; k++) {
					// cout << "iterating total list: " << totalList[k] << endl;
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
					
						entries->AddAttribute(page->attribute());
					}
					else if (groupbyList.find(totalList[k]) != groupbyList.end()) {
						// cout << "please tell me you found something" << endl;
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
					}
					else {
						cerr << "I don't think this is possible." << endl;
						return;
					}
				}
				entries->item.push_back(entry);
				// cout << "entry pushed: " << endl;
				// for (auto it = entry->attributeList.begin(); it != entry->attributeList.end(); it++) {
				// 	cout << it->second << " ";
				// }
				// cout << endl;
			}
		}
	}

	// group
	set<string> glist;

	// cols in the selectList but not groupbyList
	// they need to be processed by aggregation functions
	// vector<string> neworder;

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
				// neworder.push_back(col);
			}
		}

		entries->attrnames = neworder;

		// group entries by given keys
		for (Entry* entry : entries->item) {
			vector<string> groupkey;
			vector<string> groupval;
			for (string attr : totalList) {
				// cout << "attr is: " << attr;
				if (glist.find(attr) != glist.end()) {
					// cout << " in groupby!" << endl;
					groupkey.push_back(entry->attributeGroupby[attr]);
				}
			}
			string key = vtos(groupkey);

			for (string attr : selectOrder) {
				groupval.push_back(entry->attributeList[attr]);
			}
			// cout << "Key is: " << key << endl;

			if (entries->groups.find(key) == entries->groups.end()) { // if this is a new key
				// cout << "Key is new: " << key << endl;
				//store it in entries
				entries->groups[key] = make_pair(groupval, 1);
				for (size_t i = 0; i < selectOrder.size(); i++) {
					if (selectList[selectOrder[i]]->type == aFuncCOUNT) {
						entries->groups[key].first[i] = "1";
					}
				}
			}
			else { // update the entry according to the aggregate
				entries->groups[key].second++;
				for (size_t i = 0; i < selectOrder.size(); i++) {
					if (selectList.find(selectOrder[i]) != selectList.end()) {

						if (selectList[selectOrder[i]]->attribute->type() == "INT") {
							if (selectList[selectOrder[i]]->type == aFuncAVG) {
								int temp = stoi(entries->groups[key].first[i]) * (entries->groups[key].second - 1);
								temp += stoi(groupval[i]);
								temp /= entries->groups[key].second;
								entries->groups[key].first[i] = to_string(temp);

							}
							else if (selectList[selectOrder[i]]->type == aFuncCOUNT) {
								entries->groups[key].first[i] = to_string(entries->groups[key].second);
							}
							else if (selectList[selectOrder[i]]->type == aFuncMIN) {
								entries->groups[key].first[i] = to_string(min(stoi(entries->groups[key].first[i]), stoi(groupval[i])));
							}
							else if (selectList[selectOrder[i]]->type == aFuncMAX) {
								entries->groups[key].first[i] = to_string(max(stoi(entries->groups[key].first[i]), stoi(groupval[i])));
							}
							else if (selectList[selectOrder[i]]->type == aFuncSUM) {
								int temp = stoi(entries->groups[key].first[i]) + stoi(groupval[i]);
								entries->groups[key].first[i] = to_string(temp);
							}
							else if (selectList[selectOrder[i]]->type == aFuncNA) {
								// don't need to do anything
							}
							else {
								cerr << "Error: Unsupported aggregation functions." << endl;
								return;
							}
						}
						else if (selectList[selectOrder[i]]->attribute->type() == "DOUBLE") {
							if (selectList[selectOrder[i]]->type == aFuncAVG) {
								double temp = stod(entries->groups[key].first[i]) * (entries->groups[key].second - 1);
								temp += stod(groupval[i]);
								temp /= entries->groups[key].second;
								entries->groups[key].first[i] = to_string(temp);

							}
							else if (selectList[selectOrder[i]]->type == aFuncCOUNT) {
								entries->groups[key].first[i] = to_string(entries->groups[key].second);
							}
							else if (selectList[selectOrder[i]]->type == aFuncMIN) {
								entries->groups[key].first[i] = to_string(min(stod(entries->groups[key].first[i]), stod(groupval[i])));
							}
							else if (selectList[selectOrder[i]]->type == aFuncMAX) {
								entries->groups[key].first[i] = to_string(max(stod(entries->groups[key].first[i]), stod(groupval[i])));
							}
							else if (selectList[selectOrder[i]]->type == aFuncSUM) {
								double temp = stod(entries->groups[key].first[i]) + stod(groupval[i]);
								entries->groups[key].first[i] = to_string(temp);
							}
							else if (selectList[selectOrder[i]]->type == aFuncNA) {
								// don't need to do anything
							}
							else {
								cerr << "Error: Unsupported aggregation functions." << endl;
								return;
							}
						}
						else if (selectList[selectOrder[i]]->attribute->type() == "TEXT") {
							if (selectList[selectOrder[i]]->type == aFuncCOUNT) {
								entries->groups[key].first[i] = to_string(entries->groups[key].second);
							}
							else {
								// don't need to do anything
							}
						}
						else {
							cerr << "Error: Unsupported data type." << endl;
							return;
						}
					}
				}
			}
		}
		// print it
		vector<string> entryorder;
		for (auto it = entries->groups.begin(); it != entries->groups.end(); it++) {
			entryorder.push_back(it->first);
		}
		sort(entryorder.begin(), entryorder.end());

		entries->PrintGroup(entryorder, selectOrder, attributeOrder);
	}
	else { // no group by clause
		entries->PrintAll(selectOrder);
	}
}