#ifndef CLASSES_H
#define CLASSES_H

#include "raylib.h"
#include "Structs.h"

class Player 
{
public: //1280, 720

	static constexpr Vector2int screenPos{ 640, 360 }; // middle of screen, HAS TO BE ADJUSTED FOR HALF OF PLAYER SIZE
	//static constexpr Texture2D image{}; // ENTER IMAGE HERE ONCE READY

	Vector2int location{ 1'500'000'000,1'500'000'000 };
	// old location remoced, could be possibly added for walking effects such as trailing dust (however that could be generated while walking and rise slowly)

	int hp{ 100 };
	int level{ 0 };
	int xp{ 0 };
};

class Enemy {
public:

	const int damage{};
	const Texture2D image{};

	Vector2int relativePositionToPlayer{};
	int hp{};
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

#endif