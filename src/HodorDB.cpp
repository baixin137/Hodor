#include "QueryParser/QueryParser.h"

int main() {
	// file system, fetch files from and flush pages to disk
	FileManager* fm = new FileManager();

	// buffer manager, manage pages in memory
	BufferManager* bf = new BufferManager(10);

	// parse incoming queries and act accordingly
	QueryParser* parser = new QueryParser(fm, bf);
	// bf->fetch(1);
	// bf->fetch(2);

	// bf->getbuffer()->display();

	// bf->flush(1);

	// bf->getbuffer()->display();

	while (true) {
		string query;
		cout << "Enter your SQL query" << endl;
		getline(cin, query);

		hsql::SQLParserResult result;
		hsql::SQLParser::parse(query, &result);

		if (result.isValid() && result.size() > 0) {
			const hsql::SQLStatement* statement = result.getStatement(0);
			if (statement->isType(hsql::kStmtCreate)) {
				parser->ParseCREATE(statement);
			}
			else if (statement->isType(hsql::kStmtInsert)) {
				parser->ParseINSERT(statement);
			}
			else if (statement->isType(hsql::kStmtSelect)) {
				parser->ParserSELECT(statement);
			}
			// write metainfo of new table to disk
		}
		else 
			cout << "Invalid query" << endl;
	}

	// string query = "SELECT * FROM Movie";

	// hsql::SQLParserResult result;
	// hsql::SQLParser::parse(query, &result);

	// if (result.isValid() && result.size() > 0) {
	// 	cout << "lalala wo cheng gong la!" << endl;
	// }
	return 0;
}