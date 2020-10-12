/* 
   ========================================================================\n"
    File: dinorun.c"
    Date: 2/10/19"
    Creator: Asaad Noman Abbasi $\n"
    Notice: (C) Copyright 2019 by Asaad, Inc. All Rights Reserved. $\n"
    ======================================================================== 
*/

#include "dinorun.h"

/* ############################## UTITLITES ################################### */

void bubbleSort(int arr[], int n) 
{ 
   int i, j; 
   int swapped; 
   for (i = 0; i < n-1; i++) 
   { 
     swapped = false; 
     for (j = 0; j < n-i-1; j++) 
     { 
        if (arr[j] > arr[j+1]) 
        { 
           int t = arr[j];
           arr[j] = arr[j + 1];
           arr[j + 1] = t; 
           swapped = true; 
        } 
     } 

     if (!swapped) 
        break; 
   } 
} 

//Generates A Random Number within a given range
int generateNumber(int low,int high,int count)
{
  for(int i = 0; i < count; i++) {
    int randomNumber;
    randomNumber = (rand() % (high - low + 1)) + low;
    return randomNumber;
  }
}

//Creates Texture
SDL_Texture* CreateTexture(char surfacePath[])
{
   SDL_Texture *texture;
   SDL_Surface *surface = IMG_Load(surfacePath);
   if(surface == NULL) printf("error creating texture: %s",SDL_GetError());
   texture = SDL_CreateTextureFromSurface(_renderer,surface);
   SDL_FreeSurface(surface);
   return texture;
}

/* ############################## FILING ################################### */

int FileAlreadyExist(char* fileName)
{
  if( access(fileName, F_OK ) != -1 ) {
    return true;
  } else {
    return false;
  }
}

void CreateFile(FILE* file)
{
  if(FileAlreadyExist("highscores.txt")) {
    printf("(LOG) Highscore list already created\n");
  } else {
    printf("(LOG) Created new Highscore list\n");
    file = fopen("highscores.txt","a");
    fclose(file);
  }
}

int ReadAndReturnHighscore(FILE* file)
{
  //Reading the file;
  file = fopen ("highscores.txt", "r");
  int score = 0;
  static int scores[100];
  fscanf (file, "%d", &score);
  int counter = 0;    
  while (!feof (file)) {  
    //printf ("%d // %d, ", score,counter);
    scores[counter] = score; counter++;
    fscanf (file, "%d", &score);      
  }  fclose (file);

  bubbleSort(scores,counter);
  return scores[counter - 1];

}

void WriteToFile(FILE* file , int score)
{
   file = fopen("highscores.txt", "a");
   if(file == NULL)
   {
      printf("(LOG) Highscore list could not be opened\n");
      exit(1);
   }
   
   fprintf(file,"%d\n", score);
   fclose(file);
}


/* ################################## FONTS ############################## */

TTF_Font *ttf_font;
SDL_Color Black = {0,0,0};
void CreateFont(struct Font* font , int x , int y , int w , int h , const char text[])
{
  ttf_font = TTF_OpenFont("res/font.ttf",24);
  if(font == NULL) printf("error font: %s",TTF_GetError());
  font->surface = TTF_RenderText_Solid(ttf_font,text,Black);
  font->texture = SDL_CreateTextureFromSurface(_renderer,font->surface);
  font->rect.x = x; font->rect.y = y; font->rect.w = w; font->rect.h = h;
  SDL_FreeSurface(font->surface);
}

void UpdateFont(struct Font* font , const char text[])
{
  font->surface = TTF_RenderText_Solid(ttf_font,text,Black);
  font->texture = SDL_CreateTextureFromSurface(_renderer,font->surface);
  SDL_FreeSurface(font->surface);
}

void RenderFont(struct Font *font) 
{ 
  SDL_RenderCopy(_renderer,font->texture,NULL,&font->rect);
}

/* ################################# BIRD ################################ */

//Creates the Bird Entity
int birdSinglePixel;
void CreateBird(struct Bird* b , float x)
{
    static int tw,th;
    SDL_QueryTexture(_birdTexture,NULL,NULL,&tw,&th);

    b->srcRect.x = (tw / 2); b->srcRect.y = 0;
    b->srcRect.w  = (tw / 2); b->srcRect.h = th;    
    b->destRect.x = x; b->destRect.y = 255;
    b->destRect.w = tw / 2; b->destRect.h = th;
    
    birdSinglePixel = (tw/2) + 1;

    b->debugRect.x = b->destRect.x; b->debugRect.y = b->destRect.y; //debug
    b->debugRect.w = 43; b->debugRect.h = 40; //debug
}

