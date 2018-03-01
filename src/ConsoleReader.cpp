#include "ConsoleReader.h"

ConsoleReader::ConsoleReader(FileManager* fs, BufferManager* bf, QueryParser* p) {
	filesystem = fs;
	buffer = bf;
	parser = p;
}

void ConsoleReader::AddDatabase(string username) {
	cout << "Add a new user \"" << username <<"\"? [Y/N]" << endl;
	cout << "HodorDB";
	if (filesystem->user) cout << " " << filesystem->user->name() << " $ ";
	else cout << "$ ";

	string answer;

	while (true) {
		cin >> answer;

		if (answer == "Y" || answer == "y") {
			string timestamp = addTimeStamp();

			Database* DB = new Database(username, 0, timestamp);
			filesystem->users[username] = DB;
			filesystem->user = DB;

			cout << "Stored user: " << username << " to database" << endl;
			break;
		}
		else if (answer != "N" && answer != "n") {
			cout << "Add a new user \"" << username <<"\"? [Y/N]" << endl;
			cout << "HodorDB";
			if (filesystem->user) cout << " " << filesystem->user->name() << "$ ";
			else cout << "$ ";
		}
		else break;
	}
}

void ConsoleReader::ReadCommand() {
	while (true) {
		string command;

		if (filesystem->user)
			cout << "HodorDB " << filesystem->user->name() << "$ ";
		else
			cout << "HodorDB$ ";

		getline(cin, command);

		hsql::SQLParserResult result;
		hsql::SQLParser::parse(command, &result);

		if (!result.isValid()) { // database command
			cout << "Invalid statement" << endl;
			cout << result.errorMsg() << endl;
			
			cout << "Not a query" << endl;
			istringstream iss(command);
			string keyword;

			if (!getline(iss, keyword, ' ')) {
				cerr << "Unsupported Command." << endl;
				continue;
			}

			ToLower(keyword);
			
			if (keyword == "use")
				SetDatabase(command);
			else if (keyword == "partition")
				PartitionTable(command);
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
				cout << "Insert!" << endl;
				parser->ParseINSERT(statement);

				// auto l = buffer->getbuffer()->linkedlist;
				// for (auto it : l) {
				// 	it.second->write();
				// }
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
		AddDatabase(username);
	}
}

void ConsoleReader::PartitionTable(string command) {
	if (!filesystem->user) {
		cout << "Please choose database first." << endl;
		cout << "Usage: <USE> <USERNAME>" << endl;
		return;
	}

	// command should look like:
	// PARTITION <table name> <partition criteria>
	istringstream iss(command);
	string table_name;
	string partition;

	getline(iss, table_name, ' ');
	getline(iss, table_name, ' '); // get table name
	getline(iss, partition,  ' '); // get partition criteria

	set<string> partitions = {"year", "month", "date", "hour", "minute"};
	if (partitions.find(partition) == partitions.end()) {
		cout << "Invalid partition critera, please choose from: " << endl;
		cout << "YEAR, MONTH, DATE, HOUR, MINUTE" << endl;
		return;
	}

	// TODO: add timestamp to tuple objects
	// TODO: create new partitioned tables
	// TODO: remove old table
	// TODO: add removed page to avalable pages, update next avalable page

	string tname = filesystem->user->name() + "::" + table_name;
	TableStorage* table = filesystem->pages[tname];

	for (PageSet* pset : table->pageset) {
		for (int page_num : pset->pageset) {
			// fetch page from disk if not in memory
			if (!buffer->iscached(page_num)) { // if not in memory
				buffer->fetch(page_num);
			}
		}

	}
}