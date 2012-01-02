/*
 * @title Hera Project
 * @part Implementation of the genetic operators
 * @author Davide Bartolini
 * @author Fabio Cancare
 * @author Matteo Carminati
 */

#include "ehw.h"
#include "genetic.h"
#include "funcCell.h"

void printCandidateSolution(int candidate_number, int generation_number, Candidate* candidate) {
	int i, j, k;
	
	printf("########Candidate solution %d Generation %d, pointer: %08X (LUT content)########\n", candidate_number, generation_number, (unsigned int)candidate);
	printf("fitness: %d\n", candidate->fitness);
	for(i = 0; i < PARTS_PER_CANDIDATE_SOLUTION; i++) {
		printf("I%d: ", i);
		for(j = 0; j < MODULES_PER_PART; j++) {
			printf("M%d: ", j);
			for(k = 0; k < CELLS_PER_MODULE; k++) {
				printf("%d- %08X ", k, candidate->parts[i].modules[j].cells[k].luts);
			}
			printf("\n");
		}
	}
	printf("########End candidate solution########\n");
}

 /*
  * Get the n-th bit of a LUT
  * @param Cell cell: the Cell to be examined
  * @param int lutN: the Lut to be examined (LUTF or LUTG)
  * @param int bitN: the index of the bit
  *	@returns int the bit value
  */
int getBit(Cell cell, int lutN, int n) {
	int32 mask = 1 << (n + LUT_SIZE * lutN);
	if (mask & cell.luts) {
		return 1;
	}
	return 0;
}
/*
 * Set the n-th bit of a LUT
 * @param Cell cell: the Cell to be changed
 * @param int lutN: the LUT to be changed (LUTF or LUTG)
 * @param int bitN: the index of the bit to be changed
 * @param int bitSet: the new bit value (0 or 1)
 */
void setBit(Cell *cell, int lutN, int n, int bitSet) {
	int32 mask = 1 << (n + LUT_SIZE * lutN);
	
	if(bitSet == 1) {
		cell->luts = cell->luts | mask;
	}
	else if (bitSet == 0) {
		mask = ~mask;
		cell->luts = cell->luts & mask;
	}
	return;
}
/*
 * Swap the n-th bit of a LUT
 * @param Cell cell: the Cell to be changed
 * @param int lutN: the LUT to be changed (LUTF or LUTG)
 * @param int bitN: the index of the bit to be swapped
 */

void swapBit(Cell *cell, int lutN, int n) {
	int32 mask = 1 << (n + LUT_SIZE * lutN);
	cell->luts = cell->luts ^ mask;
	return;
}

/*
 * Init a new population
 * @param Population *pop: pointer to the new population
 * @param Population population: structure of the population
 * @param Candidate *candidates: pointer to the array of the candidate solutions within the population
 */
 void newPopulation(Population **pop, Population *population, Candidate *candidates) {
 	int i;
 	*pop = population;
 	if(DEBUG_LEVEL >= 3) {
 		printf("creates a new population at the address: %X\n\r", (unsigned int)*pop);
 	}
 	for(i = 0; i < POPULATION_SIZE; i++) {
 		(*pop)->candidates[i] = &candidates[i];
 	}
 }

 /*
 * Create random individuals within the given population or copy them from an existing population
 *	@param Population *pop: la population da inizializzare
 *	@param firstPop unsigned int: the population to be cloned (set to NULL to create new random candidates)
 */
void initPopulation(Population *pop, unsigned int *firstPop) {
	int i, j, k;
	//if(firstPop == NULL) {
	for(i = 0; i < POPULATION_SIZE; i++) {
		pop->candidates[i]->fitness = 0;
		for(j = 0; j < PARTS_PER_CANDIDATE_SOLUTION; j++) {
			for(k = 0; k < MODULES_PER_PART; k++) {
				initModule(&(pop->candidates[i]->parts[j].modules[k]));
			}
		}
	}
	//}
	/*else {
		for(i = 0; i < POPULATION_SIZE; i++) {
			pop->candidates[i]->fitness = 0;
			for(j = 0; j < PARTS_PER_CANDIDATE_SOLUTION; j++) {
				for(k = 0; k < MODULES_PER_PART; k++) {
					for(l = 0; l < CELLS_PER_MODULE; l++) {
						pop->candidates[i]->parts[j].modules[k].cells[l].luts = firstPop[32 * PARTS_PER_CANDIDATE_SOLUTION * i + 32 * j + 8 * k + l];
					}
				}
			}
		}
	}*/
	pop->generation_number = 0;
	return;
}

