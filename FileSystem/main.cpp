#include "HodorFS.h"

int main() {
	FileManager* fm = new FileManager();
	fm->display_t();
	cout << endl;
	fm->display_p();

	return 0;
}