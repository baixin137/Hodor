#include "Lexer.h"

Token::Token(string tp, string tk) {
	type = tp;
	token = tk;
}

string Token::get_type() {
	return type;
}

string Token::get_token() {
	return token;
}

bool Token::is_keyword() {
	return type == "keyword";
}

bool Token::is_whitespace() {
	return type == "whitespace";
}

bool Token::is_others() {
	return type == "others";
}

bool Token::is_operator() {
	return type == "operator";
}

bool Token::is_identifier() {
	return type == "identifier";
}

bool Token::is_number() {
	return type == "number";
}


Lexer::Lexer(string line) {
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

void Lexer::tokenize() {
	if (!text.size()) return;

	// use 2 pointers start and end to analyze the text
	// when text.substr(start, end) is not a token but text.substr(start, end-1) is,
	// add this token to res and move forward
	int start = 0;
	int end = 1;

	// use a flag to see if the last substring is a token
	string type;

	while (end <= text.size()) {
		string token = text.substr(start, end - start);

		if (end == text.size()) {
			Token t(type, token);
			tokens.push_back(t);
			break;
		}

		// keywords should have higher priority than identifiers
		// so check if the token is keyword first, then identifier

		// cout << "current token is: " << token << endl;

		if (others.find(token) != others.end()) {
			Token t("others", token);
			tokens.push_back(t);
			start = end;
		}
		else if (keywords.find(token) != keywords.end())
			type = "keyword";
		else if (operators.find(token) != operators.end())
			type = "operator";
		else if (whitespaces.find(token) != whitespaces.end())
			type = "whitespace";
		else if (is_var(token))
			type = "identifier";
		else if (is_num(token))
			type = "number";
		else {
			// the substring is none of the above, that means the previous substring is a valid token we are going to choose
			if (type != "whitespace") {
				Token t(type, token.substr(0, token.size() - 1));
				tokens.push_back(t);
			}
			start = end - 1;
			continue;
		}
		end++;
	}

	return;
}

void Lexer::normalize() {
	vector<Token> res;

	// this is basically just remove the duplicate whitespaces for now
	int p = 0;

	// remove starting whitespaces
	while (p < tokens.size() && tokens[p].is_whitespace()) {
		if (tokens[p].is_whitespace()) p++;
	}

	while (p < tokens.size()) {
		if (!tokens[p].is_whitespace())
			res.push_back(tokens[p++]);
		else {
			Token t("whitespace", " ");
			res.push_back(t);

			while (p < tokens.size() && tokens[p].is_whitespace()) {
				p++;
			}
		}
	}
	tokens = res;
}

bool Lexer::is_var(string input) {
	// regular expression for identifiers: letter(letter + digit)*
	// using this regular expression I generated NFA -> DFA
	// the result DFA is as follows:
	// there are 4 states, where A is the start state, L (is letter), D (is digit) , U (is underline) are all final states
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
			else if (c == '_')
				state = 'U';
			else return false;
		}
	}

	if (state == 'L' || state == 'D' || state == 'U') return true;
	else return false;
}

bool Lexer::is_num(string s) {
    int i = 0;
        
    // check the significand
    if(s[i] == '+' || s[i] == '-') i++; // skip the sign if exist
    
    int n_nm, n_pt;
    for(n_nm = 0, n_pt = 0; (s[i] <= '9' && s[i] >= '0') || s[i] == '.'; i++)
        s[i] == '.' ? n_pt++ : n_nm++;       
    if(n_pt>1 || n_nm<1) // no more than one point, at least one digit
        return false;
    
    // check the exponent if exist
    if(s[i] == 'e') {
        i++;
        if(s[i] == '+' || s[i] == '-') i++; // skip the sign
        
        int n_nm = 0;
        for(; s[i]>='0' && s[i]<='9'; i++, n_nm++) {}
        if(n_nm<1)
            return false;
    }
    
    return s[i]==0;  // must reach the ending 0 of the string
}

vector<Token> Lexer::get_tokens() {
	return tokens;
}