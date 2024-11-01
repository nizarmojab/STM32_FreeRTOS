#include<string.h>
#include<stdio.h>
#include<stdint.h>
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"

#define TRUE 1
#define FALSE 0

// Function prototypes
static void prvSetupHardware(void); // Sets up hardware peripherals
void printmsg(char *msg);           // Prints a message via UART
static void prvSetupUart(void);     // Sets up UART2 for communication
void prvSetupGpio(void);            // Sets up GPIO for LED and button
void rtos_delay(uint32_t delay_in_ms); // Custom delay function for RTOS

// Task prototypes
void vTask1_handler(void *params); // Task 1 handler
void vTask2_handler(void *params); // Task 2 handler

// Global variables
char usr_msg[250] = {0};        // Message buffer for UART
TaskHandle_t xTaskHandle1 = NULL; // Handle for Task 1
TaskHandle_t xTaskHandle2 = NULL; // Handle for Task 2
uint8_t switch_prio = FALSE;       // Flag to switch priority on button press

int main(void)
{
    DWT->CTRL |= (1 << 0); // Enable CYCCNT in DWT_CTRL for timing purposes

    // 1. Reset the RCC clock configuration to the default state
    RCC_DeInit();

    // 2. Update the SystemCoreClock variable
    SystemCoreClockUpdate();

    prvSetupHardware(); // Setup hardware components

    // Start system recording (optional debugging)
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();

    sprintf(usr_msg, "This is Task Switching Priority Demo\r\n");
    printmsg(usr_msg);

    // Create Task 1 with priority 2
    xTaskCreate(vTask1_handler, "TASK-1", 500, NULL, 2, &xTaskHandle1);

    // Create Task 2 with priority 3
    xTaskCreate(vTask2_handler, "TASK-2", 500, NULL, 3, &xTaskHandle2);

    // Start the scheduler to begin task execution
    vTaskStartScheduler();

    for (;;);
}

// Task 1 handler
void vTask1_handler(void *params)
{
    UBaseType_t p1, p2;

    sprintf(usr_msg, "Task-1 is running\r\n");
    printmsg(usr_msg);

    sprintf(usr_msg, "Task-1 Priority %ld\r\n", uxTaskPriorityGet(xTaskHandle1));
    printmsg(usr_msg);

    sprintf(usr_msg, "Task-2 Priority %ld\r\n", uxTaskPriorityGet(xTaskHandle2));
    printmsg(usr_msg);

    while (1)
    {
        if (switch_prio) // Check if priority switch is requested
        {
            switch_prio = FALSE;

            // Get current priorities
            p1 = uxTaskPriorityGet(xTaskHandle1);
            p2 = uxTaskPriorityGet(xTaskHandle2);

            // Switch priorities of Task 1 and Task 2
            vTaskPrioritySet(xTaskHandle1, p2);
            vTaskPrioritySet(xTaskHandle2, p1);
        }
        else
        {
            GPIO_ToggleBits(GPIOA, GPIO_Pin_5); // Toggle LED
            rtos_delay(200); // Delay for 200 ms
        }
    }
}

// Task 2 handler
void vTask2_handler(void *params)
{
    UBaseType_t p1, p2;

    sprintf(usr_msg, "Task-2 is running\r\n");
    printmsg(usr_msg);

    sprintf(usr_msg, "Task-1 Priority %ld\r\n", uxTaskPriorityGet(xTaskHandle1));
    printmsg(usr_msg);

    sprintf(usr_msg, "Task-2 Priority %ld\r\n", uxTaskPriorityGet(xTaskHandle2));
    printmsg(usr_msg);

    while (1)
    {
        if (switch_prio) // Check if priority switch is requested
        {
            switch_prio = FALSE;

            // Get current priorities
            p1 = uxTaskPriorityGet(xTaskHandle1);
            p2 = uxTaskPriorityGet(xTaskHandle2);

            // Switch priorities of Task 1 and Task 2
            vTaskPrioritySet(xTaskHandle1, p2);
            vTaskPrioritySet(xTaskHandle2, p1);
        }
        else
        {
            GPIO_ToggleBits(GPIOA, GPIO_Pin_5); // Toggle LED
            rtos_delay(1000); // Delay for 1000 ms
        }
    }
}

// Hardware setup function
static void prvSetupHardware(void)
{
    prvSetupGpio(); // Setup GPIO for button and LED
    prvSetupUart(); // Setup UART for communication
}

// UART message printing function
void printmsg(char *msg)
{
    for (uint32_t i = 0; i < strlen(msg); i++)
    {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
        USART_SendData(USART2, msg[i]);
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
}

// UART setup function
static void prvSetupUart(void)
{
    GPIO_InitTypeDef gpio_uart_pins;
    USART_InitTypeDef uart2_init;

    // Enable clocks for UART2 and GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // Configure PA2 (TX) and PA3 (RX) for UART2
    memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins));
    gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
    gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_uart_pins.GPIO_OType = GPIO_OType_PP;
    gpio_uart_pins.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOA, &gpio_uart_pins);

    // Set alternate function for pins
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // PA2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // PA3

    // Configure UART parameters
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

// GPIO setup function
void prvSetupGpio(void)
{
    // Enable clocks for GPIOA, GPIOC, and SYSCFG (for interrupt)
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // Configure LED on PA5
    GPIO_InitTypeDef led_init, button_init;
    led_init.GPIO_Mode = GPIO_Mode_OUT;
    led_init.GPIO_OType = GPIO_OType_PP;
    led_init.GPIO_Pin = GPIO_Pin_5;
    led_init.GPIO_Speed = GPIO_Low_Speed;
    led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &led_init);

    // Configure button on PC13
    button_init.GPIO_Mode = GPIO_Mode_IN;
    button_init.GPIO_Pin = GPIO_Pin_13;
    button_init.GPIO_Speed = GPIO_Low_Speed;
    button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &button_init);

    // Configure interrupt for button on PC13
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
    EXTI_InitTypeDef exti_init;
    exti_init.EXTI_Line = EXTI_Line13;
    exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
    exti_init.EXTI_Trigger = EXTI_Trigger_Falling;
    exti_init.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_init);

    // Configure NVIC for EXTI line 13
    NVIC_SetPriority(EXTI15_10_IRQn, 5);
    NVIC_EnableIRQ(EXTI15_10_IRQn);
}

// Interrupt handler for button press
void EXTI15_10_IRQHandler(void)
{
    traceISR_ENTER();
    EXTI_ClearITPendingBit(EXTI_Line13); // Clear interrupt flag
    switch_prio = TRUE; // Set flag to switch priority
    traceISR_EXIT();
}

// Custom delay function
void rtos_delay(uint32_t delay_in_ms)
{
    uint32_t tick_count_local = xTaskGetTickCount();
    uint32_t delay_in_ticks = (delay_in_ms * configTICK_RATE_HZ) / 1000;
    while (xTaskGetTickCount() < (tick_count_local + delay_in_ticks));
}