/*
 * Init a module
 * @param Module* module: the module to be initialized
 */
void initModule(Module *module) {
	int i;
	for(i = 0; i < CELLS_PER_MODULE; i++) {
		initCell(&(module->cells[i]));
	}
	return;
}

/*
 * Init a cell
 * @param Cell *cell: the cell to be initialized
 */
void initCell(Cell *cell) {
	int32 ran;
	int32 temp;
	//If the circuit is combinatorial then the two LUTs within the cell must be identical
	if(KIND_OF_CIRCUIT == COMBINATORIAL) {
		ran = halfFcWeightedRandom();
		temp = ran << 16;
		cell->luts = temp | ran;
	}
	else {
		ran = fcWeightedRandom();
		cell->luts = ran;
	}
	return;
}

/*
 * Apply (with a given probability) the mutation operator (at a given granularity level) to a candidate solutions
 * @param int* offspring: the candidate solution to be mutated
 * @param int  probability: mutation probability
 * @param int  level: mutation granularity level (MODULE_LEVEL || CELL_LEVEL || MODULE_LEVEL_BIT || CELL_LEVEL_BIT BIT_LEVEL)
 * @param float convergency_level: how far is the best solution obtained from the optimal fitness
 */
void mutation(Candidate *offspring, float probability, int level, float convergency_level) {
 	int indRan, modRan, celRan, lutRan, bitRan, multi_num;
 	int i, j;
 	if((float)(iRandom()) / (float)(POPULATION_SIZE) < probability) {
		switch(level) {
			//Change an entire module
			case MUTATION_MODULE_LEVEL:
				indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
				modRan = rand() % MODULES_PER_PART;
				initModule(&(offspring->parts[indRan].modules[modRan]));
			break;

			//Change an entire cell
			case MUTATION_CELL_LEVEL:
				indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
				modRan = rand() % MODULES_PER_PART;
				celRan = rand() % CELLS_PER_MODULE;
				initCell(&(offspring->parts[indRan].modules[modRan].cells[celRan]));
			break;

			//Change a single bit
			case MUTATION_BIT_LEVEL:
				indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
				modRan = rand() % MODULES_PER_PART;
				celRan = rand() % CELLS_PER_MODULE;
				bitRan = rand() % LUT_SIZE;
				if(KIND_OF_CIRCUIT == COMBINATORIAL) {
					if(DEBUG_LEVEL >= 3) {
						printf("\nP:level_bit: %08X\n",offspring->parts[indRan].modules[modRan].cells[celRan].luts);
					}
					swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], 0, 15 - bitRan);
					swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], 1, 15 - bitRan);
					if(DEBUG_LEVEL >= 3) {
						printf("\nD:level_bit: %08X\n",offspring->parts[indRan].modules[modRan].cells[celRan].luts);
					}
				}
				else {
					lutRan = rand() % LUTS_PER_CELL;
					swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], lutRan, 15 - bitRan);
				}
			break;

			//Change a bit for every module of a given part
			case MUTATION_MODULE_LEVEL_BIT:
				indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
				for(i = 0; i < MODULES_PER_PART; i++) {
					celRan = rand() % CELLS_PER_MODULE;
					bitRan = rand() % LUT_SIZE;
					if(KIND_OF_CIRCUIT == COMBINATORIAL) {
						if(DEBUG_LEVEL >= 3) {
							printf("\nP:level_bit: %08X\n",offspring->parts[indRan].modules[i].cells[celRan].luts);
						}
						swapBit(&offspring->parts[indRan].modules[i].cells[celRan], 0, 15 - bitRan);
						swapBit(&offspring->parts[indRan].modules[i].cells[celRan], 1, 15 - bitRan);
						if(DEBUG_LEVEL >= 3) {
							printf("\nD:level_bit: %08X\n",offspring->parts[indRan].modules[i].cells[celRan].luts);
						}
					}
					else {
						lutRan = rand() % LUTS_PER_CELL;
						swapBit(&offspring->parts[indRan].modules[i].cells[celRan], lutRan, 15 - bitRan);
					}
				}
			break;

			//Change MULTIBIT_NUMBER bits
			case MUTATION_MULTIBIT_LEVEL:
				multi_num = (rand() % MAX_MULTIBIT_NUMBER) + 1;
				for(i = 0; i < multi_num; i++) {
					indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
					modRan = rand() % MODULES_PER_PART;
					celRan = rand() % CELLS_PER_MODULE;
					bitRan = rand() % LUT_SIZE;
					if(KIND_OF_CIRCUIT == COMBINATORIAL) {
						swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], 0, 15 - bitRan);
						swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], 1, 15 - bitRan);
					}
					else {
						lutRan = rand() % LUTS_PER_CELL;
						swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], lutRan, 15 - bitRan);
					}
				}
			break;

			//Change MULTICELL_NUMBER cells
			case MUTATION_MULTICELL_LEVEL:
				multi_num = (rand() % MAX_MULTICELL_NUMBER) + 1;
				for(i = 0; i < multi_num; i++) {
					indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
					modRan = rand() % MODULES_PER_PART;
					celRan = rand() % CELLS_PER_MODULE;
					initCell(&(offspring->parts[indRan].modules[modRan].cells[celRan]));
				}
			break;

			//Change a bit in every cell of a given part
			case MUTATION_PART_LEVEL_BIT:
				indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
				for(i = 0; i < MODULES_PER_PART; i++) {
					for(j = 0; j < CELLS_PER_MODULE; j++) {
						bitRan = rand() % LUT_SIZE;
						if(KIND_OF_CIRCUIT == COMBINATORIAL) {
							if(DEBUG_LEVEL >= 3) {
								printf("\nP:level_cell_bit: %08X, %d \n",offspring->parts[indRan].modules[i].cells[j].luts, bitRan);
							}
							swapBit(&offspring->parts[indRan].modules[i].cells[j], 0, 15 - bitRan);
							swapBit(&offspring->parts[indRan].modules[i].cells[j], 1, 15 - bitRan);
							if(DEBUG_LEVEL >= 3) {
								printf("\nD:level_cell_bit: %08X\n",offspring->parts[indRan].modules[i].cells[j].luts);
							}
						}
						else {
							lutRan = rand() % LUTS_PER_CELL;
							swapBit(&offspring->parts[indRan].modules[i].cells[j], lutRan, 15 - bitRan);
						}
					}
				}
			break;

			//Mutates either some cells (MULTICELL, function level) or some bits (MULTIBIT, gate level)
			case MUTATION_MIXED_LEVEL:
				//MULTICELL, function level
				if(convergency_level < 0.95) {
					multi_num = rand() % MAX_MULTICELL_NUMBER;
					for(i = 0; i < multi_num; i++) {
						indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
						modRan = rand() % MODULES_PER_PART;
						celRan = rand() % CELLS_PER_MODULE;
						initCell(&(offspring->parts[indRan].modules[modRan].cells[celRan]));
					}
				}
				//MULTIBIT, gate level
				else {
					multi_num = (rand() % MAX_MULTIBIT_NUMBER) + 1;
					for(i = 0; i < multi_num; i++) {
						indRan = rand() % PARTS_PER_CANDIDATE_SOLUTION;
						modRan = rand() % MODULES_PER_PART;
						celRan = rand() % CELLS_PER_MODULE;
						bitRan = rand() % LUT_SIZE;
						if(KIND_OF_CIRCUIT == COMBINATORIAL) {
							swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], 0, 15 - bitRan);
							swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], 1, 15 - bitRan);
						}
						else {
							lutRan = rand() % LUTS_PER_CELL;
							swapBit(&offspring->parts[indRan].modules[modRan].cells[celRan], lutRan, 15 - bitRan);
						}
					}
				}
			break;

			default:
				printf("Invalid mutation level\n");
				exit(1);
			break;
		}
 	}
 	offspring->fitness = 0;
 	return;
}

