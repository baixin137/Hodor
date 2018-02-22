#include "HodorFS/HodorFS.h"
#include "third_party/hyrise_sqlparser/src/SQLParser.h"

int main() {
	FileManager* fm = new FileManager();
	fm->display_t();
	cout << endl;
	fm->display_p();

	BufferManager* bf = new BufferManager(10);
	bf->fetch(1);
	bf->fetch(2);

	bf->getbuffer()->display();

	bf->flush(1);

	bf->getbuffer()->display();
	
	string query = "SELECT * FROM Movie";

	hsql::SQLParserResult result;
	hsql::SQLParser::parse(query, &result);

	if (result.isValid() && result.size() > 0) {
		cout << "lalala wo cheng gong la!" << endl;
	}

	return 0;
}