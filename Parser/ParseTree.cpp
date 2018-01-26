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

bool ParseTree::singleattr() {
	
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

}

