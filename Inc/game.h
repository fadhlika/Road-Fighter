//
// Created by fadhlika on 12/7/2017.
//

#ifndef ROADFIGHTER_GAME_H
#define ROADFIGHTER_GAME_H

#define MAX_ENEMY 3
static short int VELOCITY = 2;
static int score = 0;

typedef enum _GameState {
	Main_Menu,
	Starting,
	Running,
	Game_Over
} GameState;

typedef enum _Track {
	Engine,
	Explosion,
	Explosion_started,
	Start
} Track;

typedef struct _Player {
	uint8_t w, h; //16, 32
	short int x, y, x_old;
	uint8_t col;
	float vel;
} Player;


typedef struct _Enemy {
	uint8_t w, h; //18, 30
	short int x, y, y_old;
	uint8_t col;
	float vel;
	uint32_t appear_time;
	uint8_t appear;
	uint8_t state;
} Enemy;

Player player;
Enemy enemy[MAX_ENEMY];

void main_menu(void);
void game_over(void);

void init_player(void);
void init_enemy(void);

void render_starting(void);
void render_player(void);
void render_road(void);
void render_enemy(void);
void render_score(void);

void update_game(void);
void update_player(float dir);
void check_if_over(void);

#endif //ROADFIGHTER_GAME_H
