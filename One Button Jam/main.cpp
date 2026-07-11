/* TAP WIZARD

* Premise:
* This game is made for the One Button Jam 2026 (by Clyde) on Itch.io. 
* As the name says: only one button can be used as input (mouse movement / etc is not allowed, aside in menus).

* Initial idea:
* "Tap Wizard" / "Tap Wizards" (not as fitting as no multiplayer): a game where you can cast different spells to attack
* mobs / enemies. To activate spells you need to do different combinations of tapping or holding your button (key).
* You can collect / learn new spells (either by randomly trying stuff / upgrading your level / picking or buying them
* as a reward for money/xp you got from eliminating mobs.
* There could be one central main goal such as "Save X" / "Get your revenge" / etc.

* Created by CrazyPengion, July of 2026

* Gemini was primarily used for web compartablity.
*/

#include <iostream> // DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG DEBUG 

#include <random>
#include "Classes.h" // also includes "Structs.h"
#include "Generator.h"

/// If we compile for web later, we need this header
#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

// Global Variables
constexpr int screenWidth{ 1280 };
constexpr int screenHeight{ 720 };

Texture2D groundImages[16];
Texture2D enemyImages[3];
uint32_t seed{ 0 };

Player player;
std::vector<Enemy> activeEnemies;

// Forward Declarations of functions by order of appearance in the following code
void UpdateAndDrawFrame();
unsigned int SeedGenerator();

//LOGIC
// void UpdatePlayer();
void UpdateEnemies();

//DRAWING
void GenerateGround();
// void DrawPlayer();
void DisplayEnemies();
// void DrawEffects();

//UTILITY
Vector2 worldPosToScreenPos(Vector2 worldPosition);


// DEBUG start
Enemy testenemy1;
Enemy testenemy2;
Enemy testenemy3;

// DEBUG end

int testVariable{ 0 };

// main is only used for stuff to launch ONCE at start up - NO (infinite) LOOPS
int main()
{
    InitWindow(screenWidth, screenHeight, "Tap Wizard"); //720p, should fit 1080p monitors without fullscreen well (as well as old ones with full screen

    for (int i = 0; i < 16; i++) // fills groundImages with tile_grass_0 to _15     // works only after InitWindow()
    {
        groundImages[i] = LoadTexture(TextFormat("assets/tile_grass_%d.png", i));
    }

    for (int i = 0; i < 3; i++) // fills enemyImages with enemy_0 to 2
    {
        enemyImages[i] = LoadTexture(TextFormat("assets/enemy_%d.png", i));
    }

    seed = GetRandomValue(0, INT_MAX);

    // DEBUG start

    testenemy1.location.x = 31950;
    testenemy1.location.y = 31950;
    testenemy2.location.x = 32100;
    testenemy2.location.y = 32100;
    testenemy3.location.x = 32100;
    testenemy3.location.y = 31900;

    testenemy1.image = 0;
    testenemy2.image = 1;
    testenemy3.image = 2;

    testenemy1.speed = 30;
    testenemy2.speed = 40;
    testenemy3.speed = 50;

    testenemy1.size = 16;
    testenemy2.size = 24;
    testenemy3.size = 32;

    activeEnemies.push_back(testenemy1);
    activeEnemies.push_back(testenemy2);
    activeEnemies.push_back(testenemy3);

    // DEBUG end


    /*
    	int image{}; // Not const as deleting algorithm (std::erase_if) complains if it is
	int damage{}; //^
	int speed{}; // 1 = normal speed      // COULD ADD FREEZE SPELL

	Vector2 location{};
	int hp{};*/
    



#if defined(PLATFORM_WEB) // if on web => let browser set FPS and run (monitor native)
    emscripten_set_main_loop(UpdateAndDrawFrame, 0, 1);
#else // if not on web => run this
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
            UpdateAndDrawFrame();
            
            if (KEY_W)
            {
                player.location.y -= 0.1f * GetFrameTime();
            }
            if (KEY_A)
            {
                player.location.x -= 0.1f * GetFrameTime();
            }
            if (KEY_S)
            {
                player.location.y += 0.1f * GetFrameTime();
            }
            if (KEY_D)
            {
                player.location.x += 0.1f * GetFrameTime();
            }
    }



#endif
    /// UnloadTexture(playerSprite); /// Clean up assets
    /// CloseAudioDevice();

    for (int i = 0; i < 16; i++) // unload ground images
    {
        UnloadTexture(groundImages[i]);
    }

    CloseWindow();
    return 0;
}








