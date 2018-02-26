#include "QueryParser/QueryParser.h"
#include "ConsoleReader.h"

int main() {
	// file system, fetch files from and flush pages to disk
	FileManager* filesystem = new FileManager();

	// buffer manager, manage pages in memory
	BufferManager* buffer = new BufferManager(10);

	// parse incoming queries and act accordingly
	QueryParser* parser = new QueryParser(filesystem, buffer);

	// Periodically flush dirty pages to disk
	AutoSave* save = new AutoSave(buffer, filesystem);
	save->StartInternalThread();

	ConsoleReader* input = new ConsoleReader(filesystem, buffer, parser);
	input->ReadCommand();

	return 0;
}