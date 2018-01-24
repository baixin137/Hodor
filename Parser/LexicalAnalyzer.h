#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include <regex>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_set>

using namespace std;

class LexicalAnalyzer {
private:
	string text;

	unordered_set<string> others; // parens, brackets, braces
	unordered_set<string> keywords;
	unordered_set<string> operators;
	unordered_set<string> whitespaces;

public:
	LexicalAnalyzer(string line);

	vector<pair<string, string>> analyze(); // return a list of tokens to the parser
	bool is_var(string s); // return true if the given string can be an identifier (variable)
}

#endif