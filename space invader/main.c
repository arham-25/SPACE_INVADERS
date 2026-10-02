#include "raylib.h"
#include "raymath.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define SCREEN_WIDTH 1500                  // Window width
#define SCREEN_HEIGHT 800                  // Window height
#define PLAYER_HEIGHT 60                   // Player height
#define PLAYER_WIDTH 60                    // Player width
#define PLAYER_MAX_SPEED 300               // Player speed
#define ENEMY_ROWS 4                       // Starting enemy rows
#define ENEMY_COLS 10                      // Enemies per row
#define MAX_ENEMY_ROWS 8                   // Maximum enemy rows
#define ENEMY_WIDTH 40                     // Enemy width
#define ENEMY_HEIGHT 40                    // Enemy height
#define ENEMY_SPEED 100                    // Enemy speed
#define ENEMY_MAX_SPEED 400                // Maximum enemy speed
#define ENEMY_MOVINGDOWN 15                // Enemy drop distance
#define ENEMY_STEP_TIME 0.25f              // Enemy movement interval
#define FORMATION_START_X 100              // Formation start X
#define FORMATION_START_Y 100              // Formation start Y
#define ENEMY_SPACING_X 130                // Horizontal spacing
#define ENEMY_SPACING_Y 70                 // Vertical spacing
#define MAX_BULLETS 50                     // Player bullet limit
#define MAX_BULLET_SPEED 500               // Player bullet speed
#define BULLET_HEIGHT 10                   // Bullet height
#define BULLET_WIDTH 5                     // Bullet width
#define MAX_ENEMY_BULLETS 20               // Enemy bullet limit
#define WALL_COUNT 3                       // Maximum number of protective walls.
#define WALL_HEALTH 4                      // Enemy bullets each wall can absorb.
#define WALL_WIDTH 80                      // Width of each protective wall.
#define WALL_HEIGHT 50                     // Height of each protective wall.
#define ENEMY_BULLET_SPEED 300             // Enemy bullet speed
#define MAX_SCORES 10                      // Leaderboard size
#define MAX_NAME_LENGTH 15                 // Maximum player-name length
#define DEFAULT_PLAYER_NAME "PLAYER"       // Default player name
#define LEADERBOARD_FILE "leaderboard.txt" // Score file
typedef struct
{ // Score data structure
    char name[30];
    int score;
} ScoreEntry;
typedef enum
{ // Game screen states
    MAIN_MENU,
    NAME_INPUT,
    MODE_SELECTION,
    LEADERBOARD,
    SETTINGS,
    ABOUT,
    HOW_TO_PLAY,
    CREDITS,
    DEVELOPER,
    GAMEPLAY,
    GAME_OVER
} GameScreen;
typedef enum
{ // Difficulty levels
    EASY,
    MEDIUM,
    HARD
} Difficulty;
void LoadLeaderboard(void);
void SaveLeaderboard(void);
void AddScore(int newScore);
void InitializeEnemies(void);
void SpawnEnemyRow(int row);
bool CanSpawnEnemyRow(int row);
int GetActiveEnemyCount(void);
void ClearCharQueue(void);
void InitializeWalls(void); // Set wall count, positions, and health.
void ResetGame(void);
void ClearCharQueue(void);
void ResetGame(void);
void DrawBackground(Texture2D texture);
void DrawSprite(Texture2D texture, Rectangle destination, Color fallbackColor);
void DrawSpriteRotated(Texture2D texture, Rectangle destination, Color fallbackColor, float rotation);
void DrawWallSprite(Texture2D texture, Rectangle destination); // Draw the wall sprite inside its collision rectangle.
void DrawMainMenu(void);
void DrawNameInput(void);
void DrawModeSelection(void);
void DrawLeaderboard(void);
void DrawSettings(void);
void DrawAbout(void);
void DrawHowToPlay(void);
void DrawCredits(void);
void DrawDeveloper(void);
void DrawGameplay(void);
void DrawGameOver(void);
void ChangeMusic(Music newMusic);
void ApplyAudioSettings(void);
GameScreen currentScreen = MAIN_MENU;
Difficulty difficulty = EASY;
Texture2D playerTexture;
Texture2D playerTexture1;
Texture2D playerTexture2;
Texture2D enemyTexture;
Texture2D enemyTexture1;
Texture2D enemyTexture2;
Texture2D enemyMiddle1Texture;
Texture2D enemyMiddle1Texture1;
Texture2D enemyMiddle1Texture2;
Texture2D enemyMiddle2Texture;
Texture2D enemyMiddle2Texture1;
Texture2D enemyMiddle2Texture2;
Texture2D wallTexture; // Protective wall sprite.
Texture2D menuBackground;
Texture2D levelBackground;
Texture2D aboutBackground;
Texture2D leaderboardBackground;
Texture2D howToPlayBackground;
Texture2D creditsBackground;
Sound playerShootSound;
Sound enemyShootSound;
Sound enemyHitSound;
Sound playerHitSound;
Music menuMusic;
Music aboutMusic;
Music easyMusic;
Music mediumMusic;
Music hardMusic;
Music currentMusic;
Rectangle player;
Rectangle enemies[MAX_ENEMY_ROWS][ENEMY_COLS];
Rectangle bullets[MAX_BULLETS];
Rectangle enemyBullets[MAX_ENEMY_BULLETS];
bool enemyActive[MAX_ENEMY_ROWS][ENEMY_COLS];
bool bulletActive[MAX_BULLETS];
bool enemyBulletActive[MAX_ENEMY_BULLETS];
Rectangle walls[WALL_COUNT]; // Protective wall rectangles.
int wallHealth[WALL_COUNT];  // Remaining hits for each wall.
int activeWallCount = 0;     // Number of walls used for the current difficulty.
float musicVolume = 0.35f;
float soundVolume = 0.50f;
bool musicMuted = false;
bool soundMuted = false;
int settingsSelection = 0;

void InitializeWalls(void)
{
    if (difficulty == EASY)
    {
        activeWallCount = 3;
    }
    else if (difficulty == MEDIUM)
    {
        activeWallCount = 1;
    }
    else
    {
        activeWallCount = 0;
    }

    float spacing = SCREEN_WIDTH / (float)(activeWallCount + 1);

    for (int i = 0; i < WALL_COUNT; i++)
    {
        wallHealth[i] = 0;
        walls[i].width = WALL_WIDTH;
        walls[i].height = WALL_HEIGHT;

        if (i < activeWallCount)
        {
            walls[i].x = spacing * (i + 1) - WALL_WIDTH / 2;
            walls[i].y = player.y - 80;
            wallHealth[i] = WALL_HEALTH;
        }
    }
}

