all: HodorDB

BIN         = bin
SRC         = src
HODORFS     = src/HodorFS
QUERYPARSER = src/QueryParser
HYRISE      = src/third_party/hyrise_sqlparser
SRCHYRISE   = src/third_party/hyrise_sqlparser/src
SRCPARSER   = src/third_party/hyrise_sqlparser/src/parser
HYRISE_H    = $(SRCHYRISE)/SQLParser.h    $(SRCHYRISE)/SQLParserResult.h
HYRISE_SO   = $(HYRISE)/libsqlparser.so

MY_CPP    = $(HODORFS)/HodorFS.cpp $(HODORFS)/BufferManager.cpp $(QUERYPARSER)/QueryParser.cpp $(SRC)/HodorDB.cpp
MY_H      = $(shell find $(SRC) -name '*.h' -not -path "src/third_party/*")
MY_OBJ    = $(MY_CPP:%.cpp=%.o)


# links all "my" compiled .o files with external library
HodorDB: $(MY_OBJ) $(HYRISE_SO)
	g++ -std=c++11 -o HodorDB $(MY_OBJ) $(HYRISE_SO)

# compiles all "my" .cpp files into .o files
%.o: %.cpp
	g++ -std=c++11 -c -o $@ $<

# compiles external library into .so file
$(HYRISE_SO): $(SRCHYRISE)/SQLParser.h $(SRCHYRISE)/SQLParserResult.h
	make -C $(HYRISE) library

clean:
	rm -f */*.o */*/*.o $(HYRISE_SO)