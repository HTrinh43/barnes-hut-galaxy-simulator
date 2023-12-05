#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <chrono>
#include "Body.h"
#include "io.h"
const double boundaryXMin = 0.0;
const double boundaryXMax = 4.0;
const double boundaryYMin = 0.0;
const double boundaryYMax = 4.0;


void readInputFile(const char *filename, Body **bodies, int *num_bodies);
void sequential(Body *bodies, int numBodies, double G, double rlimit, double dt, int steps);


int main(int argc, char *argv[]) {

    int numBodies;
    Body *bodies = NULL;

    char *inputfilename = NULL;
    char *outputfilename = NULL;
    int steps = 10;
    double theta = 0.0;
    int visualization_flag = 0;
    const double G = 0.0001;
    const double rlimit = 0.03;  
    double dt = 0.005;
    int print_flag = 0; // 0 means don't print, 1 means print

    // Parse command line arguments
    for (int i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-i") == 0) {
            inputfilename = argv[++i];
        } else if (strcmp(argv[i], "-o") == 0) {
            outputfilename = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0) {
            steps = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-t") == 0) {
            theta = atof(argv[++i]);
        } else if (strcmp(argv[i], "-d") == 0) {
            dt = atof(argv[++i]);
        } else if (strcmp(argv[i], "-V") == 0) {
            visualization_flag = 1;
        }else if (strcmp(argv[i], "-p") == 0) {
            print_flag = 1;
        }
    }

    readInputFile(inputfilename, &bodies, &numBodies);

    // Sequential
    auto start = std::chrono::high_resolution_clock::now();
    sequential(bodies, numBodies, G, rlimit, dt, steps); // Run one step
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << elapsed.count() << std::endl;
    
    // Print the output
    if (print_flag == 1) {
        for (int i = 0; i < numBodies; ++i) {
            bodies[i].print();
        }
    }
    if (outputfilename != NULL) {
        writeOutputData(outputfilename, bodies, numBodies);
    }
    delete[] bodies; // Free the allocated memory
    return 0;
}


void sequential(Body *bodies, int numBodies, double G, double rlimit, double dt, int steps) {
    
    for (int step = 0; step < steps; ++step) {
        // Reset force on each body
        for (int i = 0; i < numBodies; ++i) {
            bodies[i].resetForce();
            if (bodies[i].mass != -1 && 
               (bodies[i].px < boundaryXMin || bodies[i].px > boundaryXMax ||
                bodies[i].py < boundaryYMin || bodies[i].py > boundaryYMax)) {
                bodies[i].mass = -1; // Mark as lost
            }
        }

        // Compute forces
        for (int i = 0; i < numBodies; ++i) {
            for (int j = i + 1; j < numBodies; ++j) {
                bodies[i].addForce(bodies[j], G, rlimit);
                bodies[j].addForce(bodies[i], G, rlimit);
            }
        }

        // Update positions and velocities
        for (int i = 0; i < numBodies; ++i) {
            bodies[i].update(dt);
        }
        // Output the positions for visualization or analysis (optional)
    }
}
