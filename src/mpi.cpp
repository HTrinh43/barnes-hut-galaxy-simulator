
#include <iostream>
#include <vector>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <chrono>
#include "body.h"
#include "io.h"
#include "node.h"
#include <mpi.h>

const double boundaryXMin = 0.0;
const double boundaryXMax = 4.0;
const double boundaryYMin = 0.0;
const double boundaryYMax = 4.0;


int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int world_rank, world_size;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    

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

    // MPI_Datatype mpi_body_type = create_mpi_body_type();
    const int nitems = 8; // Number of elements in the Body class
    int blocklengths[nitems] = {1, 1, 1, 1, 1, 1, 1, 1}; // All elements are single values

    // Calculate displacements of each element
    MPI_Aint offsets[nitems];
    offsets[0] = offsetof(Body, mass);
    offsets[1] = offsetof(Body, px);
    offsets[2] = offsetof(Body, py);
    offsets[3] = offsetof(Body, vx);
    offsets[4] = offsetof(Body, vy);
    offsets[5] = offsetof(Body, fx);
    offsets[6] = offsetof(Body, fy);
    offsets[7] = offsetof(Body, ind);

    MPI_Datatype types[nitems] = {MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_DOUBLE, MPI_INT};
    
    MPI_Datatype mpi_body_type;
    MPI_Type_create_struct(nitems, blocklengths, offsets, types, &mpi_body_type);
    MPI_Type_commit(&mpi_body_type);

    if (world_rank == 0) {
        readInputFile(inputfilename, &bodies, &numBodies);
    }


    MPI_Bcast(&numBodies, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int num_bodies_per_process = numBodies / world_size;
    int start_index = world_rank * num_bodies_per_process;
    int end_index = (world_rank + 1) * num_bodies_per_process - 1;
    if (world_rank == world_size - 1) {
        end_index = numBodies - 1;
    }





    double start;
    double end;
    if (world_rank == 0) {
        start = MPI_Wtime();
    }


    // Allocate memory for local processing results
    std::vector<Body> local_results;

    if (world_rank != 0) {
        bodies = (Body *)malloc(numBodies * sizeof(Body));
    }

    // Sequential
    // auto start = std::chrono::high_resolution_clock::now();
    int err = 0;
    for (int a = 0; a < steps; a++) {

        // Allocate memory for bodies on all processes
        if (world_size > 1) {
            err = MPI_Bcast(bodies, numBodies, mpi_body_type, 0, MPI_COMM_WORLD);
            if (err != MPI_SUCCESS) {
                std::cerr << "MPI_Bcast failed on process " << world_rank << std::endl;
            }
        }
        if (bodies != nullptr) {
            for (int i = 0; i < numBodies; ++i) {
                bodies[i].resetForce();
                if (bodies[i].mass != -1 && 
                (bodies[i].px < boundaryXMin || bodies[i].px > boundaryXMax ||
                    bodies[i].py < boundaryYMin || bodies[i].py > boundaryYMax)) {
                    bodies[i].mass = -1; // Mark as lost
                }
            }

            Node *root = new Node(2, 2, 4);
            for (int i = 0; i < numBodies; i++) {
            root->insertBody(&bodies[i]);
            }
            for (int j = start_index; j <= end_index; j++) {
                if (bodies[j].mass == -1) continue;
                // check if body is within boundary
                if (!root->isWithinBoundary(&bodies[j])) {
                    bodies[j].mass = -1;
                    continue;
                }
                // bodies[i].resetForce();
                Vec2 vec = root->calculateForceForBody(&bodies[j], theta, G, rlimit);
                bodies[j].fx += vec.x;
                bodies[j].fy += vec.y;
                bodies[j].update(dt);
                local_results.push_back(bodies[j]);
            }
            delete root;
            root = nullptr;
        } else {
            std::cerr << "Error: bodies is NULL on process " << world_rank << std::endl;
        }

    if (world_size > 1) {

    // send the local results to process 0
        int local_result_count = local_results.size();
        std::vector<int> recvcounts(world_size);
        std::vector<int> displs(world_size);
        err = MPI_Gather(&local_result_count, 1, MPI_INT, recvcounts.data(), 1, MPI_INT, 0, MPI_COMM_WORLD);
        if (err != MPI_SUCCESS) {
            std::cerr << "MPI_Bcast failed on process " << world_rank << std::endl;
            // Handle the error...
        }
        std::vector<Body> all_results;
        if (world_rank == 0) {
            // Prepare the displacements and the array to hold all results
            int total_count = 0;
            for (int i = 0; i < world_size; ++i) {
                displs[i] = total_count;
                total_count += recvcounts[i];
            }
            all_results.resize(total_count);
        }
        err = MPI_Gatherv(local_results.data(), local_result_count, mpi_body_type,
                    all_results.data(), recvcounts.data(), displs.data(), mpi_body_type,
                    0, MPI_COMM_WORLD);

        if (err != MPI_SUCCESS) {
            std::cerr << "MPI_Bcast failed on process " << world_rank << std::endl;
            // Handle the error...
        }
        
        if (world_rank == 0) {
            delete[] bodies;
            bodies = new Body[numBodies];

            // Copy results from all_results to bodies
            std::copy(all_results.begin(), all_results.end(), bodies);

        }
        local_results.clear();
        all_results.clear();
        }
    }
    // printf("worldrank %d End loop\n", world_rank);

    if (print_flag == 1 && world_rank == 0) {
        std::cout << "Process " << world_rank <<  std::endl;
        for (int i = 0; i < numBodies; ++i) {
            bodies[i].print();
        }
    }

    // Write output data
    // if (outputfilename != NULL) {
    //     writeOutputData(outputfilename, bodies, numBodies);
    // }
    if (world_rank == 0) {
        // Stop the timer on process 0
        end = MPI_Wtime();
        std::cout << (end - start) << std::endl;
    }
    if (outputfilename != NULL) {
        writeOutputData(outputfilename, bodies, numBodies);
    }
    // Free memory
    delete bodies;
    MPI_Type_free(&mpi_body_type);
    MPI_Finalize();

    return 0;
}

