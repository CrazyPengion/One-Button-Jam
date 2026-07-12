#ifndef CLASSES_H
#define CLASSES_H

#include "raylib.h"
#include "Structs.h"

class Player 
{
public:

	Vector2int screenPos{ (GetScreenWidth() / 2), (GetScreenHeight() / 2)}; // middle of screen, HAS TO BE ADJUSTED FOR HALF OF PLAYER SIZE
	//static constexpr Texture2D image{}; // ENTER IMAGE HERE ONCE READY

	Vector2 location{ 32'000,32'000 };
	// old location remoced, could be possibly added for walking effects such as trailing dust (however that could be generated while walking and rise slowly)

	int hp{ 1000 };
	int level{ 1 };
	int xpToNextLevel{ 10 };
	int xp{ 0 };
};

class Enemy {
public:

	int image{}; // Not const as deleting algorithm (std::erase_if) complains if it is
	int size{}; // used to only render images within screen
	int damage{1}; //^
	int speed{10}; // 30 = slow speed      // COULD ADD FREEZE SPELL
	int xpDropAmount{ 1 };

	Vector2 location{};
	float distanceToPlayer{100};
	int hp{100};

	// constuctor - for writing Enemy(image, size, ...) inside of code
	Enemy(int cimage, int csize, int cdamage, int cspeed, int cxpDropAmount, Vector2 clocation, int chp)
	{
		image = cimage;
		size = csize;
		damage = cdamage;
		speed = cspeed;
		xpDropAmount = cxpDropAmount;
		location = clocation;
		hp = chp;
	}
};

class Spell
{
public:

	const Texture2D icon{};
	const char* animationFilePath{};
	int frameRate{ 10 }; // NOT SURE ABOUT FPS, COULD BE DIFFERENT

	void PlayAnimation()
	{
		LoadImageAnim(animationFilePath, &frameRate);
	}
};

class Clock
{
public:
	float timeSinceFrame{ 0.0 };
	int secondsSinceStart{ 0 };

	bool didEnemiesSpawnThisSecond{ false };
};

#endif