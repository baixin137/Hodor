#include "QueryParser/QueryParser.h"
#include "ConsoleReader.h"

int main() {
	// file system, fetch files from and flush pages to disk
	// cout << "Starting Database System..." << endl;

	FileManager* filesystem = new FileManager();

	// cout << "File System Started." << endl;

	// buffer manager, manage pages in memory
	BufferManager* buffer = new BufferManager(BUFFERSIZE);

	// cout << "Cache System Started." << endl;

	// parse incoming queries and act accordingly
	QueryParser* parser = new QueryParser(filesystem, buffer);

	// cout << "Parser Started." << endl;

	// Periodically flush dirty pages to disk
	AutoSave* save = new AutoSave(buffer, filesystem);

	// cout << "Console Reader started." << endl;

	save->StartInternalThread();

	ConsoleReader* input = new ConsoleReader(filesystem, buffer, parser);
	input->ReadCommand();

	return 0;
}