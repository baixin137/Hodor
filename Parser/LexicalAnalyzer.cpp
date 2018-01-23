#include <vector>
#include <iostream>
#include <string>
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
};

// vector<pair<string, string>> LexicalAnalyzer::analyze() {

// }