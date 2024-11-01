#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"

#define TRUE 1
#define FALSE 0

// Function prototypes for hardware setup
static void prvSetupHardware(void);
void printmsg(char *msg);
static void prvSetupUart(void);
void prvSetupGpio(void);

// Task prototypes
void vTask1_handler(void *params);
void vTask2_handler(void *params);

// Global variables
char usr_msg[250] = {0};                 // Buffer for UART messages
TaskHandle_t xTaskHandle1 = NULL;         // Task handle for Task 1
TaskHandle_t xTaskHandle2 = NULL;         // Task handle for Task 2

int main(void)
{
    // Enable the CPU cycle counter (used for debugging/profiling)
    DWT->CTRL |= (1 << 0);

    // Reset the RCC clock configuration to default (system clock = 16MHz)
    RCC_DeInit();
    SystemCoreClockUpdate();

    // Initialize hardware components
    prvSetupHardware();

    // Start SEGGER SYSVIEW recording (for debugging)
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();

    // Print initial message over UART
    sprintf(usr_msg, "This is Task Switching Priority Demo\r\n");
    printmsg(usr_msg);

    // Create Task 1 with priority 2
    xTaskCreate(vTask1_handler, "TASK-1", 500, NULL, 2, &xTaskHandle1);

    // Create Task 2 with priority 3
    xTaskCreate(vTask2_handler, "TASK-2", 500, NULL, 3, &xTaskHandle2);

    // Start the FreeRTOS scheduler to begin task execution
    vTaskStartScheduler();

    for (;;);  // Infinite loop if the scheduler fails to start
}

// Task 1 handler function
void vTask1_handler(void *params)
{
    while (1)
    {
        // Read and print the LED status to UART
        sprintf(usr_msg, "Status of the LED is: %d\r\n", GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_5));
        printmsg(usr_msg);

        // Delay task for 1000 ms (1 second)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Task 2 handler function
void vTask2_handler(void *params)
{
    while (1)
    {
        // Toggle the LED state
        GPIO_ToggleBits(GPIOA, GPIO_Pin_5);

        // Delay task for 1000 ms (1 second)
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// Hardware setup function
static void prvSetupHardware(void)
{
    // Setup GPIO for Button and LED
    prvSetupGpio();

    // Setup UART2 for serial communication
    prvSetupUart();
}

// UART message printing function
void printmsg(char *msg)
{
    for (uint32_t i = 0; i < strlen(msg); i++)
    {
        // Wait until the TX buffer is empty
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

    // Enable clocks for UART2 and GPIOA peripherals
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // Configure GPIO pins for UART TX (PA2) and RX (PA3)
    memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins));
    gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
    gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_uart_pins.GPIO_OType = GPIO_OType_PP;
    gpio_uart_pins.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOA, &gpio_uart_pins);

    // Set alternate function for UART TX and RX pins
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // PA2 -> USART2_TX
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // PA3 -> USART2_RX

    // Initialize UART parameters (baud rate, flow control, etc.)
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

// GPIO setup function for LED and Button
void prvSetupGpio(void)
{
    // Enable clocks for GPIOA (LED) and GPIOC (Button)
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

    // Configure Button pin (PC13) as input
    button_init.GPIO_Mode = GPIO_Mode_IN;
    button_init.GPIO_OType = GPIO_OType_PP;
    button_init.GPIO_Pin = GPIO_Pin_13;
    button_init.GPIO_Speed = GPIO_Low_Speed;
    button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &button_init);
}
