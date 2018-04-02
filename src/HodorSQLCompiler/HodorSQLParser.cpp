#include "HodorSQLParser.h"

using namespace HodorSQL;

bool ParseTree::isselect(ParseTreeNode* root) {
	Token t = tokens[next];
	if (t.type != "keyword" || t.token != "SELECT")
		return false;
	next++;
	ParseTreeNode* select = new ParseTreeNode(kNodeClauseSELECT);
	root->select = select;

	return attributes(select) || isasterisk(select);
}

bool ParseTree::issfrom(ParseTreeNode* root) {
	Token t = tokens[next];
	if (t.type != "keyword" || t.token != "FROM")
		return false;
	next++;
	ParseTreeNode* from = new ParseTreeNode(kNodeClauseFROM);
	return tables(from);
}

bool ParseTree::iswhere(ParseTreeNode* root) {
	Token t = tokens[next];
	if (t.type != "keyword" || t.token != "WHERE")
		return false;
	next++;
	ParseTreeNode* where = new ParseTreeNode(kNodeClauseWHERE);
	return iscondition();
}

bool ParseTree::isasterisk(ParseTreeNode* parent) {
	Token t = tokens[next];
	if (t.type != "operator" || t.token != "*")
		return false;
	next++;
	ParseTreeNode* asterisk = new ParseTreeNode(kNodeSymbolAsterisk);
	parent->ExprList->push_back(asterisk);
	return true;
}

bool ParseTree::iscomma() {
	Token t = tokens[next];
	if (t.type != "operator" || t.token != ",")
		return false;
	next++;
	return true;
}

bool ParseTree::isidentifier() {
	Token t = tokens[next];
	if (t.type != "identifier")
		return false;
	next++;
	return true;
}

// bool ParseTree::isparen_l() {
// 	Token t = tokens[next];
// 	if (t.type != "others" || t.token != "(")
// 		return false;
// 	next++;
// 	return true;
// }

// bool ParseTree::isparen_r() {
// 	Token t = tokens[next];
// 	if (t.type != "operator" || t.token != ")")
// 		return false;
// 	next++;
// 	return true;
// }

bool ParseTree::attributes(ParseTreeNode* parent) {
	Token t = tokens[next];
	while (isidentifier()) {
		if (tokens[next].get_type() == "keyword") {
			ParseTreeNode* attr = new ParseTreeNode(kNodeNameAttribute);
			parent->ExprList->push_back(attr);
			return true;
		}
		else if (tokens[next].get_token() == ",")
			next++;
		else return false;
	}
}

bool ParseTree::tables(ParseTreeNode* parent) {
	Token t = tokens[next];
	while (isidentifier()) {
		if (tokens[next].get_type() == "keyword") {
			ParseTreeNode* table = new ParseTreeNode(kNodeNameTable);
			parent->ExprList->push_back(table);
			return true;
		}
		else if (tokens[next].get_token() == ",")
			next++;
		else return false;
	}
}

// bool iscondition() {
// 	int save = next;
// 	if (attributes() &&
// 		isin() &&
// 		issubquery() &&
// 		next == tokens.size())
// 		return true;

// 	next = save;
// 	if (attributes() &&
// 		islike() &&
// 		ispattern() &&
// 		next == tokens.size())
// 		return true;

// 	next = save;
// 	return false;
// }

// bool issubquery() {
// 	int save = next;
// 	if (isparen_l() &&
// 	    querysf() &&
// 	    isparen_r() &&
// 	    next == tokens.size())
// 	    return true;

// 	next = save;
// 	if (isparen_l() &&
// 	    querysfw() &&
// 	    isparen_r() &&
// 	    next == tokens.size())
// 	    return true;

// 	next = save;
// 	return false;
// }

bool ParseTree::querysf() {
	return isselect() &&
		   isfrom() &&
		   next == tokens.size();
}

bool ParseTree::querysfw() {
	return isselect() &&
		   isfrom() &&
		   iswhere() &&
		   next == tokens.size();
}

bool ParseTree::querysfwg() {
	return isselect() &&
		   isfrom() &&
		   iswhere() &&
		   isgroupby() &&
		   next == tokens.size();
}

ParseTree::ParseTree(vector<Token*> ts) {
	root = new ParseTreeNode();
	tokens = ts;

	next = 0;
}

ParseTree::~ParseTree() {
	delete root;

}

void ParseTree::build_tree() {
	int save = next;
	if (querysfw()) return;

	next = save;
	if (querysf()) return;

	next = save;
	if (querysfwg()) return;
}