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
#include "Classes.h"

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
    if (testVariable == 0)
    {
        GenerateGround();
        testVariable++;
    }
    
}



unsigned int SeedGenerator() // ------------------------------------------------------------------------------------------------------------------------
{
    return(static_cast<unsigned int>(GetRandomValue(1'000'000'000, 2'000'000'000)));
}



void GenerateGround() // ------------------------------------------------------------------------------------------------------------------------
{
    int incrementorY{ 0 };
    int incrementorX{ 0 };

    BeginDrawing();
    for (int i = player.location.y - 24; i < player.location.y + 24; i++) // for every y within screen + margin
    {
        incrementorY++;
        incrementorX = 0;

        for (int j = player.location.x - 70; j < player.location.x + 70; j++) // for every x within screen + margin (y+x = all ground spots)
        {
            // PSEUDO RANDOM NUMBER GENERATOR
            std::mt19937 gen();
            std::uniform_int_distribution<> distr(0, 15);
            int randomTextureIndex = distr(gen);
            DrawTexture(groundImages[randomTextureIndex], (-player.location.x) + j + incrementorX * 16, (-player.location.y) + i + 0 + incrementorY * 16 - 18, WHITE);
            std::cout << (-player.location.x) + j + 69 + incrementorX * 16 << '\n';
            incrementorX++;
        }
    }
    EndDrawing();

    /*
    STUFF TO DO/FIX:
    2. the seed is currently never changed, it needs to be either influenced by x + y / etc or by each 30x30 plot having its own id 
    3. the images dont get displayed when using DrawTexture(groundImages[3], 100, 100, WHITE);
    4. printing j always gives 0
       
    5. it seems to be slow, this would need either a different system or multithreading (~4 y lines per thread?)
 

    for (int i = 0; i < 5; i++) //for amount of tiles needed
    {
        std::cout << i << "\n";
    }


    BeginDrawing();

    ClearBackground(WHITE);
    DrawText("Rendering works", 190, 200, 20, BLACK);

    EndDrawing();
    */
}

/*
WHAT IS NEEDED TO GENERATE PLOTS

0) Figure out which plot needs a texture
1) Enter X/Y into generator
2) Get result of which plot it needs
3) Draw it


0) WHICH PLOTS?
INITIAL GENERATION:
Geg.: player.posx ; player.posy ; screen size ; what isn't generated (everything)
Ges.: generate initial screen full

AFTER INITIAL GENERATION:
Geg.: player.posx ; player.posy ; screen size ; what side needs to be generate
Ges.: generate in that direction



INITIAL:
Take 0,0 as first generation
Screen size: 1280, 720
Block size: 30, 30
Block distance: 15, 15

720 / 15 = 48         >=> 60  ==> 30 in each direction / 50 - 25
1280 / 15 = 105.333...>=> 120 ==> 60 in each direction / 108 - 54

for i in 30 (start -30, end 30, 15u steps) { get texture, display }



NO STORAGE OPTION:

Always generate + 50 / + 108

for i in y
for j in x
a = seed(j, i)
render(a, j, i)

y starts at: player y - 25
x starts at: player x - 54


CODE:
posx - for (int i = posx - 54*15; i < posx + 54*15; i+=15)
posy
getPlot(posx,posy)
{
    id = SEED GENERATOR(posx, posy)
    return id
}

render(id, posx, posy)

32 PIXELS PER IMAGE
y: 720/16  >==> 45   => 46 => 48 - 24 for (int i = posx - 24*16; i < posx + 24*16; i+=16)
x: 1080/16 >==> 67.5 => 68 => 70 - 35 for (int j = posx - 70*16; i < posx + 70*16; i+=16)







POSSIBLE SYMMETRICITY FIX:
each pixel / 16 pixels get their own "id" - put that into generator 
*/