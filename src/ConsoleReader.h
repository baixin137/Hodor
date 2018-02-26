#ifndef CONSOLEREADER_H
#define CONSOLEREADER_H

#include "HodorFS/HodorFS.h"
#include "QueryParser/QueryParserh"

class ConsoleReader {
private:
	FileManager* filesystem;
	BufferManager* buffer;
	QueryParser* parser;

public:
	ConsoleReader(FileManager* fs, BufferManager* bf, QueryParser* p);

	void ReadCommand();
	bool isSelectDatabase(string command);
	void SetDatabase(string command);
};


#endif