#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define NUM_LANES 3
#define LANE_WIDTH (300 / NUM_LANES)

typedef struct {
    Rectangle rect;
    Color color; // Can keep as fallback tint
    bool active;
    float speed;
    int spriteType; // 0 for sprite sheet A, 1 for sprite sheet B
} Obstacle;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Paradise: Scooty Ride");
    SetTargetFPS(60);

    // --- ASSET LOADING ---
    Texture2D background = LoadTexture("paraBgr.png");
    Texture2D scootyTex = LoadTexture("paradHeroscooty.png");
    // --- LOAD OBSTACLE TEXTURES ---
Texture2D obstacleTex1 = LoadTexture("shikanjaMalik.png"); // e.g., Rival motorcycle
Texture2D obstacleTex2 = LoadTexture("chotaMalik.png"); // e.g., Vintage car / rioter
    
    if (background.id == 0) {
        TraceLog(LOG_WARNING, "Background texture failed to load!");
    }
    if (scootyTex.id == 0) {
        TraceLog(LOG_WARNING, "Scooty texture failed to load!");
    }

    // --- SCARFY-STYLE ANIMATION & SCALING SETUP ---
    int maxFrames = 4; // Adjust to match the exact number of frames horizontally in your sheet
    float frameWidth = (float)scootyTex.width / maxFrames;
    float frameHeight = (float)scootyTex.height;
    
    Rectangle frameRec = { 0.0f, 0.0f, frameWidth, frameHeight };
    int currentFrame = 0;
    int framesCounter = 0;

Rectangle playerCollider = { 
    (float)(LANE_WIDTH * 1 + LANE_WIDTH / 2.0f - 15.0f)+150, 
    (float)(SCREEN_HEIGHT - 140), 
    30.0f,  // Width of collision box (make it narrower than the sprite)
    70.0f   // Height of collision box
};

    float scale = 0.35f; // Scale down the sprite if it appears too large
    Rectangle playerDestRec = { 
        (float)(LANE_WIDTH * 1 + LANE_WIDTH / 2.0f - (frameWidth * scale) / 2.0f)+150, 
        (float)(SCREEN_HEIGHT - 160), 
        frameWidth * scale, 
        frameHeight * scale 
    };
    Vector2 playerOrigin = { 0.0f, 0.0f };
    float playerSpeed = 350.0f;

    // Obstacles setup
    #define MAX_OBSTACLES 5
