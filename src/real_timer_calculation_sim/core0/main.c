/** interrupts cpu0 **/

#include "platform.h"
#include "xil_printf.h"

#include "stdlib.h"
#include "stdio.h"

#include "xscugic.h"
#include "xscutimer.h"

#include "convolution.h"

static XScuTimer timer; // the timer.
static XScuGic intr_controller; // interrupt controller.

/**
 * This function initializes the original
 * array with zeros.
 */
static void initialize_org_array()
{
    for (int i = 0; i < ARRAY_R_EX; i++) {
        for (int j = 0; j < ARRAY_C_EX; j++) {
            // put zero into the current index.
            ORIGINAL_ARRAY_INDEX(i, j) = (uint8_t) 0;
        }
    }
}

/**
 * This function fill the original array
 * with random values to create an 128x128
 * array. We ignore the perimeter and fill
 * only the 128x128 values of 130x130.
 */
static void create_128x128_array(XScuTimer *timer)
{
    srand((uint) XScuTimer_GetCounterValue(timer));
    for (int i = 1; i <= ARRAY_R; i++) {
        for (int j = 1; j <= ARRAY_C; j++) {
            // Assign a random value into the current index.
            ORIGINAL_ARRAY_INDEX(i, j) = (uint8_t)(rand() % (255 - 0 + 1)) + 0;
        }
    }
}

static void calculate_array()
{
    int counter = 0;
    for (int i = 1; i <= ARRAY_R; i++) {
        for (int j = 1; j <= ARRAY_C; j++) {
            PRODUCT_ARRAY_INDEX(i - 1, j - 1) = LEFT_TOP_PIXEL(i, j)      * (-1) +
                                                MIDDLE_TOP_PIXEL(i, j)    * (-1) +
                                                RIGHT_TOP_PIXEL(i, j)     * (-1) +
                                                LEFT_PIXEL(i, j)          * (-1) +
                                                MIDDLE_PIXEL(i, j)        * (9)  +
                                                RIGHT_PIXEL(i, j)         * (-1) +
                                                BOTTOM_LEFT_PIXEL(i, j)   * (-1) +
                                                BOTTOM_MIDDLE_PIXEL(i, j) * (-1) +
                                                BOTTOM_RIGHT_PIXEL(i, j)  * (-1);
            ++counter;
            // stop cpu0, at the center of the array.
            if (counter == (ARRAY_SIZE / 2)) return;
        }
    }
}

static void verify(int i, int j)
{
    uint8_t e1 = LEFT_TOP_PIXEL(i, j);
    uint8_t e2 = MIDDLE_TOP_PIXEL(i, j);
    uint8_t e3 = RIGHT_TOP_PIXEL(i, j);
    uint8_t e4 = LEFT_PIXEL(i, j);
    uint8_t e5 = MIDDLE_PIXEL(i, j);
    uint8_t e6 = RIGHT_PIXEL(i, j);
    uint8_t e7 = BOTTOM_LEFT_PIXEL(i, j);
    uint8_t e8 = BOTTOM_MIDDLE_PIXEL(i, j);
    uint8_t e9 = BOTTOM_RIGHT_PIXEL(i, j);

    printf("Array to calculate: \n");
    printf("[ -1, -1, -1 ]  [%d,   %d,   %d   ]\n"
           "[ -1,  9, -1 ]  [%d,   %d,   %d   ]\n"
           "[ -1, -1, -1 ]  [%d,   %d,   %d   ]\n", e1, e2, e3, e4, e5, e6, e7, e8, e9);

    e1 = PRODUCT_ARRAY_INDEX(i - 1, j - 1);

    printf("Result [%d, %d]:\n", i - 1, j - 1);
    printf("%d\n", e1);
}

// The interrupt service routine.
static void blur_filder_isr(void *timer_arg)
{
    // reset.
    STATUS_CPU0 = 0;
    STATUS_CPU1 = 0;
    // make the new array.
    create_128x128_array((XScuTimer * )(XScuTimer * )
    timer_arg);

    // start processing.
    STATUS_CPU0 = CPU0_DONE_GEN;

    // start calculation.
    calculate_array();

    while (STATUS_CPU1 != CPU1_DONE_PROC);
    printf("Done with calculations of one image\n");
}

static int configure_interrupts()
{
    XScuGic_Config *config;
    int status;

    config = XScuGic_LookupConfig(XPAR_PS7_SCUGIC_0_DEVICE_ID);
    if (config == NULL) return XST_FAILURE;

    status = XScuGic_CfgInitialize(&intr_controller, config,
                                   config->CpuBaseAddress);
    if (status != XST_SUCCESS) return XST_FAILURE;

    // enable interrupts on hardware.
    Xil_ExceptionInit();

    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_IRQ_INT,
                                 (Xil_ExceptionHandler) XScuGic_InterruptHandler,
                                 &intr_controller);

    // enable interrupts for scu timer.
    status = XScuGic_Connect(&intr_controller, XPAR_PS7_SCUTIMER_0_INTR,
                             (Xil_ExceptionHandler) blur_filder_isr,
                             (void *) &timer);
    if (status != XST_SUCCESS) return XST_FAILURE;

    // enable interrupts for the timer device.
    XScuGic_Enable(&intr_controller, XPAR_PS7_SCUTIMER_0_INTR);
    XScuTimer_EnableInterrupt(&timer);

    Xil_ExceptionEnable();

    return 0;
}

static int configure_timer()
{
    XScuTimer_Config *config;
    int status;

    config = XScuTimer_LookupConfig(XPAR_XSCUTIMER_0_DEVICE_ID);

    status = XScuTimer_CfgInitialize(&timer, config,
                                     config->BaseAddr);
    if (status != XST_SUCCESS) return XST_FAILURE;

    status = configure_interrupts();
    if (status != XST_SUCCESS) return XST_FAILURE;

    XScuTimer_EnableAutoReload(&timer);
    XScuTimer_LoadTimer(&timer, TIMER_LOAD);
    XScuTimer_Start(&timer);
    return 0;
}

int main(void)
{
    if (STATUS_CPU0 != 0) {
        STATUS_CPU0 = 0;
        STATUS_CPU1 = 0;
    }

    initialize_org_array();

    configure_timer();

    init_platform();
    Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

    // run forever.
    while (1);
    cleanup_platform();
}
