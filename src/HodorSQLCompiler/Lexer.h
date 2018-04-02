#ifndef LEXER_H
#define LEXER_H

#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>

using namespace std;

class Token {
private:
	string tokentype;
	string tokenvalue;

public:
	Token(string tp, string tk);

	string type();
	string token();

	bool is_keyword();
	bool is_whitespace();
	bool is_operator();
	bool is_others();
	bool is_identifier();
	bool is_number();
};

class Lexer {
private:
	string text;

	unordered_set<string> others; // parens, brackets, braces
	unordered_set<string> keywords;
	unordered_set<string> operators;
	unordered_set<string> whitespaces;

	vector<Token*> tokens; // tokenized SQL statement

public:
	Lexer(string line);

	void tokenize(); // return a list of tokens to the parser
	void normalize(); // normalize the input SQL text
	bool is_var(string s); // return true if the given string can be an identifier (variable)
	bool is_num(string s); // return true if the given string is an integer / float
	vector<Token*> get_tokens();
};

#endif