#include <SDL2/SDL.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_image.h>
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

int init(void);
void update(float delta);
void render();
void updateTexture();
int intRange(int oldMin, int oldMax, int newMin, int newMax, int value);
int countNeighbours(unsigned int a[SCREEN_WIDTH][SCREEN_HEIGHT], int x, int y);
void updateLife();
