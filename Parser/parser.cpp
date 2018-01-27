#include <fstream>
#include <streambuf>
#include "Lexer.h"
#include "ParseTree.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Usage: ./Parser <filename>" << endl;
		exit(1);
	}

	// read SQL from file in to buffer
	ifstream text(argv[1]);
	string buffer((istreambuf_iterator<char>(text)), istreambuf_iterator<char>());

	// lexical analysis
	Lexer* lexer = new Lexer(buffer);
	lexer->tokenize();

	cout << "tokenized" << endl;

	// lexer->normalize(tokens);

	// cout << "normalized" << endl;

	// for (auto t : tokens) {
	// 	cout << t.get_token();
	// }

	// cout << endl;

	// start to build tree
	ParseTree* tree = new ParseTree(lexer->get_tokens());

	return 0;
}