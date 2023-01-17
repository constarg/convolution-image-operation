#include "xscutimer.h"

#include "stdlib.h"
#include "stdio.h"

/*
 * Sizes
 */
// The size of the origianl array.
#define ARRAY_R_EX 130 // original array rows extended
#define ARRAY_C_EX 130 // original array columns extended
#define ARRAY_SIZE_EX (ARRAY_R_EX * ARRAY_C_EX)
// The size of the product array.
#define ARRAY_R 128 // array rows
#define ARRAY_C 128 // array columns
#define ARRAY_SIZE (ARRAY_R * ARRAY_C)

/*
 * Memory locations
 */
// 0xFFFF000 is the base address for the memory.
#define MEMORY_BASE_ADDR (0xFFFF0000)
// The location of the original array.
// The original array of 128x128 elements.
#define ORIGINAL_ARRAY_LOC (0xFFFF0001)
// The location of the product array.
#define PRODUCT_ARRAY_LOC  (ORIGINAL_ARRAY_LOC + ARRAY_SIZE_EX + 1)

/**
 * Variables
 */
#define STATUS (*(volatile uint8_t *) MEMORY_BASE_ADDR)

/**
 * Function macros
 */
/*
 * This macro function is responsible to assign a value into the right index
 * of the original array.
 */
#define ORIGINAL_ARRAY_INDEX(I, J) \
    *((volatile uint8_t *) (ORIGINAL_ARRAY_LOC + (ARRAY_C_EX * I) + J))
/*
 * This macro function is responsible to assign a value into the right index
 * of the product array.
 */
#define PRODUCT_ARRAY_INDEX(I, J) \
    *((volatile uint8_t *) (PRODUCT_ARRAY_LOC + (ARRAY_C * I) + J))

/**
 * This function initializes the original 
 * array with zeros.
 */
static void initialize_org_array()
{
    for (int i = 0; i < ARRAY_R_EX; i++) {
        for (int j = 0; j < ARRAY_C_EX; j++) {
            // put zero into the current index.
            ORIGINAL_ARRAY_INDEX(i,j) = (uint8_t) 0;
        }
    }
}

/**
 * This function fill the original array 
 * with random values to create an 128x128 
 * array. We ignore the perimeter and fill
 * only the 128x128 values of 130x130.
 */
static void create_128x128_array()
{
    srand(0);
    for (int i = 1; i < ARRAY_R; i++) {
        for (int j = 1; j < ARRAY_C; j++) {
            // Assign a random value into the current index.
            ORIGINAL_ARRAY_INDEX(i,j) = (uint8_t) (rand() % (255 - 0 + 1)) + 0;
        }
    }
}


int main()
{
    STATUS = 0;
    initialize_org_array();
    // Make the original array.
    create_128x128_array();

    // start processing.
    STATUS = 1;
}
