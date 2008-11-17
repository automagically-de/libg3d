#include <iostream>

#include <g3d/g3d.h>

using namespace std;

int main(int argc, char *argv[])
{
	G3DContext *context;

	context = g3d_context_new();
	if(context) {
		cout << "got context" << endl;
		g3d_context_free(context);
	}

	return 0;
}
