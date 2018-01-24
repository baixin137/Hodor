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
	// constructor
	LexicalAnalyzer(string line) {
		text = line;

		// set other symbol in SQL
		vector<string> sb = {"(", ")", "[", "]", "{", "}", ";"};
		copy(sb.begin(), sb.end(), inserter(others, others.end()));

		// set SQL keywords
		vector<string> kw = {
			"ADD", "EXTERNAL", "PROCEDURE", "ALL", "FETCH", "PUBLIC", 
			"ALTER", "FILE", "RAISERROR", "AND", "FILLFACTOR", "READ", 
			"ANY", "FOR", "READTEXT", "AS", "FOREIGN", "RECONFIGURE", 
			"ASC", "FREETEXT", "REFERENCES", "AUTHORIZATION", 
			"FREETEXTTABLE", "REPLICATION", "BACKUP", "FROM", 
			"RESTORE", "BEGIN", "FULL", "RESTRICT", "BETWEEN", 
			"FUNCTION", "RETURN", "BREAK", "GOTO", "REVERT", "BROWSE", 
			"GRANT", "REVOKE", "BULK", "GROUP", "RIGHT", "BY", "HAVING", 
			"ROLLBACK", "CASCADE", "HOLDLOCK", "ROWCOUNT", "CASE", 
			"IDENTITY", "ROWGUIDCOL", "CHECK", "IDENTITY_INSERT", "RULE", 
			"CHECKPOINT", "IDENTITYCOL", "SAVE", "CLOSE", "IF", "SCHEMA", 
			"CLUSTERED", "IN", "SECURITYAUDIT", "COALESCE", "INDEX", 
			"SELECT", "COLLATE", "INNER", "SEMANTICKEYPHRASETABLE", 
			"COLUMN", "INSERT", "SEMANTICSIMILARITYDETAILSTABLE", 
			"COMMIT", "INTERSECT", "SEMANTICSIMILARITYTABLE", "COMPUTE", 
			"INTO", "SESSION_USER", "CONSTRAINT", "IS", "SET", "CONTAINS", 
			"JOIN", "SETUSER", "CONTAINSTABLE", "KEY", "SHUTDOWN", 
			"CONTINUE", "KILL", "SOME", "CONVERT", "LEFT", "STATISTICS", 
			"CREATE", "LIKE", "SYSTEM_USER", "CROSS", "LINENO", "TABLE", 
			"CURRENT", "LOAD", "TABLESAMPLE", "CURRENT_DATE", "MERGE", 
			"TEXTSIZE", "CURRENT_TIME", "NATIONAL", "THEN", "CURRENT_TIMESTAMP", 
			"NOCHECK", "TO", "CURRENT_USER", "NONCLUSTERED", "TOP", "CURSOR", 
			"NOT", "TRAN", "DATABASE", "NULL", "TRANSACTION", "DBCC", "NULLIF", 
			"TRIGGER", "DEALLOCATE", "OF", "TRUNCATE", "DECLARE", "OFF", 
			"TRY_CONVERT", "DEFAULT", "OFFSETS", "TSEQUAL", "DELETE", "ON", 
			"UNION", "DENY", "OPEN", "UNIQUE", "DESC", "OPENDATASOURCE", 
			"UNPIVOT", "DISK", "OPENQUERY", "UPDATE", "DISTINCT", "OPENROWSET", 
			"UPDATETEXT", "DISTRIBUTED", "OPENXML", "USE", "DOUBLE", "OPTION", 
			"USER", "DROP", "OR", "VALUES", "DUMP", "ORDER", "VARYING", "ELSE", 
			"OUTER", "VIEW", "END", "OVER", "WAITFOR", "ERRLVL", "PERCENT", 
			"WHEN", "ESCAPE", "PIVOT", "WHERE", "EXCEPT", "PLAN", "WHILE", 
			"EXEC", "PRECISION", "WITH", "EXECUTE", "PRIMARY", "WITHIN", "GROUP", 
			"EXISTS", "PRINT", "WRITETEXT", "EXIT", "PROC"
		};
		copy(kw.begin(), kw.end(), inserter(keywords, keywords.end()));

		// set SQL operators
		vector<string> op = {
			"+", "-", "*", "/", "%", 
			"&", "|", "^",
			"==", ">=", "<=", ">", "<", "<>",
			"+=", "-=", "*=", "/=", "%=",
			"&=", "^-=", "|*="
		};
		copy(op.begin(), op.end(), inserter(operators, operators.end()));

		// set whitespaces
		vector<string> ws = {" ", "\t", "\n"};
		copy(ws.begin(), ws.end(), inserter(whitespaces, whitespaces.end()));
	}

	vector<pair<string, string>> analyze(); // return a list of tokens to the parser
	bool is_var(string s); // return true if the given string can be an identifier (variable)
};

vector<pair<string, string>> LexicalAnalyzer::analyze() {
	vector<pair<string, string>> res;
	if (!text.size()) return res;

	// use 2 pointers start and end to analyze the text
	// when text.substr(start, end) is not a token but text.substr(start, end-1) is,
	// add this token to res and move forward
	int start = 0;
	int end = 1;

	// use a flag to see if the last substring is a token
	string type;

	while (end <= text.size()) {
		string token = text.substr(start, end - start);

		// keywords have higher priority than identifiers
		if (others.find(token) != others.end()) {
			pair<string, string> p = make_pair("others", token);
			res.push_back(p);
			start = end;
		}
		else if (keywords.find(token) != keywords.end() || operators.find(token) != operators.end() || is_var(token))
			type = "keyword";
		else if (operators.find(token) != operators.end())
			type = "operator";
		else if (is_var(token))
			type = "identifier";

		else {
			// the substring is none of the above, that means the previous substring is a valid token we are going to choose
			pair<string, string> p = make_pair(type, token.substr(0, token.size()-1));
			res.push_back(p);
			start = end;
		}
		end++;
	}

	return res;
}

bool LexicalAnalyzer::is_var(string input) {
	// regular expression for identifiers: letter(letter + digit)*
	// using this regular expression I generated NFA -> DFA
	// the result DFA is as follows:
	// there are 3 states, where A is the start state, L and D are both final states
	// for A, if the input is a letter, it transits to L, otherwise false should be returned
	// for L and D it transits to L if the input is a letter, and it transits to D if the input is a digit
	// this is pretty simple so I'm not building a table for it

	char state = 'A';
	for (auto c : input) {
		if (state == 'A') {
			if (isalpha(c)) state = 'L';
			else return false;
		}
		else {
			if (isdigit(c))
				state = 'D';
			else if (isalpha(c))
				state = 'L';
			else return false;
		}
	}

	if (state == 'L' || state == 'D') return true;
	else return false;
}