// MAIN | GENERAL
void UpdateAndDrawFrame()
{
    // Gameplay
    //get input
    //calculate stuff like spells
    UpdateEnemies();

    // Displaying
    BeginDrawing();

    GenerateGround();
    // DisplayPlayer
    DisplayEnemies();
    // DisplayEffects
    EndDrawing();

    // DEBUG start
    if (IsKeyDown(KEY_W))
    {
        player.location.y -= 16;
    }
    if (IsKeyDown(KEY_S))
    {
        player.location.y += 16;
    }
    if (IsKeyDown(KEY_A))
    {
        player.location.x -= 16;
    }
    if (IsKeyDown(KEY_D))
    {
        player.location.x += 16;
    }

    // DEBUG end
}

unsigned int SeedGenerator() // ------------------------------------------------------------------------------------------------------------------------
{
    return(static_cast<unsigned int>(GetRandomValue(1'000'000'000, 2'000'000'000)));
}


// MAIN LOGIC - first spells to potentially kill mobs before they kill player

// void UpdateSpells();

// void UpdatePlayer();

void UpdateEnemies()
{
    // check if enemy died, if yes delete
    std::erase_if(activeEnemies, [](const Enemy& x)
    {
        return x.hp <= 0;
    });

    // movement
    for (Enemy& enemy : activeEnemies)
    {
        Vector2 tempDirection{ (player.location.x - enemy.location.x), (player.location.y - enemy.location.y) }; // get direction from enemy towards player
        float tempDistance{ sqrtf(tempDirection.x * tempDirection.x + tempDirection.y * tempDirection.y) }; // get distance from enemy to player

        if (tempDistance >= 0.4) // if not inside player
        {
            Vector2 tempNormalisedDirection{ tempDirection.x / tempDistance, tempDirection.y / tempDistance }; // get normalised direction (at what ratio to go into x/y direction)

            // move enemy towards player
            enemy.location.x += tempNormalisedDirection.x * enemy.speed * GetFrameTime();
            enemy.location.y += tempNormalisedDirection.y * enemy.speed * GetFrameTime();
        }
        
        
        
    }

}


// DISPLAYING STUFF - first background - last foreground

void GenerateGround() // ------------------------------------------------------------------------------------------------------------------------
{
    // 1. GET THE CENTER TILE
    Vector2int center{};

    bool foundCenterTile{ false };
    center.x = static_cast<int>(player.location.x);
    center.y = static_cast<int>(player.location.y);

    while (!foundCenterTile)
    {
        if (center.x % 32 == 0 and center.y % 32 == 0)
        {
            foundCenterTile = true;
        }
        else
        {
            if (center.x % 32 != 0)
            {
                center.x++;
            }

            if (center.y % 32 != 0)
            {
                center.y++;
            }

        }
    }

    // 2. RENDER TILES AROUND IT

    int incrementorY{ 0 };
    int incrementorX{ 0 };

     // 1280, 720

    for (int i = center.y - 90 * 32; i < center.y + 90 * 32; i += 32) // for every y within screen + margin
    {
        incrementorY++;
        incrementorX = 0;

        for (int j = center.x - 90 * 32; j < center.x + 90 * 32; j += 32) // for every x within screen + margin (y+x = all ground spots)
        {
            // Pseudo Random Number Generator (when given the same seed and number it provides the same result)
            int randomTextureIndex = GetTileTextureIndex((j + i * 63246), seed);

            DrawTexture(groundImages[randomTextureIndex], (-player.location.x) + j , (-player.location.y) + i , WHITE); // draw the background around the player

            incrementorX++;
        }
    }
}

// void DisplayPlayer();

void DisplayEnemies() // 1280, 720
{
    for (int i = 0; i < activeEnemies.size(); i++) // for all enemies
    {
        // if enemy visible on screen
        Vector2 tempLocation{ worldPosToScreenPos(activeEnemies[i].location) }; // get screen position, 0;0 is middle

        if (tempLocation.x < (GetScreenWidth() / 2)                                // further left than right screen edge
            and (tempLocation.x + activeEnemies[i].size) > -(GetScreenWidth() / 2) // further right than left screen edge
            and tempLocation.y < (GetScreenHeight() / 2)                           // further up than bottom screen edge
            and tempLocation.y + activeEnemies[i].size > -(GetScreenHeight() / 2)) // further down than tom sceen edge
        {
            // draw enemy
            Vector2 tempPosition = worldPosToScreenPos(activeEnemies[i].location);
            tempPosition.x += (GetScreenWidth() / 2);
            tempPosition.y += (GetScreenHeight() / 2);
            DrawTexture(enemyImages[activeEnemies[i].image], tempPosition.x, tempPosition.y, WHITE);
            std::cout << "| " << tempPosition.x << " | " << tempPosition.y << " |\n";
        }
    }
    
}

// void DisplaySpells();

// get coordinates where something should be displayed on screen from the world position
// does NOT account for something being out of frame - always returns value
Vector2 worldPosToScreenPos(Vector2 worldPosition)
{
    Vector2 screenPosition;

    screenPosition.x = worldPosition.x - player.location.x;
    screenPosition.y = worldPosition.y - player.location.y;
    
    return screenPosition;
}