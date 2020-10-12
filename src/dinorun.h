   
   /* 
   ========================================================================\n"
    File: dinorun.h"
    Date: 2/10/19"
    Creator: Asaad Noman Abbasi $\n"
    Notice: (C) Copyright 2019 by Asaad, Inc. All Rights Reserved. $\n"
    ======================================================================== 
*/

#ifndef DINORUN_H
#define DINORUN_H

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#include <SDL2/sdl.h>
#include <SDL2/sdl_image.h>
#include <SDL2/sdl_ttf.h> 
#include <SDL2/sdl_mixer.h>

//These are ANSI C console foreground colors for LOGS
#define CMD_COLOR_RED "\x1b[31m"
#define CMD_COLOR_YELLOW "\x1b[33m"
#define CMD_COLOR_BLUE "\x1b[34m"
#define CMD_COLOR_RESET "\x1b[0m"

//Our true and false for easier readablity
#define true  1;
#define false 0;

struct Platform 
{ 
  SDL_Rect rect; 
};

struct Bird 
{ 
  SDL_Rect destRect,srcRect,debugRect; 
};

struct Cactus 
{ 
  SDL_Rect destRect,srcRect,debugRect;
  SDL_Rect largeRect , largeDebugRect;
};

struct Player
{
   SDL_Rect destRect,srcRect;
   SDL_Rect debugRect_one,debugRect_two,debugRect_three;
   int gravity,velocity;

   SDL_Rect bendingDestRect,bendingSrcRect;
   SDL_Rect bendingDebugRect;
};

struct Font
{
  SDL_Rect rect;
  SDL_Texture *texture;
  SDL_Surface *surface;
};

const int _windowWidth    = 800;
const int _windowHeight   = 500;
const char _windowTitle[] = "Dinorun // Asaad Noman Abbasi // BCS 1-F // 1912297";

static int _gameSpeed   = 7;
static int _jumpSpeed   = -8;
static int _running     = false;

static int _debugMode     = false;
static int _collisionMode = true;
struct Font _debugModeFont,_collisionModeFont;

static int _gameOver    = false;
static int _bendingDinoFlag = false;
       int _jumpFlag = false;

struct Font _scoreFont;
int  _scoreHolder = 0;
int  _score = 0;

SDL_Window*      _window;
SDL_Renderer*    _renderer;
static SDL_Event _event;

SDL_Texture *_platformTexture;
SDL_Texture *_playerTexture;
SDL_Texture *_playerBendingTexture;
SDL_Texture *_cactusTexture;
SDL_Texture *_birdTexture;
SDL_Texture *_largeCactusTexture;

//For the sound effect
Mix_Chunk *jumpSoundEffect;
Mix_Chunk *speedIncreasedEffect;

static int MenuRunning = true;
struct Font highscoreTitle,highscoreScoreTitle,menuTitle,menuDescription,startTitle;

static struct Platform _plats[1];  
struct Platform _p1;

static struct Cactus _cacts[1];
struct Cactus _c1;

struct Player _player;
int           _playerAnimationDelta;

int         _bendingDinoAnimationDelta;

struct Bird _bird;

const int _groundPosition = 375;
const int _jumpRange = 320;

//proto
void CreateLevel();
void CollisionPlayer(struct Player* p, SDL_Rect* r);

FILE* HighscoreFile;
int CurrentHighScore = 0;

#endif