#include "ParseTreeNode.h"

using namespace HodorSQL;

ParseTreeNode::ParseTreeNode(NodeType t) {
	type = t;
	if (t == kNodeQuery) {
		terminal = false;
	}
	else if (t == kNodeClauseSELECT) {
		terminal = false;
		ExprList = new vector<ParseTreeNode*>;
	}
	else if (t == kNodeClauseFROM) {
		terminal = false;
		ExprList = new vector<ParseTreeNode*>;
	}
	else if (t == kNodeClauseWHERE) {
		terminal = false;
	}
	else if (t == kNodeClauseGROUPBY) {
		terminal = false;
		ExprList = new vector<ParseTreeNode*>;
	}
	else if (t == )
}