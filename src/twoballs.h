#pragma once

#define FALSE 0
#define TRUE 1
#define BALL_SIZE 38

struct point {
	float x;
	float y;
};

enum FloorPosition { posMidscreen = 0, posGoldenRatio = 1, posBottom = 2 };

struct appParams 
{
	int floorPos;
	double gravity;
	double bounceSlowdown ;
};

void game_cycle (struct point * a, struct point * b) ;
void setSizes (const int width, const int height, struct appParams * params) ;
void game_init (struct appParams * params);