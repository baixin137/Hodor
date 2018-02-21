#include "HodorFS/HodorFS.h"
#include "HodorFS/BufferManager.h"
#include "third_party/hyrise_sqlparser/src/SQLParser.h"

int main() {
	string query = "SELECT * FROM Movie";

	hsql::SQLParserResult result;
	hsql::SQLParser::parse(query, &result);

	if (result.isValid() && result.size() > 0) {
		cout << "lalala wo cheng gong la!" << endl;
	}

	return 0;
}