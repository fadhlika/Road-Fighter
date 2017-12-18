/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2017 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

/* USER CODE BEGIN Includes */     
#include "st7735.h"
#include "game.h"
#include "dac.h"
#include "tim.h"
#include "sound.h"
/* USER CODE END Includes */

/* Variables -----------------------------------------------------------------*/
osThreadId defaultTaskHandle;
osThreadId taskDisplayGameHandle;
osThreadId taskSoundEffectHandle;
osThreadId taskUpdateGameHandle;
osMessageQId gameStateHandle;
osMessageQId trackHandle;
osMessageQId trackISRHandle;
osMessageQId trackHalfISRHandle;

/* USER CODE BEGIN Variables */

/* USER CODE END Variables */

/* Function prototypes -------------------------------------------------------*/
void StartDefaultTask(void const * argument);
void DisplayGame(void const * argument);
void SoundEffect(void const * argument);
void UpdateGame(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

/* Hook prototypes */

/* Init FreeRTOS */

void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
	ST7735_Init();
	ST7735_AddrSet(0,0,159,127);
	ST7735_Clear(0x0000);
	ST7735_Orientation(scr_180);
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

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of taskDisplayGame */
  osThreadDef(taskDisplayGame, DisplayGame, osPriorityNormal, 0, 1024);
  taskDisplayGameHandle = osThreadCreate(osThread(taskDisplayGame), NULL);

  /* definition and creation of taskSoundEffect */
  osThreadDef(taskSoundEffect, SoundEffect, osPriorityNormal, 0, 128);
  taskSoundEffectHandle = osThreadCreate(osThread(taskSoundEffect), NULL);

  /* definition and creation of taskUpdateGame */
  osThreadDef(taskUpdateGame, UpdateGame, osPriorityAboveNormal, 0, 128);
  taskUpdateGameHandle = osThreadCreate(osThread(taskUpdateGame), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Create the queue(s) */
  /* definition and creation of gameState */
  osMessageQDef(gameState, 1, uint8_t);
  gameStateHandle = osMessageCreate(osMessageQ(gameState), NULL);

  /* definition and creation of track */
  osMessageQDef(track, 1, uint8_t);
  trackHandle = osMessageCreate(osMessageQ(track), NULL);

  /* definition and creation of trackISR */
  osMessageQDef(trackISR, 1, uint8_t);
  trackISRHandle = osMessageCreate(osMessageQ(trackISR), NULL);

  /* definition and creation of trackHalfISR */
  osMessageQDef(trackHalfISR, 1, uint8_t);
  trackHalfISRHandle = osMessageCreate(osMessageQ(trackHalfISR), NULL);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* StartDefaultTask function */
void StartDefaultTask(void const * argument)
{

  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {

    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* DisplayGame function */
void DisplayGame(void const * argument)
{
  /* USER CODE BEGIN DisplayGame */
	init_player();
	init_enemy();

	GameState game_state = Main_Menu;
	osEvent event;
  /* Infinite loop */
  for(;;)
  {
	event = osMessageGet(gameStateHandle, 0);
	if(event.status == osEventMessage) game_state = event.value.v;
	switch(game_state){
	case Main_Menu:
		main_menu();
		break;
	case Starting:
		render_starting();
		osSignalWait(0x0001, osWaitForever);
		game_state = Running;
		osMessagePut(trackHandle, Engine, 0);
		break;
	case Running:
		render_player();
		render_enemy();
		render_road();
		render_score();
		check_if_over();
		break;
	case Game_Over:
		game_over();
		break;
	default:
		break;
	}
    osDelay(1);
  }
  /* USER CODE END DisplayGame */
}

/* SoundEffect function */
void SoundEffect(void const * argument)
{
  /* USER CODE BEGIN SoundEffect */
	HAL_TIM_Base_Start(&htim6);

	osEvent event;
  /* Infinite loop */
  for(;;)
  {
	  event = osMessageGet(trackHandle, osWaitForever);
	  if(event.status == osEventMessage){
		  switch(event.value.v){
		  case Explosion:
			  HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
			  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)explosion_PCM, explosion_Len, DAC_ALIGN_12B_L);
			  osMessagePut(trackHalfISRHandle, Explosion_started, 0);
			  break;
		  case Engine:
			  HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
			  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)engine_PCM, engine_Len, DAC_ALIGN_12B_L);
			  break;
		  case Start:
			  HAL_DAC_Start_DMA(&hdac, DAC_CHANNEL_1, (uint32_t*)start_PCM, start_Len, DAC_ALIGN_12B_L);
			  osMessagePut(trackISRHandle, Start, 0);
			  break;
		  default:
			  HAL_DAC_Stop_DMA(&hdac, DAC_CHANNEL_1);
			  break;
		  }
	  }
	  osDelay(1);
  }
  /* USER CODE END SoundEffect */
}

/* UpdateGame function */
void UpdateGame(void const * argument)
{
  /* USER CODE BEGIN UpdateGame */
  /* Infinite loop */
  for(;;)
  {
	  osDelay(40);
	  update_player(0);
	  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET) {
		  update_player(VELOCITY);
	  }
	  if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET) {
		  update_player(-VELOCITY);
	  }

	  update_game();
  }
  /* USER CODE END UpdateGame */
}

/* USER CODE BEGIN Application */
void HAL_DAC_ConvHalfCpltCallbackCh1(DAC_HandleTypeDef* hdac) {
	osEvent event = osMessageGet(trackHalfISRHandle, 0);
	if(event.status == osEventMessage) {
		switch(event.value.v){
		case Explosion_started:
			HAL_DAC_Stop_DMA(hdac, DAC_CHANNEL_1);
			break;
		default:
			break;
		}
	}
}

void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac) {
	osEvent event = osMessageGet(trackISRHandle, 0);
	if(event.status == osEventMessage) {
		switch(event.value.v){
		case Start:
			HAL_DAC_Stop_DMA(hdac, DAC_CHANNEL_1);
			osSignalSet(taskDisplayGameHandle, 0x0001);
			break;
		default:
			break;
		}
	}
}
/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
