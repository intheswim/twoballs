/*
 * twoballs.c
 *
 *      Author: Yuriy Yakimenko. 
 * 		Ported into C from original code in JS (written circa 2004).
 */

#include <math.h>
#include <stdlib.h>
#include <limits.h>

#include "twoballs.h"

#define GOLDEN_RATIO 1.61803398875
#define DROP_HEIGHT	80

double math_random()
{
	return (double)rand() / RAND_MAX;
}

struct BallStruct
{
	float left, top;
	struct point pos;
	struct point speed;
	struct point pos_max;
	struct point last_pos;
};

static int FLOOR_LEVEL = 300;

struct BallStruct objBall1, objBall2;

#define xpos1 objBall1.pos.x
#define ypos1 objBall1.pos.y
#define xspeed1 objBall1.speed.x
#define yspeed1 objBall1.speed.y
#define xpos1_max objBall1.pos_max.x
#define ypos1_max objBall1.pos_max.y
#define last_xpos1 objBall1.last_pos.x
#define last_ypos1 objBall1.last_pos.y

#define xpos2 objBall2.pos.x
#define ypos2 objBall2.pos.y 
#define xspeed2 objBall2.speed.x
#define yspeed2 objBall2.speed.y
#define xpos2_max objBall2.pos_max.x 
#define ypos2_max objBall2.pos_max.y
#define last_xpos2 objBall2.last_pos.x
#define last_ypos2 objBall2.last_pos.y

static float xdiff, ydiff;
static int last_dist = 0, dist = 0;

static int winW = 0, winH = 0;

static double GetCoef (void)
{
   	float vx1 = (xpos1_max - last_xpos1);
   	float vx2 = (xpos2_max - last_xpos2);
   	float vy1 = (ypos1_max - last_ypos1);
   	float vy2 = (ypos2_max - last_ypos2);

   	double a = (vx1 - vx2) * (vx1 - vx2) + (vy1 - vy2) * (vy1 - vy2) ;
   	double b = 2 * (vx1 - vx2) * (last_xpos1 - last_xpos2) + 2 * (vy1 - vy2) * (last_ypos1 - last_ypos2);
   	double c = (last_xpos1 - last_xpos2) * (last_xpos1 - last_xpos2) + (last_ypos1 - last_ypos2) * (last_ypos1 - last_ypos2) - 
   				(BALL_SIZE + 1) * (BALL_SIZE + 1);

   	double rt = sqrt (b * b - 4 * a * c);

   	double k1 = (-b + rt) / ( 2 * a);
   	double k2 = (-b - rt) / ( 2 * a);

   	if (k2 > 0 && k2 < 1)
	{
        return k2;
	}

   	if (k1 > 0 && k1 < 1)
	{
        return k1; 
	}
  
   	return 1.0;
}

void setSizes(const int width, const int height) 
{
  	winW = width - BALL_SIZE - 1;
  	winH = height - 16;

  	FLOOR_LEVEL = (height / GOLDEN_RATIO) + (BALL_SIZE / 2);

  	if (FLOOR_LEVEL < DROP_HEIGHT + BALL_SIZE)
  	{
	  	FLOOR_LEVEL = height - 10 - BALL_SIZE;
  	}
}

