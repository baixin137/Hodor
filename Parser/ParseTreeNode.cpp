#include "ParseTree.h"

ParseTreeNode::ParseTreeNode(const string & tp, bool tm) {
	type = tp;
	terminal = tm;
}

bool ParseTreeNode::is_terminal() {
	return terminal;
}

string ParseTreeNode::get_type() {
	return type;
}

Query::Query(const string & tp, bool tm) {
	type = tp;
	terminal = tm;

	swf = new SWF();
}

Query::~Query() {
	delete swf;
}

SWF::SWF(const string & tp, bool tm) {
	type = tp;
	terminal = tm;

	select = new ParseTreeNode("SELECT", true);
	sellist = new SelList();
	from = new ParseTreeNode("FROM", true);
	fromlist = new FromList();
	where = new ParseTreeNode("WHERE", true);
	condition = new Condition();
}

SWF::~SWF() {
	delete select;
	delete sellist;
	delete from;
	delete fromlist;
	delete where;
	delete condition;
}

SelList::SelList(const string & tp, bool tm) {
	type = tp;
	terminal = tm;
}

SelList::~SelList() {
	for (auto iter = attributes.begin(); iter < attributes.end(); iter++) {
		delete *iter;
	}
}

Attribute::Attribute(const string & tp, bool tm) {
	type = tp;
	terminal = tm;

	attribute = new ParseTreeNode("attribute", true);
}

Attribute::~Attribute() {
	delete attribute;
}

FromList::FromList(const string & tp, bool tm) {
	type = tp;
	terminal = tm;
}

FromList::~FromList() {
	for (auto iter = relnames.begin(); iter < relnames.end(); iter++) {
		delete *iter;
	}
}

RelName::RelName(const string & tp, bool tm) {
	type = tp;
	terminal = tm;

	relname = new RelName();
}

RelName::~RelName() {
	delete relname;
}

Condition::Condition(const string & tp, bool tm) {
	type = tp;
	terminal = tm;

	tuple = new Tuple();
	in = new ParseTreeNode("IN", true);
	subquery = new Subquery();
}

Condition::~Condition() {
	delete tuple;
	delete in;
	delete subquery;
}

Tuple::Tuple(const string & tp, bool tm) {
	type = tp;
	terminal = tm;
}

Tuple::~Tuple() {
	for (auto iter = attributes.begin(); iter < attributes.end(); iter++) {
		delete *iter;
	}
}

Subquery::Subquery(const string & tp, bool tm) {
	type = tp;
	terminal = tm;

	paren_l = new ParseTreeNode("(", true);
	paren_r = new ParseTreeNode(")", true);
	query = new Query();
}

Subquery::~Subquery() {
	delete paren_l;
	delete paren_r;
	delete query;
}