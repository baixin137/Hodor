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

template<typename T> void QueryResult::PrintElement(T t, const int& width) {
	cout << left << setw(width) << setfill(' ') << t << '|';
}

void QueryResult::PrintGroup(vector<string>& order, vector<string>& attrorder, unordered_map<string, string>& attributes) {
	size_t size = order.size();
	PrintLine(BOXWIDTH, attrorder.size());
	cout << '|';
	for (string name : attrorder) {
		if (attributes[name] == "")
			PrintElement(name, BOXWIDTH);
		else 
			PrintElement(name + "_" + attributes[name], BOXWIDTH);
	}
	cout << endl;

	if (size == 0) {
		PrintLine(BOXWIDTH, attrorder.size());
		return;
	}

	PrintLineInner(BOXWIDTH, attrorder.size());
	for (size_t i = 0; i < order.size(); i++) {
		cout << '|';
		for (size_t j = 0; j < attrorder.size(); j++) {
			PrintElement(groups[order[i]].first[j], BOXWIDTH);
		}
		cout << endl;
		if (i != order.size() - 1)
			PrintLineInner(BOXWIDTH, attrorder.size());
	}
	PrintLine(BOXWIDTH, attrorder.size());
}

void QueryResult::PrintAll(vector<string>& attrs) {
	size_t size = item.size();
	PrintLine(BOXWIDTH, attrs.size());

	cout << '|';
	for (string name : attrs) {
			PrintElement(name, BOXWIDTH);
			// cout << "name is:" << endl;
	}
	cout << endl;

	if (size == 0) {
		PrintLine(BOXWIDTH, attrs.size());
		return;
	}

	PrintLineInner(BOXWIDTH, attrs.size());

	for (size_t i = 0; i < item.size(); i++) {
		Entry* entry = item[i];
		cout << '|';
		for (string attr : attrs) {
			PrintElement(entry->attributeList[attr], BOXWIDTH);
			// cout << "entry key is: " << attr << endl;
			// cout << "entry val is: " << entry->attributeList[attr] << endl;
		}
		cout << endl;
		if (i != item.size() - 1)
			PrintLineInner(BOXWIDTH, attrs.size());
	}
	PrintLine(BOXWIDTH, attrs.size());
}

void QueryResult::AddAttribute(string attr) {
	if (attrset.find(attr) == attrset.end()) {
		attrnames.push_back(attr);
		attrset.insert(attr);
	}
}

Attribute* QueryParser::FindAttribute(hsql::Expr* expr) {
	string tablename = filesystem->user->name() + "::" + string(expr->table);
	Table* table = filesystem->user->tables[tablename];
	string col = string(expr->name);

	Attribute* attr = table->attributes[col];
	return attr;
}

string QueryParser::ConditionType(vector<string>& cols, Table* table, hsql::Expr* expr) {
	string type;
	if (expr->type && expr->type == hsql::kExprLiteralString) {
		return "TEXT";
	}
	else if (expr->name) {
		// cout << "should enter here" << endl;
		string name(expr->name);
		cols.push_back(name);
		type = table->attributes[name]->type();
		return type;
	}
	else if (expr->ival) {
		return "INT";
	}
	else if (expr->fval) {
		return "DOUBLE";
	}

	string left;
	string right;

	try {
		left = ConditionType(cols, table, expr->expr);
		right = ConditionType(cols, table, expr->expr2);
	} catch(string e) {
		cout << e << endl;
	}


	if (left == right) return left;
	else {
		if ((left == "TEXT" || right == "TEXT") && left != right) {
			throw string("Invalid operation between TEXT and INT/DOUBLE");
		}
		else if (left == "DOUBLE" || right == "DOUBLE") {
			return "DOUBLE";
		}
		else return "INT";
	}
}

bool QueryParser::ConditionMet(hsql::OperatorType op, int target, int condition) {
	if (op == hsql::kOpEquals) {
		if (target == condition) return true;
	}
	else if (op == hsql::kOpNotEquals) {
		if (target != condition) return true;
	}
	else if (op == hsql::kOpLess) {
		if (target > condition) return true;
	}
	else if (op == hsql::kOpLessEq) {
		if (target >= condition) return true;
	}
	else if (op == hsql::kOpGreater) {
		if (target < condition) return true;
	}
	else if (op == hsql::kOpGreaterEq) {
		if (target <= condition) return true;
	}
	else {
		cerr << "Error: Unsupported operation." << endl;
	}

	return false;
}

