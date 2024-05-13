/********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************
Set Display Attributes
Set Attribute Mode	<ESC>[{attr1};...;{attrn}m
Sets multiple display attribute settings. The following lists standard attributes:
0	Reset all attributes
1	Bright
2	Dim
4	Underscore
5	Blink
7	Reverse
8	Hidden

	Foreground Colors
30	Black
31	Red
32	Green
33	Yellow
34	Blue
35	Magenta
36	Cyan
37	White

	Background Colors
40	Black
41	Red
42	Green
43	Yellow
44	Blue
45	Magenta
46	Cyan
47	White

SOURCE:
http://graphcomp.com/info/specs/ansi_col.html

*********************************************************************************************************
*********************************************************************************************************
*********************************************************************************************************/



#include "NVIC.h"
#include <stdbool.h>
#include "fsl_uart.h"
#include "fsl_port.h"


/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* UART instance and clock */

#define UART_CLK_FREQ   CLOCK_GetFreq(UART0_CLK_SRC)
#define UART_BAUDRATE 115200
#define PIN16                       16u   /*!< Pin number for pin 16 in a port */
#define PIN17                      17u   /*!< Pin number for pin 17 in a port */



typedef struct{
	uint8_t flag; /** Flag to indicate that there is new data*/
	uint8_t mail_box; /** it contains the received data*/
} uart_mail_box_t;


/*VT100 command for clearing the screen*/
uint8_t g_vt100_clear[] = "\033[2J";
/** VT100 command for setting the background*/
uint8_t g_vt100_color_1[] = "\033[0;46m";
/** VT100 command for setting the foreground*/
uint8_t g_vt100_color_2[] = "\033[34m";
/** VT100 command for setting the background and foreground*/
uint8_t g_vt100_color_3[] = "\033[1;32;41m";
/** VT100 command for setting the background and foreground*/
uint8_t g_vt100_color_4[] = "\033[0;33;40m";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_xy_1[] = "\033[10;20H";
uint8_t g_vt100_strg_1[] = "Diseno con Micros";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_xy_2[] = "\033[11;23H";
uint8_t g_vt100_strg_2[] =  "CINVESTAV\r";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_xy_3[] = "\033[12;20H";
uint8_t g_vt100_strg_3[] =  "TAE2024\r";
/** VT100 command for positioning the cursor in x and y position*/
uint8_t g_vt100_xy_4[] = "\033[13;20H";



uart_mail_box_t g_mail_box_uart_0 ={0, 0};



/*******************************************************************************
 * Code
 ******************************************************************************/

void UART0_RX_TX_IRQHandler(void)
{


    /* If new data arrived. */
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART0))
    {
    	g_mail_box_uart_0.mail_box = UART_ReadByte(UART0);
    	g_mail_box_uart_0.flag = true;

    }
    SDK_ISR_EXIT_BARRIER;
}

/*!
 * @brief Main function
 */
int main(void)
{
    uart_config_t config;
    uint32_t uart_clock;

    CLOCK_EnableClock(kCLOCK_PortB);

    PORT_SetPinMux(PORTB, PIN16, kPORT_MuxAlt3);
    PORT_SetPinMux(PORTB, PIN17, kPORT_MuxAlt3);


    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = UART_BAUDRATE;
    config.enableTx     = true;
    config.enableRx     = true;

    uart_clock = CLOCK_GetFreq(UART0_CLK_SRC);

    UART_Init(UART0, &config, uart_clock);

    /* Send g_tipString out. */
    UART_WriteBlocking(UART0, g_vt100_clear, sizeof(g_vt100_clear) / sizeof(g_vt100_clear[0]));
    UART_WriteBlocking(UART0, g_vt100_xy_1, sizeof(g_vt100_xy_1) / sizeof(g_vt100_xy_1[0]));
    UART_WriteBlocking(UART0, g_vt100_color_1, sizeof(g_vt100_color_1) / sizeof(g_vt100_color_1[0]));
    UART_WriteBlocking(UART0, g_vt100_clear, sizeof(g_vt100_clear) / sizeof(g_vt100_clear[0]));
    UART_WriteBlocking(UART0, g_vt100_strg_1, sizeof(g_vt100_strg_1) / sizeof(g_vt100_strg_1[0]));
    UART_WriteBlocking(UART0, g_vt100_xy_2, sizeof(g_vt100_xy_2) / sizeof(g_vt100_xy_2[0]));
    UART_WriteBlocking(UART0, g_vt100_color_2, sizeof(g_vt100_color_2) / sizeof(g_vt100_color_2[0]));
    UART_WriteBlocking(UART0, g_vt100_strg_2, sizeof(g_vt100_strg_2) / sizeof(g_vt100_strg_2[0]));
    UART_WriteBlocking(UART0, g_vt100_xy_3, sizeof(g_vt100_xy_3) / sizeof(g_vt100_xy_3[0]));
    UART_WriteBlocking(UART0, g_vt100_color_3, sizeof(g_vt100_color_3) / sizeof(g_vt100_color_3[0]));
    UART_WriteBlocking(UART0, g_vt100_strg_3, sizeof(g_vt100_strg_3) / sizeof(g_vt100_strg_3[0]));
    UART_WriteBlocking(UART0, g_vt100_xy_4, sizeof(g_vt100_xy_4) / sizeof(g_vt100_xy_4[0]));
    UART_WriteBlocking(UART0, g_vt100_color_4, sizeof(g_vt100_color_4) / sizeof(g_vt100_color_4[0]));

    /* Enable RX interrupt. */
    UART_EnableInterrupts(UART0, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);

    NVIC_enable_interrupt_and_priotity(UART0_IRQ, PRIORITY_10);
    NVIC_global_enable_interrupts;

    while (1)
    {
    	if(g_mail_box_uart_0.flag)
		{
			/**Sends to the PCA the received data in the mailbox*/
    		UART_WriteBlocking(UART0, &g_mail_box_uart_0.mail_box, 1);

			/**clear the reception flag*/
			g_mail_box_uart_0.flag = 0;
		}
    }
}
