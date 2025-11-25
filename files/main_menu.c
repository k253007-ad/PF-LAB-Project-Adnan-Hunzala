#ifndef MENU_SYSTEM_C
#define MENU_SYSTEM_C

// I copied this online and altered it because, I tought it would make game more fun.
// I can remove this file if its not appropriate.

#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define MENU_BUTTON_WIDTH 300
#define MENU_BUTTON_HEIGHT 50
#define MENU_BUTTON_SPACING 10
#define MENU_ANIMATION_SPEED 0.15f

typedef enum {
    MENU_STATE_MAIN = 0,
    MENU_STATE_PLAYING,
    MENU_STATE_PAUSED,
    MENU_STATE_OPTIONS,
    MENU_STATE_EXIT_CONFIRM
} MenuState;

typedef struct {
    Rectangle bounds;
    const char* text;
    Color baseColor;
    Color hoverColor;
    Color textColor;
    float hoverProgress;
    bool isHovered;
} MenuButton;

static MenuState currentMenuState = MENU_STATE_MAIN;
static MenuState previousMenuState = MENU_STATE_MAIN;
static float menuTransitionAlpha = 0.0f;
static bool menuIsTransitioning = false;
static Vector2 menuMousePoint = { 0.0f, 0.0f };
static float menuMusicVolume = 1.0f;

MenuButton MenuCreateButton(float x, float y, const char* text)
{
    MenuButton button = {
        .bounds = { x, y, MENU_BUTTON_WIDTH, MENU_BUTTON_HEIGHT },
        .text = text,
        .baseColor = (Color){ 70, 70, 80, 255 },
        .hoverColor = (Color){ 100, 100, 120, 255 },
        .textColor = WHITE,
        .hoverProgress = 0.0f,
        .isHovered = false
    };
    return button;
}

void MenuUpdateButton(MenuButton* button, Vector2 mousePos)
{
    button->isHovered = CheckCollisionPointRec(mousePos, button->bounds);
    
    if (button->isHovered) {
        button->hoverProgress += MENU_ANIMATION_SPEED;
        if (button->hoverProgress > 1.0f) button->hoverProgress = 1.0f;
    } else {
        button->hoverProgress -= MENU_ANIMATION_SPEED;
        if (button->hoverProgress < 0.0f) button->hoverProgress = 0.0f;
    }
}

void MenuDrawButton(MenuButton button)
{
    Color currentColor = {
        (unsigned char)(button.baseColor.r + (button.hoverColor.r - button.baseColor.r) * button.hoverProgress),
        (unsigned char)(button.baseColor.g + (button.hoverColor.g - button.baseColor.g) * button.hoverProgress),
        (unsigned char)(button.baseColor.b + (button.hoverColor.b - button.baseColor.b) * button.hoverProgress),
        255
    };
    
    DrawRectangleRounded(button.bounds, 0.15f, 8, currentColor);
    DrawRectangleRoundedLines(button.bounds, 0.15f, 8,
        button.isHovered ? WHITE : (Color){ 100, 100, 110, 255 });
    
    int textWidth = MeasureText(button.text, 20);
    int textX = button.bounds.x + (button.bounds.width - textWidth) / 2;
    int textY = button.bounds.y + (button.bounds.height - 20) / 2;
    
    if (button.isHovered) {
        textY -= 1;
    }
    
    DrawText(button.text, textX, textY, 20, button.textColor);
}

bool MenuIsButtonClicked(MenuButton button)
{
    return button.isHovered && IsMouseButtonPressed(MOUSE_LEFT_BUTTON);
}

void MenuChangeState(MenuState newState)
{
    previousMenuState = currentMenuState;
    currentMenuState = newState;
    menuIsTransitioning = true;
    menuTransitionAlpha = 0.0f;
}

void MenuUpdateTransition(void)
{
    if (menuIsTransitioning) {
        menuTransitionAlpha += MENU_ANIMATION_SPEED * 2;
        if (menuTransitionAlpha >= 1.0f) {
            menuTransitionAlpha = 1.0f;
            menuIsTransitioning = false;
        }
    }
}

MenuState MenuGetCurrentState(void)
{
    return currentMenuState;
}

Texture2D menuSprite;
bool menuSpriteInit = false;
int currentFrameM = 0;
int totalFramesM = 4;
float frameTimeM = 0.0f;
float frameDelayM = 0.15f;