/*
 * Selects two parents for creating new offsprings
 * @param Population *pop: population within to choose
 * @param Candidate *ind1, *ind2: pointers that will be set to the addresses of the candidates selected
 * @param float advantage: how much fitter candidates are advantaged over other candidates
 */
void naturalSelection(Population *pop, Candidate **can1, Candidate **can2) {
	int i;
	int fitnessNorm;
	int fitness;
	int choice1 = 0, choice2 = 0;
	int max1 = -1;
	int max2 = -1;

	for(i = 0; i < POPULATION_SIZE; i++) {
		fitnessNorm = (pop->candidates[i]->fitness * 1000) / pop->candidates[0]->fitness;
		fitness = fitnessNorm * SELECTION_FITTEST_PROB + rand() % 1000;
		if(fitness > max1) {
			choice2 = choice1;
			max2 = max1;
			choice1 = i;
			max1 = fitness;
		}
		else if(fitness > max2) {
			choice2 = i;
			max2 = fitness;
		}
	}
	*can1 = (pop->candidates[choice1]);
	*can2 = (pop->candidates[choice2]);
	
	if(DEBUG_LEVEL >= 3) {
		printf("Selected %d, %d, with fitness %d, %d\n", choice1, choice2, pop->candidates[choice1]->fitness, pop->candidates[choice2]->fitness);
		printf("Addresses: %08X, %08X, (%08X, %08X)\n", (unsigned int)*can1, (unsigned int)*can2, (unsigned int)can1, (unsigned int)can2);
	}
	return;
}

