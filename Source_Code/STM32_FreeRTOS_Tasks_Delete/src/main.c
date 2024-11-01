#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include "stm32f4xx.h"        // STM32F4 standard peripheral library
#include "FreeRTOS.h"         // FreeRTOS library
#include "task.h"             // FreeRTOS task management library

#define TRUE 1
#define FALSE 0
#define NOT_PRESSED FALSE
#define PRESSED TRUE

// Function prototypes
static void prvSetupHardware(void);    // Hardware setup function
void printmsg(char *msg);              // Function to send messages via UART
static void prvSetupUart(void);        // UART setup function
void prvSetupGpio(void);               // GPIO setup function
void rtos_delay(uint32_t delay_in_ms); // Custom delay function for RTOS

// Task prototypes
void vTask1_handler(void *params);     // Task 1 handler function
void vTask2_handler(void *params);     // Task 2 handler function

// Global variables
char usr_msg[250] = {0};               // Message buffer for UART communication
TaskHandle_t xTaskHandle1 = NULL;      // Task handle for Task 1
TaskHandle_t xTaskHandle2 = NULL;      // Task handle for Task 2

int main(void)
{
    // Enable cycle counter for debugging/performance measurement
    DWT->CTRL |= (1 << 0);

    // Reset clock configuration to default
    RCC_DeInit();

    // Update system core clock to reflect changes
    SystemCoreClockUpdate();

    // Initialize hardware
    prvSetupHardware();

    // Initial message to indicate the start of the program
    sprintf(usr_msg, "This is Task Delete Demo\r\n");
    printmsg(usr_msg);

    // Create Task 1 with priority 1
    xTaskCreate(vTask1_handler, "TASK-1", 500, NULL, 1, &xTaskHandle1);

    // Create Task 2 with priority 2
    xTaskCreate(vTask2_handler, "TASK-2", 500, NULL, 2, &xTaskHandle2);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Infinite loop in case scheduler fails
    for (;;);
}

// Task 1 handler
void vTask1_handler(void *params)
{
    sprintf(usr_msg, "Task-1 is running\r\n");
    printmsg(usr_msg);

    while (1)
    {
        // Delay 200 ms between toggles
        vTaskDelay(200);
        GPIO_ToggleBits(GPIOA, GPIO_Pin_5); // Toggle LED connected to PA5
    }
}

// Task 2 handler
void vTask2_handler(void *params)
{
    sprintf(usr_msg, "Task-2 is running\r\n");
    printmsg(usr_msg);

    while (1)
    {
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13))
        {
            // If the button is not pressed, toggle LED every 1 second
            rtos_delay(1000);
            GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
        }
        else
        {
            // If the button is pressed, delete Task 2
            sprintf(usr_msg, "Task2 is getting deleted\r\n");
            printmsg(usr_msg);
            vTaskDelete(NULL); // Delete current task
        }
    }
}

// Function to set up hardware
static void prvSetupHardware(void)
{
    prvSetupGpio(); // Setup GPIO for LED and button
    prvSetupUart(); // Setup UART for debugging
}

// Function to send a message via UART
void printmsg(char *msg)
{
    for (uint32_t i = 0; i < strlen(msg); i++)
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET); // Wait for transmit buffer to be empty
        USART_SendData(USART2, msg[i]); // Send each character
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET); // Wait for transmission to complete
}

// UART setup function
static void prvSetupUart(void)
{
    GPIO_InitTypeDef gpio_uart_pins;
    USART_InitTypeDef uart2_init;

    // Enable UART2 and GPIOA clocks
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

    // Set alternate functions for PA2 and PA3 to UART2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // UART configuration
    memset(&uart2_init, 0, sizeof(uart2_init));
    uart2_init.USART_BaudRate = 115200;
    uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    uart2_init.USART_Parity = USART_Parity_No;
    uart2_init.USART_StopBits = USART_StopBits_1;
    uart2_init.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &uart2_init);

    // Enable UART2
    USART_Cmd(USART2, ENABLE);
}

// GPIO setup for LED and button
void prvSetupGpio(void)
{
    // Enable clocks for GPIOA and GPIOC
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    GPIO_InitTypeDef led_init, button_init;

    // LED (PA5) configuration
    led_init.GPIO_Mode = GPIO_Mode_OUT;
    led_init.GPIO_OType = GPIO_OType_PP;
    led_init.GPIO_Pin = GPIO_Pin_5;
    led_init.GPIO_Speed = GPIO_Low_Speed;
    led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &led_init);

    // Button (PC13) configuration
    button_init.GPIO_Mode = GPIO_Mode_IN;
    button_init.GPIO_OType = GPIO_OType_PP;
    button_init.GPIO_Pin = GPIO_Pin_13;
    button_init.GPIO_Speed = GPIO_Low_Speed;
    button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &button_init);
}

// Custom delay function using RTOS ticks
void rtos_delay(uint32_t delay_in_ms)
{
    uint32_t current_tick_count = xTaskGetTickCount(); // Get current tick count
    uint32_t delay_in_ticks = (delay_in_ms * configTICK_RATE_HZ) / 1000; // Convert ms to ticks

    // Loop until the required delay is met
    while (xTaskGetTickCount() < (current_tick_count + delay_in_ticks));
}