static void next () 
{
   	xpos1 += xspeed1;
   	ypos1 -= yspeed1;

   	if (xpos1 > winW && xspeed1 > 0) xspeed1 = -xspeed1;
   	if (xpos1 < 0 && xspeed1 < 0) xspeed1 = -xspeed1;

   	yspeed1 -= 1.0; // gravitation

   	if (ypos1 > FLOOR_LEVEL && yspeed1 < 0)
	{
		// ball bounce from the floor
		yspeed1 += 1;
		yspeed1 = -0.9 * yspeed1;
        xspeed1 = xspeed1 * 0.99;

	   	if ( fabs(yspeed1) < 0.01 && fabs(xspeed1) < 0.2)
		{
			if (xspeed1 < 0)
			{
				xpos1 = 0;
			}
			else
			{
				xpos1 = winW;
			}
			ypos1 = DROP_HEIGHT;

			last_xpos1 = xpos1;
			last_ypos1 = ypos1;

			yspeed1 = 0;
	 		xspeed1 = floor(math_random() * 10) + 2;
			if (xpos1 != 0)
				xspeed1 = -xspeed1;
		}
	}


   	xpos2 += xspeed2;
   	ypos2 -= yspeed2;

   	if (xpos2 > winW && xspeed2 > 0) xspeed2 = -xspeed2;
   	if (xpos2 < 0 && xspeed2 < 0) xspeed2 = -xspeed2;

   	yspeed2 -= 1.0;  // gravitation

   	if (ypos2 > FLOOR_LEVEL && yspeed2 < 0)
	{
		// ball bounce from the floor
		yspeed2 += 1;
		yspeed2 = -0.9 * yspeed2;
        xspeed2 = xspeed2 * 0.99;

	   	if (fabs(yspeed2) < 0.001 && fabs(xspeed2) < 0.2 )
		{
			if (xspeed2 < 0)
			{
				xpos2 = 0;
			}
			else
			{
				xpos2 = winW;
			}
			ypos2 = DROP_HEIGHT;

			last_xpos2 = xpos2;
			last_ypos2 = ypos2;

			yspeed2 = 0;
	 		xspeed2 = floor(math_random() * 10) + 2;
			if (xpos2 != 0)
				xspeed2 = -xspeed2;
		}
	}

   	ypos1_max = ypos1;

   	if (ypos1_max > FLOOR_LEVEL) 
		ypos1_max = FLOOR_LEVEL;

   	ypos2_max = ypos2;
   	if (ypos2_max > FLOOR_LEVEL) 
		ypos2_max = FLOOR_LEVEL;

   	xpos1_max = xpos1;
   	if (xpos1_max > winW)
    	xpos1_max = winW;

   	xpos2_max = xpos2;

   	if (xpos2_max > winW)
    	xpos2_max = winW;  
	
	// this is actually squared distance
   	dist = (xpos1_max - xpos2_max) * (xpos1_max - xpos2_max) + (ypos1_max - ypos2_max) * (ypos1_max - ypos2_max) ;

   	if ((dist < last_dist && dist < (BALL_SIZE + 3) * (BALL_SIZE + 3))) // || (last_dist > 170 && dist > 170 && BallsCollapse()==1))
	{
		if (dist < (BALL_SIZE + 1) * (BALL_SIZE + 1))
		{
			double coef = GetCoef ();	

			ypos1_max = last_ypos1 + coef * (ypos1_max - last_ypos1);
			xpos1_max = last_xpos1 + coef * (xpos1_max - last_xpos1);
		
			ypos2_max = last_ypos2 + coef * (ypos2_max - last_ypos2);
			xpos2_max = last_xpos2 + coef * (xpos2_max - last_xpos2);
		}

    	xdiff = xspeed1 - xspeed2;
		xspeed1 -= xdiff;
		xspeed2 += xdiff;

		ydiff = yspeed1 - yspeed2;
		yspeed1 -= ydiff;
		yspeed2 += ydiff;
		dist = 0;
	}

    last_dist = dist;

    last_xpos1 = xpos1_max;
    last_ypos1 = ypos1_max;

    last_xpos2 = xpos2_max;
    last_ypos2 = ypos2_max;

	objBall1.left = xpos1_max;
	objBall1.top = ypos1_max;

	objBall2.left = xpos2_max;
	objBall2.top = ypos2_max;
}

//////////////////////////////////////////////////////////////////////////////

void game_init()
{
	objBall1.pos.x = 0;
	objBall1.pos.y = DROP_HEIGHT;
	objBall1.speed.x = floor(math_random() * 10 ) + 2;
	objBall1.speed.y = 0;

	objBall2.pos.x = 0;
	objBall2.pos.y = DROP_HEIGHT - (2 * BALL_SIZE + 6);
	objBall2.speed.x = objBall1.speed.x + 2;
	objBall2.speed.y = 0;

	last_dist = dist = INT_MAX;
}

void game_cycle (struct point * a, struct point * b) 
{
	next();

	a->x = objBall1.left;
	a->y = objBall1.top;

	b->x = objBall2.left;
	b->y = objBall2.top;
}
