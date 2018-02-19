#include "HodorFS.h"
#include "BufferManager.h"

int main() {
	FileManager* fm = new FileManager();
	// fm->display_t();
	// cout << endl;
	// fm->display_p();

	BufferManager* bf = new BufferManager(10);
	bf->fetch(1);
	bf->fetch(2);

	bf->buffer->display();

	bf->flush(1);

	bf->buffer->display();

	return 0;
}