#ifndef __HODORPARSER__PARSER_H__
#define __HODORPARSER__PARSER_H__

namespace HodorSQL {
	class ParseTree {
	public:
		ParseTreeNode* root;
		vector<Token*> tokens;

		int next; // keeps track of correctly assigned tokens in the backtracking process
		 
		 // recursive functions for backtracking
		 // nonterminals
		 bool isselect();
		 bool isfrom();
		 bool iswhere();
		 bool isasterisk();
		 bool iscomma();
		 bool isidentifier();
		 bool isparen_l();
		 bool isparen_r();
		 
		 bool attributes();
		 bool tables();
		 
		 bool iscondition();
		 bool issubquery();

		 bool querysf(); // select from
		 bool querysfw(); // select from where
		 bool querysfwg(); // select from where groupby

		 ParseTree(vector<Token*>& ts);
		 ~ParseTree();

		 void build_tree(); // uses the top-down method here to build a parse tree
	}
}

#endif