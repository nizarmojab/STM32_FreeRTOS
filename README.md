# STM32 FreeRTOS Project

This project demonstrates the use of FreeRTOS on an STM32F446RE microcontroller. It includes practical implementations of FreeRTOS functionalities, such as task management, synchronization mechanisms, and real-time monitoring. The project aims to showcase FreeRTOS's capability to handle concurrent tasks on an STM32 platform with precision timing and efficient resource management.

## Project Overview

The repository is organized into multiple folders, each demonstrating a unique feature or example of FreeRTOS functionality on the STM32 platform. Examples include task creation, queue management, semaphore handling, mutual exclusion using binary semaphores and mutexes, and task notification systems.

### Key Project Features

- **Task Management**: Examples of creating, scheduling, and managing tasks with various priorities in FreeRTOS.
- **Synchronization Mechanisms**: Demonstrations of semaphores, including binary and counting semaphores, to handle task synchronization.
- **Queue and Notification Systems**: Implementation of queues to enable inter-task communication and task notifications.
- **GPIO Control**: Examples that showcase GPIO interaction for LED and button controls using FreeRTOS tasks.
- **Power Management**: The use of idle tasks and idle hooks to reduce power consumption when the CPU is not actively used.

## Hardware and Tools

- **STM32F446RE Microcontroller**: The project is specifically designed for the STM32F446RE, a microcontroller from STMicroelectronics equipped with an ARM Cortex-M4 core. This MCU provides sufficient processing power and features for real-time applications using FreeRTOS.
- **Tera Term**: Tera Term is used for serial communication monitoring, providing a direct way to interact with and monitor the embedded system. It allows real-time observation of system behavior, logs, and debug messages, making it essential for testing and debugging the application.
- **SEGGER SystemView**: SEGGER SystemView is utilized for in-depth analysis and real-time visualization of task execution. It helps monitor task behavior, execution times, and scheduling to optimize and verify system performance under real-time constraints. 
  - Continuous recording is supported via `STM32_FreeRTOS11_continuous_recording`.
  - Single-shot recording examples are available in `STM32_FreeRTOS11_single_shot_recording`.

## Folder Structure

Each folder represents a specific FreeRTOS feature or example, allowing for targeted testing and learning of individual FreeRTOS functions.

- `STM32_HelloWorld` - Basic project setup to verify FreeRTOS and STM32 initialization.
- `STM32_FreeRTOS_Bin_Sema_Tasks` - Binary semaphore example for task synchronization.
- `STM32_FreeRTOS_Cnt_Sema_Tasks` - Counting semaphore for handling multiple concurrent events.
- `STM32_FreeRTOS_Idle_Hook_Power_Saving` - Demonstrates power saving using idle hook functions.
- `STM32_FreeRTOS_Led_and_Button` - Basic GPIO control using tasks to manage an LED and button.
- `STM32_FreeRTOS_Led_and_Button_IT` - Interrupt-based GPIO control with FreeRTOS task management.
- `STM32_FreeRTOS_MutexAPI` - Demonstrates mutex usage for mutual exclusion in shared resource access.
- `STM32_FreeRTOS_Mutex_using_Bin_Sema` - Mutual exclusion implemented with binary semaphores.
- `STM32_FreeRTOS_Queue_Processing` - Queue management example for task communication.
- `STM32_FreeRTOS_Tasks_Delete` - Task deletion example with FreeRTOS.
- `STM32_FreeRTOS_Tasks_Notify` - Task notification mechanism for inter-task signaling.
- `STM32_FreeRTOS_Tasks_Priority` - Task priority and scheduling demonstration.
- `STM32_FreeRTOS_vTaskDelay` - Example of task delays and timing control.

## Getting Started

To run the project, make sure to have the following:

1. An STM32F446RE microcontroller or compatible STM32 board.
2. A configured development environment with STM32CubeIDE or another IDE supporting STM32.
3. Tera Term installed for serial communication.
4. SEGGER SystemView for task monitoring (optional but recommended).



Use Tera Term to monitor serial outputs. For detailed task execution analysis, connect SEGGER SystemView to observe task scheduling, context switches, and response times. This setup ensures accurate performance tracking for real-time constraints in the application.

---

This README provides a comprehensive overview of the project, including its features, hardware requirements, tools, and examples. For further information on specific folders or additional configurations, please refer to the comments within each example's code files.
