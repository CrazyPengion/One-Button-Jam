#ifndef STRUCTS_H
#define STRUCTS_H

#include "raylib.h"

struct Vector2int
{
	int x;
	int y;
};

struct Click
{
	float timeSinceLastClick;
	float clickDuration;
	double timeAtClickStop;

	bool isHold;
};

#endif