Obstacle obstacles[MAX_OBSTACLES];
for (int i = 0; i < MAX_OBSTACLES; i++) {
    int lane = GetRandomValue(0, NUM_LANES - 1);
    obstacles[i] = (Obstacle){
        { (float)(lane * LANE_WIDTH + LANE_WIDTH / 2.0f - 25)+150, -100.0f - (i * 250.0f), 30, 80 },
        WHITE,
        true,
        (float)GetRandomValue(3, 6),
        GetRandomValue(0, 1) // Randomly picks sprite sheet 0 or 1
    };
}

    int score = 0;
    bool gameOver = false;
    
    // Background vertical scrolling variables (using DrawTextureEx scaling approach)
    float bgScale = (float)SCREEN_WIDTH / background.width;
    float bgScaledHeight = background.height * bgScale;
    
    float bgScrollY1 = 0.0f;
    float bgScrollY2 = -bgScaledHeight;
    float scrollSpeed = 320.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!gameOver) {
            // Player steering
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerCollider.x -= playerSpeed * dt;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerCollider.x += playerSpeed * dt;

            // Keep player inside road boundaries
            if (playerCollider.x < 170) playerCollider.x = 170;
            if (playerCollider.x > SCREEN_WIDTH - playerCollider.width-170) playerCollider.x = SCREEN_WIDTH - playerCollider.width-170;
            
            // Player steering
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerDestRec.x -= playerSpeed * dt;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerDestRec.x += playerSpeed * dt;

            // Keep player inside road boundaries
            if (playerDestRec.x < 150) playerDestRec.x = 150;
            if (playerDestRec.x > SCREEN_WIDTH - playerDestRec.width-150) playerDestRec.x = SCREEN_WIDTH - playerDestRec.width-150;
            

            // Scroll background seamlessly using scaled background height
            bgScrollY1 += scrollSpeed * dt;
            bgScrollY2 += scrollSpeed * dt;
            if (bgScrollY1 >= bgScaledHeight) bgScrollY1 = bgScrollY2 - bgScaledHeight;
            if (bgScrollY2 >= bgScaledHeight) bgScrollY2 = bgScrollY1 - bgScaledHeight;

            // Update hero animation frame (Scarfy style logic)
            framesCounter++;
            if (framesCounter >= (60 / 12)) { // 12 frames per second animation rate
                framesCounter = 0;
                currentFrame++;
                if (currentFrame >= maxFrames) currentFrame = 0;
                frameRec.x = (float)(currentFrame * frameWidth);
            }

            // Update obstacles
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                obstacles[i].rect.y += obstacles[i].speed * 100.0f * dt;

                // Reset obstacle when it goes off screen
                if (obstacles[i].rect.y > SCREEN_HEIGHT) {
                    // Inside the obstacle off-screen reset check OR game restart loop:
int lane = GetRandomValue(0, NUM_LANES - 1);
obstacles[i].rect.x = (float)(lane * LANE_WIDTH + LANE_WIDTH / 2.0f - 25)+150;
obstacles[i].rect.y = -100.0f;
obstacles[i].speed = (float)GetRandomValue(3, 6);
obstacles[i].spriteType = GetRandomValue(0, 1); // Randomize variant on respawn
score += 10;
                }
                /*if(score>100)
                {
                    if(score>200)
                    {
                        if(score > 400)
                        {
                            scrollSpeed = 630.0f;
                            
                        }
                        else
                        {
                        scrollSpeed = 520.0f;
                        
                        }
                    }
                    else
                    {
                      scrollSpeed = 420.0f;
                      
                    }
                }*/

                // Collision detection
                if (CheckCollisionRecs(playerCollider, obstacles[i].rect)) {
                    gameOver = true;
                }
            }
        } else {
            // Restart game on Enter key
            if (IsKeyPressed(KEY_ENTER)) {
                gameOver = false;
                score = 0;
                playerCollider.x = (float)(LANE_WIDTH * 1 + LANE_WIDTH / 2.0f - 15.0f)+150;
                playerDestRec.x = (float)(LANE_WIDTH * 1 + LANE_WIDTH / 2.0f - (frameWidth * scale) / 2.0f)+150;
                for (int i = 0; i < MAX_OBSTACLES; i++) {
                    int lane = GetRandomValue(0, NUM_LANES - 1);
                    obstacles[i].rect.x = (float)(lane * LANE_WIDTH + LANE_WIDTH / 2.0f - 25)+150;
                    obstacles[i].rect.y = -100.0f - (i * 250.0f);
                }
            }
        }

        // Draw everything
        BeginDrawing();
        ClearBackground((Color){ 25, 8, 8, 255 });

        // --- DRAW BACKGROUND USING DrawTextureEx (Clean & Scaled) ---
        DrawTextureEx(background, (Vector2){ 0.0f, bgScrollY1 }, 0.0f, bgScale, WHITE);
        DrawTextureEx(background, (Vector2){ 0.0f, bgScrollY2 }, 0.0f, bgScale, WHITE);

        // Draw obstacles (rival traffic/crowd obstacles)
        // --- DRAW OBSTACLES (Sprite Sheets) ---
for (int i = 0; i < MAX_OBSTACLES; i++) {
    // Select texture based on spriteType
    Texture2D currentObsTex = (obstacles[i].spriteType == 0) ? obstacleTex1 : obstacleTex2;
    
    // Assuming 4 frames horizontally per obstacle sheet
    int obsMaxFrames = 4;
    float obsFrameWidth = (float)currentObsTex.width / obsMaxFrames;
    float obsFrameHeight = (float)currentObsTex.height;
    
    // Simple frame calculation or static frame 0
    Rectangle obsFrameRec = { 0.0f, 0.0f, obsFrameWidth, obsFrameHeight };
    
    // Draw the obstacle sprite using its collision rect as destination
//DrawRectangleRec(obstacles[i].rect, GOLD);
    DrawTexturePro(currentObsTex, obsFrameRec, (Rectangle){obstacles[i].rect.x-22,obstacles[i].rect.y-20,obstacles[i].rect.width+45,obstacles[i].rect.height+40}, (Vector2){0.0f, 0.0f}, 0.0f, WHITE);
}

        // --- DRAW HERO ON SCOOTY (Scarfy Style via DrawTexturePro) ---
        //DrawRectangleRec(playerCollider, GOLD);
        DrawTexturePro(scootyTex, frameRec, playerDestRec, playerOrigin, 0.0f, WHITE);
        

        // UI & HUD
        DrawText(TextFormat("SCORE: %d", score), 20, 20, 22, ORANGE);
        DrawText("THE PARADISE", SCREEN_WIDTH - 160, 20, 16, LIGHTGRAY);

        if (gameOver) {
            DrawRectangle(0, SCREEN_HEIGHT / 2 - 70, SCREEN_WIDTH, 140, Fade(BLACK, 0.85f));
            DrawText("MOB COLLISION!", SCREEN_WIDTH / 2 - MeasureText("MOB COLLISION!", 36) / 2, SCREEN_HEIGHT / 2 - 45, 36, RED);
            DrawText("Press [ENTER] to Ride Again", SCREEN_WIDTH / 2 - MeasureText("Press [ENTER] to Ride Again", 20) / 2, SCREEN_HEIGHT / 2 + 10, 20, RAYWHITE);
        }

        EndDrawing();
    }

    UnloadTexture(background);
