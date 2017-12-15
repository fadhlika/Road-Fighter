//
// Created by fadhlika on 12/7/2017.
//

#include <stdlib.h>
#include <st7735.h>
#include "cmsis_os.h"
#include "game.h"
#include "sprites.h"

extern osMessageQId trackHandle;
extern osMessageQId gameStateHandle;

void main_menu(void) {
	static uint8_t selection = 1;
	static uint8_t button0 = 0, button1 = 0;
	ST7735_PutStr7x11(10, 10, "Road Fighter", 0xFFFF, 0x0000);

	switch(selection){
		case 1:
			ST7735_PutStr5x7(10, 30, "* Start", 0xFFFF);
			ST7735_PutStr5x7(10, 40, "  Help", 0xFFFF);
			break;
		case 2:
			ST7735_PutStr5x7(10, 30, "  Start", 0xFFFF);
			ST7735_PutStr5x7(10, 40, "* Help", 0xFFFF);
			break;
		default:
			break;
	}

	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET && button0 == 0) {
		button0 = 1;
	} else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET && button1 == 0) {
		button1 = 1;
	}

	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET && button0 == 1) {
		if(selection == 1) {
			osMessagePut(gameStateHandle, Running, 0);
			osMessagePut(trackHandle, Engine, 0);
			ST7735_AddrSet(0,0,159,127);
			ST7735_Clear(0x0000);
		}
		//else if(selection == 2) break;

	} else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET && button1 == 1) {
		selection++;
		button1 = 0;
		if(selection == 3) selection = 1;
	}
}

void game_over(void) {
	static uint8_t first_enter = 0;
	static uint8_t selection = 1;
	static uint8_t button0 = 0, button1 = 0;

	if(first_enter == 0) {
		ST7735_AddrSet(0,0,159,127);
		ST7735_Clear(0x0000);
		first_enter = 1;
	}

	ST7735_PutStr7x11(10, 10, "Game Over", 0xFFFF, 0x0000);
	ST7735_PutStr7x11(10, 30, "Score: ", 0xFFFF, 0x0000);
	uint8_t buffer[3];
	sprintf(buffer, "%d", score);
	ST7735_PutStr7x11(60, 30, &buffer, 0xFFFF, 0x0000);

	switch(selection){
		case 1:
			ST7735_PutStr5x7(10, 50, "* Retry", 0xFFFF);
			break;
		default:
			break;
	}

	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_SET && button0 == 0) {
		button0 = 1;
	} else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_SET && button1 == 0) {
		button1 = 1;
	}

	if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) == GPIO_PIN_RESET && button0 == 1) {
		if(selection == 1) {
			NVIC_SystemReset();
		}
		//else if(selection == 2) break;

	} else if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_1) == GPIO_PIN_RESET && button1 == 1) {

	}
}

void init_player(void) {
	player.w = 16;
    player.h = 32;
    player.col = 0;
    player.x = (120/2) - 8;
    player.y = (160-32);
    player.x_old = player.x;
    player.vel = 0;
}

void init_enemy(void) {
    uint8_t i;
    for(i=0;i<MAX_ENEMY;i++) {
        enemy[i].w = 18;
        enemy[i].h = 30;
        enemy[i].y = 0;
        enemy[i].x = rand() % 100 + 7;
        enemy[i].vel = VELOCITY;
        enemy[i].appear_time = (rand() % 1500) + osKernelSysTick() + 1000;
        enemy[i].appear = 0;
        enemy[i].state = 1;
    }
}

void render_player(void) {
	static uint32_t tick = 0;
	if(tick == 0) tick = osKernelSysTick();

	uint16_t i, j, k;

	//Interpolating player movement
	int distance = player.vel * (osKernelSysTick() - tick)/10.0;
	tick = osKernelSysTick();

	player.x += distance;

	//Limit player movement
	if(player.x <= 7) player.x = 7;
	else if(player.x >= 119 - player.w) player.x = 119 - player.w;

	//Clear dirty pixel player
	if(player.x > player.x_old){
		ST7735_FillRect(player.x_old, player.y, player.x, player.y + player.h, 0x0000);
	} else if(player.x < player.x_old){
		ST7735_FillRect(player.x + player.w, player.y, player.x_old + player.w, player.y + player.h, 0x0000);
	}

	//Draw player car
	k = 0;
	for(j=player.y; j < player.y + player.h; j++) {
		for(i=player.x; i < player.x +  player.w; i++) {
			ST7735_Pixel(i, j, player_pixel_data[k++]); //Draw car per pixel color
		}
	}

	player.x_old = player.x;
}

