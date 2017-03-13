#include <stdio.h>
#include <matrix.h>

int main(int argc, char** argv) {
	MAT *A;
	VEC *x;
	PERM *p;

	A = m_get(3, 4);
	x = v_get(10);
	p = px_get(10);

	v_output(x);
	return 0;
}