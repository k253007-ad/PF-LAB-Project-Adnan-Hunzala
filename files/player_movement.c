#include "raylib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>

#define MOVE_SPEED 360.0f
#define ACCELERATION 1500.0f

#define GRAVITY 2000.0f
#define JUMP_FORCE -2669.0f
#define MAX_FALL_SPEED 900.0f

#define COYOTE_TIME 0.1f
#define JUMP_BUFFER_TIME 0.1f
#define JUMP_HOLD_TIME 0.22f

#define TILE_SIZE 16
#define SCALE 3.0f
#define SCALED_TILE (TILE_SIZE * SCALE)

#define MAP_WIDTH 200
#define MAP_HEIGHT 100
#define MAX_TILES 20000

#define MAX_COINS 500

float startX = 200;
float startY = 4300;
float playerX;
float playerY;

float velocityX = 0;
float velocityY = 0;
bool isGrounded = false;
bool facingRight = true;

AnimState currentAnim = ANIM_IDLE;
int currentFrame = 0;
float frameTimer = 0;
bool canRoll = true;
float rollDuration = 0;

float coyoteTimer = 0.0f;
float jumpBufferTimer = 0.0f;
float jumpHoldTimer = 0.0f;
bool jumping = false;
float deltaTime;

Sound jumpSound;
Sound rollSound;

float hitboxWidth = 29;
float hitboxHeight = 43;
float hitboxOffsetX = 34;
float hitboxOffsetY = 40;

int coinFrame = 0;
float coinFrameTimer = 0;
float coinFrameSpeed = 0.08f;

Coin coins[MAX_COINS];
int coinCount = 0;
int coinsCollected = 0;

Tile tiles[MAX_TILES];
int tileCount = 0;

bool CheckTileCollision(float*playerX, float*playerY, float*velocityX, float*velocityY, Tile tiles[],int tileCount, bool *isGrounded, float hitboxWidth, float hitboxHeight, float hitboxOffsetX, float hitboxOffsetY){
    Rectangle playerRect = {*playerX + hitboxOffsetX, *playerY + hitboxOffsetY, hitboxWidth, hitboxHeight};
    *isGrounded = false;
    for (int i = 0; i < tileCount; i++){
        
        if(!tileTypes[tiles[i].type].solid) continue;
        
        Rectangle tileRect = {
          tiles[i].x * SCALED_TILE,
          tiles[i].y * SCALED_TILE,
          SCALED_TILE,
          SCALED_TILE
        };
        
        if (CheckCollisionRecs(playerRect,tileRect)){
            float overlapX = fminf(playerRect.x + playerRect.width,tileRect.x + tileRect.width) - fmaxf(playerRect.x,tileRect.x);
            float overlapY = fminf(playerRect.y + playerRect.height,tileRect.y + tileRect.height) - fmaxf(playerRect.y,tileRect.y);
            
            if (overlapX < overlapY){
                if (playerRect.x < tileRect.x){
                    *playerX = tileRect.x - hitboxWidth - hitboxOffsetX;
                } else {
                    *playerX = tileRect.x + SCALED_TILE - hitboxOffsetX;
                }
                *velocityX = 0;
            } else {
                if (playerRect.y < tileRect.y){
                    if(*velocityY >= 0){
                        *playerY = tileRect.y - hitboxHeight - hitboxOffsetY;
                        *velocityY = 0;
                        *isGrounded = true;
                    }
                } else {
                    *playerY = tileRect.y + SCALED_TILE - hitboxOffsetY;
                    *velocityY = 0;
                }
            }
            
            playerRect.x = *playerX + hitboxOffsetX;
            playerRect.y = *playerY + hitboxOffsetY;
        }
    }
    
    return *isGrounded;
}

void PlayerMovement(void){
    
    if(currentAnim == ANIM_ROLL){
        rollDuration -= deltaTime;
        if(rollDuration <= 0){
            currentAnim = ANIM_IDLE;
            currentFrame = 0;
            velocityX = 0;
        }
    }
    
    if(currentAnim != ANIM_ROLL){
        bool moving = false;
        
        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)){
            if(velocityX < 0) velocityX *= -1;
            if(velocityX < MOVE_SPEED)velocityX += ACCELERATION * deltaTime;
            else velocityX = MOVE_SPEED;
            facingRight = true;
            moving = true;
        }
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)){
            if(velocityX > 0) velocityX *= -1;
            if(velocityX > -MOVE_SPEED)velocityX -= ACCELERATION * deltaTime;
            else velocityX = -MOVE_SPEED;
            facingRight = false;
            moving = true;
        }
        
        if(!moving) velocityX = 0;
        
        if (moving && isGrounded){
            if(currentAnim != ANIM_RUN){
                currentAnim = ANIM_RUN;
                currentFrame = 0;
                frameTimer = 0;
            }
        } else if (isGrounded){
            if(currentAnim != ANIM_IDLE){
                currentAnim = ANIM_IDLE;
                currentFrame = 0;
                frameTimer = 0;
            }
        }
            
        if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W))){
            jumpBufferTimer = JUMP_BUFFER_TIME;
        }
        
        if (jumpBufferTimer > 0 && isGrounded){
            jumpHoldTimer = JUMP_HOLD_TIME;
            velocityY = 0;
            isGrounded = false;
            coyoteTimer = 0;
            jumpBufferTimer = 0;
            PlaySound(jumpSound);
        }
        
        if (jumpHoldTimer > 0){
            
            velocityY += (JUMP_FORCE-velocityY) * deltaTime;
            
            // if (velocityY > JUMP_FORCE_CAP{600}) velocityY += JUMP_FORCE{2469} * deltaTime;
            // else velocityY = JUMP_FORCE_CAP{600};
            jumpHoldTimer -= deltaTime;
            if (!(IsKeyDown(KEY_UP) || IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W))){
                jumpHoldTimer = 0;
            }
        } else { 
            velocityY += GRAVITY * deltaTime;
        }
        
        if (IsKeyPressed(KEY_LEFT_SHIFT) && canRoll){
            currentAnim = ANIM_ROLL;
            currentFrame = 0;
            frameTimer = 0;
            canRoll = false;
            rollDuration = 0.45f;
            velocityX = facingRight ? MOVE_SPEED * 2.0f : -MOVE_SPEED * 2.0f;
            PlaySound(rollSound);
        }
    }
    
    if (currentAnim == ANIM_ROLL){
        velocityY = 0; 
    }

    if(velocityY > MAX_FALL_SPEED){
        velocityY = MAX_FALL_SPEED;
    }
    
    playerX += velocityX * deltaTime;
    playerY += velocityY * deltaTime;
    bool onGround = CheckTileCollision(&playerX, &playerY, &velocityX, &velocityY, tiles, tileCount, &isGrounded, hitboxWidth, hitboxHeight, hitboxOffsetX, hitboxOffsetY);
    
    if (onGround) {
        isGrounded = true;
        coyoteTimer = COYOTE_TIME;
        canRoll = true;
        velocityY = 0;
        playerY -= 0.5;
    } else {
        if (coyoteTimer > 0){
            coyoteTimer -= deltaTime;
            isGrounded = true;
        } else {
            isGrounded = false;
        }
    }
    
    if(jumpBufferTimer > 0){
        jumpBufferTimer -= deltaTime;
    }

}