/*
 * Selects two parents for creating new offsprings
 * @param Population *pop: population within to choose
 * @param Candidate *ind1, *ind2: pointers that will be set to the addresses of the candidates selected
 */
void tournamentSelection(Population *pop, Candidate **can1, Candidate **can2) {
	int i, choice1, choice2;
	int participants[TOURNAMENT_SIZE];

	for(i = 0; i < TOURNAMENT_SIZE; i++) {
		participants[i] = rand() % POPULATION_SIZE;
	}
	choice1 = participants[0];
	choice2 = participants[1];
	for(i = 2; i < TOURNAMENT_SIZE; i++) {
		if(i%2==0) {
			if(pop->candidates[participants[i]]->fitness > pop->candidates[choice1]->fitness) {
				if((float)((float)rand()/INT_MAX) < TOURNAMENT_ADVANTAGE) {
					choice1 = participants[i];
				}
			}
		}
		else {
			if(pop->candidates[participants[i]]->fitness > pop->candidates[choice2]->fitness) {
				if((float)((float)rand()/INT_MAX) < TOURNAMENT_ADVANTAGE) {
					choice2 = participants[i];
				}
			}
		}
	}
	*can1 = (pop->candidates[choice1]);
	*can2 = (pop->candidates[choice2]);
	
	if(DEBUG_LEVEL >= 3) {
		printf("Selected %d, %d, with fitness %d, %d\n", choice1, choice2, pop->candidates[choice1]->fitness, pop->candidates[choice2]->fitness);
		printf("Addresses: %08X, %08X, (%08X, %08X)\n", (unsigned int)*can1, (unsigned int)*can2, (unsigned int)can1, (unsigned int)can2);
	}
	return;
}

/*
 * Apply (with a given probability) the crossover operator (at a given granularity level) for the creation of two offsprings
 * @param Candidate *parent1: the first parent
 * @param Candidate *parent2: the second parent
 * @param Candidate *offspring1: the first offspring
 * @param Candidate *offspring2: the second offspring
 * @param float probability: crossover probability
 * @param int level: crossover granularity level (CROSSOVER_PART_LEVEL || CROSSOVER_MODULE_LEVEL || CROSSOVER_CELL_LEVEL)
 */
