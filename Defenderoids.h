#ifndef _DEFENDEROIDS
#define _DEFENDEROIDS

// Tile constants
#define bgTileBase 256
#define spTileBase 137
#define borderTilebase 128
#define LogoTileBase 256

// Palette constants
// SCR_1_PLANE
#define PAL_BITMAP 0
#define PAL_LOGO 0
#define PAL_STARFIELD 1
#define PAL_BORDER 4

// SCR_2_PLANE
#define PAL_DEBUG 0
#define PAL_SCORE 1

// SPR_PLANE/SCR1_PLANE (Lemmanoid/City)
#define PAL_SPRITE 0
#define PAL_INVADER 1
#define PAL_LEMMANOID 2
#define PAL_CITY 3
#define PAL_PICTSEL 4
#define PAL_ANGRYINVADER 5
#define PAL_UMBRELLA 6

//Global consts
const u8 SPRITE_SCALE=7;
const u8 BITMAP_WIDTH=144;
const u8 BITMAP_HEIGHT=112;

//Game constants
#define SPRITE_MAX_WIDTH (512)
#define MAX_SPRITE (64)
#define MAX_VECTOR 48

// Vector Type Objects
#define VEC_NONE 0
#define VEC_SHIP 1
#define VEC_ASTEROID 2
#define VEC_EXPLOSION 3
#define VEC_SHOT 4
#define VEC_QIX 5

typedef struct SpritePoint
{
	u16 x;
	u16 y;
} SPRITEPOINT;

//Direction is affected by sprite type
//Lemmanoids will be left/right - possibly up/down if they are floaters or being kidnapped?
//Invaders are multi-directional (and may not have a fixed direction anyway)
//Cities will use it as the "age/build completion"
//Pictcells will have no direction attrib
//
typedef struct Sprite
{
	SPRITEPOINT Position;
	u8 SpriteID;
	u16 SpriteType;
	u16 BaseTile;
	u16 Frame;
	u16 Direction;
	u8 RelatedSpriteID;
	bool Initiated;
} SPRITE;

// Direction constants
#define DIR_EAST 0
#define DIR_WEST 4
#define DIR_SOUTH 8
#define DIR_NORTH 12
#define DIR_HOME 16
#define DIR_MUTANOID 20
#define DIR_HEADSTONE 24
#define DIR_SAFE 25

// City block IDs (left-right)
#define CITYBLOCK1 0
#define CITYBLOCK2 1
#define CITYBLOCK3 2
#define CITYBLOCK4 3
#define CITYAGE1 0
#define CITYAGE2 4
#define CITYAGE3 8
#define CITYAGE4 12
#define CITY_COMPLETE 12

// 

#define MIN_CITY 1024
#define MAX_CITY 4096

typedef struct Level
{
	char LevelName[20];
	u8 InvaderCount;
	u8 LemmanoidCount;
	u8 AsteroidCount;
	u8 CityStatus;
	u8 Saved;
	u8 Died;
} LEVEL;

const LEVEL DefenderoidsLevels[] = {
	{"THE FALL OF LEMMAN",16,1,1,0,0,0},
	{" THE LAST OF THEM ",1,3,1,12,0,0},
	{"COLLECT PICTCELLS ",3,5,2,8,0,0},
	{"BUILD LEMMANOPOLIS",8,6,2,4,0,0},
	{"    MINED OUT!    ",0,8,3,0,0,0},
	{"THE LAST LEMMANOID",4,1,4,0,0,0},
	{" WHO DEFENDS WHO? ",6,6,3,0,0,0}
};

#define MAX_LEVEL 5

// Function templates
u8 DefenderoidsLogo();
void DrawGameScreen();
void DefenderoidsMain();

#endif

