#include "xscutimer.h"

#include "stdlib.h"
#include "stdio.h"

// The size of arrays.
#define ARRAY_SIZE_ORG (130 * 130)

/**
 * Status numbers
 * 1: initialized
 */
// 0xFFFF000 is the base address for the memory.
#define MEMORY_BASE_ADDR (0xFFFF0000)
#define STATUS (*(volatile uint8_t *) MEMORY_BASE_ADDR)

// The location of the original array.
// The original array of 128x128 elements.
#define ORIGINAL_ARRAY_LOC (0xFFFF0001)

// The location of the product array.
#define NEW_ARRAY_LOC  (ORIGINAL_ARRAY_LOC + ARRAY_SIZE_ORG + 1)

// The product array of 128x128 elements.
#define NEW_ARRAY 	   ((volatile uint8_t *) NEW_ARRAY_LOC)

// The function that is responsible to set an value into the right index.
#define ORIGINAL_ARRAY_INDEX(I, J) \
    *((volatile uint8_t *) (ORIGINAL_ARRAY_LOC + (130 * I) + J))

void initialize_array()
{
    for (int i = 0; i < 130; i++) {
        for (int j = 0; j < 130; j++) {
            ORIGINAL_ARRAY_INDEX(i,j) = (uint8_t) 0;
        }
    }
}

void create_128x128_array()
{
    srand(0);
    for (int i = 1; i < 128; i++) {
        for (int j = 1; j < 128; j++) {
            ORIGINAL_ARRAY_INDEX(i,j) = (uint8_t) (rand() % (255 - 0 + 1)) + 0;
        }
    }
}


int main()
{
    STATUS = 0;
    initialize_array();
    // Make the original array.
    create_128x128_array();

    // start processing.
    STATUS = 1;
}
