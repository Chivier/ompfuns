#include "chlock.h"
#include <omp.h>

int kl, ku, ks, jl, ju, js, il, iu, is;

void bar(float *a, int i, int j, int k) {
    int index = i + j * iu + k * iu + ju;
    a[index] = rand();
}

void sub(float *a) {
    int i, j, k;
#pragma omp for collapse(2) private(i, k, j)
    for (k = kl; k <= ku; k += ks)
        for (j = jl; j <= ju; j += js)
            for (i = il; i <= iu; i += is)
                bar(a, i, j, k);
}

int main(int argc, char **argv) {
    if (argc == 1) {
        ku = ju = iu = 100;
    } else if (argc != 4) {
        puts("Usage: bottom_test_2 ku ju iu");
        return 1;
    } else {
        ku = atoi(argv[1]);
        ju = atoi(argv[2]);
        iu = atoi(argv[3]);
    }
    kl = jl = il = 0;
    ks = (ku - kl) / 100;
    js = (ju - jl) / 100;
    is = (iu - il) / 100;

    if (ks <= 0) {
        ks = 1;
    }
    if (js <= 0) {
        js = 1;
    }
    if (is <= 0) {
        is = 1;
    }

    float *a = malloc(sizeof(float) * ku * ju * iu);
    int index;
    ChlockSetInterval();
    for (index = 0; index < 100; ++index) {
        sub(a);
    }
    float sum = 0;
    for (index = 0; index < ku * ju * iu; ++index) {
        sum += a[index];
    }
    printf("sum = %f\n", sum);
    ChlockGetInterval(1);
    ChlockFreeInterval();
    free(a);
    return 0;
}