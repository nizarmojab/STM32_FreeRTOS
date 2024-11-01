#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"

// Task handles for LED and Button tasks
TaskHandle_t xTaskHandle1 = NULL;
TaskHandle_t xTaskHandle2 = NULL;

// Task function prototypes
void vtask_led_handler(void *params);
void vtask_button_handler(void *params);

// Function prototypes for hardware setup and utilities
static void prvSetupHardware(void);
static void prvSetupUart(void);
void prvSetupGpio(void);
void printmsg(char *msg);
void rtos_delay(uint32_t delay_in_ms);

// Macro definitions for clarity
#define TRUE 1
#define FALSE 0

// Global variables
char usr_msg[250] = {0};       // Buffer to store messages for UART printing
uint32_t notification_value = 0; // Variable to hold notification values

int main(void)
{
    // Enable the DWT cycle counter for timing/debugging purposes
    DWT->CTRL |= (1 << 0); // Enable CYCCNT in DWT_CTRL.

    // 1. Reset the RCC clock configuration to the default reset state.
    // HSI ON, PLL OFF, HSE OFF, system clock = 16MHz, cpu_clock = 16MHz
    RCC_DeInit();

    // 2. Update the SystemCoreClock variable based on the new clock configuration
    SystemCoreClockUpdate();

    // Set up all required hardware components (GPIO, UART)
    prvSetupHardware();

    // Print a message to UART
    sprintf(usr_msg, "This is Demo of Task Notify APIs\r\n");
    printmsg(usr_msg);

    // Start recording for debugging using SEGGER SYSVIEW
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();

    // 3. Create LED task and Button task
    xTaskCreate(vtask_led_handler, "TASK-LED", 500, NULL, 2, &xTaskHandle1);
    xTaskCreate(vtask_button_handler, "TASK-BUTTON", 500, NULL, 2, &xTaskHandle2);

    // 4. Start the FreeRTOS scheduler to begin executing tasks
    vTaskStartScheduler();

    // Infinite loop, though the code should never reach here
    for (;;);
}

// LED task handler function
void vtask_led_handler(void *params)
{
    uint32_t current_notification_value = 0;
    while (1)
    {
        // Wait indefinitely for a notification from the button task
        if (xTaskNotifyWait(0, 0, &current_notification_value, portMAX_DELAY) == pdTRUE)
        {
            // Toggle the LED upon receiving the notification
            GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
            sprintf(usr_msg, "Notification is received: Button press count: %ld\r\n", current_notification_value);
            printmsg(usr_msg); // Print the notification count to UART
        }
    }
}

// Button task handler function
void vtask_button_handler(void *params)
{
    while (1)
    {
        // Check if the button is pressed (active-low)
        if (!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13))
        {
            // Button press detected; wait 100ms for debounce
            rtos_delay(100);

            // Notify the LED task to toggle the LED, incrementing notification value
            xTaskNotify(xTaskHandle1, 0x0, eIncrement);
        }
    }
}

// UART setup function
static void prvSetupUart(void)
{
    GPIO_InitTypeDef gpio_uart_pins;
    USART_InitTypeDef uart2_init;

    // 1. Enable UART2 and GPIOA peripheral clocks
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // PA2 is UART2_TX, PA3 is UART2_RX

    // 2. Configure PA2 and PA3 as UART2 TX and RX pins
    memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins)); // Clear the structure
    gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
    gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_uart_pins.GPIO_OType = GPIO_OType_PP;
    gpio_uart_pins.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOA, &gpio_uart_pins);

    // 3. Set the alternate function for the pins to use UART2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); // PA2
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); // PA3

    // 4. Initialize UART2 with specified parameters
    memset(&uart2_init, 0, sizeof(uart2_init)); // Clear the structure
    uart2_init.USART_BaudRate = 115200;
    uart2_init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    uart2_init.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    uart2_init.USART_Parity = USART_Parity_No;
    uart2_init.USART_StopBits = USART_StopBits_1;
    uart2_init.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART2, &uart2_init);

    // 5. Enable UART2
    USART_Cmd(USART2, ENABLE);
}

// GPIO setup function for LED and button
void prvSetupGpio(void)
{
    // Enable clocks for GPIOA (LED) and GPIOC (Button) peripherals
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    // Configure PA5 as output for the LED
    GPIO_InitTypeDef led_init, button_init;
    led_init.GPIO_Mode = GPIO_Mode_OUT;
    led_init.GPIO_OType = GPIO_OType_PP;
    led_init.GPIO_Pin = GPIO_Pin_5;
    led_init.GPIO_Speed = GPIO_Low_Speed;
    led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &led_init);

    // Configure PC13 as input for the button
    button_init.GPIO_Mode = GPIO_Mode_IN;
    button_init.GPIO_OType = GPIO_OType_PP;
    button_init.GPIO_Pin = GPIO_Pin_13;
    button_init.GPIO_Speed = GPIO_Low_Speed;
    button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &button_init);
}

// Consolidated hardware setup function
static void prvSetupHardware(void)
{
    prvSetupGpio();  // Setup GPIO for LED and button
    prvSetupUart();  // Setup UART for serial communication
}

// Function to print messages via UART
void printmsg(char *msg)
{
    for (uint32_t i = 0; i < strlen(msg); i++)
    {
        // Wait until the TX buffer is empty, then send data
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
        USART_SendData(USART2, msg[i]);
    }
    // Wait until transmission is complete
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
}

// Function to introduce a blocking delay in FreeRTOS (in milliseconds)
void rtos_delay(uint32_t delay_in_ms)
{
    // Get the current tick count
    uint32_t current_tick_count = xTaskGetTickCount();

    // Convert the delay in milliseconds to ticks
    uint32_t delay_in_ticks = (delay_in_ms * configTICK_RATE_HZ) / 1000;

    // Block until the specified delay time has elapsed
    while (xTaskGetTickCount() < (current_tick_count + delay_in_ticks));
}
