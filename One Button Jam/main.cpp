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

#include <random>
#include "Classes.h" // also includes "Structs.h"
#include "Generator.h"

/// If we compile for web later, we need this header
#if defined(PLATFORM_WEB)
#include <emscripten.h>
#endif

// Global Variables
bool mainMenuOpen{ true };
void MainMenu();
void PauseMenu();
int textWidthTitle{ 0 };
int textWidthSmall{ 0 };
bool pauseMenuOpen{ false };
Texture2D wizardBook;
bool justClosedMenu{ false };
bool displayBeginningTip{ true };

constexpr int screenWidth{ 1280 }; //1280 x 720
constexpr int screenHeight{ 720 };

Texture2D groundImages[16];
Texture2D grassImages[16];
Texture2D detailImage[4];
Texture2D enemyImages[4];
Texture2D playerImages[1];
Texture2D spellImages[2];
uint32_t seed{ 0 };

Player player;
std::vector<Click> currentClickPattern;
std::vector<Enemy> activeEnemies;

Clock gameClock{};
double timeAtClickStart{ 0 };
bool isKeyDown{ false };
bool spellComplete{ false };


//SPELLS
Vector2 getProjectileDirection();

void Spell_Walking(int mode);
void Spell_Damage(int size, int damage);

// Circle aimer
float currentAngle{ 0 };
Vector2 circleCoordinates;
float radAngle{ 0 };

// Stomp
void Spell_Stomp();
int stompDuration{ 0 };

// Dash
int dashBoostDuration{ 0 };
constexpr int dashBoost{ 10 };

// Canon ball | FIREBALL
std::vector<FlyingBall> flyingBalls;
void UpdateBalls();

// Teleport
void teleportAnimation();
Vector2 tempNewLocation{ 0 };
int teleportFramesLeft{ 0 };
int oldHp{ 1000 };

int DEBUG_DEATHS{ 0 };

// SOUNDS

enum GameSoundID {
    SND_ENEMY_DEATH_BEE,
    SND_ENEMY_DEATH_SKELETON,
    SND_ENEMY_DEATH_SPIDER,
    SND_ENEMY_DEATH_SPIKY,
    SND_PLAYER_DAMAGE,
    SND_PLAYER_DEATH,
    SND_PLAYER_LEVELUP,
    SND_SPELL_CANNONBALL,
    SND_SPELL_DASH,
    SND_SPELL_FIREBALL,
    SND_SPELL_STOMP,
    SND_SPELL_TELEPORT,
    SND_COUNT
};

std::vector<Sound> gameSounds;

// Forward Declarations of functions by order of appearance in the following code
void LoadGameSounds();
void UpdateAndDrawFrame();

//LOGIC
void TimeKeeper();
void GetInput();
void CastSpells();
void SpawnEnemies();
Vector2 EnemySpawnCoordinates(float distance); // called by SpawnEnemies();
void UpdateEnemies();
void UpdatePlayer();
void Die(); // called by UpdatePlayer();

//DRAWING
void GenerateGround();
void DisplayPlayer();
void DisplayEnemies();
void DisplaySpells();
void DisplayUI();

//UTILITY
Vector2 worldPosToScreenPos(Vector2 worldPosition);