void crossover(Candidate *parent1, Candidate *parent2, Candidate *offspring1, Candidate *offspring2, float probability, int level) {
	int part, module, cell = 0;
	float chance = (float)iRandom() / (float)(POPULATION_SIZE);
	if(chance < probability) {
		switch(level) {
			case CROSSOVER_PART_LEVEL:
				if(PARTS_PER_CANDIDATE_SOLUTION == 1) {
					part = 1;
				}
				else {
					do {
						part = (rand() % PARTS_PER_CANDIDATE_SOLUTION);
					} while (part == 0);
				}
				copyCandidate(offspring1, parent1, 0, part, 0, MODULES_PER_PART, 0, CELLS_PER_MODULE);
				copyCandidate(offspring1, parent2, part, PARTS_PER_CANDIDATE_SOLUTION, 0, MODULES_PER_PART, 0, CELLS_PER_MODULE);
				copyCandidate(offspring2, parent2, 0, part, 0, MODULES_PER_PART, 0, CELLS_PER_MODULE);
				copyCandidate(offspring2, parent1, part, PARTS_PER_CANDIDATE_SOLUTION, 0, MODULES_PER_PART, 0, CELLS_PER_MODULE);
			break;
			case CROSSOVER_MODULE_LEVEL:
				if(PARTS_PER_CANDIDATE_SOLUTION == 1) {
					part = 1;
				}
				else {
					do {
						part = (rand() % PARTS_PER_CANDIDATE_SOLUTION);
					} while (part == 0);
				}
				if(MODULES_PER_PART == 1) {
				 module = 1;
				}
				else {
					do {
						module = (rand() % MODULES_PER_PART);
					} while (module == 0);
				}
				copyCandidate(offspring1, parent1, 0, part, 0, module, 0, CELLS_PER_MODULE);
				copyCandidate(offspring1, parent2, part, PARTS_PER_CANDIDATE_SOLUTION, module, MODULES_PER_PART, 0, CELLS_PER_MODULE);
				copyCandidate(offspring2, parent2, 0, part, 0, module, 0, CELLS_PER_MODULE);
				copyCandidate(offspring2, parent1, part, PARTS_PER_CANDIDATE_SOLUTION, module, MODULES_PER_PART, 0, CELLS_PER_MODULE);
			break;
			case CROSSOVER_CELL_LEVEL:
				if(PARTS_PER_CANDIDATE_SOLUTION == 1) {
					part = 1;
				}
				else {
					do {
						part = (rand() % PARTS_PER_CANDIDATE_SOLUTION);
					} while (part == 0);
				}
				if(MODULES_PER_PART == 1) {
					module = 1;
				}
				else {
					do {
						module = (rand() % MODULES_PER_PART);
					} while (module == 0);
				}
				if(CELLS_PER_MODULE == 1) {
					cell = 1;
				}
				else {
					do {
						cell = (rand() % CELLS_PER_MODULE);
					} while (cell == 0);
				}
				copyCandidate(offspring1, parent1, 0, part, 0, module, 0, cell);
				copyCandidate(offspring1, parent2, part, PARTS_PER_CANDIDATE_SOLUTION, module, MODULES_PER_PART, cell, CELLS_PER_MODULE);
				copyCandidate(offspring2, parent2, 0, part, 0, module, 0, cell);
				copyCandidate(offspring2, parent1, part, PARTS_PER_CANDIDATE_SOLUTION, module, MODULES_PER_PART, cell, CELLS_PER_MODULE);
			break;

			default:
				printf("Crossover level not yet implemented\n\r");
				exit(-1);
			break;
		}
	}
	else {
		copyCandidate(offspring1, parent1, 0, PARTS_PER_CANDIDATE_SOLUTION, 0, MODULES_PER_PART, 0, CELLS_PER_MODULE);
		copyCandidate(offspring2, parent2, 0, PARTS_PER_CANDIDATE_SOLUTION, 0, MODULES_PER_PART, 0, CELLS_PER_MODULE);
	}
	offspring1->fitness = 0;
	offspring2->fitness = 0;
}

/*
 * Recursively rder a population according to the candidate solution fitness
 * @param Population *pop: the population to be ordered
 * @param int low: lowerbound population index
 * @param int high: upperbound population index
 */
