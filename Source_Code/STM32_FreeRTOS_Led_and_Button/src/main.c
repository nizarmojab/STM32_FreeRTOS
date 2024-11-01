#include <string.h>
#include <stdint.h>
#include "stm32f4xx.h"

#include "FreeRTOS.h"
#include "task.h"

// Define constants
#define TRUE 1
#define FALSE 0
#define NOT_PRESSED FALSE
#define PRESSED TRUE

// Function prototypes
static void prvSetupHardware(void); // Setup all required hardware peripherals
void printmsg(char *msg); // Print message via UART
static void prvSetupUart(void); // Setup UART peripheral
void prvSetupGpio(void); // Setup GPIO for LED and Button

// Task function prototypes
void led_task_handler(void *params); // LED task
void button_task_handler(void *params); // Button task

// Global variable to track button status
uint8_t button_status_flag = NOT_PRESSED;

int main(void)
{
    // 1. Reset the RCC clock configuration to the default reset state
    // HSI ON, PLL OFF, HSE OFF, system clock = 16MHz, cpu_clock = 16MHz
    RCC_DeInit();

    // 2. Update the SystemCoreClock variable
    SystemCoreClockUpdate();

    // Setup hardware peripherals (GPIO, UART)
    prvSetupHardware();

    // Start recording using SystemView for debugging
    SEGGER_SYSVIEW_Conf();
    SEGGER_SYSVIEW_Start();

    // Create LED task
    xTaskCreate(led_task_handler, "LED-TASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Create Button task
    xTaskCreate(button_task_handler, "BUTTON-TASK", configMINIMAL_STACK_SIZE, NULL, 1, NULL);

    // Start the FreeRTOS scheduler
    vTaskStartScheduler();

    // Infinite loop (control should never reach here)
    for (;;);
}

// LED task: Turns LED on/off based on button status
void led_task_handler(void *params)
{
    while (1)
    {
        if (button_status_flag == PRESSED)
        {
            // Turn on the LED
            GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_SET);
        }
        else
        {
            // Turn off the LED
            GPIO_WriteBit(GPIOA, GPIO_Pin_5, Bit_RESET);
        }
    }
}

// Button task: Polls button status and updates flag
void button_task_handler(void *params)
{
    while (1)
    {
        if (GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13))
        {
            // Button is not pressed
            button_status_flag = NOT_PRESSED;
        }
        else
        {
            // Button is pressed
            button_status_flag = PRESSED;
        }
    }
}

// Hardware setup: Configures GPIO for LED and Button, and UART
static void prvSetupHardware(void)
{
    prvSetupGpio(); // Setup GPIO for button and LED
    prvSetupUart(); // Setup UART2 for communication
}

// UART print message function
void printmsg(char *msg)
{
    for (uint32_t i = 0; i < strlen(msg); i++)
    {
        // Wait until transmit data register is empty
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET);
        USART_SendData(USART2, msg[i]);
    }

    // Wait until transmission is complete
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
}

// UART2 setup: Initializes GPIO pins and UART settings
static void prvSetupUart(void)
{
    GPIO_InitTypeDef gpio_uart_pins;
    USART_InitTypeDef uart2_init;

    // 1. Enable clocks for UART2 and GPIOA
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    // 2. Configure PA2 (TX) and PA3 (RX) as alternate function UART2
    memset(&gpio_uart_pins, 0, sizeof(gpio_uart_pins));
    gpio_uart_pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    gpio_uart_pins.GPIO_Mode = GPIO_Mode_AF;
    gpio_uart_pins.GPIO_PuPd = GPIO_PuPd_UP;
    gpio_uart_pins.GPIO_OType = GPIO_OType_PP;
    gpio_uart_pins.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOA, &gpio_uart_pins);

    // 3. Configure alternate function for pins
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

    // 4. Initialize UART parameters
    memset(&uart2_init, 0, sizeof(uart2_init));
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

// GPIO setup: Configures GPIOA for LED and GPIOC for Button
void prvSetupGpio(void)
{
    // Enable clocks for GPIOA and GPIOC
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

    GPIO_InitTypeDef led_init, button_init;

    // Configure PA5 as output for LED
    led_init.GPIO_Mode = GPIO_Mode_OUT;
    led_init.GPIO_OType = GPIO_OType_PP;
    led_init.GPIO_Pin = GPIO_Pin_5;
    led_init.GPIO_Speed = GPIO_Low_Speed;
    led_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &led_init);

    // Configure PC13 as input for Button
    button_init.GPIO_Mode = GPIO_Mode_IN;
    button_init.GPIO_OType = GPIO_OType_PP;
    button_init.GPIO_Pin = GPIO_Pin_13;
    button_init.GPIO_Speed = GPIO_Low_Speed;
    button_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &button_init);
}
