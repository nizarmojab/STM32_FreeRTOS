#include<string.h>
#include<stdint.h>
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"

// Define constants for boolean values
#define TRUE 1
#define FALSE 0

// Define constants for button state
#define NOT_PRESSED FALSE
#define PRESSED TRUE

// Function prototypes
static void prvSetupHardware(void);
void printmsg(char *msg);
static void prvSetupUart(void);
void prvSetupGpio(void);

// Task prototypes
void led_task_handler(void *params);
void button_handler(void *params);

// Global variable to store button status
uint8_t button_status_flag = NOT_PRESSED;

int main(void)
{
	// Enable CYCCNT in DWT_CTRL for timing measurements
	DWT->CTRL |= (1 << 0);

	// 1. Reset the RCC clock configuration to the default reset state
	// HSI ON, PLL OFF, HSE OFF, system clock = 16MHz, cpu_clock = 16MHz
	RCC_DeInit();

	// 2. Update the SystemCoreClock variable
	SystemCoreClockUpdate();

	// Initialize hardware components (GPIO, UART, etc.)
	prvSetupHardware();

	// Start recording with SEGGER SystemView
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();

	// Create led_task with a priority of 2
    xTaskCreate(led_task_handler, "LED-TASK", configMINIMAL_STACK_SIZE, NULL, 2, NULL);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

	// Infinite loop in case scheduler fails
	for(;;);
}

// Task handler function for LED control
void led_task_handler(void *params)
{
	while(1)
	{
		if(button_status_flag == PRESSED)
		{
			// Turn on the LED if the button is pressed
			GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
		}
		else
		{
			// Turn off the LED if the button is not pressed
			GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
		}
	}
}

// Button interrupt handler function to toggle button status flag
void button_handler(void *params)
{
	// Toggle the button status flag
	button_status_flag ^= 1;
}

// Initialize hardware (GPIO for button and LED, UART)
static void prvSetupHardware(void)
{
	// Setup GPIO for button and LED
	prvSetupGpio();

	// Setup UART for communication
	prvSetupUart();
}

// Function to print messages via UART
void printmsg(char *msg)
{
	for(uint32_t i = 0; i < strlen(msg); i++)
	{
		// Wait until the transmit data register is empty
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
		USART_SendData(USART2, msg[i]);
	}

	// Wait until transmission is complete
	while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
}

// UART setup function
static void prvSetupUart(void)
{
	GPIO_InitTypeDef gpio_uart_pins;
	USART_InitTypeDef uart2_init;

	// 1. Enable the UART2 and GPIOA peripheral clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// PA2 is UART2_TX, PA3 is UART2_RX

	// 2. Configure GPIO pins as alternate function for UART2 TX and RX
	memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins));  // Initialize structure to zero
	gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
	gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
	gpio_uart_pins.GPIO_OType = GPIO_OType_PP;
	gpio_uart_pins.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(GPIOA, &gpio_uart_pins);

	// 3. Set alternate function for PA2 and PA3 to UART2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);  // PA2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);  // PA3

	// 4. UART parameter initialization
	memset(&uart2_init, 0, sizeof(uart2_init));  // Initialize structure to zero
	uart2_init.USART_BaudRate = 115200;
	uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	uart2_init.USART_Parity = USART_Parity_No;
	uart2_init.USART_StopBits = USART_StopBits_1;
	uart2_init.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &uart2_init);

	// 5. Enable the UART2 peripheral
	USART_Cmd(USART2, ENABLE);
}

// GPIO setup for button and LED
void prvSetupGpio(void)
{
	// Enable peripheral clock for GPIOA and GPIOC and SYSCFG
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	GPIO_InitTypeDef led_init, button_init;

	// Configure LED pin (PA5) as output
	led_init.GPIO_Mode = GPIO_Mode_OUT;
	led_init.GPIO_OType = GPIO_OType_PP;
	led_init.GPIO_Pin = GPIO_Pin_5;
	led_init.GPIO_Speed = GPIO_Low_Speed;
	led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &led_init);

	// Configure button pin (PC13) as input
	button_init.GPIO_Mode = GPIO_Mode_IN;
	button_init.GPIO_OType = GPIO_OType_PP;
	button_init.GPIO_Pin = GPIO_Pin_13;
	button_init.GPIO_Speed = GPIO_Low_Speed;
	button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOC, &button_init);

	// Interrupt configuration for the button (PC13)
	// 1. Configure SYSCFG for EXTI line for PC13
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	// 2. Configure EXTI line for PC13, trigger on falling edge, interrupt mode
	EXTI_InitTypeDef exti_init;
	exti_init.EXTI_Line = EXTI_Line13;
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
	exti_init.EXTI_LineCmd = ENABLE;
	EXTI_Init(&exti_init);

	// 3. Set NVIC priority and enable EXTI interrupt for line 13
	NVIC_SetPriority(EXTI15_10_IRQn, 5);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// Interrupt handler for EXTI lines 10 to 15
void EXTI15_10_IRQHandler(void)
{
    traceISR_ENTER();

	// 1. Clear the interrupt pending bit for EXTI line 13
	EXTI_ClearITPendingBit(EXTI_Line13);

	// Call button handler to update button status
	button_handler(NULL);

	traceISR_EXIT();
}