void ordering(Population *pop, int low, int high) {
	if(DEBUG_LEVEL >= 2) printf("ordering, from %d to %d\n", low, high);
	int i = 0;
	int length = high - low + 1;
	int pivot  = 0;
	int merge1 = 0;
	int merge2 = 0;
	Candidate *working[POPULATION_SIZE];


	if(low == high) {
		return;
	}

	pivot  = (low + high) / 2;

	if(pivot > low) ordering(pop, low, pivot);
	if(high > (pivot + 1))ordering(pop, pivot + 1, high);
 
	for(i = 0; i < length; i++) {
		working[i] = pop->candidates[low + i];
	}

	if(DEBUG_LEVEL >= 2) {
		for(i = 0; i < POPULATION_SIZE; i++) {
			printf("population fitness %d: %d (%08X)\n", i, pop->candidates[i]->fitness, (unsigned int)pop->candidates[i]);
			printf("working fitness %d: %d (%08X)\n", i, working[i]->fitness, (unsigned int)working[i]);
		}
	}

	merge1 = 0;
	merge2 = pivot - low + 1;

	for(i = 0; i < length; i++) {
		if(merge2 <= high - low) {
			if(merge1 <= pivot - low) {
				if(DEBUG_LEVEL >= 2) printf("comparing fitness %d with fitness %d\n", working[merge1]->fitness, working[merge2]->fitness);
				if(working[merge1]->fitness < working[merge2]->fitness) {
					if(DEBUG_LEVEL >= 2) printf("swapping %d (%08X) with %d (%08X)\n", (i+low), (unsigned int)pop->candidates[i + low], merge2, (unsigned int)working[merge2]);
					pop->candidates[i + low] = working[merge2++];
				}
				else {
					if(DEBUG_LEVEL >= 2) printf("swapping %d (%08X) with %d (%08X)\n", (i+low), (unsigned int)pop->candidates[i + low], merge1, (unsigned int)working[merge1]);
					pop->candidates[i + low] = working[merge1++];
				}
			}
			else {
				if(DEBUG_LEVEL >= 2) printf("swapping %d (%08X) with %d (%08X)\n", (i+low), (unsigned int)pop->candidates[i + low], merge2, (unsigned int)working[merge2]);
				pop->candidates[i + low] = working[merge2++];
			}
		}
		else {
			if(DEBUG_LEVEL >= 2) printf("swapping %d (%08X) with %d (%08X)\n", (i+low), (unsigned int)pop->candidates[i + low], merge1, (unsigned int)working[merge1]);
			pop->candidates[i + low] = working[merge1++];
		}
	}
	return;
}

/*
 * Clone (parts of) a candidate solution
 * @param Candidate *destination
 * @param Candidate *source
 * @param int partStart: initial candidate solution part to be cloned
 * @param int partEnd: final candidate solution part to be cloned
 * @param int modStart: initial candidate solution module within a part to be cloned
 * @param int modEnd: final candidate solution module within a part to be cloned
 * @param int cellStart: initial candidate solution cell within a module within a part to be cloned
 * @param int cellEnd: final candidate solution cell within a module within a part to be cloned
 */
