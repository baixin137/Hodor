#ifndef __HODORPARSER__PARSETREE_NODE_H__
#define __HODORPARSER__PARSETREE_NODE_H__

using namespace std;

namespace HodorSQL {
	enum NodeType {
		kNodeQuery,

		kNodeClauseSELECT,
		kNodeClauseFROM,
		kNodeClauseWHERE,
		kNodeClauseGROUPBY,
		kNodeClauseON,
		kNodeClauseJOIN,
		kNodeFuncRef,

		kNodeNameTable,
		kNodeNameAttribute,

		kNodeSymbolAsterisk,
		kNodeSymbolComma
	};


	enum OperatorType {
	    kOpNone,

	    // Ternary operator
	    kOpBetween,

	    // n-nary special case
	    kOpCase,
	    kOpCaseListElement, // `WHEN expr THEN expr`

	    // Binary operators.
	    kOpPlus,
	    kOpMinus,
	    kOpAsterisk,
	    kOpSlash,
	    kOpPercentage,
	    kOpCaret,

	    kOpEquals,
	    kOpNotEquals,
	    kOpLess,
	    kOpLessEq,
	    kOpGreater,
	    kOpGreaterEq,
	    kOpLike,
	    kOpNotLike,
	    kOpILike,
	    kOpAnd,
	    kOpOr,
	    kOpIn,
	    kOpConcat,

	    // Unary operators.
	    kOpNot,
	    kOpUnaryMinus,
	    kOpIsNull,
	    kOpExists
	};

	class ParseTreeNode {
	public:
		ParseTreeNode(NodeType type);
		virtual ~ParseTreeNode();

		NodeType type;

		bool terminal; // the node is terminal or not
		string name; // for table or attribute name
		double dval;
		int    ival;

		ParseTreeNode* select;
		ParseTreeNode* where;
		ParseTreeNode* from;
		ParseTreeNode* groupby;

		ParseTreeNode* ExprLeft;
		ParseTreeNode* ExprRight;
		OperatorType op;

		vector<ParseTreeNode*>* ExprList; // attributes, tables or groupby list

		bool distinct;
	};
}

#endif