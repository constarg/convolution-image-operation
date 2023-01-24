#define CPU0
#include "convolution.h"

#include "stdlib.h"
#include "stdio.h"

#include "platform.h"
#include "xil_printf.h"

#include "xscutimer.h"

/
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

static void calculate_array()
{
	uint8_t counter = 0;
    for (int i = 1; i < ARRAY_R; i++) {
        for (int j = 1; j < ARRAY_C; j++) {
        	PRODUCT_ARRAY_INDEX(i, j) = ORIGINAL_ARRAY_INDEX(i - 1,j - 1)  * (-1) +
										ORIGINAL_ARRAY_INDEX(i - 1,j) 	   * (-1) +
										ORIGINAL_ARRAY_INDEX(i - 1, j + 1) * (-1) +
										ORIGINAL_ARRAY_INDEX(i, j - 1)     * (-1) +
										ORIGINAL_ARRAY_INDEX(i, j) 		   * (9)  +
										ORIGINAL_ARRAY_INDEX(i, j + 1)     * (-1) +
										ORIGINAL_ARRAY_INDEX(i + 1, j - 1) * (-1) +
										ORIGINAL_ARRAY_INDEX(i + 1, j)     * (-1) +
										ORIGINAL_ARRAY_INDEX(i + 1, j + 1) * (-1);
        	++counter;

            // stop cpu0, at the center of the array.
        	if (counter == 8192) return;
        }
    }
}

int main()
{
	init_platform();
    // Disable cache on OCM (on - chip - memory). 
	Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

	if (STATUS_RESET == MUST_RESET ||
		STATUS_CPU0 != 0) {
		STATUS_CPU0 = 0;
		STATUS_CPU1 = 0;
		STATUS_RESET = 0;
	}

	initialize_org_array();
    // Make the original array.
    create_128x128_array();
    printf("[*] 128x128 array successfully created...\n");

    // start processing.
    STATUS_CPU0 = CPU0_DONE_GEN;

    // start calculation.
    calculate_array();

    while (STATUS_CPU1 != CPU1_DONE_PROC);
    printf("[*] 128x128 convolution matrix is done...\n");

    cleanup_platform();
}
