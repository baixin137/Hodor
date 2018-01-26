#include "ParseTree.h"

ParseTreeNode::ParseTreeNode(bool t, string l) {
	label.terminal = t;
	label.label = l;
}

string ParseTreeNode::get_label() {
	return label;
}

void ParseTreeNode::add_child(ParseTreeNode* node) {
	children->push_back(node);
}

ParseTree::Parsetree(vector<Token> t) {
	root = new ParseTreeNode("<Query>");
	tokens = t;
}

ParseTree::~ParseTree() {
	delete root;
}

void ParseTree::build_tree() {

}