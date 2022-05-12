#include "chlock.h"
#include <omp.h>

int main(int argc, char **argv) {
    printf("Example 15.15: x should be 2\n");
    int x = 1;
#pragma omp parallel
    {
#pragma omp single nowait
        {
#pragma omp task shared(x) depend(out : x)
            { x = 2; }

#pragma omp task shared(x) depend(in : x)
            { printf("x = %d\n", x); }
        }
    }

    printf("Example 15.16: x should be 1\n");
    x = 1;
#pragma omp parallel
    {
#pragma omp single
        {
#pragma omp task shared(x) depend(in : x)
            { printf("x = %d\n", x); }

#pragma omp task shared(x) depend(out : x)
            { x = 2; }
        }
    }

    printf("Example 15.17: x should be 2\n");
    x = 0;
#pragma omp parallel
    {
#pragma omp single
        {
#pragma omp task shared(x) depend(out : x)
            { x = 1; }

#pragma omp task shared(x) depend(out : x)
            { x = 2; }
#pragma omp taskwait
            printf("x = %d\n", x);
        }
    }

    return 0;
}