//Renders The Bird
void RenderBird(struct Bird* b) 
{ 
  SDL_RenderCopy(_renderer,_birdTexture,&b->srcRect,&b->destRect);
  if(_debugMode) SDL_RenderDrawRect(_renderer,&b->debugRect); //debug
}

//Updates The Bird
void UpdateBird(struct Player *player,struct Bird* b)
{
   //Updating the bird animation and moving it
   b->srcRect.x = birdSinglePixel * _bendingDinoAnimationDelta; 
   b->destRect.x -= _gameSpeed;
   b->debugRect.x -= _gameSpeed; //debug
   if(_collisionMode) CollisionPlayer(player,&b->debugRect);
}

/* ################################# CACTUS ################################ */

//@BUG: 
//I just gotta make sure that on first try the counter is always onne
//we can use static counter to keep the value of the variable during the lifetime
//like we can use static to keep track of how many times a function has been called
//during its lifetime
void CreateCactus(struct Cactus* cactus , float x,int cactusTypeFlag)
{

   //Creates Cactus with different types
   static int ccCounter = 0;
   //Generate Random number for Large(0) or Small Cactus(1)
   //srand(time(0));
  
   //If SMALL OR LARGE CACTUS
   int tw,th;
   if(cactusTypeFlag == 1) {
     static int n = 0; //static is to keep its value during the lifetime of this program/function
     //srand(time(0));
     int cactusType = generateNumber(1,3,1);
    
     int debugRectWidth = 22;

     //this will make sure each time the cactus always starts with 1
     //becuase i was encountering some weird bug that it would not display
     //the right cactus whenever it did not start with a 1.    
     if(ccCounter == 0) {
       cactusType = 1;
       n = 6;

       //debug change in here
       cactus->debugRect.w = debugRectWidth;
       x -= 70;
     } 

     //Checking random cactusType generated by the number generator
     if(cactusType == 2) { n = 3; debugRectWidth *= 2; debugRectWidth -= 8; x -= 90;}
     if(cactusType == 3) { n = 2; debugRectWidth *= 3; debugRectWidth -= 10; x -= 100;}
     
     SDL_QueryTexture(_cactusTexture,NULL,NULL,&tw,&th);
     
     //Setting up the cactus according to the cactusType
     cactus->srcRect.x = (tw / n); cactus->srcRect.y = 0;
     cactus->srcRect.w  = (tw / n); cactus->srcRect.h = th;
     cactus->destRect.x = x; cactus->destRect.y = 391;
     cactus->destRect.w = tw / n ; cactus->destRect.h = th;
     
     //This is for one cactus
     cactus->debugRect.x = cactus->destRect.x - 2; //debug
     cactus->debugRect.y = cactus->destRect.y; //debug
     cactus->debugRect.w = debugRectWidth; cactus->debugRect.h = 35; //debug
  } 

  //If the cactus type is The LARGE Cactus
  else if(cactusTypeFlag == 0) {

     //Setting up the large cactus
     SDL_QueryTexture(_largeCactusTexture,NULL,NULL,&tw,&th);
     cactus->largeRect.w = tw; cactus->largeRect.h = th;     
     cactus->largeRect.x = x; cactus->largeRect.y = 378;

     //Big cactus Debug Rect;
     cactus->largeDebugRect.x = cactus->largeRect.x - 2;
     cactus->largeDebugRect.y = cactus->largeRect.y;
     cactus->largeDebugRect.w = 48; cactus->largeDebugRect.h = th;
  }
}

//Updates the cactus
void UpdateCactus(struct Cactus *cactus , struct Player *player,int cactusTypeFlag)
{
   //If small cactus
   if(cactusTypeFlag == 1) {
     cactus->destRect.x  -= _gameSpeed;
     cactus->debugRect.x -= _gameSpeed; //debug
     if(_collisionMode) CollisionPlayer(player,&cactus->debugRect);
  } 

  //if large
  else {
     cactus->largeRect.x -= _gameSpeed;
     cactus->largeDebugRect.x -= _gameSpeed;
     if(_collisionMode) CollisionPlayer(player,&cactus->largeDebugRect);
  }
}

