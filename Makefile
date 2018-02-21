all: HodorDB

BIN        = bin
SRC        = src
HODORFS    = src/HodorFS
HYRISE     = src/third_party/hyrise_sqlparser
SRCHYRISE  = src/third_party/hyrise_sqlparser/src
SRCPARSER  = src/third_party/hyrise_sqlparser/src/parser

HodorDB: $(SRC)/HodorDB.cpp $(HODORFS)/HodorFS.cpp $(HODORFS)/HodorFS.h $(HODORFS)/BufferManager.cpp $(HODORFS)/BufferManager.h $(SRCHYRISE)/SQLParser.cpp $(SRCHYRISE)/SQLParser.h $(SRCHYRISE)/SQLParserResult.h $(SRCHYRISE)/SQLParserResult.cpp
	g++ -std=c++11 -o $(SRC)/HodorDB $(SRC)/HodorDB.cpp $(HODORFS)/HodorFS.cpp $(HODORFS)/BufferManager.cpp $(SRCHYRISE)/SQLParser.cpp $(SRCHYRISE)/SQLParserResult.cpp