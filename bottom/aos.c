#include "chlock.h"
#include <omp.h>

/*
- OMPFUN BOTTOM TEST 1: AOS
AOS(Array of Structure) is a very common case in parallel computing, as well as SOA(Structure of Array). Actually, they
can both represent the structure in real world.
In this test, we will build a structure of particles. Store them in Array of Stucture. And make some calculation and
statistical work.

- Usage:
bottom_test_1 [level]
[level]: the number of particles is 10 ^ {level}, default level is 2
*/

struct particle_t {
    float x;
    float y;
    float z;
    // velocity
    float vx;
    float vy;
    float vz;
};

int main(int argc, char **argv) {
    srand(time(NULL));
    int level = 2;
    if (argc > 3) {
        puts("Too many arguments!");
        return 1;
    } else if (argc == 2) {
        level = atoi(argv[1]);
    }
    printf("Bototm test 1: AOS\n");
    printf("Level is %d\n", level);
    ChlockSetInterval();

    // Generating particles
    printf("Generating Particles:\n");
    int particle_number = 1;
    int index;

    for (index = 0; index < level; ++index) {
        particle_number *= 10;
    }

    struct particle_t *particles = (struct particle_t *)malloc(sizeof(struct particle_t) * particle_number);

    for (index = 0; index < particle_number; ++index) {
        particles[index].x = 10.0 * rand() / RAND_MAX;
        particles[index].y = 10.0 * rand() / RAND_MAX;
        particles[index].z = 10.0 * rand() / RAND_MAX;
        particles[index].vx = 3.0 * rand() / RAND_MAX;
        particles[index].vy = 3.0 * rand() / RAND_MAX;
        particles[index].vz = 3.0 * rand() / RAND_MAX;
    }

    printf("Generated %d particles\n", particle_number);
    ChlockGetInterval(1);

    // Simulation for a time step
    int step;
    for (step = 0; step < 100; ++step) {
        float delta_time = 5.0 * rand() / RAND_MAX;
#pragma omp parallel for
        for (index = 0; index < particle_number; ++index) {
            particles[index].x += particles[index].vx * delta_time;
            particles[index].y += particles[index].vy * delta_time;
            particles[index].z += particles[index].vz * delta_time;
        }
    }
    printf("100 turns Simulation is done!\n");
    ChlockGetInterval(1);

    // Statistics for particles
    struct particle_t mean_particle;
    struct particle_t min_particle;
    struct particle_t max_particle;

    mean_particle = particles[0];
    min_particle = particles[0];
    max_particle = particles[0];

    float mean_x = particles[0].x;
    float mean_y = particles[0].y;
    float mean_z = particles[0].z;
    float min_x = particles[0].x;
    float min_y = particles[0].y;
    float min_z = particles[0].z;
    float max_x = particles[0].x;
    float max_y = particles[0].y;
    float max_z = particles[0].z;

#pragma omp parallel for \
reduction(+ : mean_x) \
reduction(+ : mean_y) \
reduction(+ : mean_z) \
reduction(min : min_x) \
reduction(min : min_y) \
reduction(min : min_z) \
reduction(max : max_x) \
reduction(max : max_y) \
reduction(max : max_z)
    for (index = 1; index < particle_number; ++index) {
        // mean particle
        mean_particle.x += particles[index].x;
        mean_particle.y += particles[index].y;
        mean_particle.z += particles[index].z;
        // min particle
        if (particles[index].x < min_particle.x) {
            min_particle.x = particles[index].x;
        }
        if (particles[index].y < min_particle.y) {
            min_particle.y = particles[index].y;
        }
        if (particles[index].z < min_particle.z) {
            min_particle.z = particles[index].z;
        }
        // max particle
        if (particles[index].x > max_particle.x) {
            max_particle.x = particles[index].x;
        }
        if (particles[index].y > max_particle.y) {
            max_particle.y = particles[index].y;
        }
        if (particles[index].z > max_particle.z) {
            max_particle.z = particles[index].z;
        }
    }
    mean_particle.x /= particle_number;
    mean_particle.y /= particle_number;
    mean_particle.z /= particle_number;
    printf("Location is done!\n");
    ChlockGetInterval(1);

    float x_range_unit = (max_particle.x - min_particle.x) / 10;
    float y_range_unit = (max_particle.y - min_particle.y) / 10;
    float z_range_unit = (max_particle.z - min_particle.z) / 10;
    float stx, sty, stz, stvx, stvy, stvz;
    int x_offset_index, y_offset_index, z_offset_index;
    float total = 0;

    for (x_offset_index = 0; x_offset_index < 10; ++x_offset_index) {
        for (y_offset_index = 0; y_offset_index < 10; ++y_offset_index) {
            for (z_offset_index = 0; z_offset_index < 10; ++z_offset_index) {
                stx = 0;
                sty = 0;
                stz = 0;
                stvx = 0;
                stvy = 0;
                stvz = 0;
                float x_range = min_particle.x + x_offset_index * x_range;
                float y_range = min_particle.y + y_offset_index * y_range;
                float z_range = min_particle.z + z_offset_index * z_range;
#pragma omp parallel for \
                reduction (+:stx) \
                reduction(+:sty) \
                reduction(+:stz) \
                reduction(+:stvx) \
                reduction(+:stvy) \
                reduction(+:stvz)
                for (index = 0; index < particle_number; ++index) {
                    if (particles[index].x > mean_particle.x - x_range &&
                        particles[index].x < mean_particle.x + x_range &&
                        particles[index].y > mean_particle.y - y_range &&
                        particles[index].y < mean_particle.y + y_range &&
                        particles[index].z > mean_particle.z - z_range &&
                        particles[index].z < mean_particle.z + z_range) {
                        stx += particles[index].x;
                        sty += particles[index].y;
                        stz += particles[index].z;
                        stvx += particles[index].vx;
                        stvy += particles[index].vy;
                        stvz += particles[index].vz;
                    }
                } // particles
                total += stx;
                total -= sty;
                total += stz;
                total -= stvx;
                total += stvy;
                total -= stvz;
            } // z offset index
        }     // y offset index
    }         // x offset index

    printf("total: %f\n", total);
    printf("Reduction is done!\n");
    ChlockGetInterval(1);
    ChlockFreeInterval();
    free(particles);
    return 0;
}