//Renders The Cactus
void RenderCactus(struct Cactus *cactus , int cactusTypeFlag)
{
   //if cactus type flag is small 
   if(cactusTypeFlag == 1) {
     SDL_RenderCopy(_renderer,_cactusTexture,&cactus->srcRect,&cactus->destRect);
     if(_debugMode) SDL_RenderDrawRect(_renderer,&cactus->debugRect); //debug
  } 

  //else if cactus type flag is large
  else {
     SDL_RenderCopy(_renderer,_largeCactusTexture,NULL,&cactus->largeRect);
     if(_debugMode) SDL_RenderDrawRect(_renderer,&cactus->largeDebugRect); //debug
  }
}


/* ################################### PLAYER ############################# */

//Creates the player entity
const int _playerDebugRectHorizontalPaddingY = 20;
const int _playerDebugRectVerticalPaddingY = 5;
void CreatePlayer(struct Player* player)
{
    int numberOfSprites = 5;  
    static int tw,th;
    SDL_QueryTexture(_playerTexture,NULL,NULL,&tw,&th);

    //Setting up the player 
    player->srcRect.x = 0; player->srcRect.y = 0;
    player->srcRect.w  = tw / numberOfSprites; player->srcRect.h = th;
    player->destRect.x = 435; player->destRect.y = 375;
    player->destRect.w = tw / numberOfSprites; player->destRect.h = th;
    player->velocity = 0; player->gravity = 1;
    
    //Setting up Collision Rectangles For Pixel Perfect Collision
    {
       //Big
       player->debugRect_one.x = player->destRect.x; player->debugRect_one.y = player->destRect.y; //debug
       player->debugRect_one.w = 48; player->debugRect_one.h = 50; //debug

       //Vertical
       const int paddingX = 18; const int paddingY = 5;
       player->debugRect_two.x = player->destRect.x + paddingX; player->debugRect_two.y = player->destRect.y + _playerDebugRectVerticalPaddingY; //debug
       player->debugRect_two.w = 10; player->debugRect_two.h = 35; //debug

       //Horizontal
       player->debugRect_three.x = player->destRect.x; player->debugRect_three.y = player->destRect.y + _playerDebugRectHorizontalPaddingY; //debug
       player->debugRect_three.w = 40; player->debugRect_three.h = 10; //debug
    }

    //Setting up the bending dino
    numberOfSprites = 2;
    SDL_QueryTexture(_playerBendingTexture,NULL,NULL,&tw,&th);
    player->bendingSrcRect.x = 0; player->bendingSrcRect.y = 0;
    player->bendingSrcRect.w  = tw / numberOfSprites; player->bendingSrcRect.h = th;
    player->bendingDestRect.x = 435; player->bendingDestRect.y = 398;
    player->bendingDestRect.w = tw / numberOfSprites; player->bendingDestRect.h = th;

    player->bendingDebugRect.w = 59; player->bendingDebugRect.h = th;
    player->bendingDebugRect.x = player->bendingDestRect.x; player->bendingDebugRect.y = player->bendingDestRect.y;
}

//Checks for Collision of an object with our player 
int _scorePrintCounter = 0;
void CollisionPlayer(struct Player* player , SDL_Rect *collidingRect)
{
   //If the dino is bending then check collision for the bending dino
   if(_bendingDinoFlag) {
     if(SDL_HasIntersection(&player->bendingDebugRect,collidingRect)) {
        WriteToFile(HighscoreFile,_scoreHolder);
        _gameOver = true;
        if(_scorePrintCounter == 0) { 
          printf(CMD_COLOR_RED "\n(LOG) Gameover | Score: %d\n\n" CMD_COLOR_RESET,_scoreHolder);
          printf("--------------- ADDITIONAL LOGS ---------------\n\n");
          printf("Press CTRL-C To Exit\n\n"); 
          _scorePrintCounter++; } 
     }
   }

   //else if it is the normal position , check for that
   else {
    //if ((Horizontal or Vertical) and Big)
    if(SDL_HasIntersection(&player->debugRect_three,collidingRect) || SDL_HasIntersection(&player->debugRect_two,collidingRect) && SDL_HasIntersection(&player->debugRect_one,collidingRect)) {
        WriteToFile(HighscoreFile,_scoreHolder);
       _gameOver = true;
       if(_scorePrintCounter == 0) { 
          printf(CMD_COLOR_RED "\n(LOG) Gameover | Score: %d\n\n" CMD_COLOR_RESET,_scoreHolder);
          printf("--------------- ADDITIONAL LOGS ---------------\n\n"); 
          printf("Press CTRL-C To Exit\n\n");
          _scorePrintCounter++; 
        } 
     } 
  }
}