// main is only used for stuff to launch ONCE at start up - NO (infinite) LOOPS
int main()
{
    InitWindow(screenWidth, screenHeight, "Tap Wizard"); //720p, should fit 1080p monitors without fullscreen well (as well as old ones with full screen
    InitAudioDevice();
    LoadGameSounds();
    int tempTextWidth{ MeasureText("Tap Wizard", 120) };
    textWidthTitle = tempTextWidth;
    tempTextWidth = MeasureText("PRESS [SPACE] TO START", 30);
    textWidthSmall = tempTextWidth;

    for (int i = 0; i < 16; i++) // fills groundImages with tile_grass_0 to _15     // works only after InitWindow()
    {
        groundImages[i] = LoadTexture(TextFormat("assets/tile_grass_%d.png", i));
    }

    for (int i = 0; i < 16; i++) // fills grassImages with detail_grass_0 to 8
    {
        grassImages[i] = LoadTexture(TextFormat("assets/detail_grass_%d.png", i));
    }

    for (int i = 0; i < 4; i++) // fills grassImages with detail_grass_0 to 8
    {
        detailImage[i] = LoadTexture(TextFormat("assets/detail_plus_%d.png", i));
    }


    for (int i = 0; i < 4; i++) // fills enemyImages with enemy_0 to 2
    {
        enemyImages[i] = LoadTexture(TextFormat("assets/enemy_%d.png", i));
    }

    for (int i = 0; i < 1; i++) // fills playerImages with player_0 to 0
    {
        playerImages[i] = LoadTexture(TextFormat("assets/player_%d.png", i));
    }

    for (int i = 0; i < 2; i++) // fills spellImageswith spell_0 to 1
    {
        spellImages[i] = LoadTexture(TextFormat("assets/spell_%d.png", i));
    }

    Texture2D tempWizardBook = LoadTexture(TextFormat("assets/wizard_book.png"));
    wizardBook = tempWizardBook;

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
        UnloadTexture(grassImages[i]);
    }

    for (int i = 0; i < 4; i++) // unload grass images
    {
        UnloadTexture(detailImage[i]);
    }

    for (int i = 0; i < 4; i++) // unload enemy images
    {
        UnloadTexture(enemyImages[i]);
    }

    for (int i = 0; i < 1; i++) // unload player images
    {
        UnloadTexture(playerImages[i]);
    }

    UnloadTexture(wizardBook);

    for (Sound& sound : gameSounds)
    {
        UnloadSound(sound);
    }
    gameSounds.clear();

    CloseAudioDevice();
    CloseWindow();
    return 0;
}

void LoadGameSounds()
{
    gameSounds.resize(SND_COUNT);

    gameSounds[SND_ENEMY_DEATH_BEE] = LoadSound("sounds/enemy_death_bee.wav");
    gameSounds[SND_ENEMY_DEATH_SKELETON] = LoadSound("sounds/enemy_death_skeleton.wav");
    gameSounds[SND_ENEMY_DEATH_SPIDER] = LoadSound("sounds/enemy_death_spider.wav");
    gameSounds[SND_ENEMY_DEATH_SPIKY] = LoadSound("sounds/enemy_death_spiky.wav");
    gameSounds[SND_PLAYER_DAMAGE] = LoadSound("sounds/player_damage.wav");
    gameSounds[SND_PLAYER_DEATH] = LoadSound("sounds/player_death.wav");
    gameSounds[SND_PLAYER_LEVELUP] = LoadSound("sounds/player_levelup.wav");
    gameSounds[SND_SPELL_CANNONBALL] = LoadSound("sounds/spell_cannonball.wav");
    gameSounds[SND_SPELL_DASH] = LoadSound("sounds/spell_dash.wav");
    gameSounds[SND_SPELL_FIREBALL] = LoadSound("sounds/spell_fireball.wav");
    gameSounds[SND_SPELL_STOMP] = LoadSound("sounds/spell_stomp.wav");
    gameSounds[SND_SPELL_TELEPORT] = LoadSound("sounds/spell_teleport.wav");
}

// MAIN | GENERAL
void UpdateAndDrawFrame()
{
    if (mainMenuOpen == false and pauseMenuOpen == false)
    {
        TimeKeeper();

        // Gameplay
        GetInput();
        CastSpells();
        SpawnEnemies();
        UpdateBalls();
        UpdateEnemies();
        UpdatePlayer();

        // Displaying
        BeginDrawing();

        ClearBackground(BLACK);
        GenerateGround();
        DisplayPlayer();
        DisplayEnemies();
        DisplaySpells();
        DisplayUI();

        EndDrawing();
    }

    else if (pauseMenuOpen)
    {
        PauseMenu();
    }

    else if (mainMenuOpen)
    {
        MainMenu();
    }
}

void MainMenu()
{
    if (IsKeyReleased(KEY_SPACE))
    {
        PlaySound(gameSounds[SND_PLAYER_LEVELUP]);
        mainMenuOpen = false;
    }
    BeginDrawing();
    GenerateGround();
    DrawText("Tap Wizard", (screenWidth / 2) - (textWidthTitle / 2), screenHeight / 2 - 130, 120, RAYWHITE);
    DrawText("PRESS [SPACE] TO START", (screenWidth / 2) - (textWidthSmall / 2), screenHeight / 2 + 40, 30, RAYWHITE);
    EndDrawing();
    player.location.y += 20 * GetFrameTime();
}

