#ifndef CONVOLUTION
#define CONVOLUTION

/*
 * Sizes
 */
// The size of the original array.
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
#ifdef CPU0
// The original array for the cpu0
#define ORIGINAL_ARRAY_LOC (0xFFFF0003)
#else
// The original array for the cpu1
// The difference is that in cpu1 the array start where the cpu0 stops, in the center.
#define ORIGINAL_ARRAY_CPU1_OFFSET 8191
#define ORIGINAL_ARRAY_LOC (0xFFFF0003 + ORIGINAL_ARRAY_CPU1_OFFSET)
#endif

// The location of the product array.
#define PRODUCT_ARRAY_LOC  (ORIGINAL_ARRAY_LOC + ARRAY_SIZE_EX + 1)

/**
 * Variables
 */
/**
 * STATUS_CPU0 is the status of cpu0
 * *STATUS_CPU0 can have the below values.
 * 0 = not done.
 * 1 = done array generation
 * 2 = done processing.
 */
#define STATUS_CPU0 *((volatile uint8_t *) MEMORY_BASE_ADDR)

/**
 * STATUS_CPU1 is the status of the cpu1.
 * STATUS_CPU1 can have the below values
 * 0 = not done
 * 1 = done processing.
 */
#define STATUS_CPU1 *((volatile uint8_t *) (MEMORY_BASE_ADDR + 1))

// This variable is used to check if needed to reset status bits.
/**
 * STATUS_RESET can have the below values
 * 0 = do not reset.
 * 1 = must reset.
 */
#define STATUS_RESET *((volatile uint8_t *) (MEMORY_BASE_ADDR + 2))

// Variable values
// values for cpu0 status.
#define CPU0_DONE_GEN  1 // cpu0 generates the 128x128 array.
#define CPU0_DONE_PROC 2 // cpu0 is done with the processing.

// values for cpu1 status.
#define CPU1_DONE_PROC 1 // cpu1 is done with the processing.

// values for reset variable.
#define INIT_RESET 0
#define MUST_RESET 1

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

#endif
