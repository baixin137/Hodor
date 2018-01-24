#include <fstream>
#include <streambuf>
#include "LexicalAnalyzer.h"

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Usage: ./Parser <filename>" << endl;
		exit(1);
	}

	// read SQL from file in to buffer
	ifstream text(argv[1]);
	string buffer((istreambuf_iterator<char>(text)), istreambuf_iterator<char>());

	LexicalAnalyzer* l_analyzer = new LexicalAnalyzer(buffer);

	// vector<string> test = {"a34b", "1244", "adfk2", "8af*", "*"};

	// for (auto s : test){
	// 	if (l_analyzer->is_var(s)) {
	// 		cout << "s is: " << s << endl;
	// 	}
	// }

	vector<pair<string, string>> tokens = l_analyzer->analyze();

	for (auto token : tokens) {
		cout << "Type: " << token.first << ", Token: " << token.second << endl;;
	}

	return 0;
}