void PauseMenu()
{
    BeginDrawing(); // 768 x 512 //1280 x 720
    DrawTexture(wizardBook, (1280 - 768) / 2, (720 - 512) / 2, WHITE);
    EndDrawing();

    displayBeginningTip = false;

    if (IsKeyPressed(KEY_SPACE))
    {
        justClosedMenu = true;
    }

    if (justClosedMenu == true and IsKeyReleased(KEY_SPACE))
    {
        pauseMenuOpen = false;
        justClosedMenu = false;
        currentClickPattern.clear();
    }
}

void TimeKeeper()
{
    gameClock.timeSinceFrame += GetFrameTime();
    if (gameClock.timeSinceFrame >= 1)
    {
        gameClock.secondsSinceStart += 1;
        gameClock.timeSinceFrame--;
        gameClock.didEnemiesSpawnThisSecond = false;
    }
}
// MAIN LOGIC - first spells to potentially kill mobs before they kill player

void GetInput()
{
    // space press start
    if (IsKeyDown(KEY_SPACE) and !isKeyDown)
    {
        timeAtClickStart = GetTime();
        isKeyDown = true;
    }

    // space let go
    if (IsKeyUp(KEY_SPACE) and isKeyDown == true)
    {
        double tempTime = GetTime(); // ensure quick reaction and that everything uses same time
        isKeyDown = false; // ensure it gets registered a single time
        Click tempClick;

        tempClick.clickDuration = tempTime - timeAtClickStart;
        tempClick.timeAtClickStop = tempTime;

        if (!currentClickPattern.empty())
        {
            Click tempLastClick{ currentClickPattern.back() };

            tempClick.timeSinceLastClick = tempTime - tempLastClick.timeAtClickStop;
        }
        else
            tempClick.timeSinceLastClick = -1;
        currentClickPattern.push_back(tempClick);
    }

    // space let go after spell
    if (spellComplete == true and IsKeyUp(KEY_SPACE) and !currentClickPattern.empty())
    {
        currentClickPattern.clear();
        spellComplete = false;
    }

    if (!currentClickPattern.empty())
    {
        if (IsKeyUp(KEY_SPACE) and IsKeyUp(KEY_SPACE) and GetTime() - currentClickPattern.back().timeAtClickStop >= 1)
        {
            currentClickPattern.clear();
        }
    }

}

void CastSpells()
{

    if (isKeyDown)
    {
        if (GetTime() - timeAtClickStart > 0.25) // if holding SPACE (longerthan 0.25s)
        {
            if (currentClickPattern.empty()) //wizard book
            {
                player.isWalking = false;

                if (GetTime() - timeAtClickStart > 0.5)
                {
                    pauseMenuOpen = true;
                    currentClickPattern.clear();
                }
            }

            else if (currentClickPattern.size() == 1) // walking
            {
                Spell_Walking(0);
                spellComplete = true;
            }

            else if (currentClickPattern.size() == 2) // stomp
            {
                Spell_Stomp();
            }

            else if (currentClickPattern.size() == 3 and player.level >= 2 and spellComplete == false) // dash
            {
                Spell_Walking(1);
            }

            else if (currentClickPattern.size() == 5 and player.level >= 3 and spellComplete == false) // canonball 3
            {
                PlaySound(gameSounds[SND_SPELL_CANNONBALL]);
                Spell_Walking(2);
            }

            else if (currentClickPattern.size() == 10 and player.level >= 4 and spellComplete == false) // fireball 4
            {
                PlaySound(gameSounds[SND_SPELL_FIREBALL]);
                Spell_Walking(3);
            }

            else if (currentClickPattern.size() == 15 and player.level >= 5 and spellComplete == false) // teleport 5
            {
                PlaySound(gameSounds[SND_SPELL_TELEPORT]);
                tempNewLocation = EnemySpawnCoordinates(800.0f);
                currentClickPattern.clear();
                teleportFramesLeft = 60;
                oldHp = player.hp;
            }

            else if (currentClickPattern.size() > 15)
            {
                currentClickPattern.clear();
            }
        }
    }
}