//Updates our player
void UpdatePlayer(struct Player* player)
{
   //Updates the sprite of the bending dino
   if(_bendingDinoFlag) {
     player->bendingSrcRect.x = 59 * _bendingDinoAnimationDelta;
   }

   //Else it animated the walking animation of the dino
   else {
      player->srcRect.x = 44 * _playerAnimationDelta;

      //This is the jumping mechanics of the dino
      if(_jumpFlag && player->destRect.y < _jumpRange) player->velocity += player->gravity;
      if(player->destRect.y > _groundPosition) {
          _jumpFlag = false; 
          player->destRect.y = _groundPosition;
          player->debugRect_one.y = _groundPosition;  //debug
          player->debugRect_two.y = _groundPosition + _playerDebugRectVerticalPaddingY;  //debug
          player->debugRect_three.y = _groundPosition + _playerDebugRectHorizontalPaddingY;  //debug
      }

      //Jumping Mechanics Continued
      if(_jumpFlag) {
          player->destRect.y += player->velocity;
          player->debugRect_one.y += player->velocity; //debug
          player->debugRect_two.y += player->velocity; //debug
          player->debugRect_three.y += player->velocity; //debug
      }
   }
}

//Renders our player
void RenderPlayer(struct Player* player)
{ 
   //Dead little dino
   if(_gameOver) { 
     player->srcRect.x = 44 * 4; 
   } 
   
   //If the debug mode is toggled
   if(_debugMode)
   {
      //if bending dino mode is on .. render the binding dino debug rect
      if(_bendingDinoFlag) { 
        SDL_RenderDrawRect(_renderer,&player->bendingDebugRect); 
      } else {
        SDL_RenderDrawRect(_renderer,&player->debugRect_one); //debug
        SDL_RenderDrawRect(_renderer,&player->debugRect_two); //debug
        SDL_RenderDrawRect(_renderer,&player->debugRect_three); //debug
      }
   }

   //If the player is bending then we need to render the bending dino
   if(_bendingDinoFlag) { 
     SDL_RenderCopy(_renderer,_playerBendingTexture,&player->bendingSrcRect,&player->bendingDestRect);
   } else { 
     SDL_RenderCopy(_renderer,_playerTexture,&player->srcRect,&player->destRect);
   }
}

/* ################################# PLATFORM ################################ */

//Creates The Platform Entity
void CreatePlatform(struct Platform *platform , int x)
{
   //Setting up the platform
   platform->rect.x = x;
   platform->rect.y = 420;
   SDL_QueryTexture(_platformTexture,NULL,NULL,&platform->rect.w,&platform->rect.h);
}

//Updates The Platforms
void UpdatePlatform(struct Platform* platform) 
{ 
  platform->rect.x -= _gameSpeed; 
}

//Renders The Platform Entity
void RenderPlatform(struct Platform *platform) 
{ 
  SDL_RenderCopy(_renderer,_platformTexture,NULL,&platform->rect); 
}

/* ################################## LEVEL ############################### */

//Loads all the necessary textures required in our game
void LoadLevelTextures()
{
   _platformTexture = CreateTexture("res/platform.png");
   _playerTexture   = CreateTexture("res/player.png");
   _cactusTexture   = CreateTexture("res/small_cactus.png");
   _birdTexture     = CreateTexture("res/bird.png");
   _playerBendingTexture = CreateTexture("res/bending_player.png");
   _largeCactusTexture = CreateTexture("res/large_cactus.png");
}

int type2 = 0,type = 0;
///Creates The Level
void CreateLevel()
{  
   //Fancy Console prints
   system("CLS");
   printf("\n--------------- DINORUN LOG -------------\n\n");
   printf(CMD_COLOR_YELLOW"(LOG) Debug Mode %d\n"CMD_COLOR_RESET, _debugMode);
   printf(CMD_COLOR_BLUE"(LOG) Collision Mode %d\n"CMD_COLOR_RESET, _collisionMode);
  
   LoadLevelTextures(); //Loads all the textures

   //Initializes every entity in the game
   CreatePlayer(&_player);
   CreatePlatform(&_p1,0); 
   CreateBird(&_bird,_windowWidth);

   type = generateNumber(0,1,1);
   CreateCactus(&_c1,_windowWidth - 100,type); ///
   
   CreateFont(&_scoreFont , _windowWidth/2 - 50,20,80,30,"0");
   CreateFont(&_debugModeFont,20,10,210,25,"Debug Mode: OFF");
   CreateFont(&_collisionModeFont,_windowWidth - 220,10,200,25,"Collision Mode: ON");
   
   //Initializing the entities [Platforms and Cactus]
   _plats[0] = _p1;
   _cacts[0] = _c1;
}

