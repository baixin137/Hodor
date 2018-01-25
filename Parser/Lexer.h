#ifndef LEXER_H
#define LEXER_H

#include <regex>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>

using namespace std;

class Token {
private:
	string type;
	string token;

public:
	Token(string tp, string tk);

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

public:
	Lexer(string line);

	vector<Token> tokenize(); // return a list of tokens to the parser
	vector<Token> normalize(vector<Token>& tokens); // normalize the input SQL text
	bool is_var(string s); // return true if the given string can be an identifier (variable)
	bool is_num(string s); // return true if the given string is an integer / float
};

#endif