// COMPLETE
void Spell_Walking(int mode) //mode 0 = autowak ; mode 1 = dash
{
    // get relative position of circle and player
    circleCoordinates.x = cosf(radAngle) * 50.0f + player.location.x;
    circleCoordinates.y = sinf(radAngle) * 50.0f + player.location.y;

    // get triangle leg lenght from player to circle
    Vector2 tempDirection;
    tempDirection.x = circleCoordinates.x - player.location.x;
    tempDirection.y = circleCoordinates.y - player.location.y;

    // get straight line distance from player to circle (hypotenuse)
    float distance = sqrtf(tempDirection.x * tempDirection.x + tempDirection.y * tempDirection.y);

    // normalize the vector
    if (distance != 0.0f)
    {
        tempDirection.x /= distance;
        tempDirection.y /= distance;
    }

    if (mode == 0) // walk
    {
        player.walkDirection = tempDirection;
        player.isWalking = true;

        player.velocity.x = player.walkDirection.x * player.speed;
        player.velocity.y = player.walkDirection.y * player.speed;
    }

    if (mode == 1) // dash
    {
        PlaySound(gameSounds[SND_SPELL_DASH]);
        spellComplete = true;
        dashBoostDuration = 5;
        player.walkDirection = tempDirection;
        player.velocity.x = player.walkDirection.x * player.speed;
        player.velocity.y = player.walkDirection.y * player.speed;
    }

    if (mode == 2) // canon ball
    {
        FlyingBall tempBall;
        tempBall.image = 0;
        tempBall.damage = 45;
        tempBall.range = 32;
        tempBall.location = player.location;
        tempBall.speed = 300;
        tempBall.maxDistance = 1000;
        tempBall.direction = tempDirection; // Assign here

        flyingBalls.push_back(tempBall);
        spellComplete = true;
    }

    if (mode == 3) // FIREBALL
    {
        FlyingBall tempBall;
        tempBall.image = 1;
        tempBall.damage = 90;
        tempBall.range = 64;
        tempBall.location = player.location;
        tempBall.speed = 500; // Don't forget to set speed for the fireball too!
        tempBall.maxDistance = 500; // Set a maximum travel distance
        tempBall.direction = tempDirection; // Assign here

        flyingBalls.push_back(tempBall);
        spellComplete = true;
    }
}

void Spell_Damage(int size, int damage)
{
    Vector4 damageArea{ player.location.x - size, player.location.x + size, player.location.y - size, player.location.y + size };

    for (Enemy& enemy : activeEnemies) // & ==> Modify original value and not a copy!
    {
        if (enemy.location.x > damageArea.x and enemy.location.x < damageArea.y and enemy.location.y > damageArea.z and enemy.location.y < damageArea.w)
        {
            enemy.hp -= damage;
        }
    }
}

void Spell_Stomp()
{
    if (spellComplete == false)
    {
        PlaySound(gameSounds[SND_SPELL_STOMP]);
        spellComplete = true;
        stompDuration = 30;

        Spell_Damage(128, 15);
    }

}

// COMPLETE
void SpawnEnemies()

