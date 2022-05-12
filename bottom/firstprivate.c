#include "chlock.h"
#include <omp.h>

#define NT 4
int main() {
    int section_count = 0;
    omp_set_dynamic(0);
    omp_set_num_threads(NT);
#pragma omp parallel
#pragma omp sections firstprivate(section_count)
    {
#pragma omp section
        {
            section_count++;
            /* may print the number one or two */
            printf("section_count %d\n", section_count);
        }
#pragma omp section
        {
            section_count++;
            /* may print the number one or two */
            printf("section_count %d\n", section_count);
        }
    }
    return 0;
}