int destroyPoint = 120;
int speedCounter = 0;
char scoreBuffer[1];
//This function updates Our Level
void UpdateLevel()
{
   //Update Score
   _scoreHolder = (_score++ / 6);
   
   itoa(_scoreHolder,scoreBuffer,10);
   UpdateFont(&_scoreFont,scoreBuffer);
   if(_scoreHolder % 100 == 0 && _scoreHolder != 0)
   {
     speedCounter++;
     if(speedCounter == 6) {
       speedCounter = 0;
       _gameSpeed++;
       printf("(LOG) Speed was increased %d | %d\n",_scoreHolder,_gameSpeed);
       Mix_PlayChannel(-1,speedIncreasedEffect,0);
     }
   }
   
   //Updating The Player And The Bird
   UpdatePlayer(&_player);
   UpdateBird(&_player,&_bird);
   
   //Updates Platforms and Cactus Traversing through arrays
   for(int i = 0; i <= 1; i++) { 
     UpdatePlatform(&_plats[i]); 
     UpdateCactus(&_cacts[i],&_player,type);    
   } 

   //Infinite Platform Algorithm
   if(_plats[0].rect.x < 0) {
      struct Platform p2; 
      CreatePlatform(&p2,_windowWidth);
      _plats[0] = p2; _plats[1] = _p1;
   } 

   //Infinite Cactus Algorithm
   if(_cacts[0].destRect.x < destroyPoint || _cacts[0].largeRect.x < destroyPoint) {
      struct Cactus c2;
      type = generateNumber(0,1,1);
      CreateCactus(&c2,_windowWidth - 100,type); //
      _cacts[0] = c2; _cacts[1] = _c1;
   }
}

//Renders our whole level
void RenderLevel()
{
   //Renders Platforms and Cactuses
   for(int i = 0; i <= 1; i++) { 
     RenderPlatform(&_plats[i]); 
     RenderCactus(&_cacts[i],type); 
   } 
 
   //Renders The Player And The Bird
   RenderPlayer(&_player); 
   RenderBird(&_bird);
   RenderFont(&_scoreFont); RenderFont(&_debugModeFont); RenderFont(&_collisionModeFont);
}

/* ################################## WINDOW ############################## */

void CreateMenu()
{
  CreateFont(&menuTitle,220,120,400,50,"Dinorun");
  CreateFont(&startTitle,320,180,200,30,"Press ( S ) to Start");
  CreateFont(&menuDescription,160,180 + 20 + 30,600,30,"Created By Asaad Noman Abbasi \n BCS 1-F // 1912297");
  CreateFont(&highscoreTitle,180 + 50,300,250,30,"Current Highscore: ");
  
  char buffer[5];
  itoa(CurrentHighScore,buffer,10);
  CreateFont(&highscoreScoreTitle,460 + 50,300,100,25,buffer);
}

void RenderMenu()
{
  if(MenuRunning) { 
    RenderFont(&startTitle); 
    RenderFont(&menuTitle); 
    RenderFont(&menuDescription); 
    RenderFont(&highscoreTitle);
    RenderFont(&highscoreScoreTitle);
  }
}

/* ################################## WINDOW ############################## */

