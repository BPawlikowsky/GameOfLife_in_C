#include "GameOfLife.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
char* title = "Pixel Manipulation test";
SDL_Texture* tex = NULL;
unsigned int aLife[SCREEN_WIDTH][SCREEN_HEIGHT];
unsigned int aLifeNext[SCREEN_WIDTH][SCREEN_HEIGHT];
int key = 0;

int main(void) {

  // Init Video
  init();


  if(window != NULL)
    {
      //Time tracking
      uint32_t lastTick, currentTick;
      lastTick = SDL_GetTicks();
      int frames = 0;
      double milis = 0.0f;

      SDL_bool done = SDL_FALSE;

      while(!done)
        {
          SDL_Event event;

          double delta;
          delta = (double)(currentTick - lastTick)/1000.0f;

          lastTick = currentTick;

          if(!key && (frames % 2 == 0))
            update(delta);
          render();

          //Time tracking
          currentTick = SDL_GetTicks();
          milis += delta;
          frames ++;
          if(milis >= 1.0f) {
            printf("fps: %d\n", frames);
            milis = 0.0f;
            frames = 0;
          }

          //Event polling
          while(SDL_PollEvent(&event))
            if(event.type == SDL_QUIT)
              done = SDL_TRUE;
            else if(event.type == SDL_KEYDOWN)
              key = 1;
            else if(event.type == SDL_KEYUP)
              key = 0;
        }
    }
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int init() {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Error initializing SDL! %s\n", SDL_GetError());
    return 1;
  }

  window = SDL_CreateWindow(title,
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            SCREEN_WIDTH,
                            SCREEN_HEIGHT,
                            SDL_WINDOW_FULLSCREEN_DESKTOP);
  if(!window) {
    printf("Error creating window: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  Uint32 renderFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
  renderer = SDL_CreateRenderer(window, -1, renderFlags);
  if(!renderer) {
    printf("Error creating renderer: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  tex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT);
  if(tex == NULL) {
    printf("Failed to create texture: %s\n", SDL_GetError());
  }

  //Init arrays
  for(int i = 0; i < SCREEN_WIDTH; i++) {
    for(int j = 0; j < SCREEN_HEIGHT; j++) {
      aLife[i][j] = ((rand() % 3) == 0) ? 1 : 0;
      aLifeNext[i][j] = 0;
    }
  }

  return 0;
}

void update(float delta) {
  updateLife();
  updateTexture();
}

void updateLife() {
  for(int x = 1; x < SCREEN_WIDTH-1; x++) {
    for(int y = 1; y < SCREEN_HEIGHT-1; y++) {
      if(aLife[x][y]) {
        int neighbours = countNeighbours(aLife, x, y);
        neighbours--;
        if(neighbours < 2 || neighbours > 3)
          aLifeNext[x][y] = 0;
        else
          aLifeNext[x][y] = 1;
      } else {
        int neighbours = countNeighbours(aLife, x, y);
        if(neighbours == 3)
          aLifeNext[x][y] = 1;
        else
          aLifeNext[x][y] = 0;
      }
    }
  }

  for(int x = 1; x < SCREEN_WIDTH-1; x++)
    for(int y = 1; y < SCREEN_HEIGHT-1; y++)
      aLife[x][y] = aLifeNext[x][y];
}

void render() {
  //Clear screen
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
  //SDL_RenderClear(renderer);
  SDL_RenderCopy(renderer, tex, NULL, NULL);

  SDL_RenderPresent(renderer);
}

void updateTexture() {
  SDL_Color color;
  SDL_Color white = {255, 255, 255, 255};
  SDL_Color black = {0, 0, 0, 255};
  void* pixels;
  int pitch;
  if( SDL_LockTexture(tex, NULL, &pixels, &pitch) == 0) {
    for(int i = 0; i < SCREEN_HEIGHT; i++) {
      Uint32* dst = (Uint32*)((Uint8*)pixels + i*pitch);
      for(int j = 0; j < SCREEN_WIDTH; j++){
        if(aLife[j][i] == 0)
          color = black;
        else
          color = white;
        *dst++ =  ((color.r<<16) | (color.g<<8) | (color.b))<<8 | (color.a);
      }
    }
    SDL_UnlockTexture(tex);
  }
  else
    printf("Failed to lock texture: %s\n", SDL_GetError());
}


int countNeighbours(unsigned int a[SCREEN_WIDTH][SCREEN_HEIGHT], int x, int y) {
  int count = 0;
  for(int i = 0; i < 3; i++) {
    for(int j = 0; j < 3; j++) {
      if(a[x + intRange(0, 2, -1, 1, i)][y + intRange(0, 2, -1, 1, j)])
        count++;
    }
  }
  return count;
}

int intRange(int oldMin, int oldMax, int newMin, int newMax, int value) {
  int oldRange = oldMax - oldMin;
  int newRange = newMax - newMin;
  return (((value-oldMin)*newRange)/oldRange) + newMin;
}
