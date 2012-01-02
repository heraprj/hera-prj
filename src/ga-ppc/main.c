/*
 * @title Hera Project
 * @part Main
 * @author Davide Bartolini
 * @author Fabio Cancare
 * @author Matteo Carminati
 */
#include "icap.h"
#include "ehw.h"
#include "xcache_l.h"
#include "xil_cache.h"

#if SEDPR == 0
void printCandidateSolutionBitstream(int individual_number, int generation_number, Candidate* candidate, XHwIcap HwIcap) {
	printf("######## Candidate solution %d bitstream, Gen: %d, deployed from %d, %d to %d, %d ########\n", individual_number, generation_number, X[0] , Y[0], X[PARTS_PER_CANDIDATE_SOLUTION-1] , Y[PARTS_PER_CANDIDATE_SOLUTION-1]);
	printf("Fitness: %d\n", candidate->fitness);
	printFrame(&HwIcap, X[0], Y[0]);
	printf("######## End ########\n");
}
#endif

int main() {

	XCache_EnableICache(0x80000001);
	XCache_EnableDCache(0x80000001);

	unsigned int i, j, k;
	int r, s;

	//Variables added for performing scalability analysis
	const float MUTATION_PROB = 0.8; //Probability of doing mutation
	const float CROSSOVER_PROB = 0.8;
	const int32 MUTATION_LEVEL = MUTATION_MULTIBIT_LEVEL;	//How to perform mutation

	//Which fitness function has to be used
	int fitnessFunction = 4;

	//Optimal value of the fitness function
	unsigned int fitnessMax = 229376;

	//Seed for the RNG
	unsigned int seed;

#if SEDPR == 0
	static XHwIcap HwIcap;
#endif
	//Actual population
	Population *pop;
	static Population population;
	static Candidate candidatesPop[POPULATION_SIZE];

	//Next population
	Population *nextPop;
	static Population next_population;
	static Candidate candidatesNextPop[POPULATION_SIZE];
	Candidate *indCross1;
	Candidate *indCross2;

	int status;

	float convergency_level;

#if MONITOR_PERFORMANCE
	printf("Initializing the timer/counter\n");
	if (XTmrCtr_Initialize(&TimerCounter0, XPAR_XPS_TIMER_0_DEVICE_ID) != XST_SUCCESS) {
		printf("Timer init error\n");
		return -1;
	}
	if (XTmrCtr_SelfTest(&TimerCounter0, 0) != XST_SUCCESS) {
		printf("Timer self-test error\n");
		return -1;
	}
#endif

	if(DEBUG_LEVEL >= 2) printf("Initializing the ICAP controller\n");
#if SEDPR == 0
	//ICAP initialization
	if(initIcap(&HwIcap) == XST_FAILURE) {
		printf("ICAP init error\n\r");
		return -1;
	}
#else
	//SEDPR initialization
	SEDPR_CNTLR_mWriteSlaveReg0(XPAR_SEDPR_CNTLR_0_BASEADDR, 0, ALL_ZEROS);
	//Bitstream initialization
	BitstreamInit();
#endif

#if HWF
	if(DEBUG_LEVEL >= 2) {
		printf("Testing of the 32bit EHW controller first memory\n");
		for(i=0; i < TEST_MEMORY_SIZE; i++) {
			//printf("Writing %d\n", i);
			EHW_CNTLR_32BIT_mWriteMemory(i*4 + XPAR_EHW_CNTLR_32BIT_0_MEM0_BASEADDR, i);
		}
		for(i=0; i < TEST_MEMORY_SIZE; i++) {
			if(i != EHW_CNTLR_32BIT_mReadMemory(i*4 + XPAR_EHW_CNTLR_32BIT_0_MEM0_BASEADDR)) {
				printf("Wrong 32bit EHW controller first memory content, %d\n", i);
				exit(1);
			}
		}
		printf("Testing of the 32bit EHW controller second memory\n");
		for(i=0; i < TEST_MEMORY_SIZE; i++) {
			//printf("Writing %d\n", i);
			EHW_CNTLR_32BIT_mWriteMemory(i*4 + XPAR_EHW_CNTLR_32BIT_0_MEM1_BASEADDR, i);
		}
		for(i=0; i < TEST_MEMORY_SIZE; i++) {
			if(i != EHW_CNTLR_32BIT_mReadMemory(i*4 + XPAR_EHW_CNTLR_32BIT_0_MEM1_BASEADDR)) {
				printf("Wrong 32bit EHW controller second memory content, %d\n", i);
				exit(1);
			}
		}
	}
	if(DEBUG_LEVEL >= 2) printf("Setting up the fitness evaluation process\n");
	#if MONITOR_PERFORMANCE && 0
		XTmrCtr_Start(&TimerCounter0, 0);
		Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
	#endif
		fitness_setup(fitnessFunction);
	#if MONITOR_PERFORMANCE && 0
		Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
		XTmrCtr_Stop(&TimerCounter0, 0);
		XTmrCtr_Reset(&TimerCounter0, 0);
		printf("----- Fitness evaluation: %u -----\n", (Value2 - Value1));
	#endif

	if(DEBUG_LEVEL >= 2) {
		printf("First memory content\n");
		for(i=0; i< 256; i++) {
			printf("Word %u:%08X\n", i, (unsigned int)EHW_CNTLR_32BIT_mReadMemory(i*4 + XPAR_EHW_CNTLR_32BIT_0_MEM0_BASEADDR));
		}
		printf("Second memory content\n");
		for(i=0; i< 256; i++) {
			printf("Word %u:%08X\n", i, (unsigned int)EHW_CNTLR_32BIT_mReadMemory(i*4 + XPAR_EHW_CNTLR_32BIT_0_MEM1_BASEADDR));
		}
	}
#endif

	//for(MUTATION_LEVEL = MUTATION_MIXED_LEVEL; MUTATION_LEVEL >= MUTATION_MULTIBIT_LEVEL; MUTATION_LEVEL--) {
		//for(CROSSOVER_PROB = 0.9; CROSSOVER_PROB < 0.95; CROSSOVER_PROB += 0.1) {
			//for(MUTATION_PROB = 1.0; MUTATION_PROB < 1.95; MUTATION_PROB += 0.1) {
				for(seed = 0; seed < 20; seed++) {
					printf("\n\nFitness function: %d, RNG seed: %d\n\n", fitnessFunction , seed);
					if(MUTATION_LEVEL == MUTATION_MIXED_LEVEL) printf("Evolution type: MIXED\n");
					if(MUTATION_LEVEL == MUTATION_MULTIBIT_LEVEL) printf("Evolution type: MIXED\n");
					if(MUTATION_LEVEL == MUTATION_MULTICELL_LEVEL) printf("Evolution type: FUNCTIONAL\n");
					printf("GA parameters: elitism: %f, mutation(level, p): (%d, %f), crossover(level, p): (%d, %f), selection prob.: %f\n", ELITISM_PROB, MUTATION_LEVEL, MUTATION_PROB, CROSSOVER_LEVEL, CROSSOVER_PROB, SELECTION_FITTEST_PROB);

					if(DEBUG_LEVEL >= 2) printf("Instantiating memory for the populations\n");
					newPopulation(&pop, &population, candidatesPop);
					newPopulation(&nextPop, &next_population, candidatesNextPop);
	
					if(DEBUG_LEVEL >= 2) printf("Initializing the first population\n");
					randomInit(seed);
			#if MONITOR_PERFORMANCE && 0
					XTmrCtr_Start(&TimerCounter0, 0);
					Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
			#endif
					initPopulation(pop, NULL);
			#if MONITOR_PERFORMANCE && 0
					Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
					XTmrCtr_Stop(&TimerCounter0, 0);
					XTmrCtr_Reset(&TimerCounter0, 0);
					printf("----- Pop init: %u -----\n", (Value2 - Value1));
			#endif

					//Evaluate the fitness of the first population individuals and order them according to that
					for(j = 0; j < POPULATION_SIZE; j++) {
						if(DEBUG_LEVEL >= 2) {
							printf("Writing the individual number %d\n", j);
						}
			#if SEDPR == 0 && MONITOR_PERFORMANCE == 1 && 0
						XTmrCtr_Start(&TimerCounter0, 0);
						Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
						status = deployToFPGA(&HwIcap, pop->candidates[j], X, Y);
						Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
						XTmrCtr_Stop(&TimerCounter0, 0);
						XTmrCtr_Reset(&TimerCounter0, 0);
						printf("----- DeployToFPGA: %u -----\n", (Value2 - Value1));
			#elif SEDPR == 0
						status = deployToFPGA(&HwIcap, pop->candidates[j], X, Y);
			#elif SEDPR == 1 && MONITOR_PERFORMANCE == 1 && 0
						XTmrCtr_Start(&TimerCounter0, 0);
						Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
						status = deployToFPGA(pop->candidates[j], X, Y);
						Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
						XTmrCtr_Stop(&TimerCounter0, 0);
						XTmrCtr_Reset(&TimerCounter0, 0);
						printf("----- DeployToFPGA: %u -----\n", (Value2 - Value1));
			#elif SEDPR == 1
						status = deployToFPGA(pop->candidates[j], X, Y);
			#endif
						if(status != XST_SUCCESS) {
							printf("Error while writing the bitstream\n");
							return -1;
						}

			#if MONITOR_PERFORMANCE && 0
					XTmrCtr_Start(&TimerCounter0, 0);
					Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
			#endif
			#if HWF
						fitness(&(pop->candidates[j]->fitness), fitnessFunction, fitnessMax, 0);
			#else
						fitness(&(pop->candidates[j]->fitness), fitnessFunction, fitnessMax, 0);
			#endif
			#if MONITOR_PERFORMANCE && 0
						Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
						XTmrCtr_Stop(&TimerCounter0, 0);
						XTmrCtr_Reset(&TimerCounter0, 0);
						printf("----- Fitness evaluation: %u -----\n", (Value2 - Value1));
			#endif


						if(DEBUG_LEVEL >= 2) {
							printf("Computing the fitness of individual number %d\n", j);
						}
			#if SEDPR == 0
						if(DEBUG_LEVEL >= 3) {
							printCandidateSolutionBitstream(j, 0, pop->candidates[j], HwIcap);
						}
			#endif
					}
			#if MONITOR_PERFORMANCE && 0
					XTmrCtr_Start(&TimerCounter0, 0);
					Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
			#endif
					ordering(pop, 0, POPULATION_SIZE - 1);
					convergency_level = (float)pop->candidates[0]->fitness/(float)fitnessMax;
			#if MONITOR_PERFORMANCE && 0
						Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
						XTmrCtr_Stop(&TimerCounter0, 0);
						XTmrCtr_Reset(&TimerCounter0, 0);
						printf("----- Population ordering: %u -----\n", (Value2 - Value1));
			#endif
					if(DEBUG_LEVEL >= 1) {
						printf("Initial, ordered, population fitness:");
						for(j = 0; j < POPULATION_SIZE; j++) {
							printf(" %d", pop->candidates[j]->fitness);
						}
						printf("\n\n");
					}

					//Evolutionary cycle, stops when the overall population fitness achieve
					//the specified result or when MAX_GENERATION_NUMBER generations were created
					for(i = 1; i <= MAX_GENERATION_NUMBER && !resultFound(pop, fitnessMax); i++) {
						nextPop->generation_number = i;
			#if MONITOR_PERFORMANCE
						XTmrCtr_Start(&TimerCounter0, 0);
						Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
			#endif
						for(j = 0; j < POPULATION_SIZE - 1 - (int)(ELITISM_PROB * (float)POPULATION_SIZE); j = j + 2) {
							//Select the two candidate solutions (parents) for creating two offsprings
							tournamentSelection(pop, &indCross1, &indCross2);

							//Eventually perform crossover
							crossover(indCross1, indCross2, nextPop->candidates[j], nextPop->candidates[j + 1], CROSSOVER_PROB, CROSSOVER_LEVEL);

							//Eventually perform mutation on the first offspring
							mutation(nextPop->candidates[j], MUTATION_PROB, MUTATION_LEVEL, convergency_level);

							//Eventually perform mutation on the second offspring
							mutation(nextPop->candidates[j + 1], MUTATION_PROB, MUTATION_LEVEL, convergency_level);
						}

						//Elitism
						for(k = 0; k < (int)(ELITISM_PROB * (float)POPULATION_SIZE) && j < POPULATION_SIZE; k++) {
							if(DEBUG_LEVEL >= 2) {
								printf("Elitism: k: %d, j: %d, POP_SIZE: %d\n",k,j,POPULATION_SIZE);
							}
							copyCandidate(nextPop->candidates[j], pop->candidates[k], 0, PARTS_PER_CANDIDATE_SOLUTION, 0, MODULES_PER_PART, 0, CELLS_PER_MODULE);
							j++;
							if(DEBUG_LEVEL >= 2) {
								printf("Candidate solution %d with fitness: %d has been copied to the next population (elitism)\n", k, pop->candidates[k]->fitness);
								if(DEBUG_LEVEL >= 3) {
									printCandidateSolution(j, i, nextPop->candidates[j]);
								}
							}
						}

						//Preserve population cardinality
						while(j < POPULATION_SIZE) {
							if(DEBUG_LEVEL >= 2) {
								printf("Warning: preservation needed\n\n");
							}
							//Selection
							tournamentSelection(pop, &indCross1, &indCross2);
							//Crossover
							crossover(indCross1, indCross2, nextPop->candidates[j], nextPop->candidates[j], CROSSOVER_PROB, CROSSOVER_LEVEL);
							//Mutation
							mutation(nextPop->candidates[j], MUTATION_PROB, MUTATION_LEVEL, convergency_level);
							j++;
						}
			#if MONITOR_PERFORMANCE && 0
								Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
								XTmrCtr_Stop(&TimerCounter0, 0);
								XTmrCtr_Reset(&TimerCounter0, 0);
								printf("----- Genetic algorithm (all individuals): %u -----\n", (Value2 - Value1));
			#endif

						//Compute the fitness of the new candidate solutions
						for(j = 0; j < POPULATION_SIZE; j++) {
							//Write the candidate solution bitstream to the evolvable area
			#if SEDPR == 0 && MONITOR_PERFORMANCE == 1 && 0
							XTmrCtr_Start(&TimerCounter0, 0);
							Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
							status = deployToFPGA(&HwIcap, nextPop->candidates[j], X, Y);
							Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
							XTmrCtr_Stop(&TimerCounter0, 0);
							XTmrCtr_Reset(&TimerCounter0, 0);
							printf("----- DeployToFPGA: %u -----\n", (Value2 - Value1));
			#elif SEDPR == 0
							status = deployToFPGA(&HwIcap, nextPop->candidates[j], X, Y);
			#elif SEDPR == 1 && MONITOR_PERFORMANCE == 1 && 0
							XTmrCtr_Start(&TimerCounter0, 0);
							Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
							status = deployToFPGA(nextPop->candidates[j], X, Y);
							Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
							XTmrCtr_Stop(&TimerCounter0, 0);
							XTmrCtr_Reset(&TimerCounter0, 0);
							printf("----- DeployToFPGA: %u -----\n", (Value2 - Value1));
			#elif SEDPR == 1
							status = deployToFPGA(nextPop->candidates[j], X, Y);
			#endif
							if(status != XST_SUCCESS) {
								printf("Error while writing the bitstream\n");
								return -1;
							}
			#if MONITOR_PERFORMANCE && 0
							XTmrCtr_Start(&TimerCounter0, 0);
							Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
			#endif
			#if HWF
						fitness(&(nextPop->candidates[j]->fitness), fitnessFunction, fitnessMax, 0);
			#else
						fitness(&(nextPop->candidates[j]->fitness), fitnessFunction, fitnessMax, 0);
			#endif
			#if MONITOR_PERFORMANCE && 0
							Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
							XTmrCtr_Stop(&TimerCounter0, 0);
							XTmrCtr_Reset(&TimerCounter0, 0);
							printf("----- Fitness evaluation: %u -----\n", (Value2 - Value1));
			#endif
						}
						//Order the new candidate solutions according to their fitness
			#if MONITOR_PERFORMANCE && 0
						XTmrCtr_Start(&TimerCounter0, 0);
						Value1 = XTmrCtr_GetValue(&TimerCounter0, 0);
			#endif
						ordering(nextPop, 0, POPULATION_SIZE - 1);
						convergency_level = (float)pop->candidates[0]->fitness/(float)fitnessMax;
			#if MONITOR_PERFORMANCE && 0
						Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
						XTmrCtr_Stop(&TimerCounter0, 0);
						XTmrCtr_Reset(&TimerCounter0, 0);
						printf("----- Population ordering: %u -----\n", (Value2 - Value1));
			#endif

						//Check whether the fitness of the best candidate solution within the new population is smaller than the one of the old population
						if(DEBUG_LEVEL >= 2) {
							if(nextPop->candidates[0]->fitness < pop->candidates[0]->fitness) {
								printf("\nThe maximum fitness has been decreased\n");
							}
						}

						switchPopulation(&nextPop, &pop);

						if(DEBUG_LEVEL >= 1 && !(i % 50)) {
							printf("Generation #%d, fitness:", i);
							for(j = 0; j < POPULATION_SIZE; j++) {
								printf(" %d", pop->candidates[j]->fitness);
								if(DEBUG_LEVEL >= 3 && !(i % 100)) {
									printf("\n");
									printf("########Individual %d Generation %d (LUT content)########\n", j, i);
									printf("fitness: %d\n", pop->candidates[j]->fitness);
									for(k = 0; k < PARTS_PER_CANDIDATE_SOLUTION; k++) {
										for(r = 0; r < MODULES_PER_PART; r++) {
											for(s = 0; s < CELLS_PER_MODULE; s++) {
												printf("0x%08X", pop->candidates[j]->parts[k].modules[r].cells[s].luts);
												if(r < MODULES_PER_PART - 1 || s < CELLS_PER_MODULE - 1) {
													printf(", ");
												}
											}
										}
										printf("\n");
									}
								}
							}
							printf("\n\n");
						}
			#if MONITOR_PERFORMANCE
								Value2 = XTmrCtr_GetValue(&TimerCounter0, 0);
								XTmrCtr_Stop(&TimerCounter0, 0);
								XTmrCtr_Reset(&TimerCounter0, 0);
								printf("----- Generation time: %u -----\n", (Value2 - Value1));
			#endif
					}
					if(i >= MAX_GENERATION_NUMBER) {
						printf("Perfect solution not found\n\n");
					}
					if(fitnessFunction == 4) {
						classifier_setup(1, 0);
#if SEDPR == 0
						status = deployToFPGA(&HwIcap, pop->candidates[0], X, Y);
#elif SEDPR == 1
						status = deployToFPGA(pop->candidates[0], X, Y);
#endif
						if(status != XST_SUCCESS) {
							printf("Error while writing the bitstream\n");
							return -1;
						}
#if HWF
						fitness(&(pop->candidates[0]->fitness), fitnessFunction, fitnessMax, 1);
#else
						fitness(&(pop->candidates[0]->fitness), fitnessFunction, fitnessMax, 0);
#endif


						printf("Validation phase:\n accuracy of the best classifier found during the training phase: ");
						printf("%f", (float)((float)pop->candidates[0]->fitness/(float)fitnessMax));
						printf("\n\n");
					}
				}
			//}
		//}
	//}
	printf("######## END #######\n");
	Xil_DCacheDisable();
	Xil_ICacheDisable();
	return XST_SUCCESS;
}
