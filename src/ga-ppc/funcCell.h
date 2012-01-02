#include "ehw.h"

typedef struct func_cell_t {
	int16 luts;
	int weight;
} FuncCell;

int fcPoolSize();
int fcPoolWeight(int);
void initFcWeightVector(int[], int);

int16 halfFcRandom();
int16 halfFcWeightedRandom();

int32 fcRandom();
int32 fcWeightedRandom();
