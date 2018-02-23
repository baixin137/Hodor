#include "QueryParser.h"

QueryParser::QueryParser(FileManager* fs, BufferManager* bf) {
	filesystem = fs;
	buffer = bf;
}

void QueryParser::ParseCREATE(hsql::SQLStatement* statement) {
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
	fm->add(table);
}

void QueryParser::ParseINSERT(hsql::SQLStatement* statement) {
	const hsql::CreateStatement* insert = (const hsql::InsertStatement*) statement;

	if (!insert->columns) { // attributes not specified
		
	}
}