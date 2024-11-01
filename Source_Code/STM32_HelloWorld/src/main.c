#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"

// Task handles for Task-1 and Task-2
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

// Task function prototypes
void vTask1_handler(void *params);
void vTask2_handler(void *params);

#ifdef USE_SEMIHOSTING
// Function for semihosting support
extern void initialise_monitor_handles();
#endif

// Function prototypes for hardware setup and message printing
static void prvSetupHardware(void);
static void prvSetupUart(void);
void printmsg(char *msg);

// Some macros for state definitions
#define TRUE 1
#define FALSE 0
#define AVAILABLE TRUE
#define NOT_AVAILABLE FALSE

// Global variables
char usr_msg[250] = {0};
uint8_t UART_ACCESS_KEY = AVAILABLE;

int main(void)
{

#ifdef USE_SEMIHOSTING
	// Initialize semihosting for printf output
	initialise_monitor_handles();
	printf("This is hello world example code\n");
#endif

	// Enable CYCCNT in DWT_CTRL for cycle counting
	DWT->CTRL |= (1 << 0);

	// Reset RCC clock configuration to default reset state
	RCC_DeInit();

	// Update SystemCoreClock variable
	SystemCoreClockUpdate();

	// Setup hardware peripherals
	prvSetupHardware();

	// Initial message indicating the application has started
	sprintf(usr_msg, "This is hello-world application starting\r\n");
	printmsg(usr_msg);

	// Start SEGGER SystemView recording for debugging
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	// Create two tasks, Task-1 and Task-2, with the same priority
	xTaskCreate(vTask1_handler, "Task-1", configMINIMAL_STACK_SIZE, NULL, 2, &xTaskHandle1);
	xTaskCreate(vTask2_handler, "Task-2", configMINIMAL_STACK_SIZE, NULL, 2, &xTaskHandle2);

	// Start the FreeRTOS scheduler
	vTaskStartScheduler();

	// Infinite loop in case of failure to start the scheduler
	for(;;);
}

void vTask1_handler(void *params)
{
	while(1)
	{
		// Check if UART is available for printing
		if(UART_ACCESS_KEY == AVAILABLE)
		{
			UART_ACCESS_KEY = NOT_AVAILABLE; // Lock UART access
			printmsg("Hello-world: From Task-1\r\n");
			UART_ACCESS_KEY = AVAILABLE; // Release UART access
			SEGGER_SYSVIEW_Print("Task1 is yielding"); // Log message for debugging
			traceISR_EXIT_TO_SCHEDULER(); // Log ISR exit for SystemView
			taskYIELD(); // Force a context switch to allow Task-2 to run
		}
	}
}

void vTask2_handler(void *params)
{
	while(1)
	{
		// Check if UART is available for printing
		if(UART_ACCESS_KEY == AVAILABLE)
		{
			UART_ACCESS_KEY = NOT_AVAILABLE; // Lock UART access
			printmsg("Hello-world: From Task-2\r\n");
			UART_ACCESS_KEY = AVAILABLE; // Release UART access
			SEGGER_SYSVIEW_Print("Task2 is yielding"); // Log message for debugging
			traceISR_EXIT_TO_SCHEDULER(); // Log ISR exit for SystemView
			taskYIELD(); // Force a context switch to allow Task-1 to run
		}
	}
}

static void prvSetupUart(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;

	// Enable clocks for UART2 and GPIOA
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// Configure PA2 as UART2_TX and PA3 as UART2_RX
	memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins));
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_uart_pins.GPIO_OType = GPIO_OType_PP;
	gpio_uart_pins.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	// Set alternate functions for UART2 TX and RX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // PA2 as USART2 TX
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // PA3 as USART2 RX

	// Initialize UART parameters for communication
	memset(&uart2_init, 0, sizeof(uart2_init));
	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &uart2_init);

	// Enable UART2 peripheral
	USART_Cmd(USART2, ENABLE);
}

static void prvSetupHardware(void)
{
	// Initialize UART2 for communication
	prvSetupUart();
}

void printmsg(char *msg)
{
	// Transmit each character of the message via UART2
	for(uint32_t i = 0; i < strlen(msg); i++)
	{
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
		USART_SendData(USART2, msg[i]);
	}

	// Wait until transmission is complete
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
}