UnloadTexture(scootyTex);
UnloadTexture(obstacleTex1);
UnloadTexture(obstacleTex2);

    CloseWindow();
    return 0;
}


/*
#include "raylib.h"
#include <stdlib.h>
#include <stdbool.h>

#define SCREEN_WIDTH 600
#define SCREEN_HEIGHT 800
#define NUM_LANES 3
#define LANE_WIDTH (SCREEN_WIDTH / NUM_LANES)

typedef struct {
    Rectangle rect;
    Color color;
    bool active;
    float speed;
} Obstacle;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "The Paradise: Scooty Ride");
    SetTargetFPS(60);

    // --- ASSET LOADING (Uncomment when you have your image files ready) ---
     Texture2D background = LoadTexture("paraBgr.png");
     Texture2D scootyTex = LoadTexture("paradCycpicpng.png");
    if (background.id == 0) {
    TraceLog(LOG_WARNING, "Texture failed to load");
    }

    // Scarfy-style animation setup for the hero on scooty
    int maxFrames = 4;                // Number of horizontal frames in your sprite sheet
    float frameWidth = (float)scootyTex.width / maxFrames;
    float frameHeight = (float)scootyTex.height;        // Height of a single frame
    Rectangle frameRec = { 0.0f, 0.0f, frameWidth, frameHeight };
    int currentFrame = 0;
    int framesCounter = 0;

    // Player setup
    Rectangle playerRect = { LANE_WIDTH * 1 + LANE_WIDTH / 2.0f - frameWidth / 2.0f, SCREEN_HEIGHT - 140, frameWidth, frameHeight };
    float playerSpeed = 350.0f;

    // Obstacles (rival riders / crowd members in the red zone)
    #define MAX_OBSTACLES 5
    Obstacle obstacles[MAX_OBSTACLES];
    for (int i = 0; i < MAX_OBSTACLES; i++) {
        int lane = GetRandomValue(0, NUM_LANES - 1);
        obstacles[i] = (Obstacle){
            { (float)(lane * LANE_WIDTH + LANE_WIDTH / 2.0f - 25), -100.0f - (i * 250.0f), 50, 80 },
            MAROON,
            true,
            (float)GetRandomValue(3, 6)
        };
    }

    int score = 0;
    bool gameOver = false;
    
    // Background vertical scrolling variables
    float bgScrollY1 = 0.0f;
    float bgScrollY2 = -SCREEN_HEIGHT;
    float scrollSpeed = 320.0f;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (!gameOver) {
            // Player steering (Left / Right arrows or A / D)
            if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) playerRect.x -= playerSpeed * dt;
            if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) playerRect.x += playerSpeed * dt;

            // Keep player inside road boundaries
            if (playerRect.x < 0) playerRect.x = 0;
            if (playerRect.x > SCREEN_WIDTH - playerRect.width) playerRect.x = SCREEN_WIDTH - playerRect.width;

            // Scroll background seamlessly
            bgScrollY1 += scrollSpeed * dt;
            bgScrollY2 += scrollSpeed * dt;
            if (bgScrollY1 >= background.height) bgScrollY1 = bgScrollY2 - background.height;
            if (bgScrollY2 >= background.height) bgScrollY2 = bgScrollY1 - background.height;

            // Update hero animation frame (Scarfy style logic)
            framesCounter++;
            if (framesCounter >= (60 / 12)){ // 12 frames per second animation rate
                framesCounter = 0;
                currentFrame++;
                if (currentFrame >= maxFrames) currentFrame = 0;
                frameRec.x = (float)(currentFrame * frameWidth);
                frameRec.width = frameWidth;
                frameRec.height = frameHeight;
            }

            // Update obstacles
            for (int i = 0; i < MAX_OBSTACLES; i++) {
                obstacles[i].rect.y += obstacles[i].speed * 100.0f * dt;

                // Reset obstacle when it goes off screen
                if (obstacles[i].rect.y > SCREEN_HEIGHT) {
                    int lane = GetRandomValue(0, NUM_LANES - 1);
                    obstacles[i].rect.x = (float)(lane * LANE_WIDTH + LANE_WIDTH / 2.0f - 25);
                    obstacles[i].rect.y = -100.0f;
                    obstacles[i].speed = (float)GetRandomValue(3, 6);
                    score += 10;
                }

                // Collision detection
                if (CheckCollisionRecs(playerRect, obstacles[i].rect)) {
                    gameOver = true;
                }
            }
        } else {
            // Restart game on Enter key
            if (IsKeyPressed(KEY_ENTER)) {
                gameOver = false;
                score = 0;
                playerRect.x = LANE_WIDTH * 1 + LANE_WIDTH / 2.0f - frameWidth / 2.0f;
                for (int i = 0; i < MAX_OBSTACLES; i++) {
                    int lane = GetRandomValue(0, NUM_LANES - 1);
                    obstacles[i].rect.x = (float)(lane * LANE_WIDTH + LANE_WIDTH / 2.0f - 25);
                    obstacles[i].rect.y = -100.0f - (i * 250.0f);
                }
            }
        }

        // Draw everything
        BeginDrawing();
        ClearBackground((Color){ 25, 8, 8, 255 }); // Deep fiery cinematic dark tone

        // --- DRAW BACKGROUND ASSET ---
         
        //Uncomment once you load texture assets:
        DrawTextureRec(background, (Rectangle){ 0, 0, (float)background.width, (float)background.height }, (Vector2){ 0, bgScrollY1 }, WHITE);
        DrawTextureRec(background, (Rectangle){ 0, 0, (float)background.width, (float)background.height }, (Vector2){ 0, bgScrollY2 }, WHITE);
        
        
        // Procedural fallback background styled like the movie's red environment:
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, (Color){ 40, 12, 12, 255 });
        for (int i = 1; i < NUM_LANES; i++) {
            DrawLineV((Vector2){ (float)(i * LANE_WIDTH), 0 }, (Vector2){ (float)(i * LANE_WIDTH), SCREEN_HEIGHT }, (Color){ 100, 25, 25, 255 });
        }
        // Animated fiery lane markers
        for (float y = -40 + (int)(bgScrollY1) % 80; y < SCREEN_HEIGHT; y += 80) {
            DrawLine((int)(SCREEN_WIDTH / 3), (int)y, (int)(SCREEN_WIDTH / 3), (int)y + 40, ORANGE);
            DrawLine((int)(SCREEN_WIDTH / 3) * 2, (int)y, (int)(SCREEN_WIDTH / 3) * 2, (int)y + 40, ORANGE);
        }

        // --- DRAW HERO ON SCOOTY (Scarfy Style) ---
        
        //Uncomment once texture sheet is loaded:
        DrawTexturePro(scootyTex, frameRec, playerRect, (Vector2){0, 0}, 0.0f, WHITE);
        
        // Fallback geometric placeholder representing the hero on scooty:
        DrawRectangleRec(playerRect, GOLD);
        DrawCircle((int)(playerRect.x + playerRect.width / 2), (int)(playerRect.y + 25), 14, (Color){ 200, 30, 30, 255 });

        // Draw obstacles (rival traffic/crowd obstacles)
        for (int i = 0; i < MAX_OBSTACLES; i++) {
            DrawRectangleRec(obstacles[i].rect, (Color){ 160, 35, 35, 255 });
        }

        // UI & HUD
        DrawText(TextFormat("SCORE: %d", score), 20, 20, 22, ORANGE);
        DrawText("THE PARADISE", SCREEN_WIDTH - 160, 20, 16, LIGHTGRAY);

        if (gameOver) {
            DrawRectangle(0, SCREEN_HEIGHT / 2 - 70, SCREEN_WIDTH, 140, Fade(BLACK, 0.85f));
            DrawText("MOB COLLISION!", SCREEN_WIDTH / 2 - MeasureText("MOB COLLISION!", 36) / 2, SCREEN_HEIGHT / 2 - 45, 36, RED);
            DrawText("Press [ENTER] to Ride Again", SCREEN_WIDTH / 2 - MeasureText("Press [ENTER] to Ride Again", 20) / 2, SCREEN_HEIGHT / 2 + 10, 20, RAYWHITE);
        }

        EndDrawing();
    }

     UnloadTexture(background);
     UnloadTexture(scootyTex);

    CloseWindow();
    return 0;
}*/