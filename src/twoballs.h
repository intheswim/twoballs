#pragma once

#define FALSE 0
#define TRUE 1
#define BALL_SIZE 38

struct point {
	float x;
	float y;
};

void game_cycle (struct point * a, struct point * b) ;
void setSizes (const int width, const int height) ;
void game_init ();