#ifndef IO_H
#define IO_H

#include "body.h"
#include <stdio.h>
#include <stdlib.h>

// Function declarations
void readInputFile(const char *filename, Body **bodies, int *num_bodies);
void writeOutputData(char* outputfilename, Body *bodies, int num_bodies);

#endif // IO_H
