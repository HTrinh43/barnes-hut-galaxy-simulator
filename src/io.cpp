
#include "io.h"



void readInputFile(const char *filename, Body **bodies, int *num_bodies) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        fprintf(stderr, "Failed to open input file.\n");
    }

    fscanf(file, "%d", num_bodies);

    *bodies = (Body *)malloc(*num_bodies * sizeof(Body));
    for (int i = 0; i < *num_bodies; i++) {
        fscanf(file, "%d %lf %lf %lf %lf %lf", 
               &(*bodies)[i].ind, &(*bodies)[i].px, &(*bodies)[i].py, 
               &(*bodies)[i].mass, &(*bodies)[i].vx, &(*bodies)[i].vy);
    }
    fclose(file);
}

void writeOutputData(char* outputfilename, Body *bodies, int num_bodies) {
    FILE *file = fopen(outputfilename, "w");
    if (!file) {
        fprintf(stderr, "Failed to open output file.\n");
        return; // Exit the function if file opening failed
    }

    fprintf(file, "%d\n", num_bodies);
    for (int i = 0; i < num_bodies; i++) {
        fprintf(file, "%d %.6e %.6e %.6e %.6e %.6e\n", 
                bodies[i].ind, bodies[i].px, bodies[i].py, 
                bodies[i].mass, bodies[i].vx, bodies[i].vy);
    }
    fclose(file);
}