int frameSquareM = 32;
bool MenuDrawMainMenu(int screenWidth, int screenHeight)
{
    static MenuButton startButton;
    static MenuButton optionsButton;
    static MenuButton exitButton;
    static bool initialized = false;
    
    if (menuSpriteInit == false) {
        menuSprite = LoadTexture("assets/menuSprite.png");
        menuSpriteInit = true;
    }
    
    if (!initialized) {
        float centerX = (screenWidth - MENU_BUTTON_WIDTH) / 2;
        float startY = screenHeight / 2 + 30;
        
        startButton = MenuCreateButton(centerX, startY, "Start Game");
        optionsButton = MenuCreateButton(centerX, startY + MENU_BUTTON_HEIGHT + MENU_BUTTON_SPACING, "Options");
        exitButton = MenuCreateButton(centerX, startY + (MENU_BUTTON_HEIGHT + MENU_BUTTON_SPACING) * 2, "Exit");
        
        initialized = true;
    }
    
    MenuUpdateButton(&startButton, menuMousePoint);
    MenuUpdateButton(&optionsButton, menuMousePoint);
    MenuUpdateButton(&exitButton, menuMousePoint);
    
    if (MenuIsButtonClicked(startButton)) {
        MenuChangeState(MENU_STATE_PLAYING);
        return true; // Signal to start game
    }
    if (MenuIsButtonClicked(optionsButton)) {
        MenuChangeState(MENU_STATE_OPTIONS);
    }
    if (MenuIsButtonClicked(exitButton)) {
        MenuChangeState(MENU_STATE_EXIT_CONFIRM);
    }
    
    const char* title = "PIXEL KNIGHT";
    int titleFontSize = 70;
    int titleWidth = MeasureText(title, titleFontSize);
    int titleX = (screenWidth - titleWidth) / 2;
    int titleY = screenHeight / 4 - 40;
    
    DrawText(title, titleX + 3, titleY + 3, titleFontSize, (Color){ 0, 0, 0, 100 });
    DrawText(title, titleX, titleY, titleFontSize, (Color){ 230, 230, 255, 255 });
    
    const char* subtitle = "A Knight's Journey By Adnan and Hunzala";
    int subtitleWidth = MeasureText(subtitle, 20);
    DrawText(subtitle, (screenWidth - subtitleWidth) / 2, titleY + 80, 20, (Color){ 180, 180, 200, 255 });
    
    MenuDrawButton(startButton);
    MenuDrawButton(optionsButton);
    MenuDrawButton(exitButton);
    
    DrawText("v1. 8. 0", 10, screenHeight - 40, 30, (Color){ 120, 120, 140, 255 });
    
    // MENU KNIGHT
        frameTimeM += GetFrameTime();
        if (frameTimeM >= frameDelayM){
            frameTimeM = 0.0f;
            currentFrameM++;
            if (currentFrameM >= totalFramesM){
                currentFrameM = 0;
            }
        }
         Rectangle source = {
            currentFrameM * frameSquareM,
            0,
            frameSquareM,
            frameSquareM
        };
        
        Rectangle dest = {
            200,
            300,
            frameSquareM * 10,
            frameSquareM * 10
        };
        
        DrawTexturePro(menuSprite, source, dest, (Vector2){0, 0}, 0.0f, WHITE);
        
    return false;
}

void MenuDrawPauseMenu(int screenWidth, int screenHeight, bool* shouldRestart, bool* shouldReturnToMain)
{
    static MenuButton resumeButton;
    static MenuButton restartButton;
    static MenuButton mainMenuButton;
    static bool initialized = false;
    
    if (!initialized) {
        float centerX = (screenWidth - MENU_BUTTON_WIDTH) / 2;
        float startY = screenHeight / 2 - 20;
        
        resumeButton = MenuCreateButton(centerX, startY, "Resume");
        restartButton = MenuCreateButton(centerX, startY + MENU_BUTTON_HEIGHT + MENU_BUTTON_SPACING, "Restart Level");
        mainMenuButton = MenuCreateButton(centerX, startY + (MENU_BUTTON_HEIGHT + MENU_BUTTON_SPACING) * 2, "Main Menu");
        
        initialized = true;
    }
    
    // Draw semi-transparent overlay
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){ 0, 0, 0, 180 });
    
    MenuUpdateButton(&resumeButton, menuMousePoint);
    MenuUpdateButton(&restartButton, menuMousePoint);
    MenuUpdateButton(&mainMenuButton, menuMousePoint);
    
    // Handle button clicks
    if (MenuIsButtonClicked(restartButton)) {
        *shouldRestart = true;
        MenuChangeState(MENU_STATE_PLAYING);
    }
    else if (MenuIsButtonClicked(mainMenuButton)) {
        *shouldReturnToMain = true;
        MenuChangeState(MENU_STATE_MAIN);
    }
    else if (MenuIsButtonClicked(resumeButton) || IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_P)) {
        // Just resume - don't set any flags
        MenuChangeState(MENU_STATE_PLAYING);
    }
    
    const char* title = "PAUSED";
    int titleWidth = MeasureText(title, 60);
    DrawText(title, (screenWidth - titleWidth) / 2, 120, 60, WHITE);
    
    MenuDrawButton(resumeButton);
    MenuDrawButton(restartButton);
    MenuDrawButton(mainMenuButton);
}