Vector2 playerSpeed = {0};
int lives = 3;
int score = 0;
char playerName[MAX_NAME_LENGTH + 1] = ""; // Player name is preserved when restarting.
bool paused = false;
bool gameOver = false;
float gameTime = 0.0f;
float enemyFireTimer = 0.0f;
float enemyMoveTimer = 0.0f;
float animationTimer = 0.0f;
int animationFrame = 0;
int enemyDirection = 1;
int activeEnemyRows = ENEMY_ROWS;
int completedEnemySets = 0;    // Number of complete enemy sets cleared.
float formationOffsetX = 0.0f; // Horizontal offset of the whole formation.
float formationOffsetY = 0.0f; // Vertical offset of the whole formation.
ScoreEntry leaderboard[MAX_SCORES];
int leaderboardCount = 0;
int currentScoreRank = -1;
bool scoreSaved = false;
int main(void)
{ // Program entry point
    // SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders"); // create the game window
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);                                                         // set game frame rate
    playerTexture = LoadTexture("assets/player.png");                         // load an image texture
    playerTexture1 = LoadTexture("assets/player1.png");                       // load an image texture
    playerTexture2 = LoadTexture("assets/player2.png");                       // load an image texture
    enemyTexture = LoadTexture("assets/enemy.png");                           // load an image texture
    enemyTexture1 = LoadTexture("assets/enemy1.png");                         // load an image texture
    enemyTexture2 = LoadTexture("assets/enemy2.png");                         // load an image texture
    enemyMiddle1Texture = LoadTexture("assets/enemy_middle1.png");            // load an image texture
    enemyMiddle1Texture1 = LoadTexture("assets/enemy_middle1_1.png");         // load an image texture
    enemyMiddle1Texture2 = LoadTexture("assets/enemy_middle1_2.png");         // load an image texture
    enemyMiddle2Texture = LoadTexture("assets/enemy_middle2.png");            // load an image texture
    enemyMiddle2Texture1 = LoadTexture("assets/enemy_middle2_1.png");         // load an image texture
    enemyMiddle2Texture2 = LoadTexture("assets/enemy_middle2_2.png");         // load an image texture
    wallTexture = LoadTexture("assets/wall.png");                             // load the protective wall sprite
    menuBackground = LoadTexture("assets/menu_background.png");               // load an image texture
    levelBackground = LoadTexture("assets/level_background.png");             // load an image texture
    aboutBackground = LoadTexture("assets/about_background.png");             // load an image texture
    leaderboardBackground = LoadTexture("assets/leaderboard_background.png"); // load an image texture
    howToPlayBackground = LoadTexture("assets/how_to_play_background.png");   // load an image texture
    creditsBackground = LoadTexture("assets/credits_background.png");         // load an image texture
    playerShootSound = LoadSound("assets/player_shoot.wav");                  // load a sound effect
    enemyShootSound = LoadSound("assets/enemy_shoot.wav");                    // load a sound effect
    enemyHitSound = LoadSound("assets/enemy_hit.wav");                        // load a sound effect
    playerHitSound = LoadSound("assets/player_hit.wav");                      // load a sound effect
    menuMusic = LoadMusicStream("assets/menu_music.mp3");                     // load music stream
    aboutMusic = LoadMusicStream("assets/about_music.mp3");                   // load music stream
    easyMusic = LoadMusicStream("assets/easy_music.mp3");                     // load music stream
    mediumMusic = LoadMusicStream("assets/medium_music.mp3");                 // load music stream
    hardMusic = LoadMusicStream("assets/hard_music.mp3");                     // load music stream
    menuMusic.looping = true;
    aboutMusic.looping = true;
    easyMusic.looping = true;
    mediumMusic.looping = true;
    hardMusic.looping = true;
    SetMusicVolume(menuMusic, musicVolume);   // set music volume
    SetMusicVolume(aboutMusic, musicVolume);  // set music volume
    SetMusicVolume(easyMusic, musicVolume);   // set music volume
    SetMusicVolume(mediumMusic, musicVolume); // set music volume
    SetMusicVolume(hardMusic, musicVolume);   // set music volume
    SetSoundVolume(playerShootSound, soundVolume);
    SetSoundVolume(enemyShootSound, soundVolume);
    SetSoundVolume(enemyHitSound, soundVolume);
    SetSoundVolume(playerHitSound, soundVolume);
    ApplyAudioSettings();
    LoadLeaderboard();
    ResetGame();
    ChangeMusic(menuMusic);
    while (!WindowShouldClose())
    {                                     // Loop through items
        float deltaTime = GetFrameTime(); // get elapsed frame time
        UpdateMusicStream(currentMusic);  // update music stream
        if (currentScreen == MAIN_MENU)
        {
            if (IsKeyPressed(KEY_ONE))
            {                     // check for a key press
                ClearCharQueue(); // Prevent the menu key from entering the name box.
                currentScreen = NAME_INPUT;
            }
            if (IsKeyPressed(KEY_TWO))
            {
                currentScreen = LEADERBOARD;
            } // check for a key press
            if (IsKeyPressed(KEY_THREE))
            { // check for a key press
                currentScreen = ABOUT;
                ChangeMusic(aboutMusic);
            }
            if (IsKeyPressed(KEY_FOUR))
            {
                settingsSelection = 0;
                currentScreen = SETTINGS;
            }
            if (IsKeyPressed(KEY_FIVE) || IsKeyPressed(KEY_ESCAPE))
            {
                break;
            } // check for a key press
        }
        else if (currentScreen == NAME_INPUT)
        {
            int nameLength = (int)strlen(playerName);
            int key = GetCharPressed(); // get next typed character
            while (key > 0)
            {                   // Loop through items
                if (key == ' ') // Spaces are replaced because the leaderboard uses spaces as separators.
                {
                    key = '_';
                }
                if (key >= 33 && key <= 126 && nameLength < MAX_NAME_LENGTH)
                {
                    playerName[nameLength] = (char)key;
                    nameLength++;
                    playerName[nameLength] = '\0';
                }
                key = GetCharPressed(); // get next typed character
            }
            if (IsKeyPressed(KEY_BACKSPACE) && nameLength > 0)
            { // check for a key press
                nameLength--;
                playerName[nameLength] = '\0';
            }
            if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
            { // check for a key press
                if (nameLength == 0)
                {
                    strcpy(playerName, DEFAULT_PLAYER_NAME);
                }
                currentScreen = MODE_SELECTION;
            }
            if (IsKeyPressed(KEY_ESCAPE))
            { // check for a key press
                currentScreen = MAIN_MENU;
                ChangeMusic(menuMusic);
            }
        }
        else if (currentScreen == MODE_SELECTION)
        {
            if (IsKeyPressed(KEY_ONE))
            { // check for a key press
                difficulty = EASY;
                ResetGame();
                currentScreen = GAMEPLAY;
                ChangeMusic(easyMusic);
            }
            if (IsKeyPressed(KEY_TWO))
            { // check for a key press
                difficulty = MEDIUM;
                ResetGame();
                currentScreen = GAMEPLAY;
                ChangeMusic(mediumMusic);
            }
            if (IsKeyPressed(KEY_THREE))
            { // check for a key press
                difficulty = HARD;
                ResetGame();
                currentScreen = GAMEPLAY;
                ChangeMusic(hardMusic);
            }
            if (IsKeyPressed(KEY_ESCAPE))
            { // check for a key press
                ClearCharQueue();
                currentScreen = NAME_INPUT;
            }
        }
        else if (currentScreen == LEADERBOARD)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            { // check for a key press
                currentScreen = MAIN_MENU;
                ChangeMusic(menuMusic);
            }
        }
        else if (currentScreen == SETTINGS)
        {
            if (IsKeyPressed(KEY_UP))
            {
                settingsSelection--;
                if (settingsSelection < 0)
                {
                    settingsSelection = 3;
                }
            }
            if (IsKeyPressed(KEY_DOWN))
            {
                settingsSelection++;
                if (settingsSelection > 3)
                {
                    settingsSelection = 0;
                }
            }
            if (settingsSelection == 0)
            {
                if (IsKeyPressed(KEY_LEFT))
                {
                    musicVolume -= 0.1f;
                }
                if (IsKeyPressed(KEY_RIGHT))
                {
                    musicVolume += 0.1f;
                }
                musicVolume = Clamp(musicVolume, 0.0f, 1.0f);
                ApplyAudioSettings();
            }
            else if (settingsSelection == 1)
            {
                if (IsKeyPressed(KEY_LEFT))
                {
                    soundVolume -= 0.1f;
                }
                if (IsKeyPressed(KEY_RIGHT))
                {
                    soundVolume += 0.1f;
                }
                soundVolume = Clamp(soundVolume, 0.0f, 1.0f);
                ApplyAudioSettings();
            }
            else if (settingsSelection == 2)
            {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
                {
                    musicMuted = !musicMuted;
                    ApplyAudioSettings();
                }
            }
            else if (settingsSelection == 3)
            {
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER))
                {
                    soundMuted = !soundMuted;
                    ApplyAudioSettings();
                }
            }
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = MAIN_MENU;
                ChangeMusic(menuMusic);
            }
        }
        else if (currentScreen == ABOUT)
        {
            if (IsKeyPressed(KEY_ONE))
            {
                currentScreen = HOW_TO_PLAY;
            } // check for a key press
            if (IsKeyPressed(KEY_TWO))
            {
                currentScreen = CREDITS;
            } // check for a key press
            if (IsKeyPressed(KEY_THREE))
            {
                currentScreen = DEVELOPER;
            } // check for a key press
            if (IsKeyPressed(KEY_ESCAPE))
            { // check for a key press
                currentScreen = MAIN_MENU;
                ChangeMusic(menuMusic);
            }
        }
        else if (currentScreen == HOW_TO_PLAY)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = ABOUT;
            }
        } // check for a key press
        else if (currentScreen == CREDITS)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = ABOUT;
            }
        } // check for a key press
        else if (currentScreen == DEVELOPER)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            {
                currentScreen = ABOUT;
            }
        } // check for a key press
        else if (currentScreen == GAMEPLAY)
        {
            if (IsKeyPressed(KEY_ESCAPE))
            { // check for a key press
                paused = false;
                currentScreen = MAIN_MENU;
                ChangeMusic(menuMusic);
            }
            if (IsKeyPressed(KEY_P))
            {
                paused = !paused;
            } // check for a key press
            if (!paused)
            {
                animationTimer += deltaTime;
                if (animationTimer >= 0.15f)
                {
                    animationTimer = 0.0f;
                    animationFrame = !animationFrame;
                }
                playerSpeed.x = 0;
                if (IsKeyDown(KEY_LEFT))
                {
                    playerSpeed.x = -PLAYER_MAX_SPEED;
                } // check whether key is held
                if (IsKeyDown(KEY_RIGHT))
                {
                    playerSpeed.x = PLAYER_MAX_SPEED;
                } // check whether key is held
                player.x += playerSpeed.x * deltaTime;
                if (player.x < 0)
                {
                    player.x = 0;
                }
                if (player.x + player.width > SCREEN_WIDTH)
                {
                    player.x = SCREEN_WIDTH - player.width;
                }
                if (IsKeyPressed(KEY_SPACE))
                { // check for a key press
                    for (int i = 0; i < MAX_BULLETS; i++)
                    { // Loop through player bullets
                        if (!bulletActive[i])
                        {
                            bullets[i].x = player.x + player.width / 2 - BULLET_WIDTH / 2;
                            bullets[i].y = player.y;
                            bullets[i].width = BULLET_WIDTH;
                            bullets[i].height = BULLET_HEIGHT;
                            bulletActive[i] = true;
                            PlaySound(playerShootSound); // play a sound effect
                            break;
                        }
                    }
                }
                gameTime += deltaTime;
                for (int i = 0; i < MAX_BULLETS; i++)
                { // Loop through player bullets
                    if (bulletActive[i])
                    {
                        bullets[i].y -= MAX_BULLET_SPEED * deltaTime;
                        if (bullets[i].y + bullets[i].height < 0)
                        {
                            bulletActive[i] = false;
                        }
                    }
                }
                float currentEnemySpeed = ENEMY_SPEED + gameTime * 3.0f;
                if (currentEnemySpeed > ENEMY_MAX_SPEED)
                {
                    currentEnemySpeed = ENEMY_MAX_SPEED;
                }
                enemyMoveTimer += deltaTime;
                if (enemyMoveTimer >= ENEMY_STEP_TIME)
                {
                    enemyMoveTimer = 0;
                    bool anyEnemyAlive = false;
                    float leftMost = SCREEN_WIDTH;
                    float rightMost = 0;
                    for (int row = 0; row < activeEnemyRows; row++)
                    { // Loop through active enemy rows
                        for (int col = 0; col < ENEMY_COLS; col++)
                        { // Loop through enemy columns
                            if (enemyActive[row][col])
                            {
                                anyEnemyAlive = true;
                                if (enemies[row][col].x < leftMost)
                                {
                                    leftMost = enemies[row][col].x;
                                }
                                if (enemies[row][col].x + enemies[row][col].width > rightMost)
                                {
                                    rightMost = enemies[row][col].x + enemies[row][col].width;
                                }
                            }
                        }
                    }
                    if (anyEnemyAlive)
                    {
                        bool hitEdge = false;
                        if (enemyDirection < 0 && leftMost <= 0.5f)
                        {
                            hitEdge = true;
                        }
                        if (enemyDirection > 0 && rightMost >= SCREEN_WIDTH - 0.5f)
                        {
                            hitEdge = true;
                        }
                        if (hitEdge)
                        {
                            enemyDirection *= -1;
                            for (int row = 0; row < activeEnemyRows; row++)
                            { // Loop through active enemy rows
                                for (int col = 0; col < ENEMY_COLS; col++)
                                { // Loop through enemy columns
                                    if (enemyActive[row][col])
                                    {
                                        enemies[row][col].y += ENEMY_MOVINGDOWN;
                                    }
                                }
                            }
                            formationOffsetY += ENEMY_MOVINGDOWN;
                        }
                        float stepX = enemyDirection * currentEnemySpeed * ENEMY_STEP_TIME;
                        if (enemyDirection < 0 && leftMost + stepX < 0)
                        {
                            stepX = -leftMost;
                        }
                        if (enemyDirection > 0 && rightMost + stepX > SCREEN_WIDTH)
                        {
                            stepX = SCREEN_WIDTH - rightMost;
                        }
                        for (int row = 0; row < activeEnemyRows; row++)
                        { // Loop through active enemy rows
                            for (int col = 0; col < ENEMY_COLS; col++)
                            { // Loop through enemy columns
                                if (enemyActive[row][col])
                                {
                                    enemies[row][col].x += stepX;
                                }
                            }
                        }
                        formationOffsetX += stepX;
                    }
                }
                float enemyFireInterval;
                if (difficulty == EASY)
                {
                    enemyFireInterval = 1.5f;
                }
                else if (difficulty == MEDIUM)
                {
                    enemyFireInterval = 1.0f;
                }
                else
                {
                    enemyFireInterval = 0.6f;
                }
                enemyFireInterval -= gameTime * 0.01f;
                if (enemyFireInterval < 0.2f)
                {
                    enemyFireInterval = 0.2f;
                }
                enemyFireTimer += deltaTime;
                if (enemyFireTimer >= enemyFireInterval)
                {
                    enemyFireTimer = 0;
                    int shooterRows[ENEMY_COLS]; // Bottom-most alive enemy in each column can shoot.
                    int shooterColumns[ENEMY_COLS];
                    int shooterCount = 0;
                    for (int col = 0; col < ENEMY_COLS; col++)
                    { // Loop through enemy columns
                        for (int row = activeEnemyRows - 1; row >= 0; row--)
                        { // Loop through active enemy rows
                            if (enemyActive[row][col])
                            {
                                shooterRows[shooterCount] = row;
                                shooterColumns[shooterCount] = col;
                                shooterCount++;
                                break;
                            }
                        }
                    }
                    int selectedRow = -1;
                    int selectedColumn = -1;
                    if (shooterCount > 0)
                    {
                        int chosen = GetRandomValue(0, shooterCount - 1);
                        selectedRow = shooterRows[chosen];
                        selectedColumn = shooterColumns[chosen];
                    }
                    if (selectedRow != -1)
                    {
                        for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
                        { // Loop through enemy bullets
                            if (!enemyBulletActive[i])
                            {
                                enemyBullets[i].x = enemies[selectedRow][selectedColumn].x + ENEMY_WIDTH / 2 - BULLET_WIDTH / 2;
                                enemyBullets[i].y = enemies[selectedRow][selectedColumn].y + ENEMY_HEIGHT;
                                enemyBullets[i].width = BULLET_WIDTH;
                                enemyBullets[i].height = BULLET_HEIGHT;
                                enemyBulletActive[i] = true;
                                PlaySound(enemyShootSound); // play a sound effect
                                break;
                            }
                        }
                    }
                }
                for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
                { // Loop through enemy bullets
                    if (enemyBulletActive[i])
                    {
                        enemyBullets[i].y += ENEMY_BULLET_SPEED * deltaTime;

                        for (int w = 0; w < activeWallCount; w++) // Check enemy bullets against walls.
                        {
                            if (wallHealth[w] > 0 &&
                                CheckCollisionRecs(
                                    enemyBullets[i],
                                    walls[w]))
                            {
                                wallHealth[w]--;
                                enemyBulletActive[i] = false;

                                if (wallHealth[w] <= 0)
                                {
                                    wallHealth[w] = 0;
                                }

                                break;
                            }
                        }

                        if (enemyBullets[i].y > SCREEN_HEIGHT)
                        {
                            enemyBulletActive[i] = false;
                        }
                    }
                }
                for (int i = 0; i < MAX_BULLETS; i++)
                { // Loop through player bullets
                    if (!bulletActive[i])
                    {
                        continue;
                    }
                    for (int row = 0; row < activeEnemyRows; row++)
                    { // Loop through active enemy rows
                        for (int col = 0; col < ENEMY_COLS; col++)
                        { // Loop through enemy columns
                            if (enemyActive[row][col] && CheckCollisionRecs(bullets[i], enemies[row][col]))
                            { // check rectangle collision
                                bulletActive[i] = false;
                                enemyActive[row][col] = false;
                                score += 10;
                                PlaySound(enemyHitSound); // play a sound effect
                                break;
                            }
                        }
                        if (!bulletActive[i])
                        {
                            break;
                        }
                    }
                }
                for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
                { // Loop through enemy bullets
                    if (enemyBulletActive[i] && CheckCollisionRecs(enemyBullets[i], player))
                    { // check rectangle collision
                        enemyBulletActive[i] = false;
                        lives--;
                        PlaySound(playerHitSound); // play a sound effect
                        if (lives <= 0)
                        {
                            lives = 0;
                            gameOver = true;
                            currentScreen = GAME_OVER;
                            ChangeMusic(menuMusic);
                        }
                        break;
                    }
                }
                if (!gameOver)
                {
                    for (int row = 0; row < activeEnemyRows; row++)
                    { // Loop through active enemy rows
                        for (int col = 0; col < ENEMY_COLS; col++)
                        { // Loop through enemy columns
                            if (enemyActive[row][col] && CheckCollisionRecs(enemies[row][col], player))
                            { // check rectangle collision
                                lives = 0;
                                gameOver = true;
                                currentScreen = GAME_OVER;
                                ChangeMusic(menuMusic);
                                break;
                            }
                        }
                        if (gameOver)
                        {
                            break;
                        }
                    }
                }
                if (!gameOver)
                {
                    for (int row = 0; row < activeEnemyRows; row++)
                    { // Loop through active enemy rows
                        for (int col = 0; col < ENEMY_COLS; col++)
                        { // Loop through enemy columns
                            if (enemyActive[row][col] && enemies[row][col].y + enemies[row][col].height >= player.y)
                            {
                                lives = 0;
                                gameOver = true;
                                currentScreen = GAME_OVER;
                                ChangeMusic(menuMusic);
                                break;
                            }
                        }
                        if (gameOver)
                        {
                            break;
                        }
                    }
                }
                if (!gameOver)
                {
                    int activeEnemies = GetActiveEnemyCount(); // Count remaining enemies.

                    if (activeEnemies == 0)
                    {
                        completedEnemySets++; // One complete enemy set has been cleared.

                        if (completedEnemySets >= 2)
                        {
                            completedEnemySets = 0; // Two clears completed; prepare a larger set.

                            if (activeEnemyRows < MAX_ENEMY_ROWS)
                            {
                                activeEnemyRows++; // Add one row for the next set.
                            }
                        }

                        InitializeEnemies(); // Regenerate the complete current enemy set.
                        InitializeWalls();   // Restore all protective walls for the new set.
                    }
                }
            }
        }
        else if (currentScreen == GAME_OVER)
        {
            if (!scoreSaved)
            {
                AddScore(score);
                scoreSaved = true;
            }
            if (IsKeyPressed(KEY_R))
            { // check for a key press
                ResetGame();
                currentScreen = GAMEPLAY;
                if (difficulty == EASY)
                {
                    ChangeMusic(easyMusic);
                }
                else if (difficulty == MEDIUM)
                {
                    ChangeMusic(mediumMusic);
                }
                else
                {
                    ChangeMusic(hardMusic);
                }
            }
            if (IsKeyPressed(KEY_ESCAPE))
            { // check for a key press
                ResetGame();
                currentScreen = MAIN_MENU;
                ChangeMusic(menuMusic);
            }
        }
        BeginDrawing();         // begin drawing frame
        ClearBackground(BLACK); // clear the screen
        if (currentScreen == MAIN_MENU)
        {
            DrawMainMenu();
        }
        else if (currentScreen == NAME_INPUT)
        {
            DrawNameInput();
        }
        else if (currentScreen == MODE_SELECTION)
        {
            DrawModeSelection();
        }
        else if (currentScreen == LEADERBOARD)
        {
            DrawLeaderboard();
        }
        else if (currentScreen == SETTINGS)
        {
            DrawSettings();
        }
        else if (currentScreen == ABOUT)
        {
            DrawAbout();
        }
        else if (currentScreen == HOW_TO_PLAY)
        {
            DrawHowToPlay();
        }
        else if (currentScreen == CREDITS)
        {
            DrawCredits();
        }
        else if (currentScreen == DEVELOPER)
        {
            DrawDeveloper();
        }
        else if (currentScreen == GAMEPLAY)
        {
            DrawGameplay();
        }
        else if (currentScreen == GAME_OVER)
        {
            DrawGameOver();
        }
        EndDrawing(); // finish drawing frame
    }
    UnloadTexture(playerTexture);         // unload a texture
    UnloadTexture(playerTexture1);        // unload a texture
    UnloadTexture(playerTexture2);        // unload a texture
    UnloadTexture(enemyTexture);          // unload a texture
    UnloadTexture(enemyTexture1);         // unload a texture
    UnloadTexture(enemyTexture2);         // unload a texture
    UnloadTexture(enemyMiddle1Texture);   // unload a texture
    UnloadTexture(enemyMiddle1Texture1);  // unload a texture
    UnloadTexture(enemyMiddle1Texture2);  // unload a texture
    UnloadTexture(enemyMiddle2Texture);   // unload a texture
    UnloadTexture(enemyMiddle2Texture1);  // unload a texture
    UnloadTexture(enemyMiddle2Texture2);  // unload a texture
    UnloadTexture(wallTexture);           // unload the protective wall sprite
    UnloadTexture(menuBackground);        // unload a texture
    UnloadTexture(levelBackground);       // unload a texture
    UnloadTexture(aboutBackground);       // unload a texture
    UnloadTexture(leaderboardBackground); // unload a texture
    UnloadTexture(howToPlayBackground);   // unload a texture
    UnloadTexture(creditsBackground);     // unload a texture
    UnloadSound(playerShootSound);        // unload a sound effect
    UnloadSound(enemyShootSound);         // unload a sound effect
    UnloadSound(enemyHitSound);           // unload a sound effect
    UnloadSound(playerHitSound);          // unload a sound effect
    UnloadMusicStream(menuMusic);         // unload music stream
    UnloadMusicStream(aboutMusic);        // unload music stream
    UnloadMusicStream(easyMusic);         // unload music stream
    UnloadMusicStream(mediumMusic);       // unload music stream
    UnloadMusicStream(hardMusic);         // unload music stream
    CloseAudioDevice();
    CloseWindow(); // close the game window
    return 0;
}
void ChangeMusic(Music newMusic)
{ // Switch background music
    if (currentMusic.stream.buffer != NULL && currentMusic.stream.buffer == newMusic.stream.buffer)
    {
        return;
    }
    if (currentMusic.stream.buffer != NULL)
    {
        StopMusicStream(currentMusic);
    } // stop music playback
    currentMusic = newMusic;
    PlayMusicStream(currentMusic); // start music playback
}
void ApplyAudioSettings(void)
{
    float currentMusicVolume = musicMuted ? 0.0f : musicVolume;
    float currentSoundVolume = soundMuted ? 0.0f : soundVolume;

    SetMusicVolume(menuMusic, currentMusicVolume);
    SetMusicVolume(aboutMusic, currentMusicVolume);
    SetMusicVolume(easyMusic, currentMusicVolume);
    SetMusicVolume(mediumMusic, currentMusicVolume);
    SetMusicVolume(hardMusic, currentMusicVolume);

    SetSoundVolume(playerShootSound, currentSoundVolume);
    SetSoundVolume(enemyShootSound, currentSoundVolume);
    SetSoundVolume(enemyHitSound, currentSoundVolume);
    SetSoundVolume(playerHitSound, currentSoundVolume);
}
void LoadLeaderboard(void)
{ // Load saved scores
    FILE *file = fopen(LEADERBOARD_FILE, "r");
    leaderboardCount = 0;
    if (file == NULL)
    {
        return;
    }
    while (leaderboardCount < MAX_SCORES && fscanf(file, "%29s %d", leaderboard[leaderboardCount].name, &leaderboard[leaderboardCount].score) == 2)
    { // Read saved scores
        leaderboardCount++;
    }
    fclose(file);
}
void SaveLeaderboard(void)
{ // Save scores to file
    FILE *file = fopen(LEADERBOARD_FILE, "w");
    if (file == NULL)
    {
        return;
    }
    for (int i = 0; i < leaderboardCount; i++)
    { // Loop through leaderboard
        fprintf(file, "%s %d\n", leaderboard[i].name, leaderboard[i].score);
    }
    fclose(file);
}
void AddScore(int newScore)
{ // Add score to leaderboard
    int position = 0;
    while (position < leaderboardCount && leaderboard[position].score >= newScore)
    { // Loop through leaderboard
        position++;
    }
    if (position >= MAX_SCORES)
    {
        currentScoreRank = -1;
        return;
    }
    currentScoreRank = position + 1;
    if (leaderboardCount < MAX_SCORES)
    {
        leaderboardCount++;
    }
    for (int i = leaderboardCount - 1; i > position; i--)
    { // Loop through leaderboard
        leaderboard[i] = leaderboard[i - 1];
    }
    const char *nameToSave = (playerName[0] != '\0') ? playerName : DEFAULT_PLAYER_NAME;
    snprintf(leaderboard[position].name, sizeof(leaderboard[position].name), "%s", nameToSave);
    leaderboard[position].score = newScore;
    SaveLeaderboard();
}
void InitializeEnemies(void)
{ // Reset enemy formation
    activeEnemyRows = ENEMY_ROWS;
    enemyDirection = 1;
    formationOffsetX = 0.0f;
    formationOffsetY = 0.0f;
    for (int row = 0; row < MAX_ENEMY_ROWS; row++)
    { // Loop through rows
        for (int col = 0; col < ENEMY_COLS; col++)
        { // Loop through enemy columns
            enemies[row][col].x = FORMATION_START_X + col * ENEMY_SPACING_X;
            enemies[row][col].y = FORMATION_START_Y + row * ENEMY_SPACING_Y;
            enemies[row][col].width = ENEMY_WIDTH;
            enemies[row][col].height = ENEMY_HEIGHT;
            enemyActive[row][col] = false;
        }
    }
    for (int row = 0; row < ENEMY_ROWS; row++)
    { // Loop through rows
        for (int col = 0; col < ENEMY_COLS; col++)
        { // Loop through enemy columns
            enemyActive[row][col] = true;
        }
    }
}
void SpawnEnemyRow(int row)
{ // Spawn a new enemy row
    if (row < 0 || row >= MAX_ENEMY_ROWS)
    {
        return;
    }
    float rowWidth = (ENEMY_COLS - 1) * ENEMY_SPACING_X + ENEMY_WIDTH;
    float minOffsetX = -FORMATION_START_X;
    float maxOffsetX = SCREEN_WIDTH - FORMATION_START_X - rowWidth;
    float rowOffsetX = Clamp(formationOffsetX, minOffsetX, maxOffsetX);
    for (int col = 0; col < ENEMY_COLS; col++)
    { // Loop through enemy columns
        enemies[row][col].x = FORMATION_START_X + col * ENEMY_SPACING_X + rowOffsetX;
        enemies[row][col].y = FORMATION_START_Y + row * ENEMY_SPACING_Y + formationOffsetY;
        enemies[row][col].width = ENEMY_WIDTH;
        enemies[row][col].height = ENEMY_HEIGHT;
        enemyActive[row][col] = true;
    }
}
bool CanSpawnEnemyRow(int row)
{ // Check if a row can spawn
    if (row < 0 || row >= MAX_ENEMY_ROWS)
    {
        return false;
    }
    float rowBottom = FORMATION_START_Y + row * ENEMY_SPACING_Y + formationOffsetY + ENEMY_HEIGHT;
    return rowBottom + ENEMY_SPACING_Y < player.y;
}
int GetActiveEnemyCount(void)
{ // Count active enemies
    int count = 0;
    for (int row = 0; row < activeEnemyRows; row++)
    { // Loop through active enemy rows
        for (int col = 0; col < ENEMY_COLS; col++)
        { // Loop through enemy columns
            if (enemyActive[row][col])
            {
                count++;
            }
        }
    }
    return count;
}
void ClearCharQueue(void)
{ // Clear pending keyboard input
    while (GetCharPressed() > 0)
    { // Clear input queue
    }
}
void ResetGame(void)
{ // Reset game state
    lives = 3;
    score = 0;
    paused = false;
    gameOver = false;
    gameTime = 0.0f;
    enemyFireTimer = 0.0f;
    enemyMoveTimer = 0.0f;
    enemyDirection = 1;
    animationTimer = 0.0f;
    animationFrame = 0;
    activeEnemyRows = ENEMY_ROWS;
    completedEnemySets = 0;
    currentScoreRank = -1;
    scoreSaved = false;
    player.x = SCREEN_WIDTH / 2 - PLAYER_WIDTH / 2;
    player.y = 90 * SCREEN_HEIGHT / 100;
    player.width = PLAYER_WIDTH;
    player.height = PLAYER_HEIGHT;
    InitializeWalls(); // Reset protective walls for the new game.
    playerSpeed.x = 0;
    playerSpeed.y = 0;
    for (int i = 0; i < MAX_BULLETS; i++)
    { // Loop through player bullets
        bulletActive[i] = false;
        bullets[i] = (Rectangle){0, 0, 0, 0};
    }
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    { // Loop through enemy bullets
        enemyBulletActive[i] = false;
        enemyBullets[i] = (Rectangle){0, 0, 0, 0};
    }
    InitializeEnemies();
}
void DrawBackground(Texture2D texture)
{ // Draw screen background
    if (texture.id == 0)
    {
        return;
    }
    DrawTexturePro(texture, (Rectangle){0, 0, (float)texture.width, (float)texture.height}, (Rectangle){0, 0, SCREEN_WIDTH, SCREEN_HEIGHT}, (Vector2){0, 0}, 0.0f, WHITE); // draw transformed texture
}
void DrawMainMenu(void)
{ // Draw main menu
    DrawBackground(menuBackground);
    DrawText("SPACE INVADERS", SCREEN_WIDTH / 2 - MeasureText("SPACE INVADERS", 70) / 2, 180, 70, WHITE); // draw text
    DrawText("1. START", SCREEN_WIDTH / 2 - 120, 320, 35, WHITE);                                         // draw text
    DrawText("2. LEADERBOARD", SCREEN_WIDTH / 2 - 170, 380, 35, WHITE);                                   // draw text
    DrawText("3. ABOUT", SCREEN_WIDTH / 2 - 120, 440, 35, WHITE);                                         // draw text
    DrawText("4. SETTINGS", SCREEN_WIDTH / 2 - 120, 500, 35, WHITE);                                      // draw text
    DrawText("5. EXIT", SCREEN_WIDTH / 2 - 120, 560, 35, WHITE);                                          // draw text
}
void DrawNameInput(void)
{ // Draw name input
    DrawBackground(menuBackground);
    DrawText("ENTER YOUR NAME", SCREEN_WIDTH / 2 - MeasureText("ENTER YOUR NAME", 60) / 2, 180, 60, WHITE); // draw text
    Rectangle inputBox = {
        SCREEN_WIDTH / 2 - 250, 350, 500, 70};
    DrawRectangleLinesEx(inputBox, 3, WHITE);
    DrawText(playerName, (int)inputBox.x + 20, (int)inputBox.y + 18, 35, WHITE); // draw text
    int nameLength = (int)strlen(playerName);
    if (nameLength < MAX_NAME_LENGTH && ((int)(GetTime() * 2)) % 2 == 0)
    {
        DrawText("_", (int)inputBox.x + 20 + MeasureText(playerName, 35), (int)inputBox.y + 18, 35, WHITE);
    } // get elapsed time
    char lengthText[30];
    snprintf(lengthText, sizeof(lengthText), "%d / %d", nameLength, MAX_NAME_LENGTH);
    DrawText(lengthText, (int)(inputBox.x + inputBox.width) - MeasureText(lengthText, 20), (int)inputBox.y + 80, 20, GRAY); // draw text
    DrawText("Spaces are replaced with '_'", (int)inputBox.x, (int)inputBox.y + 80, 20, GRAY);                              // draw text
    DrawText("Press ENTER to continue", SCREEN_WIDTH / 2 - MeasureText("Press ENTER to continue", 30) / 2, 520, 30, WHITE); // draw text
    DrawText("Press ESC to go back", SCREEN_WIDTH / 2 - MeasureText("Press ESC to go back", 25) / 2, 600, 25, GRAY);        // draw text
}
void DrawModeSelection(void)
{ // Draw difficulty menu
    DrawBackground(menuBackground);
    DrawText("SELECT MODE", SCREEN_WIDTH / 2 - MeasureText("SELECT MODE", 60) / 2, 180, 60, WHITE); // draw text
    DrawText("1. EASY", SCREEN_WIDTH / 2 - 120, 350, 35, WHITE);                                    // draw text
    DrawText("2. MEDIUM", SCREEN_WIDTH / 2 - 120, 410, 35, WHITE);                                  // draw text
    DrawText("3. HARD", SCREEN_WIDTH / 2 - 120, 470, 35, WHITE);                                    // draw text
    DrawText("Press 0 to go back", SCREEN_WIDTH / 2 - 150, 600, 25, GRAY);                          // draw text
}
void DrawLeaderboard(void)
{ // Draw leaderboard
    DrawBackground(leaderboardBackground);
    DrawText("LEADERBOARD", SCREEN_WIDTH / 2 - MeasureText("LEADERBOARD", 60) / 2, 100, 60, WHITE); // draw text
    if (leaderboardCount == 0)
    {
        DrawText("No scores yet.", SCREEN_WIDTH / 2 - 100, 300, 30, WHITE);
    } // draw text
    for (int i = 0; i < leaderboardCount; i++)
    { // Loop through leaderboard
        char rankText[64];
        char scoreText[32];
        snprintf(rankText, sizeof(rankText), "%d. %s", i + 1, leaderboard[i].name);
        snprintf(scoreText, sizeof(scoreText), "%d", leaderboard[i].score);
        DrawText(rankText, SCREEN_WIDTH / 2 - 300, 220 + i * 55, 30, WHITE);                               // draw text
        DrawText(scoreText, SCREEN_WIDTH / 2 + 300 - MeasureText(scoreText, 30), 220 + i * 55, 30, WHITE); // draw text
    }
    DrawText("Press 0 to go back", SCREEN_WIDTH / 2 - 150, 850, 25, GRAY); // draw text
}
void DrawSettings(void)
{ // Draw settings menu
    DrawBackground(menuBackground);
    DrawText("SETTINGS", SCREEN_WIDTH / 2 - MeasureText("SETTINGS", 60) / 2, 120, 60, WHITE); // draw text

    Color musicColor = settingsSelection == 0 ? YELLOW : WHITE;
    Color soundColor = settingsSelection == 1 ? YELLOW : WHITE;
    Color musicMuteColor = settingsSelection == 2 ? YELLOW : WHITE;
    Color soundMuteColor = settingsSelection == 3 ? YELLOW : WHITE;

    char musicVolumeText[50];
    char soundVolumeText[50];
    snprintf(musicVolumeText, sizeof(musicVolumeText), "Music Volume       %d%%", (int)(musicVolume * 100));
    snprintf(soundVolumeText, sizeof(soundVolumeText), "Sound Volume       %d%%", (int)(soundVolume * 100));

    DrawText(musicVolumeText, SCREEN_WIDTH / 2 - 190, 270, 35, musicColor);
    DrawText(soundVolumeText, SCREEN_WIDTH / 2 - 190, 340, 35, soundColor);

    DrawText("Music", SCREEN_WIDTH / 2 - 190, 410, 35, musicMuteColor);
    DrawText(musicMuted ? "OFF" : "ON", SCREEN_WIDTH / 2 + 130, 410, 35, musicMuteColor);

    DrawText("Sound", SCREEN_WIDTH / 2 - 190, 480, 35, soundMuteColor);
    DrawText(soundMuted ? "OFF" : "ON", SCREEN_WIDTH / 2 + 130, 480, 35, soundMuteColor);

   DrawText("UP ARROW / DOWN ARROW to Select", SCREEN_WIDTH / 2 - 120, 600, 25, GRAY);
   DrawText("LEFT ARROW / RIGHT ARROW to Change Volume", SCREEN_WIDTH / 2 - 120, 635, 25, GRAY);
   DrawText("ENTER to On / Off Music or Sound", SCREEN_WIDTH / 2 - 120, 670, 25, GRAY);
   DrawText("0  Back", SCREEN_WIDTH / 2 - 120, 705, 25, GRAY);
}
void DrawAbout(void)
{ // Draw about menu
    DrawBackground(aboutBackground);
    DrawText("ABOUT", SCREEN_WIDTH / 2 - MeasureText("ABOUT", 60) / 2, 120, 60, WHITE); // draw text
    DrawText("1. HOW TO PLAY", SCREEN_WIDTH / 2 - 180, 300, 35, WHITE);                 // draw text
    DrawText("2. CREDITS", SCREEN_WIDTH / 2 - 180, 360, 35, WHITE);                     // draw text
    DrawText("3. ABOUT THE DEVELOPER", SCREEN_WIDTH / 2 - 180, 420, 35, WHITE);         // draw text
    DrawText("Press 0 to go back", SCREEN_WIDTH / 2 - 150, 600, 25, GRAY);              // draw text
}
void DrawHowToPlay(void)
{ // Draw controls/instructions
    DrawBackground(howToPlayBackground);
    DrawText("HOW TO PLAY", SCREEN_WIDTH / 2 - MeasureText("HOW TO PLAY", 55) / 2, 70, 55, WHITE); // draw text
    DrawText("CONTROLS", 180, 170, 35, WHITE);                                                     // draw text
    DrawText("Press LEFT ARROW to move left.", 180, 220, 25, WHITE);                               // draw text
    DrawText("Press RIGHT ARROW to move right.", 180, 260, 25, WHITE);                             // draw text
    DrawText("Press SPACE to shoot bullets.", 180, 300, 25, WHITE);                                // draw text
    DrawText("Press P to pause or resume the game.", 180, 340, 25, WHITE);                         // draw text
    DrawText("GAMEPLAY", 180, 410, 35, WHITE);                                                     // draw text
    DrawText("Destroy enemies to earn points.", 180, 460, 25, WHITE);                              // draw text
    DrawText("Avoid enemy bullets.", 180, 500, 25, WHITE);                                         // draw text
    DrawText("You have 3 lives.", 180, 540, 25, WHITE);                                            // draw text
    DrawText("New enemies will appear as you continue playing.", 180, 580, 25, WHITE);             // draw text
    DrawText("The game becomes harder as time passes.", 180, 620, 25, WHITE);                      // draw text
    DrawText("Choose Easy, Medium, or Hard mode before starting.", 180, 660, 25, WHITE);           // draw text
    DrawText("The game is endless and has no winning condition.", 180, 700, 25, WHITE);            // draw text
    DrawText("The game ends when all 3 lives are lost.", 180, 740, 25, WHITE);                     // draw text
    DrawText("Press 0 to go back", SCREEN_WIDTH / 2 - 150, 850, 25, GRAY);                         // draw text
}
void DrawCredits(void)
{ // Draw credits
    DrawBackground(creditsBackground);
    DrawText("CREDITS", SCREEN_WIDTH / 2 - MeasureText("CREDITS", 60) / 2, 120, 60, WHITE); // draw text
    DrawText("Image / Sprite Credit: XXXX", 300, 300, 30, WHITE);                           // draw text
    DrawText("Music Credit: XXXX", 300, 360, 30, WHITE);                                    // draw text
    DrawText("BG Music Credit: XXXX", 300, 420, 30, WHITE);                                 // draw text
    DrawText("Press 0 to go back", SCREEN_WIDTH / 2 - 150, 700, 25, GRAY);                  // draw text
}
void DrawDeveloper(void)
{ // Draw developer info
    DrawBackground(aboutBackground);
    DrawText("ABOUT THE DEVELOPER", SCREEN_WIDTH / 2 - MeasureText("ABOUT THE DEVELOPER", 55) / 2, 70, 55, WHITE); // draw text
    DrawText("Md. Habib Un Nabi", 200, 200, 30, WHITE);                                                            // draw text
    DrawText("CSE Department, BUET", 200, 240, 25, WHITE);                                                         // draw text
    DrawText("Student Id : 2505120", 200, 280, 25, WHITE);                                                         // draw text
    DrawText("Md. Arham Abir", 200, 360, 30, WHITE);                                                               // draw text
    DrawText("CSE Department, BUET", 200, 400, 25, WHITE);                                                         // draw text
    DrawText("Student Id : 2505102", 200, 440, 25, WHITE);                                                         // draw text
    DrawText("Supervisor:", 200, 520, 30, WHITE);                                                                  // draw text
    DrawText("Md. Ashrafur Rahman Khan", 200, 560, 25, WHITE);                                                     // draw text
    DrawText("Lecturer, CSE Department, BUET", 200, 600, 25, WHITE);                                               // draw text
    DrawText("M.Sc. in CSE, BUET. (Ongoing)", 200, 640, 25, WHITE);                                                // draw text
    DrawText("B.Sc. Engg. in CSE, BUET.", 200, 680, 25, WHITE);                                                    // draw text
    DrawText("Press 0 to go back", SCREEN_WIDTH / 2 - 150, 850, 25, GRAY);                                         // draw text
}
void DrawSprite(Texture2D texture, Rectangle destination, Color fallbackColor)
{                        // Draw a sprite or fallback
    if (texture.id == 0) // Use a fallback rectangle if the sprite failed to load.
    {
        DrawRectangleRec(destination, fallbackColor);
        return;
    }
    DrawTexturePro(texture, (Rectangle){0, 0, (float)texture.width, (float)texture.height}, destination, (Vector2){0, 0}, 0.0f, WHITE); // draw transformed texture
}
void DrawSpriteRotated(Texture2D texture, Rectangle destination, Color fallbackColor, float rotation)
{ // Draw a rotated sprite
    if (texture.id == 0)
    {
        DrawRectanglePro(destination, (Vector2){destination.width / 2, destination.height / 2}, rotation, fallbackColor);
        return;
    }
    DrawTexturePro(texture, (Rectangle){0, 0, (float)texture.width, (float)texture.height}, destination, (Vector2){destination.width / 2, destination.height / 2}, rotation, WHITE); // draw transformed texture
}
void DrawWallSprite(Texture2D texture, Rectangle destination) // Draw the wall sprite.
{
    if (texture.id == 0)
    {
        DrawRectangleRec(destination, GREEN); // Fallback if wall.png is missing.
        return;
    }

    DrawTexturePro(
        texture,
        (Rectangle){0, 0, (float)texture.width, (float)texture.height},
        destination,
        (Vector2){0, 0},
        0.0f,
        WHITE);
}