{
    // spawn enemies every 3 seconds
    if (gameClock.didEnemiesSpawnThisSecond == false and gameClock.secondsSinceStart % 3 == 0)
    {
        int maxEnemySpawnAmount{ gameClock.secondsSinceStart / 2 }; // max enemies spawnes is tied to playtime
        int howManyEnemiesShouldSpawn{ GetRandomValue(0,maxEnemySpawnAmount) }; // get amount of enemies spawned
        if (howManyEnemiesShouldSpawn > 0)
        {
            for (int i{ 0 }; i < howManyEnemiesShouldSpawn; i++)
            {
                int temp{ GetRandomValue(0, howManyEnemiesShouldSpawn) };

                // giant enemy
                if (temp > 120)
                {
                    activeEnemies.push_back(Enemy(3, 32, 5, 45, 5, EnemySpawnCoordinates(745.0f), 150));
                }

                // large enemy
                else if (temp < 121 and temp > 60)
                { //(int image, int size, int damage, int speed, int xpDropAmount, Vector2 location, int hp)
                    activeEnemies.push_back(Enemy(2, 40, 3, 40, 3, EnemySpawnCoordinates(745.0f), 90));
                }

                // medium enemy
                else if (temp < 61 and temp > 30)
                { //(int image, int size, int damage, int speed, int xpDropAmount, Vector2 location, int hp)
                    activeEnemies.push_back(Enemy(1, 32, 2, 50, 2, EnemySpawnCoordinates(745.0f), 60));
                }

                // small enemy
                else //(temp < 31)
                { //(int image, int size, int damage, int speed, int xpDropAmount, Vector2 location, int hp)
                    activeEnemies.push_back(Enemy(0, 24, 1, 60, 1, EnemySpawnCoordinates(745.0f), 30));
                }

                gameClock.didEnemiesSpawnThisSecond = true;
            }
        }
        else
            gameClock.didEnemiesSpawnThisSecond = true;
    }

}

// COMPLETE 
Vector2 EnemySpawnCoordinates(float distance)
{
    // only called right above it to get the coordinates of where to spawn enemies (slightly out of the window on all 4 sides)
    float randomAngle = (float)GetRandomValue(0, 360) * DEG2RAD; // get a specific direction

    Vector2 returnCoordinates;
    returnCoordinates.x = cos(randomAngle) * distance + player.location.x;
    returnCoordinates.y = sin(randomAngle) * distance + player.location.y;

    return returnCoordinates;
}

void UpdateBalls()
{
    if (flyingBalls.size() != 0)
    {
        // update ball location 
        for (FlyingBall& ball : flyingBalls)
        {
            float distanceThisFrame = ball.speed * GetFrameTime();

            // move ball, if it's close to max flight distance, only fly to there
            if (distanceThisFrame < ball.maxDistance)
            {
                ball.location.x += ball.direction.x * distanceThisFrame;
                ball.location.y += ball.direction.y * distanceThisFrame;
            }
            else
            {
                ball.location.x += ball.direction.x * ball.maxDistance;
                ball.location.y += ball.direction.y * ball.maxDistance;
            }
            ball.maxDistance -= distanceThisFrame;
        }

        // remove balls at their max distance
        std::erase_if(flyingBalls, [](const FlyingBall& x)
            {
                return x.maxDistance <= 0;
            });

        // damage enemies
        for (FlyingBall& ball : flyingBalls)
        {
            Vector4 damageArea(ball.location.x - ball.range, ball.location.x + ball.range, ball.location.y - ball.range, ball.location.y + ball.range);

            for (Enemy& enemy : activeEnemies) // & ==> Modify original value and not a copy!
            {
                if (enemy.location.x > damageArea.x and enemy.location.x < damageArea.y and enemy.location.y > damageArea.z and enemy.location.y < damageArea.w)
                {
                    enemy.hp -= ball.damage;
                }
            }
        }
    }
}

// COMPLETE
void UpdateEnemies()
{
    // check if enemy is dead, if yes award xp to player
    for (const Enemy& enemy : activeEnemies)
    {
        if (enemy.hp <= 0)
        {
            player.xp += enemy.xpDropAmount;
            if (enemy.image == 0)
                PlaySound(gameSounds[SND_ENEMY_DEATH_BEE]);
            if (enemy.image == 1)
                PlaySound(gameSounds[SND_ENEMY_DEATH_SPIKY]);
            if (enemy.image == 2)
                PlaySound(gameSounds[SND_ENEMY_DEATH_SPIDER]);
            if (enemy.image == 3)
                PlaySound(gameSounds[SND_ENEMY_DEATH_SKELETON]);
        }
    }

    // check if enemy died, if yes delete
    std::erase_if(activeEnemies, [](const Enemy& x)
        {
            return x.hp <= 0;
        });

    // movement
    for (Enemy& enemy : activeEnemies)
    {
        Vector2 tempDirection{ (player.location.x + 8 - enemy.location.x), (player.location.y + 8 - enemy.location.y) }; // get direction from enemy towards player
        enemy.distanceToPlayer = (sqrtf(tempDirection.x * tempDirection.x + tempDirection.y * tempDirection.y)); // get distance from enemy to player

        if (enemy.distanceToPlayer > 1000) // tp enemy around player if player is over 1000 pixels away (prevents running away)
        {
            enemy.location = EnemySpawnCoordinates(1000.0f);
        }

        if (enemy.distanceToPlayer >= 0.4) // if not inside player
        {
            Vector2 tempNormalisedDirection{ tempDirection.x / enemy.distanceToPlayer, tempDirection.y / enemy.distanceToPlayer }; // get normalised direction (at what ratio to go into x/y direction)

            // move enemy towards player
            enemy.location.x += tempNormalisedDirection.x * enemy.speed * GetFrameTime();
            enemy.location.y += tempNormalisedDirection.y * enemy.speed * GetFrameTime();
        }
    }
}