//This updates our keyboard whenever there is a window event
void UpdateKeyboard(SDL_Event *event , struct Player *player)
{
   if(event->type == SDL_KEYDOWN && event->key.repeat == 0) {
      switch(event->key.keysym.sym) {

        case SDLK_s: {
          if(MenuRunning) {
            CreateLevel();
            MenuRunning = false;
          }
        } break;

         //TAB is the Exit Button
         case SDLK_TAB: {
           //Cannot quit unitl the game is over :v)
           if(_gameOver) SDL_Quit();
         } break;

         //UP is the jump Button
         case SDLK_UP: {
            //if the dino is bending do not jump
            if(!_bendingDinoFlag) {
              //Make sure the player is on the ground to avoid double jump
              if(player->destRect.y == _groundPosition) {
                _jumpFlag = true;
                if(!_gameOver) Mix_PlayChannel(-1,jumpSoundEffect,0);
                player->velocity = _jumpSpeed;
              }
            } 
         } break;
         
         //F1 is the debug toggle button
         case SDLK_F1: {
            if(_debugMode) { 
              _debugMode = false;
              UpdateFont(&_debugModeFont,"Debug Mode: OFF"); 
            } else {
              _debugMode = true;
              UpdateFont(&_debugModeFont,"Debug Mode: ON");
            } printf(CMD_COLOR_YELLOW"(LOG) Debug Mode %d\n"CMD_COLOR_RESET, _debugMode);
         } break;

         //F2 is the collision toggle button
         case SDLK_F2:{
           if(_collisionMode) { 
            _collisionMode = false; 
            UpdateFont(&_collisionModeFont,"Collision Mode: OFF");
          } else {
            _collisionMode = true;
            UpdateFont(&_collisionModeFont,"Collision Mode: ON");
          } printf(CMD_COLOR_BLUE"(LOG) Collision Mode %d\n"CMD_COLOR_RESET, _collisionMode);
         } break;

         //DOWN is the bending dino
         case SDLK_DOWN: if(!_gameOver && !_jumpFlag) _bendingDinoFlag = true; break;
      }
   } 

   //If we release the down button then we know the dino is not bending
   else if(event->type == SDL_KEYUP && event->key.repeat == 0) {
     switch(event->key.keysym.sym) {
       case SDLK_DOWN: if(!_gameOver) _bendingDinoFlag = false; break;
     }
   }
}

//This updates the level , until the game is over @60FPS
void Update()
{
   if(!MenuRunning) if(!_gameOver) UpdateLevel();
}

//This Renders the level @60FPS
void Render()
{
   //This just render clears and renders the level
   SDL_SetRenderDrawColor(_renderer,255,255,255,0);
   SDL_RenderClear(_renderer);
   if(_debugMode) 
    SDL_SetRenderDrawColor(_renderer,255,0,0,0); //for debug
   if(!MenuRunning) { RenderLevel(); } else { RenderMenu(); }
   SDL_RenderPresent(_renderer);
}

//This creates the window and renderer and initializes necessary sdl functions
int Init()
{
   //Setting up all the SDL Library functions
   if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) { printf("error sdl_init: %s",SDL_GetError()); return 0; }
   if(IMG_Init(IMG_INIT_PNG) < 0) { printf("error img_init"); return 0; }
   if(TTF_Init() < 0) { printf("error ttf_init: %s",TTF_GetError()); return 0; }
   if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,2048) < 0) { printf("error mix_audio_init: %s",Mix_GetError()); return 0;}

   _window = SDL_CreateWindow(_windowTitle,SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,_windowWidth,_windowHeight,SDL_WINDOW_SHOWN);
   if(_window == NULL) { printf("error creating_window: %s",SDL_GetError()); return 0; }

   _renderer = SDL_CreateRenderer(_window,0,SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
   if(_renderer == NULL) { printf("error creating_render: %s",SDL_GetError()); return 0; }

   //Loading the sound effect
   jumpSoundEffect = Mix_LoadWAV("res/jump.wav");
   speedIncreasedEffect = Mix_LoadWAV("res/speedincreased.wav");
   if(jumpSoundEffect == NULL && speedIncreasedEffect == NULL) printf("Could not load sound effects\n"); 

   return 1;
}

//Frees up space whenver our program is terminated
void Terminate()
{
    Mix_FreeChunk(jumpSoundEffect);
    Mix_CloseAudio();
    TTF_Quit();
    SDL_Quit();
   _running = false;
}  

//MAIN entry point
int main(int argc , char *argv[])
{

   CreateFile(HighscoreFile);
   CurrentHighScore = ReadAndReturnHighscore(HighscoreFile);

   const int FPS = 250;
   const int frameTime = 1000 / FPS;
   Uint32 lastTime,delta;

   //Main GAMELOOP
   if(Init()) {
      _running = true;
      CreateMenu();
      while(_running) {
         lastTime = SDL_GetTicks();
         _playerAnimationDelta = (lastTime / 100) % 4;
         _bendingDinoAnimationDelta = (lastTime / 100) % 2;
         if(SDL_PollEvent(&_event)) {
            switch(_event.type) {
               case SDL_QUIT:
                  Terminate(); break;
            } UpdateKeyboard(&_event,&_player);
         }
         
         Update(); Render();
         delta = SDL_GetTicks() - lastTime;
         if(delta < frameTime) {
          SDL_Delay(frameTime - delta);
        }
      }
   }
}