void DrawGameplay(void)
{
    // Draw gameplay background first.
    DrawBackground(levelBackground);

    // Draw protective walls after the background so they remain visible.
    for (int i = 0; i < activeWallCount; i++)
    {
        if (wallHealth[i] > 0)
        {
            DrawWallSprite(wallTexture, walls[i]);
        }
    }
    Texture2D playerFrame = playerTexture;
    if (animationFrame == 0 && playerTexture1.id != 0)
    {
        playerFrame = playerTexture1;
    }
    else if (animationFrame == 1 && playerTexture2.id != 0)
    {
        playerFrame = playerTexture2;
    }
    DrawSprite(playerFrame, player, GREEN);
    for (int row = 0; row < activeEnemyRows; row++)
    { // Loop through active enemy rows
        for (int col = 0; col < ENEMY_COLS; col++)
        { // Loop through enemy columns
            if (!enemyActive[row][col])
            {
                continue;
            }
            Texture2D textureToDraw = enemyTexture;
            if (row == 0)
            {
                if (animationFrame == 0 && enemyTexture1.id != 0)
                {
                    textureToDraw = enemyTexture1;
                }
                else if (animationFrame == 1 && enemyTexture2.id != 0)
                {
                    textureToDraw = enemyTexture2;
                }
            }
            else if (row == 1)
            {
                if (animationFrame == 0 && enemyMiddle1Texture1.id != 0)
                {
                    textureToDraw = enemyMiddle1Texture1;
                }
                else if (animationFrame == 1 && enemyMiddle1Texture2.id != 0)
                {
                    textureToDraw = enemyMiddle1Texture2;
                }
            }
            else
            {
                if (animationFrame == 0 && enemyMiddle2Texture1.id != 0)
                {
                    textureToDraw = enemyMiddle2Texture1;
                }
                else if (animationFrame == 1 && enemyMiddle2Texture2.id != 0)
                {
                    textureToDraw = enemyMiddle2Texture2;
                }
            }
            DrawSprite(textureToDraw, enemies[row][col], PURPLE);
        }
    }
    for (int i = 0; i < MAX_BULLETS; i++)
    { // Loop through player bullets
        if (bulletActive[i])
        {
            DrawRectangle((int)bullets[i].x - 2, (int)bullets[i].y - 5, (int)bullets[i].width + 4, (int)bullets[i].height + 10, Fade(WHITE, 0.25f)); // draw rectangle
            DrawRectangleRec(bullets[i], WHITE);
        }
    }
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
    { // Loop through enemy bullets
        if (enemyBulletActive[i])
        {
            DrawRectangle((int)enemyBullets[i].x - 2, (int)enemyBullets[i].y - 5, (int)enemyBullets[i].width + 4, (int)enemyBullets[i].height + 10, Fade(RED, 0.25f)); // draw rectangle
            DrawRectangleRec(enemyBullets[i], RED);
        }
    }
    char scoreText[50];
    snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
    DrawText(scoreText, 30, 25, 30, WHITE); // draw text
    char livesText[50];
    snprintf(livesText, sizeof(livesText), "Lives: %d", lives);
    DrawText(livesText, SCREEN_WIDTH - 180, 25, 30, WHITE); // draw text
    char difficultyText[50];
    if (difficulty == EASY)
    {
        strcpy(difficultyText, "Mode: EASY");
    }
    else if (difficulty == MEDIUM)
    {
        strcpy(difficultyText, "Mode: MEDIUM");
    }
    else
    {
        strcpy(difficultyText, "Mode: HARD");
    }
    DrawText(difficultyText, 30, 65, 25, WHITE); // draw text
    if (paused)
    {
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.5f));                                                          // draw rectangle
        DrawText("PAUSED", SCREEN_WIDTH / 2 - MeasureText("PAUSED", 70) / 2, SCREEN_HEIGHT / 2 - 50, 70, WHITE);                      // draw text
        DrawText("Press P to resume", SCREEN_WIDTH / 2 - MeasureText("Press P to resume", 30) / 2, SCREEN_HEIGHT / 2 + 40, 30, GRAY); // draw text
    }
}
void DrawGameOver(void)
{ // Draw game-over screen
    DrawBackground(menuBackground);
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - MeasureText("GAME OVER", 80) / 2, 180, 80, RED); // draw text
    char nameText[60];
    snprintf(nameText, sizeof(nameText), "Player: %s", playerName);
    DrawText(nameText, SCREEN_WIDTH / 2 - MeasureText(nameText, 30) / 2, 275, 30, WHITE); // draw text
    char scoreText[100];
    snprintf(scoreText, sizeof(scoreText), "Your Score: %d", score);
    DrawText(scoreText, SCREEN_WIDTH / 2 - MeasureText(scoreText, 35) / 2, 330, 35, WHITE); // draw text
    int highestScore = score;
    if (leaderboardCount > 0 && leaderboard[0].score > highestScore)
    {
        highestScore = leaderboard[0].score;
    }
    char highestScoreText[100];
    snprintf(highestScoreText, sizeof(highestScoreText), "Highest Score: %d", highestScore);
    DrawText(highestScoreText, SCREEN_WIDTH / 2 - MeasureText(highestScoreText, 35) / 2, 390, 35, WHITE); // draw text
    if (currentScoreRank > 0)
    {
        char rankText[100];
        snprintf(rankText, sizeof(rankText), "Your Position: %d", currentScoreRank);
        DrawText(rankText, SCREEN_WIDTH / 2 - MeasureText(rankText, 30) / 2, 450, 30, WHITE); // draw text
    }
    else if (scoreSaved) // Score was outside the top 10.
    {
        DrawText("You did not reach the Top 10.", SCREEN_WIDTH / 2 - MeasureText("You did not reach the Top 10.", 30) / 2, 450, 30, GRAY);
    }                                                                                                                                                   // draw text
    DrawText("Press 'R' to Restart.", SCREEN_WIDTH / 2 - MeasureText("Press 'R' to Restart.", 30) / 2, 570, 30, WHITE);                                 // draw text
    DrawText("Press '0' to return to the main menu.", SCREEN_WIDTH / 2 - MeasureText("Press '0' to return to the main menu.", 30) / 2, 620, 30, WHITE); // draw text
}