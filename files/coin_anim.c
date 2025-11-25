#define FRAME_SIZE 32

typedef struct {
    float x, y;
    bool collected;
} Coin;

typedef enum {
    ANIM_IDLE,
    ANIM_RUN,
    ANIM_ROLL,
    ANIM_COIN
} AnimState;

typedef struct {
    int frameCount;
    int startRow;
    int startCol;
    int framesPerRow;
    float frameSpeed;
} AnimationDef;

AnimationDef animations[] = {
    [ANIM_IDLE] = {4, 0, 0, 8, 0.15f},
    [ANIM_RUN] = {16, 2, 3, 16, 0.08f},
    [ANIM_ROLL] = {8, 5, 5, 8, 0.15f},
    [ANIM_COIN] = {12, 0, 0, 12, 0.15f}
};