bool QueryParser::ConditionMet(hsql::OperatorType op, double target, double condition) {
	if (op == hsql::kOpEquals) {
		if (abs(target - condition) < EPSILON) return true;
	}
	else if (op == hsql::kOpNotEquals) {
		if (abs(target - condition) > EPSILON) return true;
	}
	else if (op == hsql::kOpLess) {
		if (condition - target < EPSILON) return true;
	}
	else if (op == hsql::kOpLessEq) {
		if (condition - target <= EPSILON) return true;
	}
	else if (op == hsql::kOpGreater) {
		if (target - condition < EPSILON) return true;
	}
	else if (op == hsql::kOpGreaterEq) {
		if (target - condition <= EPSILON) return true;
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

int QueryParser::ParseExprINT(unordered_map<string, int>& map, hsql::Expr* expr) {
	if (expr->name) {
		string col(expr->name);
		return map[col];
	}
	else if (expr->ival) {
		return expr->ival;
	}

	if (expr->opType == hsql::kOpPlus) {
		return ParseExprINT(map, expr->expr) + ParseExprINT(map, expr->expr2);
	}
	else if (expr->opType == hsql::kOpMinus) {
		return ParseExprINT(map, expr->expr) - ParseExprINT(map, expr->expr2);
	}
	else if (expr->opType == hsql::kOpAsterisk) {
		return ParseExprINT(map, expr->expr) * ParseExprINT(map, expr->expr2);
	}
	else if (expr->opType == hsql::kOpSlash) {
		return ParseExprINT(map, expr->expr) / ParseExprINT(map, expr->expr2);
	}
	else if (expr->opType == hsql::kOpPercentage) {
		return ParseExprINT(map, expr->expr) % ParseExprINT(map, expr->expr2);
	}
	else {
		cerr << "Error: Unsupported Operator." << endl;
		return 0;
	}
}

double QueryParser::ParseExprDOUBLE(unordered_map<string, double>& map, hsql::Expr* expr) {
	if (expr->name) {
		string col(expr->name);
		return map[col];
	}
	else if (expr->ival) {
		return double(expr->ival);
	}
	else if (expr->fval) {
		return expr->fval;
	}

	if (expr->opType == hsql::kOpPlus) {
		// double a = ParseExprDOUBLE(map, expr->expr);
		// double b = ParseExprDOUBLE(map, expr->expr2);
		// cout << "a: " << a << ", b: " << b << endl;
		// return a + b;
		return ParseExprDOUBLE(map, expr->expr) + ParseExprDOUBLE(map, expr->expr2);
	}
	else if (expr->opType == hsql::kOpMinus) {
		return ParseExprDOUBLE(map, expr->expr) - ParseExprDOUBLE(map, expr->expr2);
	}
	else if (expr->opType == hsql::kOpAsterisk) {
		return ParseExprDOUBLE(map, expr->expr) * ParseExprDOUBLE(map, expr->expr2);
	}
	else if (expr->opType == hsql::kOpSlash) {
		return ParseExprDOUBLE(map, expr->expr) / ParseExprDOUBLE(map, expr->expr2);
	}
	else {
		cerr << "Error: Unsupported Operator." << endl;
		return 0;
	}
}

void QueryParser::FilterInt(vector<string>& leftList, vector<string>& rightList, Table* table, 
							unordered_map<string, Column*>& selectList, hsql::OperatorType op, 
							vector<string>& totalList, size_t i, size_t j, size_t k, Entry* entry,
							hsql::Expr* left, hsql::Expr* right, unordered_map<string, Column*>& groupbyList,
							QueryResult* entries, bool& satisfied) {
	// a map to store which attribute has what val in this tuple
	unordered_map<string, int> val_con;
	int target;
	int val;
	for (string col : leftList) {
		int page_condnum = table->attributes[col]->page_order[i];
		if (!buffer->iscached(page_condnum))
			buffer->fetch(page_condnum);
		Page* page_cond = buffer->get(page_condnum);
		val_con[col] = page_cond->content[j]->ival;
	}
	for (string col : rightList) {
		int page_condnum = table->attributes[col]->page_order[i];
		if (!buffer->iscached(page_condnum))
			buffer->fetch(page_condnum);
		Page* page_cond = buffer->get(page_condnum);
		val_con[col] = page_cond->content[j]->ival;
	}

	if (selectList.find(totalList[k]) != selectList.end()) {
		int page_num = selectList[totalList[k]]->attribute->page_order[i];
		if (!buffer->iscached(page_num))
			buffer->fetch(page_num);

		Page* page = buffer->get(page_num);

		target = ParseExprINT(val_con, right);
		val    = ParseExprINT(val_con, left );

		entries->AddAttribute(page->attribute());
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
		}
		else
			satisfied = false;
	}
	else { // column in groupby list but not select list
		int page_num = groupbyList[totalList[k]]->attribute->page_order[i];
		if (!buffer->iscached(page_num))
			buffer->fetch(page_num);

		Page* page = buffer->get(page_num);

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

void QueryParser::FilterDouble(vector<string>& leftList, vector<string>& rightList, Table* table, 
							   unordered_map<string, Column*>& selectList, hsql::OperatorType op, 
							   vector<string>& totalList, size_t i, size_t j, size_t k, Entry* entry,
							   hsql::Expr* left, hsql::Expr* right, unordered_map<string, Column*>& groupbyList,
							   QueryResult* entries, bool& satisfied) {
	// a map to store which attribute has what val in this tuple
	unordered_map<string, double> val_con;
	double target;
	double val;
	for (string col : leftList) {
		int page_condnum = table->attributes[col]->page_order[i];
		if (!buffer->iscached(page_condnum))
			buffer->fetch(page_condnum);
		Page* page_cond = buffer->get(page_condnum);
		if (page_cond->type() == "DOUBLE")
			val_con[col] = page_cond->content[j]->dval;
		else
			val_con[col] = page_cond->content[j]->ival;
	}
	for (string col : rightList) {
		int page_condnum = table->attributes[col]->page_order[i];
		if (!buffer->iscached(page_condnum))
			buffer->fetch(page_condnum);
		Page* page_cond = buffer->get(page_condnum);
		if (page_cond->type() == "DOUBLE")
			val_con[col] = page_cond->content[j]->dval;
		else
			val_con[col] = page_cond->content[j]->ival;
	}

	if (selectList.find(totalList[k]) != selectList.end()) {
		int page_num = selectList[totalList[k]]->attribute->page_order[i];
		if (!buffer->iscached(page_num))
			buffer->fetch(page_num);

		Page* page = buffer->get(page_num);

		target = ParseExprDOUBLE(val_con, right);
		val    = ParseExprDOUBLE(val_con, left );

		entries->AddAttribute(page->attribute());
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
		}
		else
			satisfied = false;
	}
	else { // column in groupby list but not select list
		int page_num = groupbyList[totalList[k]]->attribute->page_order[i];
		if (!buffer->iscached(page_num))
			buffer->fetch(page_num);

		Page* page = buffer->get(page_num);

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

void QueryParser::filter(QueryResult* entries, hsql::Expr* left, hsql::Expr* right, string type, 
						 hsql::OperatorType op, unordered_map<string, Column*>& selectList, Table* table, 
						 vector<string>& selectOrder, unordered_map<string, Column*>& groupbyList, 
						 vector<string>& totalList, vector<string>& leftList, vector<string>& rightList) {
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
				if (type == "INT") {
					FilterInt(leftList, rightList, table, selectList, op, totalList, i, j, k, entry,
							  left, right, groupbyList, entries, satisfied);
				}
				else {
					FilterDouble(leftList, rightList, table, selectList, op, totalList, i, j, k, entry,
								 left, right, groupbyList, entries, satisfied);
				}
			}
			if (satisfied) {
				entries->item.push_back(entry);
			}
		}
	}
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
					entries->AddAttribute(page->attribute());

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
}

void QueryParser::AddtoJoinedTable(size_t i, size_t j, size_t jj, vector<Attribute*>& attr_left, vector<Attribute*>& attr_right,
					  			   size_t cols_left, size_t cols_right, string tname, vector<string>& selectList) {
	vector<string> elements;
	for (string col : selectList) {
		for (size_t k = 0; k < cols_left; k++) {
			if (attr_left[k]->name() == col) {
				size_t pnum_new = attr_left[k]->page_order[i];
				if (!buffer->iscached(pnum_new))
					buffer->fetch(pnum_new);
				Page* pnew = buffer->get(pnum_new);
				if (pnew->type() == "TEXT")
					elements.push_back("'" + pnew->content[j]->sval + "'");
				else if (pnew->type() == "DOUBLE") {
					double val = pnew->content[j]->dval;
					elements.push_back(to_string(val));
				}
				else {
					int val = pnew->content[j]->ival;
					elements.push_back(to_string(val));
				}
				break;
			}
		}
		for (size_t kk = 0; kk < cols_right; kk++) {
			if (attr_right[kk]->name() == col) {
				size_t pnum_new = attr_right[kk]->page_order[i];
				if (!buffer->iscached(pnum_new))
					buffer->fetch(pnum_new);
				Page* pnew = buffer->get(pnum_new);
				if (pnew->type() == "TEXT")
					elements.push_back("'" + pnew->content[jj]->sval + "'");
				else if (pnew->type() == "DOUBLE") {
					double val = pnew->content[jj]->dval;
					elements.push_back(to_string(val));
				}
				else {
					int val = pnew->content[jj]->ival;
					elements.push_back(to_string(val));
				}
				break;
			}
		}
	}
	string head = "INSERT INTO " + TableName(tname) + " VALUES (";
	for (size_t i = 0; i < elements.size(); i++) {
		if (i != elements.size() - 1)
			head += (elements[i] + ", ");
		else 
			head += (elements[i] + ")");
	}
	// cout << "command is: " << head << endl;

	hsql::SQLParserResult result;
	hsql::SQLParser::parse(head, &result);
	const hsql::SQLStatement* statement = result.getStatement(0);
	ParseINSERT(statement);
}

Table* QueryParser::JoinTable(hsql::TableRef* fromTable, const hsql::SelectStatement* select) {
	if (fromTable->type == hsql::kTableName) {
		// return the content of the table
		string tname(fromTable->name);
		string tablename = filesystem->user->name() + "::" + tname;
		if (filesystem->tables.find(tablename) == filesystem->tables.end()) {
			cerr << "Error: Cannot find given table name \"" << tname << "\" in user \"" << filesystem->user->name() << "\"." << endl;
			return nullptr;
		}
		return filesystem->tables[tablename];
	}

	Table* left = JoinTable(fromTable->join->left, select);
	Table* right = JoinTable(fromTable->join->right, select);

	Table* JoinedTable = nullptr;

	if (fromTable->join->type == hsql::kJoinInner) {
		JoinedTable = InnerJoin(left, right, fromTable->join->condition, select);
	}
	// else if (join->type == hsql::kJoinFull) {
	// 	JoinedTable = FullJoin(left, right, fromTable->join->condition, select);
	// }
	// else if (join->type == hsql::kJoinLeft) {
	// 	JoinedTable = LeftJoin(left, right, fromTable->join->condition, select);
	// }
	// else if (join->type == hsql::kJoinRight) {
	// 	JoinedTable = LeftJoin(right, left, fromTable->join->condition, select);
	// }
	// else if (join->type == hsql::kJoinCross) {
	// 	JoinedTable = CrossJoin(left, right, fromTable->join->condition, select);
	// }
	// else if (join->type == hsql::kJoinNatural) {
	// 	JoinedTable = NaturalJoin(left, right, fromTable->join->condition, select);
	// }
	else {
		cerr << "Error: Unknown JOIN type." << endl;
		return nullptr;
	}

	return JoinedTable;
}

QueryParser::QueryParser(FileManager* fs, BufferManager* bf) {
	filesystem = fs;
	buffer = bf;
}

template <typename T> void QueryParser::PrintElement(T elem, const int& width) {
	cout << left << setw(width) << setfill(' ') << elem << '|';
}