#ifndef PARSETREE_H
#define PARSETREE_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>

#include "Lexer.h"

using namespace std;

class ParseTreeNode {
private:
	string type;
	bool terminal;

public:
	ParseTreeNode(const & string tp, bool tm);
};

class Query : public ParseTreeNode {
private:
	ParseTreeNode* SFW;

public:
	Query(const & string tp = "<Query>", bool tm = false);
};

class SWF : public ParseTreeNode {
private:
	ParseTreeNode* select;
	ParseTreeNode* selist;
	ParseTreeNode* from;
	ParseTreeNode* fromlist;
	ParseTreeNode* where;
	ParseTreeNode* condition;

public:
	SWF(const & string tp = "<SWF>", bool tm = false);
};

class : public ParseTreeNode {
private:
	vector<ParseTreeNode*> attributes; // a list that contains one or more attributes

public:
	SelList(const & string tp = "<SelList>", bool tm = false);
};

class Attribute : public ParseTree {
private:
	ParseTreeNode* attribute;

public:
	Attribute(const & string tp = "<Attribute>", bool tm = false);
};

class FromList : public ParseTreeNode {
private:
	vector<ParseTreeNode*> relnames; // a list of table names

public:
	FromList(const & string tp = "<FromList>", bool tm = false);
};

class RelName : public ParseTreeNode {
private:
	ParseTreeNode* relname;

public:
	RelName(const & string tp = "<RelName>", bool tm = false);
};

class Condition : public ParseTreeNode {
private:
	ParseTreeNode* tuple;
	ParseTreeNode* in;
	ParseTreeNode* subquery;

public:
	Condition(const & string tp = "<Condition>", bool tm = false);
};

class Tuple : public ParseTreeNode {
private:
	vector<ParseTreeNode*> attributes;

public:
	Tuple(const & string tp = "<Tuple>", bool tm = false);
};

class Subquery : public ParseTreeNode {
private:
	ParseTreeNode* paren_l;
	ParseTreeNode* paren_r;
	ParseTreeNode* query;

public:
	Subquery(const & string tp = "<Subquery>", bool tm = false)
};

class ParseTree {
private:
	ParseTreeNode* root;
	vector<Tokens> tokens;

	int next; // keeps track of correctly assigned tokens in the backtracking process

public:
	ParseTree();
	~ParseTree();

	void build_tree(); // uses the top-down method here to build a parse tree
}

#endif