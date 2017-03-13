#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv) {
	VEC *x;
	x = v_get(10);
	v_output(x);

	return EXIT_SUCCESS;
}