void MenuDrawOptionsMenu(int screenWidth, int screenHeight, Music* mainTheme)
{
    static MenuButton backButton;
    static MenuButton volumeUpButton;
    static MenuButton volumeDownButton;
    static bool initialized = false;
    
    if (!initialized) {
        backButton = MenuCreateButton((screenWidth - MENU_BUTTON_WIDTH) / 2, screenHeight - 100, "Back");
        volumeUpButton = MenuCreateButton(500, 200, "+");
        volumeDownButton = MenuCreateButton(400, 200, "-");
        volumeUpButton.bounds.width = 60;
        volumeDownButton.bounds.width = 60;
        initialized = true;
    }
    
    MenuUpdateButton(&backButton, menuMousePoint);
    MenuUpdateButton(&volumeUpButton, menuMousePoint);
    MenuUpdateButton(&volumeDownButton, menuMousePoint);
    
    if (MenuIsButtonClicked(backButton) || IsKeyPressed(KEY_ESCAPE)) {
        MenuChangeState(MENU_STATE_MAIN);
    }
    
    if (MenuIsButtonClicked(volumeUpButton)) {
        menuMusicVolume += 0.1f;
        if (menuMusicVolume > 1.0f) menuMusicVolume = 1.0f;
        if (mainTheme) SetMusicVolume(*mainTheme, menuMusicVolume);
    }
    if (MenuIsButtonClicked(volumeDownButton)) {
        menuMusicVolume -= 0.1f;
        if (menuMusicVolume < 0.0f) menuMusicVolume = 0.0f;
        if (mainTheme) SetMusicVolume(*mainTheme, menuMusicVolume);
    }
    
    const char* title = "OPTIONS";
    int titleWidth = MeasureText(title, 50);
    DrawText(title, (screenWidth - titleWidth) / 2, 80, 50, WHITE);
    
    char volumeText[64];
    sprintf(volumeText, "Music Volume: %d%%", (int)(menuMusicVolume * 100));
    DrawText(volumeText, 200, 210, 20, (Color){ 200, 200, 210, 255 });
    
    MenuDrawButton(volumeDownButton);
    MenuDrawButton(volumeUpButton);
    
    DrawText("Nothing to see here", 200, 280, 20, (Color){ 200, 200, 210, 255 });
    DrawText("More options are under construction", 200, 320, 20, (Color){ 200, 200, 210, 255 });
    
    MenuDrawButton(backButton);
}

bool MenuDrawExitConfirmation(int screenWidth, int screenHeight)
{
    static MenuButton yesButton;
    static MenuButton noButton;
    static bool initialized = false;
    
    if (!initialized) {
        float centerX = (screenWidth - MENU_BUTTON_WIDTH) / 2;
        yesButton = MenuCreateButton(centerX, screenHeight / 2 + 20, "Yes, You Sure");
        noButton = MenuCreateButton(centerX, screenHeight / 2 + 80, "No, We Cool");
        yesButton.hoverColor = (Color){ 180, 60, 60, 255 };
        initialized = true;
    }
    
    DrawRectangle(0, 0, screenWidth, screenHeight, (Color){ 0, 0, 0, 200 });
    
    MenuUpdateButton(&yesButton, menuMousePoint);
    MenuUpdateButton(&noButton, menuMousePoint);
    
    if (MenuIsButtonClicked(yesButton)) {
        return true; // Signal to exit
    }
    if (MenuIsButtonClicked(noButton) || IsKeyPressed(KEY_ESCAPE)) {
        MenuChangeState(MENU_STATE_MAIN);
    }
    
    const char* text = "Are you sure you want to exit?\nHave you completed the game?";
    int textWidth = MeasureText(text, 24);
    DrawText(text, (screenWidth - textWidth) / 2, screenHeight / 2 - 40, 24, WHITE);
    
    MenuDrawButton(yesButton);
    MenuDrawButton(noButton);
    
    return false;
}

bool MenuUpdate(int screenWidth, int screenHeight, Music* mainTheme, bool* shouldRestart, bool* shouldReturnToMain)
{
    menuMousePoint = GetMousePosition();
    MenuUpdateTransition();
    
    bool actionResult = false;
    
    switch (currentMenuState)
    {
        case MENU_STATE_MAIN:
            actionResult = MenuDrawMainMenu(screenWidth, screenHeight);
            break;
            
        case MENU_STATE_PAUSED:
        {
            MenuDrawPauseMenu(screenWidth, screenHeight, shouldRestart, shouldReturnToMain);
            break;
        }
            
        case MENU_STATE_OPTIONS:
            MenuDrawOptionsMenu(screenWidth, screenHeight, mainTheme);
            break;
            
        case MENU_STATE_EXIT_CONFIRM:
            if (MenuDrawExitConfirmation(screenWidth, screenHeight)) {
                return true; // Exit game
            }
            break;
            
        default:
            break;
    }
    
    if (menuIsTransitioning) {
        int alpha = (int)((1.0f - menuTransitionAlpha) * 255);
        DrawRectangle(0, 0, screenWidth, screenHeight, (Color){ 0, 0, 0, alpha });
    }
    
    return actionResult;
}

bool MenuIsInGame(void)
{
    return currentMenuState == MENU_STATE_PLAYING;
}

bool MenuIsPaused(void)
{
    return currentMenuState == MENU_STATE_PAUSED;
}

void MenuPause(void)
{
    if (currentMenuState == MENU_STATE_PLAYING) {
        MenuChangeState(MENU_STATE_PAUSED);
    }
}

void MenuResume(void)
{
    if (currentMenuState == MENU_STATE_PAUSED) {
        MenuChangeState(MENU_STATE_PLAYING);
    }
}

#endif // MENU_SYSTEM_C