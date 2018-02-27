#ifndef CONSOLEREADER_H
#define CONSOLEREADER_H

#include "HodorFS/HodorFS.h"
#include "QueryParser/QueryParser.h"

class ConsoleReader {
private:
	FileManager* filesystem;
	BufferManager* buffer;
	QueryParser* parser;

public:
	ConsoleReader(FileManager* fs, BufferManager* bf, QueryParser* p);

	void ReadCommand();

	void SetDatabase(string command);
	void PartitionTable(string command);
};

#endif