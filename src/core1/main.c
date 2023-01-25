#include "convolution.h"

#include "stdlib.h"
#include "stdio.h"

#include "platform.h"
#include "xil_printf.h"

static void calculate_array()
{
    for (int i = 65; i <= ARRAY_R; i++) {
        for (int j = 1; j <= ARRAY_C; j++) {
            PRODUCT_ARRAY_INDEX(i - 1, j - 1) = LEFT_TOP_PIXEL(i,j)       * (-1) +
                                                MIDDLE_TOP_PIXEL(i, j)    * (-1) +
                                                RIGHT_TOP_PIXEL(i, j)     * (-1) +
                                                LEFT_PIXEL(i, j)          * (-1) +
                                                MIDDLE_PIXEL(i, j)        * (9)  +
                                                RIGHT_PIXEL(i, j)         * (-1) +
                                                BOTTOM_LEFT_PIXEL(i, j)   * (-1) +
                                                BOTTOM_MIDDLE_PIXEL(i, j) * (-1) +
                                                BOTTOM_RIGHT_PIXEL(i, j)  * (-1);
        }
    }
}


int main(void)
{
    init_platform();
    Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

    // while the cpu0 is not done with the array
    while (STATUS_CPU0 != CPU0_DONE_GEN ||
           STATUS_RESET == MUST_RESET);

    calculate_array();
    STATUS_CPU1 = CPU1_DONE_PROC;

    STATUS_RESET = MUST_RESET;

    cleanup_platform();
}

