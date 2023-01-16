<LeftMouse>#include "xscutimer.h"

#include "stdlib.h"
#include "stdio.h"

// The size of arrays.
#define ARRAY_SIZE (128 * 128)

// The location of the original array.
// 0xFFFF000 is the base address for the memory.
#define ORIGINAL_ARRAY_LOC (0xFFFF0000)

// The location of the product array.
#define NEW_ARRAY_LOC  (ORIGINAL_ARRAY_LOC + ARRAY_SIZE + 1)

// The original array of 128x128 elements.
#define ORIGINAL_ARRAY ((volatile uint8_t **) ORIGINAL_ARRAY_LOC)
// The product array of 128x128 elements.
#define NEW_ARRAY 	   ((volatile uint8_t **) NEW_ARRAY_LOC)


void create_128x128_array()
{
	srand(0);
	for (int i = 0; i < 128; i++) {
		for (int j = 0; j < 128; j++) {
			ORIGINAL_ARRAY[i][j] = (rand() % (255 - 0 + 1)) + 0;
		}
	}
}


int main()
{
	// Make the original array.		
    create_128x128_array();
}
