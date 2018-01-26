#include "ParseTree.h"

bool ParseTree::isselect() {
	Token t = tokens[next];
	if (t.type != "keyword" || t.token != "SELECT")
		return false;
	next++;
	return true;
}

bool ParseTree::issfrom() {
	Token t = tokens[next];
	if (t.type != "keyword" || t.token != "FROM")
		return false;
	next++;
	return true;
}

bool ParseTree::iswhere() {
	Token t = tokens[next];
	if (t.type != "keyword" || t.token != "WHERE")
		return false;
	next++;
	return true;
}

bool ParseTree::isasterisk() {
	Token t = tokens[next];
	if (t.type != "operator" || t.token != "*")
		return false;
	next++;
	return true;
}

bool ParseTree::isin() {
	Token t = tokens[next];
	if (t.type != "keyword" || t.token != "IN")
		return false;
	next++;
	return true;
}

bool ParseTree::islike() {
	Token t = tokens[next];
	if (t.type != "keyword" || t.token != "LIKE")
		return false;
	next++;
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

bool ParseTree::isparen_l() {
	Token t = tokens[next];
	if (t.type != "others" || t.token != "(")
		return false;
	next++;
	return true;
}

bool ParseTree::isparen_r() {
	Token t = tokens[next];
	if (t.type != "operator" || t.token != ")")
		return false;
	next++;
	return true;
}

bool ParseTree::attributes() {
	Token t = tokens[next];
	while (isidentifier()) {
		if (tokens[next].get_type() == "keyword") {
			return true;
		}
		else if (tokens[next].get_token() == ",")
			next++;
		else return false;
	}
}

bool ParseTree::tables() {
	Token t = tokens[next];
	while (isidentifier()) {
		if (tokens[next].get_type() == "keyword") {
			return true;
		}
		else if (tokens[next].get_token() == ",")
			next++;
		else return false;
	}
}

bool iscondition() {
	int save = next;
	if (attributes() &&
		isin() &&
		issubquery() &&
		next == tokens.size())
		return true;

	next = save;
	if (attributes() &&
		islike() &&
		ispattern() &&
		next == tokens.size())
		return true;

	next = save;
	return false;
}

bool issubquery() {
	int save = next;
	if (isparen_l() &&
	    querysf() &&
	    isparen_r() &&
	    next == tokens.size())
	    return true;

	next = save;
	if (isparen_l() &&
	    querysfw() &&
	    isparen_r() &&
	    next == tokens.size())
	    return true;

	next = save;
	return false;
}

bool ParseTree::querysf() {
	return isselect() &&
		   issellist() &&
		   isfrom() &&
		   isfromlist() &&
		   next == tokens.size();
}

bool ParseTree::querysfw() {
	return isselect() &&
		   issellist() &&
		   isfrom() &&
		   isfromlist() &&
		   iswhere() &&
		   iscondition() &&
		   next == tokens.size();
}

ParseTree::ParseTree(vector<Token> ts) {
	root = new Query();
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
}