void copyCandidate(Candidate *destination, Candidate *source, int partStart, int partEnd, int modStart, int modEnd, int cellStart, int cellEnd) {
	if(DEBUG_LEVEL >= 2) {
		printf("Crossover: copying from source (partStart: %d, partEnd: %d, modStart: %d, modEnd: %d, cellStart: %d, cellEnd: %d\n", partStart, partEnd, modStart, modEnd, cellStart, cellEnd);
	}
	int i, j, k;
	if(partStart == partEnd) {
		i = partEnd -1;
		if(modStart == modEnd) {
			j = modEnd - 1;
			if(cellStart == cellEnd) {
				k = cellStart - 1;
				destination->parts[i].modules[j].cells[k].luts = source->parts[i].modules[j].cells[k].luts;
			}
			else {
				for(k = cellStart; k < cellEnd; k++) {
					destination->parts[i].modules[j].cells[k].luts = source->parts[i].modules[j].cells[k].luts;
				}
			}
		}
		else {
			for(j = modStart; j < modEnd; j++) {
				if(j == modEnd - 1) {
					for(k = cellStart; k < cellEnd; k++) {
						destination->parts[i].modules[j].cells[k].luts = source->parts[i].modules[j].cells[k].luts;
					}
				}
				else {
					for(k = 0; k < CELLS_PER_MODULE; k++) {
						destination->parts[i].modules[j].cells[k].luts = source->parts[i].modules[j].cells[k].luts;
					}
				}
			}
		}
	}
	else {
		for(i = partStart; i < partEnd; i++) {
			if(i == partEnd - 1) {
				for(j = modStart; j < modEnd; j++) {
					if(j == modEnd - 1) {
						for(k = cellStart; k < cellEnd; k++) {
							destination->parts[i].modules[j].cells[k].luts = source->parts[i].modules[j].cells[k].luts;
						}
					}
					else {
						for(k = 0; k < CELLS_PER_MODULE; k++) {
							destination->parts[i].modules[j].cells[k].luts = source->parts[i].modules[j].cells[k].luts;
						}
					}
				}
			}
			else {
				for(j = 0; j < MODULES_PER_PART; j++) {
					for(k = 0; k < CELLS_PER_MODULE; k++) {
						destination->parts[i].modules[j].cells[k].luts = source->parts[i].modules[j].cells[k].luts;
					}
				}
			}
		}
	}
	destination->fitness = 0;
	return;
}

/*
 * Switch two populations
 * @param Population *destination
 * @param Population *source
 */
void switchPopulation(Population **nextPop, Population **pop) {
	Population *temp;
	temp = *pop;
	*pop = *nextPop;
	*nextPop = temp;
	return;
}

/*
 * Init Random Number Generator (RNG)
 * @param int initValue: RNG seed
 */
void randomInit(long initValue) {
	srand(initValue);
	return;
}

/*
 * Generate a random number between 0 and (2^32-1)
 * @returns int32: the random number
 */
int32 cRandom() {
	int32 randomNumber = 0;
	int32 temp = 0;
	int i;
	for(i = 0; i < 4; i++) {
		temp = (int32)(rand() % 256);
		temp = temp << (i * 8);
		randomNumber = randomNumber | temp;
	}
	return randomNumber;
}

/*
 * Generate a random number between 0 and (2^16-1)
 * @returns int16: the random number
 */
int16 halfcRandom() {
	int16 randomNumber = 0;
	int16 temp = 0;
	int i;
	for(i = 0; i < 2; i++) {
		temp = (int32)(rand() % 128);
		temp = temp << (i * 8);
		randomNumber = randomNumber | temp;
	}
	return randomNumber;
}

/*
 * Generate a random number between 0 and POPULATION_SIZE - 1
 * @returns int numero: random
 */
int iRandom() {
	return rand() % POPULATION_SIZE;
}


void setCandidate(Candidate *candidate, int32 *luts) {
	int i,j,k;
	int n = 0;

	for (k = 0; k < PARTS_PER_CANDIDATE_SOLUTION; k++) {
		for (j = 0; j < MODULES_PER_PART; j++) {
			for (i = 0; i < CELLS_PER_MODULE; i++) {
				setCell(&(candidate->parts[k].modules[j].cells[i]), *(luts+n));
				n++;
			}
		}
	}
}

void setPart(Part *part, int32 *luts) {
	int i,j;
	int n = 0;

	for (j = 0; j < MODULES_PER_PART; j++) {
		for (i = 0; i < CELLS_PER_MODULE; i++) {
			setCell(&(part->modules[j].cells[i]), *(luts+n));
			n++;
		}
	}
}

void setModule(Module *module, int32 *luts) {
	int i;
	for (i = 0; i < CELLS_PER_MODULE; i++) {
		setCell(&(module->cells[i]), *(luts+i));
	}
}

void setCell(Cell *cell, int32 luts) {
	cell->luts = luts;
}

void getLutsModule(int32 *luts[], Module *module) {
	int i;

	for (i = 0; i < CELLS_PER_MODULE; i++) {
		*(luts+i) = &(module->cells[i].luts);
	}
}
