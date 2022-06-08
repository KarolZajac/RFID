/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2022 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "../RFID/Inc/rfid_task.h"

#include "usb_host.h"
#include "fatfs.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId gfxTaskHandle;
osThreadId rfidTaskHandle;
extern ApplicationTypeDef Appli_state;

/* USER CODE END Variables */
osThreadId defaultTaskHandle;
uint32_t defaultTaskBuffer[256];
osStaticThreadDef_t defaultTaskControlBlock;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void startGfxTask(const void *_)
{
	UNUSED(_);
	MX_TouchGFX_Process();
}

void waitForUsbMsd(void)
{
	xprintf("\nwaiting for USB device...\n");
	do
	{
		vTaskDelay(100);
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
	} while (Appli_state != APPLICATION_READY);
	xprintf("USB device ready!\n");
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
}

void listdir()
{
	xprintf("Listing files:\n");
	FRESULT res;
	DIR d;
	FILINFO f;
	res = f_findfirst(&d, &f, "0:/", "*");
	xprintf("Opened first!\n");
	while (res == FR_OK && f.fname[0])
	{
		xprintf("Found file: %s\n", f.fname);
		res = f_findnext(&d, &f);
	}
	f_closedir(&d);
}

void dumpFile()
{
	/* USER CODE END 2 */
	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	FIL f;
	if (f_open(&f, "0:/test.txt", FA_READ) != FR_OK)
		xprintf("Failed to open file!\n");
	int sumDumped = 0;

	char buffer[17];
	buffer[16] = 0;
	int numread = 0;
	while (f_read(&f, buffer, 16, &numread) == FR_OK && numread != 0)
	{
		sumDumped += numread;
		xprintf("Read chunk: %s\n", buffer);
	}
	xprintf("Read %d bytes from file.\n", sumDumped);
	f_close(&f);
}

void touchgfxSignalVSync(void);
/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const *argument);

extern void MX_USB_HOST_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize);

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
		StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
	*ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
	*ppxIdleTaskStackBuffer = &xIdleStack[0];
	*pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
	/* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
	osThreadStaticDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 256,
			defaultTaskBuffer, &defaultTaskControlBlock);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	osThreadDef(gfxTask, startGfxTask, osPriorityNormal, 0, 512);
	gfxTaskHandle = osThreadCreate(osThread(gfxTask), NULL);

	osThreadDef(rfidTask, startRfidTask, osPriorityNormal, 0, 256);
	rfidTaskHandle = osThreadCreate(osThread(rfidTask), NULL);
	/* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const *argument)
{
	/* init code for USB_HOST */
	MX_USB_HOST_Init();
	/* USER CODE BEGIN StartDefaultTask */
	MX_DriverVbusFS(1);

	waitForUsbMsd();
	/* Infinite loop */
	xprintf("\nwaiting for USB device...\n");
	for (;;)
	{
		osDelay(100);
		touchgfxSignalVSync();
		if (Appli_state == APPLICATION_READY)
		{
			//xprintf("USB connected!\n");
			static uint8_t done = 0;
			if (!done)
			{
				done = 1;
				listdir();
				xprintf("Listed files!\n");
			}

		}
	}
	/* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
