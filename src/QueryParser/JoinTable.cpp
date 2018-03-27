#include "QueryParser.h"

Table* QueryParser::InnerJoin(  Table* left, Table* right, hsql::Expr* condition, const hsql::SelectStatement* select) {
	// cout << "left attribute is: " << left->name() << endl;
	// cout << "right attribute is: " << right->name() << endl;

	string tablename = "Join" + TableName(left->name()) + TableName(right->name());
	string tname = filesystem->user->name() + "::" + tablename;
	string timestamp = addTimeStamp();
	Table* JoinedTable = new Table(tname, filesystem->user->name(), timestamp, 0, select->selectList->size() + 1);

	// selectList.push_back("TimeStamp");
	vector<string> selectList;
	for (auto attr : *select->selectList) {
		string attrname(attr->name);
		selectList.push_back(attrname);
	}

	// build new table
	Attribute* ts = new Attribute("TimeStamp", "TEXT", tname);
	JoinedTable->attr_order.push_back("TimeStamp");
	JoinedTable->attributes["TimeStamp"] = ts;

	vector<Attribute*> attr_left;
	vector<Attribute*> attr_right;

	for (string attr : selectList) {
		if (left->attributes.find(attr) != left->attributes.end()) {
			JoinedTable->attr_order.push_back(attr);
			Attribute* attr_new = new Attribute(attr, left->attributes[attr]->type(), tname);
			JoinedTable->attributes[attr] = attr_new;
			attr_left.push_back(left->attributes[attr]);
		}
		else if (right->attributes.find(attr) != right->attributes.end()) {
			JoinedTable->attr_order.push_back(attr);
			Attribute* attr_new = new Attribute(attr, right->attributes[attr]->type(), tname);
			JoinedTable->attributes[attr] = attr_new;
			attr_right.push_back(right->attributes[attr]);
		}
		else {
			cerr << "Error: Nonexisting attribute name \"" << attr << "\"." << endl;
			return nullptr;
		}
	}
	// add new table to database
	filesystem->add(JoinedTable);
	filesystem->user->IncrementSize(1);

	// cout << "parse condition" << endl;
	// cout << "cond left: " << string(condition->expr->name) << endl;
	// cout << "cond right: " << string(condition->expr2->name) << endl;
	// parse condition
	Attribute* cond_left = FindAttribute(condition->expr);
	Attribute* cond_right = FindAttribute(condition->expr2);

	// add elements to new table
	// use nested loop to check each piece of entry

	size_t num_pages_left = left->attributes[left->attr_order[0]]->pages.size();
	size_t cols_left = attr_left.size();

	for (size_t i = 0; i < num_pages_left; i++) {
		if (!attr_left[0]->page_order.size()) {
			// empty table
			return nullptr;
		}
		int page_num_left = attr_left[0]->page_order[0];
		if (!buffer->iscached(page_num_left))
			buffer->fetch(page_num_left);

		Page* p_left = buffer->get(page_num_left);
		size_t num_tuples_left = p_left->size();

		for (size_t j = 0; j < num_tuples_left; j++) {
			size_t num_pages_right = right->attributes[right->attr_order[0]]->pages.size();
			size_t cols_right = attr_right.size();

			for (size_t ii = 0; ii < num_pages_right; ii++) {
				if (!attr_right[0]->page_order.size()) {
					return nullptr;
				}
				int page_num_right = attr_right[0]->page_order[0];
				if (!buffer->iscached(page_num_right))
					buffer->fetch(page_num_right);

				Page* p_right = buffer->get(page_num_right);
				size_t num_tuples_right = p_right->size();

				for (size_t jj = 0; jj < num_tuples_right; jj++) {
					size_t pn_left = cond_left->page_order[i];
					size_t pn_right = cond_right->page_order[i];

					if (!buffer->iscached(pn_left))
						buffer->fetch(pn_left);
					if (!buffer->iscached(pn_right))
						buffer->fetch(pn_right);

					Page* page_left = buffer->get(pn_left);
					Page* page_right = buffer->get(pn_right);

					if (cond_left->type() != cond_right->type()) {
						cerr << "Error: The value types of selected keys to join don't match." << endl;
						return nullptr;
					}
					else if (cond_left->type() == "INT") {
						int val_left = page_left->content[j]->ival;
						int val_right = page_right->content[jj]->ival;
						if (val_left == val_right) {
							// TODO: Push entry into joined table
							AddtoJoinedTable(i, j, jj, attr_left, attr_right, cols_left, cols_right, tname, selectList);
						}
					}
					else if (cond_left->type() == "DOUBLE") {
						double val_left = page_left->content[j]->dval;
						double val_right = page_right->content[jj]->dval;
						if (abs(val_left - val_right) < EPSILON) {
							AddtoJoinedTable(i, j, jj, attr_left, attr_right, cols_left, cols_right, tname, selectList);
						}
					}
					else {
						string val_left = page_left->content[j]->sval;
						string val_right = page_right->content[jj]->sval;

						if (val_left == val_right) {
							AddtoJoinedTable(i, j, jj, attr_left, attr_right, cols_left, cols_right, tname, selectList); 
						}
					}
					// cout << 4 << endl;
				}
			}
		}
	}
	return JoinedTable;
}

// Table* QueryParser::FullJoin(   Table* left, Table* right, hsql::Expr* condition, hsql::SelectStatement* select) {

// }

// Table* QueryParser::LeftJoin(   Table* left, Table* right, hsql::Expr* condition, hsql::SelectStatement* select) {

// }

// Table* QueryParser::CrossJoin(  Table* left, Table* right, hsql::Expr* condition, hsql::SelectStatement* select) {

// }

// Table* QueryParser::NaturalJoin(Table* left, Table* right, hsql::Expr* condition, hsql::SelectStatement* select) {

// }