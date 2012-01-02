 /*
 * @title Hera Project
 * @author Davide Bartolini
 * @author Fabio Cancare
 * @author Matteo Carminati
 */

#ifndef EHW_H_
#define EHW_H_

#include <xparameters.h>

#define DEBUG_LEVEL 0
//Define whether the ICAP controller is the custom SEDPR controller or the Xilinx XPSHWICAP controller
#if XPAR_SEDPR_CNTLR_NUM_INSTANCES
#define SEDPR 1
#else
#define SEDPR 0
#endif

//Define whether the fitness is computed by the EHW controller or by the processor
#if XPAR_EHW_CNTLR_32BIT_NUM_INSTANCES || XPAR_EHW_CNTLR_8BIT_NUM_INSTANCES
#define HWF 1
#else
#define HWF 0
#endif

#if XPAR_XTMRCTR_NUM_INSTANCES
#include <xtmrctr.h>
#endif

//Define whether the monitoring of the timing performance of the various evolution flow parts is on
#define MONITOR_PERFORMANCE 0

//Define the width of the candidate solution datapaths
#define BIT32 1


#define TEST_MEMORY_SIZE 4096


#include <xbasic_types.h>


#if SEDPR
#include <sedpr_cntlr.h>
#include "xhwicap_i.h"
#else
#include <xhwicap.h>
#include <xhwicap_i.h>
#endif

#include <xbasic_types.h>

#if XPAR_EHW_CNTLR_32BIT_NUM_INSTANCES
#include <ehw_cntlr_32bit.h>
#elif XPAR_EHW_CNTLR_8BIT_NUM_INSTANCES
#include <ehw_cntlr_8bit.h>
#else
#include <ehw_core.h>
#endif
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include "genetic.h"
#include "frameReadWrite.h"

//Position of the evolvable hardware slots within the FPGA
#if BIT32
static const int X[] = {40, 40, 40, 40, 42, 42, 42, 42, 44, 44, 44, 44, 46, 46, 46, 46};
static const int Y[] = {96, 64, 32, 0, 96, 64, 32, 0, 96, 64, 32, 0, 96, 64, 32, 0};
#else
static const int X[] = {40};
static const int Y[] = {96};
#endif


#if MONITOR_PERFORMANCE
//Counter logic
XTmrCtr TimerCounter0;
unsigned int Value1;
unsigned int Value2;
#endif


//Masks to be used with the evolvable hardware interfacte
#define RESET_BIT					0x02000000
#define START_BIT					0x01000000
#define ACK_BIT_MASK				0x01000000

#define ONE_BIT_FITNESS_256			0x22000000
#define ONE_BIT_FITNESS_512			0x24000000
#define ONE_BIT_FITNESS_1024		0x26000000
#define ONE_BIT_FITNESS_2048		0x28000000
#define ONE_BIT_FITNESS_4096		0x2A000000

#define TWO_BIT_FITNESS_256			0x42000000
#define TWO_BIT_FITNESS_512			0x44000000
#define TWO_BIT_FITNESS_1024		0x46000000
#define TWO_BIT_FITNESS_2048		0x48000000
#define TWO_BIT_FITNESS_4096		0x4A000000

#define FOUR_BIT_FITNESS_256		0x62000000
#define FOUR_BIT_FITNESS_512		0x64000000
#define FOUR_BIT_FITNESS_1024		0x66000000
#define FOUR_BIT_FITNESS_2048		0x68000000
#define FOUR_BIT_FITNESS_4096		0x6A000000

#define EIGHT_BIT_FITNESS_256		0x82000000
#define EIGHT_BIT_FITNESS_512		0x84000000
#define EIGHT_BIT_FITNESS_1024		0x86000000
#define EIGHT_BIT_FITNESS_2048		0x88000000
#define EIGHT_BIT_FITNESS_4096		0x8A000000

#define SIXTEEN_BIT_FITNESS_256		0xA2000000
#define SIXTEEN_BIT_FITNESS_512		0xA4000000
#define SIXTEEN_BIT_FITNESS_1024	0xA6000000
#define SIXTEEN_BIT_FITNESS_2048	0xA8000000
#define SIXTEEN_BIT_FITNESS_4096	0xAA000000

#define THIRTYTWO_BIT_FITNESS_256	0xC2000000
#define THIRTYTWO_BIT_FITNESS_512	0xC4000000
#define THIRTYTWO_BIT_FITNESS_1024	0xC6000000
#define THIRTYTWO_BIT_FITNESS_2048	0xC8000000
#define THIRTYTWO_BIT_FITNESS_4096	0xCA000000

//Fitness stopping criterias
#define END_ON_MAXIMUM_FITNESS 1
#define CONVERGED_SOLUTIONS_MIN_NUMBER 1

#if HWF
//Setup the fitness evaluation process
void fitness_setup(int fitnessFunction);
//Compute the fitness of the candidate solution
void fitness(int *fitness, int fitnessFunction, unsigned int fitnessMax, const int32 training);

#else
//Write data to the candidate solution input register
void ehwWrite(volatile int32 data);
//Read the candidate solution output register
int32 ehwRead();
void fitness(int *fitness, int fitnessFunction, unsigned int fitnessMax, int verbose);
#endif

//Evaluate the stopping criteria
int resultFound(Population *pop, int fitnessMax);

void classifier_setup(const int training, const int lowerbound);

/*Which classification problem has to be faced:
 * 1: Acute Inflammation
 * 2: Cars
 * 3: Chess
 * */
#define CLASSIFICATION_TASK 3

#if CLASSIFICATION_TASK == 1
#define DIM_DATASET 120
#define DIM_EVOSET 90 //75%
#elif CLASSIFICATION_TASK == 2
#define DIM_DATASET 1728
#define DIM_EVOSET 1300 //75.2%
#elif CLASSIFICATION_TASK == 3
#define DIM_DATASET 27666
#define DIM_EVOSET 21000
#endif

#endif /*EHW_H_*/