void UpdatePlayer()
{
    // teleport animation
    if (teleportFramesLeft > 0)
    {
        player.hp = 99999;
        player.location = EnemySpawnCoordinates(1000.0f);
        teleportFramesLeft--;
    }
    if (teleportFramesLeft == 0 and tempNewLocation.x != -99999)
    {
        player.location = tempNewLocation;
        player.hp = oldHp;
        tempNewLocation.x = -99999;
    }

    // dash
    if (dashBoostDuration > 0)
    {
        player.location.x += player.velocity.x * GetFrameTime() * dashBoost;
        player.location.y += player.velocity.y * GetFrameTime() * dashBoost;
        dashBoostDuration--;

        Spell_Damage(32, 30);
    }

    // walking
    if (player.isWalking == true and dashBoostDuration == 0)
    {
        player.location.x += player.velocity.x * GetFrameTime();
        player.location.y += player.velocity.y * GetFrameTime();
    }

    // if mobs nearby - get damage
    for (const Enemy& enemy : activeEnemies)
    {
        if (enemy.distanceToPlayer < 32)
        {
            player.hp -= enemy.damage * GetFrameTime();
            PlaySound(gameSounds[SND_PLAYER_DAMAGE]);
        }

        if (player.hp <= 0)
        {
            Die();
        }
    }

    // LEVELING

    // if enough xp for next level
    if (player.xp >= player.xpToNextLevel)
    {
        PlaySound(gameSounds[SND_PLAYER_LEVELUP]);
        player.level++;
        player.hp += 1000; // get 1000, NOT fill to 1000 => rewards players for not loosing hp
        player.xp -= player.xpToNextLevel;

        // get xp to next level => 10 ; 30 ; 50 ; 100
        if (player.level == 1)
            player.xpToNextLevel = 30;
        else if (player.level == 2)
            player.xpToNextLevel = 50;
        else if (player.level == 3)
            player.xpToNextLevel = 100;
    }
}

// complete
void Die()
{
    PlaySound(gameSounds[SND_PLAYER_DEATH]);
    // reset player / world
    player = Player();
    seed = GetRandomValue(0, INT_MAX);
    DEBUG_DEATHS++;

    // reset enemies
    activeEnemies.clear();
    gameClock.timeSinceFrame = 0;
    gameClock.secondsSinceStart = 0;
}


// DISPLAYING STUFF - first background - last foreground

  // complete
