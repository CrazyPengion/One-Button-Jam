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

Player player;
Texture2D groundImages[16];
uint64_t seed{ 0 };

void UpdateAndDrawFrame();
unsigned int SeedGenerator();
void GenerateGround();

int testVariable{ 0 };

// main is only used for stuff to launch ONCE at start up - NO (infinite) LOOPS
int main()
{
    InitWindow(1280, 720, "Tap Wizard"); //720p, should fit 1080p monitors without fullscreen well (as well as old ones with full screen

    for (int i = 0; i < 16; i++) // fills groundImages with tile_grass_0 to _15     // works only after InitWindow()
    {
        groundImages[i] = LoadTexture(TextFormat("assets/tile_grass_%d.png", i));
    }

    seed = GetRandomValue(0, INT_MAX);

    



#if defined(PLATFORM_WEB) // if on web => let browser set FPS and run (monitor native)
    emscripten_set_main_loop(UpdateAndDrawFrame, 0, 1);
#else // if not on web => run this
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
            UpdateAndDrawFrame();
            
            if (KEY_W)
            {
                player.location.y -= 1;
            }
            if (KEY_A)
            {
                player.location.x -= 1;
            }
            if (KEY_S)
            {
                player.location.y += 1;
            }
            if (KEY_D)
            {
                player.location.x += 1;
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








// everything outside of main (gameplay loops) - this verion's main(){} ------------------------------------------------------------------
void UpdateAndDrawFrame()
{
    GenerateGround();

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



void GenerateGround() // ------------------------------------------------------------------------------------------------------------------------
{
    // 1. GET THE CENTER TILE
    Vector2int center{};

    bool foundCenterTile{ false };
    center = player.location;

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

    BeginDrawing(); // 1280, 720

    for (int i = center.y - 24 * 32; i < center.y + 24 * 32; i += 32) // for every y within screen + margin
    {
        incrementorY++;
        incrementorX = 0;

        for (int j = center.x - 42 * 32; j < center.x + 42 * 32; j += 32) // for every x within screen + margin (y+x = all ground spots)
        {
            // Pseudo Random Number Generator
            int randomTextureIndex = GetTileTextureIndex((j + i * 63246), seed);

            DrawTexture(groundImages[randomTextureIndex], (-center.x) + j , (-center.y) + i , WHITE);

            incrementorX++;
        }
    }

    EndDrawing();
}