void render_road(void) {
	static uint32_t tick = 0;
	if(tick == 0) tick = osKernelSysTick();

	//Interpolating road movement
	uint32_t distance = VELOCITY * (osKernelSysTick() - tick)/10.0;
	tick = osKernelSysTick();

	uint8_t i, j;
    static uint8_t frame[4] = { 0, 40, 80, 120 };
    for(i=0; i < 4; i++){
        if(frame[i] > 130) {
            ST7735_FillRect(0, 0, 5, 30 - (160 - frame[i]), 0xffff);
            ST7735_FillRect(122, 0, 127, 30 - (160 - frame[i]), 0xffff);
        }

        for(j=0; j < distance;j++) {
            ST7735_HLine(0, 5, frame[i]+j, 0x0000);
            ST7735_HLine(122, 127, frame[i]+j, 0x0000);
        }

        ST7735_FillRect(0, 10 + frame[i], 5, 30 + frame[i], 0xffff);
        ST7735_FillRect(122, 10 + frame[i], 127, 30 + frame[i], 0xffff);

        frame[i] += distance;
        if(frame[i] >= 157) frame[i] = 0;
    }
}

void render_enemy(void) {
	static uint32_t tick = 0;
	if(tick == 0) tick = osKernelSysTick();

    uint8_t  i, j, k;
    uint16_t l;

	//Interpolating enemy movement
   	uint32_t distance = VELOCITY * (osKernelSysTick() - tick)/10.0;
   	tick = osKernelSysTick();

    for(i=0;i<MAX_ENEMY;i++) {
        if(enemy[i].appear) {
        	//Clear dirty pixel enemy[i]
        	ST7735_FillRect(enemy[i].x, enemy[i].y, enemy[i].x + enemy[i].w, enemy[i].y + distance, 0x0000);

        	enemy[i].y += distance;

        	l = 0;
        	if(160 < enemy[i].y + enemy[i].h) { //check if will draw enemy outside of screen
        		//Draw enemy[i] car
        		for(k=enemy[i].y; k < 159; k++) {
        			for(j=enemy[i].x; j < enemy[i].x +  enemy[i].w; j++) {
        				ST7735_Pixel(j, k, enemy_pixel_data[l++]); //Draw car per pixel color
        		    }
        		}
        	} else {
        		//Draw enemy[i] car
        		for(k=enemy[i].y; k < enemy[i].y + enemy[i].h; k++) {
        			for(j=enemy[i].x; j < enemy[i].x +  enemy[i].w; j++) {
        				ST7735_Pixel(j, k, enemy_pixel_data[l++]); //Draw car per pixel color
        			}
        		}
        	}

        	if(enemy[i].y >= 160) {
        		enemy[i].appear = 0;
        	    enemy[i].state = 0;
        	}
        }
    }
}

void render_score(void) {
	uint8_t buffer[3];
	ST7735_FillRect(50, 10, 100, 17, 0x0000);
	sprintf(buffer, "%d", score);
	ST7735_PutStr5x7(50, 10, &buffer, 0xFFFF);
}

void update_game(void) {
	uint16_t i;

	for(i=0;i<MAX_ENEMY;i++){
		if((player.x <= enemy[i].x + enemy[i].w) && (enemy[i].x <= player.x + player.w)
			&& (player.y <= enemy[i].y + enemy[i].h) && (enemy[i].y <= player.y + player.h)) {
			player.col = 1;
		}
		if(enemy[i].appear == 0 && enemy[i].state == 0){
			enemy[i].y = 0;
			enemy[i].x = rand() % 100 + 7;
			enemy[i].state = 1;
			if(i > 0) enemy[i].appear_time = (rand() % 1500) + enemy[i-1].appear_time + 500;
			else enemy[i].appear_time = (rand() % 1500) + osKernelSysTick() + 1000;
			score++;
		} else if(enemy[i].appear_time < osKernelSysTick()){
        	enemy[i].appear = 1;
        }
	}
}

void update_player(float dir) {
	if(dir < 0)
		player.vel = dir;
	else if(dir > 0)
		player.vel = dir;
	else
		player.vel = 0;
}

void check_if_over(void) {
	if(player.col) {
		uint16_t i,j,k = 0;
		for(j=player.y-16;j<player.y+16;j++){
			for(i=player.x;i<player.x + 30;i++){
				ST7735_Pixel(i, j, explosion[k++]);
			}
		}
		osMessagePut(trackHandle, Explosion, 0);
		osMessagePut(gameStateHandle, Game_Over, 0);
		osDelay(1000);
	}
}
