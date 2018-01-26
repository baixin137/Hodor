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
	ParseTreeNode(const string & tp, bool tm);
	virtual ~ParseTreeNode();

	bool is_terminal();
	string get_type();
};

class Query : public ParseTreeNode {
private:
	ParseTreeNode* swf;

public:
	Query(const string & tp = "<Query>", bool tm = false);
	~Query();
};

class SWF : public ParseTreeNode {
private:
	ParseTreeNode* select;
	ParseTreeNode* sellist;
	ParseTreeNode* from;
	ParseTreeNode* fromlist;
	ParseTreeNode* where;
	ParseTreeNode* condition;

public:
	SWF(const string & tp = "<SWF>", bool tm = false);
	~SWF();
};

class SelList: public ParseTreeNode {
private:
	vector<ParseTreeNode*> attributes; // a list that contains one or more attributes

public:
	SelList(const string & tp = "<SelList>", bool tm = false);
	~SelList();
};

class Attribute : public ParseTree {
private:
	ParseTreeNode* attribute;

public:
	Attribute(const string & tp = "<Attribute>", bool tm = false);
	~Attribute();
};

class FromList : public ParseTreeNode {
private:
	vector<ParseTreeNode*> relnames; // a list of table names

public:
	FromList(const string & tp = "<FromList>", bool tm = false);
	~FromList();
};

class RelName : public ParseTreeNode {
private:
	ParseTreeNode* relname;

public:
	RelName(const string & tp = "<RelName>", bool tm = false);
	~RelName();
};

class Condition : public ParseTreeNode {
private:
	ParseTreeNode* tuple;
	ParseTreeNode* in;
	ParseTreeNode* subquery;

public:
	Condition(const string & tp = "<Condition>", bool tm = false);
	~Condition();
};

class Tuple : public ParseTreeNode {
private:
	vector<ParseTreeNode*> attributes;

public:
	Tuple(const string & tp = "<Tuple>", bool tm = false);
	~Tuple();
};

class Subquery : public ParseTreeNode {
private:
	ParseTreeNode* paren_l;
	ParseTreeNode* paren_r;
	ParseTreeNode* query;

public:
	Subquery(const string & tp = "<Subquery>", bool tm = false)
	~Subquery();
};

class ParseTree {
private:
	ParseTreeNode* root;
	vector<Token> tokens;

	ParseTreeNode* curr // keeps track of current node in the building parse tree
	int next; // keeps track of correctly assigned tokens in the backtracking process

	// recursive functions for backtracking
	// nonterminals
	bool isselect();
	bool isfrom();
	bool iswhere();
	bool isasterisk();
	bool isin();
	bool iscomma();
	bool isidentifier();

	bool singleattr();
	bool multiattrs();
	bool singletable();
	bool multitables();

	bool issellist();
	bool isfromlist();
	bool iscondition();

	bool querysf(); // select from
	bool querysfw(); // select from where
public:
	ParseTree(vector<Token> ts);
	~ParseTree();

	void build_tree(); // uses the top-down method here to build a parse tree
}

#endif