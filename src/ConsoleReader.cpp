#include "ConsoleReader.h"

ConsoleReader::ConsoleReader(FileManager* fs, BufferManager* bf, QueryParser* p) {
	filesystem = fs;
	buffer = bf;
	parser = p;
}

void ConsoleReader::ReadCommand() {
	while (true) {
		string command;

		if (filesystem->user)
			cout << filesystem->user->name() << "$ ";
		else
			cout << "$ ";

		getline(cin, command);

		hsql::SQLParserResult result;
		hsql::SQLParser::parse(command, &result);

		if (!result.isValid()) { // database command
			// cout << "Invalid statement" << endl;
			// cout << result.errorMsg() << endl;
			if (isSelectDatabase(command)) {
				SetDatabase(command);
			}
		}
		else if (result.size() > 0) {
			if (!filesystem->user) {
				cout << "PLEASE ENTER THE USERNAME" << endl;
				cout << "Usage: <USE> <USERNAME>" << endl;
				continue;
			}
			const hsql::SQLStatement* statement = result.getStatement(0);
			if (statement->isType(hsql::kStmtCreate)) {
				// cout << "CREATE..." << endl;
				parser->ParseCREATE(statement);
			}
			else if (statement->isType(hsql::kStmtInsert)) {
				parser->ParseINSERT(statement);

				auto l = buffer->getbuffer()->linkedlist;
				for (auto it : l) {
					it.second->write();
				}
			}
			// else if (statement->isType(hsql::kStmtSelect)) {
			// 	parser->ParserSELECT(statement);
			// }
			// write metainfo of new table to disk
		}
		else 
			cout << "Invalid command" << endl;
	}
}

bool ConsoleReader::isSelectDatabase(string command) {
	istringstream iss(command);
	string keyword;

	getline(iss, keyword, ' ');
	transform(keyword.begin(), keyword.end(), keyword.begin(), ::tolower);
	if (keyword == "use")
		return true;
	else return false;
}

void ConsoleReader::SetDatabase(string command) {
	// username can't have white space in it
	istringstream iss(command);
	string username;

	getline(iss, username, ' ');
	getline(iss, username, ' ');

	if (filesystem->users.find(username) != filesystem->users.end()) { // check if username already in system
		// if username exists
		filesystem->user = filesystem->users[username];
	}
	else { // create a new entry
		auto curr_time = chrono::system_clock::now();
		time_t t = chrono::system_clock::to_time_t(curr_time);

		string timestamp(ctime(&t));
		timestamp.erase(remove(timestamp.begin(), timestamp.end(), '\n'), timestamp.end());

		Database* DB = new Database(username, 0, timestamp);
		filesystem->users[username] = DB;
		filesystem->user = DB;

		cout << "Stored user: " << username << " to database" << endl;
	}
}