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

#include "raylib.h"

/// If we compile for web later, we need this header
#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

void UpdateAndDrawFrame();
void UpdateScreen();

// main is only used for stuff to launch ONCE at start up - NO LOOPS
int main()
{
    InitWindow(1280, 720, "Tap Wizard"); //720p, should fit 1080p monitors without fullscreen well (as well as old ones with full screen

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
    CloseWindow();
    return 0;
}

// everything outside of main (gameplay loops) - this verion's main(){} ------------------------------------------------------------------
void UpdateAndDrawFrame()
{
    UpdateScreen();
}

void UpdateScreen()
{
    BeginDrawing();

    ClearBackground(WHITE);
    DrawText("test . t", 190, 200, 20, BLACK);

    EndDrawing();
}