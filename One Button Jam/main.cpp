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
#include "raylib.h"
#include "Classes.h"

/// If we compile for web later, we need this header
#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

void UpdateAndDrawFrame(void* arg);
int SeedGenerator();
void GenerateGround(unsigned int seed);




// main is only used for stuff to launch ONCE at start up - NO LOOPS
int main()
{
    Texture2D groundImages[16];

    for (int i = 0; i < 16; i++) // fills groundImages with tile_grass_0 to _15
    {
        groundImages[i] = LoadTexture(TextFormat("Assets/tile_grass_%d.png", i));
    }

    InitWindow(1280, 720, "Tap Wizard"); //720p, should fit 1080p monitors without fullscreen well (as well as old ones with full screen

#if defined(PLATFORM_WEB) // if on web => let browser set FPS and run (monitor native)
    emscripten_set_main_loop_arg(UpdateAndDrawFrameArg, groundImages, 0, 1);
#else // if not on web => run this
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        UpdateAndDrawFrame(groundImages);
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
void UpdateAndDrawFrame(void* arg)
{
    const unsigned int seed{ SeedGenerator() };



    GenerateGround(seed);
}



int SeedGenerator() // ------------------------------------------------------------------------------------------------------------------------
{
    return(GetRandomValue(1, 100000));
}



void GenerateGround(unsigned int seed) // ------------------------------------------------------------------------------------------------------------------------
{
    Vector2 oldPlayerLocation{};

    currentPlayerLocation saved at startup

    if currentPlayerLocation.X > oldPlayerLocation.X + 15
    {
        load next row of tiles on right
    }

    if currentPlayerLocation.X < oldPlayerLocation.X - 15
    {
        load next row of tiles on left
    }

    if currentPlayerLocation.Y > oldPlayerLocation.Y + 15
    {
        load next row of tiles on bottom
    }

    if currentPlayerLocation.Y < oldPlayerLocation.Y - 15
    {
        load next row of tiles on top
    }


    BeginDrawing();
    
    // PSEUDO RANDOM NUMBER GENERATOR
    unsigned int seed = seed; //each seed give always the same number <== seed = random + x * y
    std::mt19937_64 gen(seed);
    std::uniform_int_distribution<> distr(0, 15);
    int randomTextureIndex = distr(gen);

    for (int i = 0; i < 5; i++) 
    {
        std::cout << i << "\n";
    }

    ClearBackground(WHITE);
    DrawText("Rendering works", 190, 200, 20, BLACK);

    EndDrawing();
}