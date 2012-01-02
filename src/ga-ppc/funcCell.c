#include <stdio.h>
#include <stdlib.h>

#include "funcCell.h"

/*
 *
 */

FuncCell fcPool[] = {
	{0x8000,1},		// and
	{0xFFFE,1},		// or
	{0x6996,1},		// xor
	{0xFFFF,1},		// set
	{0x0000,1},		 //reset
	{0x5555,1},		// even numbers
	{0xAAAA,1},		// odd numbers | forward input 0
	{0x28AE,1},		// prime numbers
	{0x0213,1},		// square numbers
	{0x00FF,1},		// numbers less than 8
	{0xFF00,1},		// numbers greater or equal than 8 | forward in 3
	{0x212F,1},		// fibonacci sequence
	{0x844A,1},		// triangular numbers
	{0xCCCC,1},		// forward input 1
	{0xF0F0,1},		// forward input 2

	{(int16) (0xFFFF - 0x8000),1},		// not and
	{(int16) (0xFFFF - 0xFFFE),1},		// not or
	{(int16) (0xFFFF - 0x6996),1},		// not xor
	{(int16) (0xFFFF - 0x28AE),1},		// not prime numbers
	{(int16) (0xFFFF - 0x0213),1},		// not square numbers
	{(int16) (0xFFFF - 0x212F),1},		// not fibonacci sequence
	{(int16) (0xFFFF - 0x844A),1},		// not triangular numbers
	{(int16) (0xFFFF - 0xCCCC),1},		// not forward input 1
	{(int16) (0xFFFF - 0xF0F0),1},		// not forward input 2
	{(int16) (0xFFFF - 0xAAAA),1},		// not odd numbers | forward input 0
	{(int16) (0xFFFF - 0xFF00),1},		// not numbers greater or equal than 8 | forward in 3
};

/*
 *
 */

/*
 * @return int numbers of functional cells available
 */
int fcPoolSize() {
	return (sizeof(fcPool) / sizeof(FuncCell));
}

/*
 * @param size numbers of functional cells available
 * @return int total weight of functional cells
 */
int fcPoolWeight(int size) {
	int i,sum = 0;
	for (i = 0; i < size; i++) {
		sum = sum + fcPool[i].weight;
	}
	return sum;
}

/*
 * Initialize the wv vector, where the index of the
 * functional cell will be repeated as many times as
 * its weight
 *
 * @param wv[] weight-vector
 * @param size size of the pool
 */
void initFcWeightVector(int wv[], int size) {
	int i,j,k;

	k = 0;
	for (i = 0; i < size; i++) {
		int weight = fcPool[i].weight;
		for (j = 0; j < weight; j++) {
			wv[k] = i;
			k++;
		}
	}
}



/*
 * External interfaces
 */


int16 halfFcRandom() {
	int i = rand() % fcPoolSize();
	return fcPool[i].luts;
}

/*
 * It cares about weight of each functional cell
 */
int16 halfFcWeightedRandom() {
	int poolSize,w,i;

	poolSize = fcPoolSize();
	w = fcPoolWeight(poolSize);

	int wv[w];
	initFcWeightVector(wv, poolSize);

	i = rand() % w;
	return fcPool[wv[i]].luts;
}



int32 fcRandom() {
	int32 luts = ALL_ZEROS;
	luts = halfFcRandom() << 16;
	luts = luts | halfFcRandom();

	return luts;
}

int32 fcWeightedRandom() {
	int32 luts = ALL_ZEROS;
	luts = halfFcWeightedRandom() << 16;
	luts = luts | halfFcWeightedRandom();

	return luts;
}
