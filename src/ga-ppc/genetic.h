/*
 * @author Davide Bartolini
 * @author Fabio Cancare
 * @author Matteo Carminati
 */
#ifndef GENETIC_H_
#define GENETIC_H_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "ehw.h"

/* CONSTANTS */

#define ALL_ONES 0xFFFFFFFF
#define ALL_ZEROS 0x00000000

//Evolutionary algorithm parameters
#define POPULATION_SIZE 100 //How many individuals within a population
#define MAX_GENERATION_NUMBER 5001 //Max population to be evolved
#define ELITISM_PROB 0.01 //Percentage of fittest individuals that survive among successive generations
//#define MUTATION_PROB 0.9 //Probability of doing mutation
//#define CROSSOVER_PROB 0.6 //Probability of doing crossover
#define SELECTION_FITTEST_PROB 0.6 //Probability of selecting the solution with the highest fitness when comparing two solutions
#define TOURNAMENT_SIZE 8
#define TOURNAMENT_ADVANTAGE 0.55

#define MUTATION_MODULE_LEVEL_BIT 1 //Mutates a single bit per module
#define MUTATION_BIT_LEVEL 2 //Mutates a single bit
#define MUTATION_CELL_LEVEL 3 //Mutates a single cell
#define MUTATION_MODULE_LEVEL 4 //Changes an entire module
#define MUTATION_MULTIBIT_LEVEL 5 //Changes MULTIBIT_NUMBER random bits
#define MUTATION_MULTICELL_LEVEL 6 //Changes MULTICELL_NUMBER cell randomly
#define MUTATION_MIXED_LEVEL 7 //Mutates either some cells (MULTICELL, function level) or some bits (MULTIBIT, gate level)
#define MUTATION_PART_LEVEL_BIT 8 //Mutates a single bit per part


#define MAX_MULTIBIT_NUMBER 12 //how many bits can be changed at most at the MULTIBIT_LEVEL
#define MAX_MULTICELL_NUMBER 4 //How many cells are changed at most at the MULTICELL_LEVEL

#define CROSSOVER_PART_LEVEL 1 //Do crossover at the part level
#define CROSSOVER_MODULE_LEVEL 2 //Do crossover at the module level
#define CROSSOVER_CELL_LEVEL 3 //Do crossover at the cell level

#define LUT_SIZE 16 //How big is a lut
#define LUTS_PER_CELL 2 //How many LUTS are there within a cell
#define CELLS_PER_MODULE 8 //How many cells are there within a module
#define MODULES_PER_PART 4 //How many modules are there within a part

#if BIT32
#define PARTS_PER_CANDIDATE_SOLUTION 16 //How many parts are there within a candidate solution
#else
#define PARTS_PER_CANDIDATE_SOLUTION 1 //How many parts are there within a candidate solution
#endif

#define LUTF 1 //The LUTF is represented with the most significative int32 bits
#define LUTG 0 //The LUTF is represented with the less significative int32 bits

#define COMBINATORIAL 0
#define SEQUENTIAL 1
#define KIND_OF_CIRCUIT COMBINATORIAL //Kind of circuit to be evolved

#define CROSSOVER_LEVEL CROSSOVER_PART_LEVEL //How to perform crossover

//BYTE
typedef char byte;

//LUT
typedef unsigned short int16;

//INT32
typedef unsigned int int32;

//CELL
typedef struct cell_t {
	int32 luts;
} Cell;

//MODULE
typedef struct module_t {
	Cell cells[CELLS_PER_MODULE];
} Module;

//part
typedef struct part_t {
	Module modules[MODULES_PER_PART];
} Part;

//candidate solution
typedef struct candidate_t {
	int fitness;
	Part parts[PARTS_PER_CANDIDATE_SOLUTION];
} Candidate;

//population, composed of a POPULATION_SIZE of candidate solutions
typedef struct population_t {
	Candidate *candidates[POPULATION_SIZE];
	int generation_number;
} Population;

//Functions for managing LUTS
int getBit(Cell cell, int lutN, int n);
void setBit(Cell *cell, int lutN, int n, int bitSet);
void swapBit(Cell *cell, int lutN, int n);

//Functions for creating and initializating populations, candidate solutions, modules and cells
void newPopulation(Population **pop, Population *population, Candidate *candidates);
void initPopulation(Population *pop, int32 *firstPop);
void initModule(Module *module);
void initCell(Cell *cell);

//Genetic operators
void mutation(Candidate *offspring, float probability, int level, float convergency_level);
void naturalSelection(Population *pop, Candidate **can1, Candidate **can2);
void tournamentSelection(Population *pop, Candidate **can1, Candidate **can2);
void crossover(Candidate *parent1, Candidate *parent2, Candidate *offspring1, Candidate *offspring2, float probability, int level);

//Other evolutionary functions
void ordering(Population *pop, int low, int high);
void copyCandidate(Candidate *destination, Candidate *source, int partStart, int partEnd, int modStart, int modEnd, int cellStart, int cellEnd);
void switchPopulation(Population **destination, Population **source);
void printCandidateSolution(int candidate_number, int generation_number, Candidate* candidate);

//Random number generation
void randomInit(long initValue);
int32 cRandom();
int16 halfcRandom();
int iRandom();

//Functions for init candidates, modules and cells from luts
void setCandidate(Candidate *candidate, int32 *luts);
void setPart(Part *part, int32 *luts);
void setModule(Module *module, int32 *luts);
void setCell(Cell *cell, int32 luts);

// Retrieve luts from module
void getLutsModule(int32 *luts[], Module *module);

#endif /*FUNCTIONS_H_*/
