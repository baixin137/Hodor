#include "HodorFS/HodorFS.h"
#include "third_party/hyrise_sqlparser/src/SQLParser.h"

int main() {
	FileManager* fm = new FileManager();
	// fm->display_t();
	// cout << endl;
	// fm->display_p();

	BufferManager* bf = new BufferManager(10);
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
				const hsql::CreateStatement* create = (const hsql::CreateStatement*) statement;

				// traverse the columns
				string tablename(create->tableName);
				size_t cols = create->columns->size();

				Table* table = new Table(tablename, 0, cols);

				for (auto col : *(create->columns)) {
					string attr(col->name);

					string type;
					if (col->type == hsql::ColumnDefinition::INT)
						type = "INT";
					else if (col->type == hsql::ColumnDefinition::DOUBLE)
						type = "DOUBLE";
					else
						type = "STRING";

					Attribute* attribute = new Attribute(attr, type, tablename);
					table->attributes.push_back(attribute);
				}
				fm->addtable(table);
				cout << endl <<  "Table created: " << endl;
				fm->display_t();
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