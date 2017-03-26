#include <matrix.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char** argv) {
	sleep(5);
	VEC *x;
	x = v_get(10);
	v_output(x);

	return EXIT_SUCCESS;
}