void GenerateGround() // ------------------------------------------------------------------------------------------------------------------------
{
    // 1. Get the center tile
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

    // 2. Draw tiles around center tile

    int incrementorY{ 0 };
    int incrementorX{ 0 };

    for (int i = center.y - 2 * 32; i < center.y + 22 * 32; i += 32) // for every y within screen + margin   //22 * 32;
    {
        incrementorY++;
        incrementorX = 0;

        for (int j = center.x - 2 * 32; j < center.x + 39 * 32; j += 32) // for every x within screen + margin (y+x = all ground spots) //39 * 32;
        {
            // Pseudo Random Number Generator (when given the same seed and number it provides the same result)
            int randomTextureIndex = GetTileTextureIndex((j + i * 63246), seed, 15); // 15 = total texture amount
            int grassCheck = GetTileTextureIndex((j + i * 63246), seed, 1); // for grass - 1 in 2
            int detailCheck = GetTileTextureIndex((j + i * 63246), seed, 255); // for log - 1 in 256

            DrawTexture(groundImages[randomTextureIndex], (-player.location.x) + j, (-player.location.y) + i, WHITE); // draw the background around the player

            if (grassCheck == 1)
            {
                DrawTexture(grassImages[randomTextureIndex], (-player.location.x) + j, (-player.location.y) + i, WHITE);
            }

            if (detailCheck == 0)
            {
                DrawTexture(detailImage[0], (-player.location.x) + j, (-player.location.y) + i, WHITE);
            }
            else if (detailCheck == 1)
            {
                DrawTexture(detailImage[1], (-player.location.x) + j, (-player.location.y) + i, WHITE);
            }
            else if (detailCheck == 2)
            {
                DrawTexture(detailImage[2], (-player.location.x) + j, (-player.location.y) + i, WHITE);
            }
            else if (detailCheck == 3)
            {
                DrawTexture(detailImage[3], (-player.location.x) + j, (-player.location.y) + i, WHITE);
            }

            incrementorX++;
        }
    }
}

void DisplayPlayer()
{
    // static png:
    DrawTexture(playerImages[0], (screenWidth / 2 - 16), (screenHeight / 2 - 32), WHITE);
}

// complete
void DisplayEnemies()
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
            tempPosition.x += (GetScreenWidth() / 2 - 16); //-16 so that they go towards player middle
            tempPosition.y += (GetScreenHeight() / 2 - 16);
            DrawTexture(enemyImages[activeEnemies[i].image], tempPosition.x, tempPosition.y, WHITE);
        }
    }

}

void DisplaySpells()
{
    // circle floating around player
    radAngle = currentAngle * DEG2RAD; // get a specific direction

    // get circle position around player
    circleCoordinates.x = cos(radAngle) * 50.0f + player.location.x + screenWidth / 2;
    circleCoordinates.y = sin(radAngle) * 50.0f + player.location.y + screenHeight / 2;

    circleCoordinates = worldPosToScreenPos(circleCoordinates);

    DrawCircle(circleCoordinates.x, circleCoordinates.y, 5, RAYWHITE);

    currentAngle += 120 * GetFrameTime();

    // stomp
    if (stompDuration > 0)
    {
        int opacity = stompDuration * 8;
        DrawRectangle(screenWidth / 2 - 128, screenHeight / 2 - 128, 256, 256, Color{ 230, 41, 55, static_cast<unsigned char>(opacity) });
        stompDuration--;
    }

    // balls
    if (flyingBalls.size() != 0)
    {
        // update ball location 
        for (FlyingBall& ball : flyingBalls)
        {
            Vector2 tempScreenPos = worldPosToScreenPos(ball.location);
            tempScreenPos.x += screenWidth / 2;
            tempScreenPos.y += screenHeight / 2;

            DrawTextureV(spellImages[ball.image], tempScreenPos, WHITE);
        }
    }

}

void DisplayUI()
{
    int displayHp{ 0 };
    int fps{ GetFPS() };
    if (teleportFramesLeft == 0)
    {
        displayHp = player.hp;
    }
    if (teleportFramesLeft > 0)
    {
        displayHp = oldHp;
    }
    DrawText(TextFormat("Health: %d\nDeaths: %d\nFPS: %d", displayHp, DEBUG_DEATHS, fps), 30, 30, 20, WHITE);
    DrawText(TextFormat("XP: %d\nLevel: %d", player.xp, player.level), 180, 30, 20, WHITE);

    if (displayBeginningTip == true)
    {
        DrawText(TextFormat("Hold [SPACE] until you open your book"), 350, 500, 30, WHITE);
    }
}

// get coordinates where something should be displayed on screen from the world position
// does NOT account for something being out of frame - always returns value
Vector2 worldPosToScreenPos(Vector2 worldPosition)
{
    Vector2 screenPosition;

    screenPosition.x = worldPosition.x - player.location.x;
    screenPosition.y = worldPosition.y - player.location.y;

    return screenPosition;
}

//INFO:
// "DEBUG" = delete before release, made for testing stuff
// "TODO" = stuff that needs to be done / change / completed