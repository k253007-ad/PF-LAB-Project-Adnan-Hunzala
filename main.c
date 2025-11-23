#include "raylib.h"
#include "files/main_menu.c"
#include "files/tilemap.c"
#include "files/coin_anim.c"
#include "files/player_movement.c"
#include "files/sound_variation.c"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

void ResetGameState(void)
{
    playerX = startX;
    playerY = startY;
    velocityX = 0;
    velocityY = 0;
    isGrounded = false;
    facingRight = true;
    currentAnim = ANIM_IDLE;
    currentFrame = 0;
    frameTimer = 0;
    canRoll = true;
    rollDuration = 0;
    coyoteTimer = 0.0f;
    coinsCollected = 0;
    
    for(int i = 0; i < coinCount; i++){
        coins[i].collected = false;
    }
}

int main(void){
    const int screenWidth = 1280;
    const int screenHeight = 720;
    
    InitWindow(screenWidth, screenHeight, "Pixel Knight - Adventure");
    SetTargetFPS(60);
    
    InitAudioDevice();
    
    Texture2D knightSprite = LoadTexture("assets/knight.png");
    Texture2D tilesetTexture = LoadTexture("assets/tileset.png");
    Texture2D coinSprite = LoadTexture("assets/coin.png");
    
    Sound coinSound = LoadSound("assets/coin.wav");
    jumpSound = LoadSound("assets/jump.wav");
    rollSound = LoadSound("assets/roll.ogg");
    
    Music mainTheme = LoadMusicStream("assets/time_for_adventure.mp3");
    PlayMusicStream(mainTheme);
    SetMusicVolume(mainTheme, 1.0f);
    
    playerX = startX;
    playerY = startY;
    
    // Tilemap
    LoadCSVToArray("files/map.csv", levelMap, &rows, &cols);
    
    for (int y = 0; y < MAP_HEIGHT; y++){
        for (int x = 0; x < MAP_WIDTH; x++){
            if(levelMap[y][x] == 213){
                if(coinCount < MAX_COINS){
                    coins[coinCount].x = x * SCALED_TILE + SCALED_TILE / 2.0f;
                    coins[coinCount].y = y * SCALED_TILE + SCALED_TILE / 2.0f;
                    coins[coinCount].collected = false;
                    coinCount++;
                }
            } else if(levelMap[y][x] != -1){
                tiles[tileCount++] = (Tile){x, y, levelMap[y][x]};
            }
        }
    }
    
    // Camera to follow
    Camera2D camera = {0};
    camera.target = (Vector2){playerX, playerY};
    camera.offset = (Vector2){screenWidth / 2.0f, screenHeight / 2.0f};
    camera.rotation = 0.0f;
    camera.zoom = 1.5f;
    
    bool gameStarted = false;
    
    while(!WindowShouldClose()) {
        
        deltaTime = GetFrameTime();
        UpdateMusicStream(mainTheme);
        
        // Checking for menu
        
        if (!MenuIsInGame()) {
            BeginDrawing();
                ClearBackground((Color){ 44, 44, 54, 255 });
                
                bool shouldRestart = false;
                bool shouldReturnToMain = false;
                
                if (MenuUpdate(screenWidth, screenHeight, &mainTheme, &shouldRestart, &shouldReturnToMain)) {
                    if (MenuGetCurrentState() == MENU_STATE_PLAYING) {
                        if (!gameStarted) {
                            ResetGameState();
                            camera.target = (Vector2){playerX, playerY};
                            gameStarted = true;
                        }
                    }
                }
                
                if (shouldRestart) {
                    ResetGameState();
                    camera.target = (Vector2){playerX, playerY};
                }
                
                if (shouldReturnToMain) {
                    gameStarted = false;
                }
                
            EndDrawing();
            continue;
            
        }
        
        if (!MenuIsPaused() && IsKeyPressed(KEY_P)) {
            MenuPause();
            
        }
        
        // Disable falling off map
        if(playerY > 4800){
            ResetGameState();
            camera.target = (Vector2){playerX, playerY};
        }
        
        PlayerMovement();
        
        frameTimer += deltaTime;
        if (frameTimer >= animations[currentAnim].frameSpeed){
            frameTimer = 0;
            currentFrame++;
            if (currentFrame >= animations[currentAnim].frameCount){
                currentFrame = 0;
            }
        }
        
        coinFrameTimer += deltaTime;
        if (coinFrameTimer >= coinFrameSpeed){
            coinFrameTimer = 0;
            coinFrame++;
            if (coinFrame >= 12){
                coinFrame = 0;
            }
        }
        
        Rectangle playerCollectRect = {
            playerX + hitboxOffsetX,
            playerY + hitboxOffsetY,
            hitboxWidth,
            hitboxHeight
        };
        
        // Check for coin collection and remove collected coins
        
        for(int i = 0; i < coinCount; i++){
            if(!coins[i].collected){
                Rectangle coinRect = {
                    coins[i].x - 24,
                    coins[i].y - 24,
                    48,
                    48
                };
                
                if(CheckCollisionRecs(playerCollectRect, coinRect)){
                    coins[i].collected = true;
                    coinsCollected++;
                    PlaySound(coinSound);
                }
            }
        }
        
        // Camera follow player
        
        camera.target.x += ((playerX + 48) - camera.target.x) * 0.1f;
        camera.target.y += (playerY - camera.target.y) * 0.1f;
        
        BeginDrawing();
            
            ClearBackground(SKYBLUE);
            
            BeginMode2D(camera);
                
                for (int i = 0; i < coinCount; i++){
                    if(!coins[i].collected){
                        Rectangle coinSourceRec = {
                            coinFrame * 16.0f,
                            0,
                            16,
                            16
                        };
                        
                        Rectangle coinDestRec = {
                            coins[i].x,
                            coins[i].y,
                            48,
                            48
                        };
                        
                        Vector2 coinOrigin = {24, 24};
                        
                        DrawTexturePro(coinSprite, coinSourceRec, coinDestRec, coinOrigin, 0, WHITE);
                    }
                }
                
                // Drawing tiles
                
                for (int i = 0; i < tileCount; i++){
                    
                    TileType tType = tileTypes[tiles[i].type];
                    int screenX = tiles[i].x * SCALED_TILE;
                    int screenY = tiles[i].y * SCALED_TILE;
                    
                    Rectangle source = {
                      tType.tileX * TILE_SIZE,
                      tType.tileY * TILE_SIZE,
                      TILE_SIZE,
                      TILE_SIZE
                    };
                    
                    Rectangle dest = {
                        screenX,
                        screenY,
                        SCALED_TILE,
                        SCALED_TILE
                    };
                    
                    DrawTexturePro(tilesetTexture, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
                }
                
                // Player Animation and Drawing on screen
                
                AnimationDef anim = animations[currentAnim];
                int row = anim.startRow + (currentFrame / anim.framesPerRow);
                int col = anim.startCol + (currentFrame % anim.framesPerRow);
                
                Rectangle source = {
                    col * FRAME_SIZE,
                    row * FRAME_SIZE,
                    FRAME_SIZE,
                    FRAME_SIZE
                }; 
                
                if(!facingRight) source.width *= -1;
                
                Rectangle dest = {playerX, playerY, 96, 96};
                DrawTexturePro(knightSprite, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
                
                // This code is here to check the hitbox
                // Rectangle hitbox = {playerX + hitboxOffsetX, playerY + hitboxOffsetY, hitboxWidth, hitboxHeight};
                // DrawRectangleLinesEx(hitbox, 2, RED);
                
            EndMode2D();
            
            //All of text on screen
            
            DrawText("WASD/Arrows: Move | SPACE: Jump | SHIFT: Air Dash\nP: Pause", 20, 20, 25, WHITE);
            
            // if ((coinsCollected%2) == 1){
                // DrawText("YOU WON", screenWidth/3, screenHeight/2.3, 100, WHITE);
            // }
            
            char coinText[32];
            sprintf(coinText, "Coins: %d / %d", coinsCollected, coinCount);
            DrawText(coinText, screenWidth - 300, 20, 30, YELLOW);
            
            char dataGet[32];
            sprintf(dataGet, "FPS : %3d | velocityY : %.2f", GetFPS(), velocityY);
            DrawText(dataGet, 20,100,25, WHITE);
            
            
        EndDrawing();
    }
    
    UnloadMusicStream(mainTheme);
    UnloadTexture(knightSprite);
    UnloadTexture(tilesetTexture);
    UnloadTexture(coinSprite);
    UnloadSound(coinSound);
    UnloadSound(jumpSound);
    UnloadSound(rollSound);
    CloseAudioDevice();
    CloseWindow();
    
    return 0;
}