#include "platform.h"
#include "xil_printf.h"

#include "stdlib.h"
#include "stdio.h"

#include "xscugic.h"
#include "xscutimer.h"

#include "convolution.h"

static XScuTimer timer; // the timer.
static XScuGic intr_controller; // interrupt controller.


static void calculate_array()
{
    for (int i = 65; i <= ARRAY_R; i++) {
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
        }
    }
}


// The interrupt service routine.
static void blur_filder_isr(void *timer_arg)
{
    // while the cpu0 is not done with the array
    while (STATUS_CPU0 != CPU0_DONE_GEN);

    calculate_array();
    STATUS_CPU1 = CPU1_DONE_PROC;
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
    init_platform();
    Xil_SetTlbAttributes(0xFFFF0000, 0x14de2);

    configure_timer();

    while (1);
    cleanup_platform();
}
