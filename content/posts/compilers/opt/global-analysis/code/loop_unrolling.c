#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void vector_mul(const int *lhs, const int *rhs, int *res, const int dim) {
    for (int i = 0; i < dim; i++) {
        res[i] = lhs[i] * rhs[i];
    }
}

void vector_mul_2(const int *lhs, const int *rhs, int *res, const int dim) {
    for (int i = 0, e = dim / 4 * 4; i < e; i += 4) {
        res[i] = lhs[i] * rhs[i];
        res[i+1] = lhs[i+1] * rhs[i+1];
        res[i+2] = lhs[i+2] * rhs[i+2];
        res[i+3] = lhs[i+3] * rhs[i+3];
    }
    for (int i = dim / 4 * 4; i < dim; i++) {
        res[i] = lhs[i] * rhs[i];
    }
}

int main() {
	int *lhs = (int *)malloc(65536 * sizeof(int));
        for (int i = 0; i < 65536; i++) {
		lhs[i] = rand();
	}
	int *rhs = (int *)malloc(65536 * sizeof(int));
        for (int i = 0; i < 65536; i++) {
		rhs[i] = rand();
	}
	int *res = (int *)malloc(65536 * sizeof(int));
	
	{
		clock_t start = clock();
		vector_mul(lhs, rhs, res, 65536);
		clock_t end = clock();
		printf("%g\n", (end - start) / CLOCKS_PER_SEC);
	}
	{
		clock_t start = clock();
		vector_mul_2(lhs, rhs, res, 65536);
		clock_t end = clock();
		printf("%g\n", (end - start) / CLOCKS_PER_SEC);
	}
}
