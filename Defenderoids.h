#ifndef _DEFENDEROIDS
#define _DEFENDEROIDS

// Tile constants
#define bgTileBase 256
#define spTileBase 137
#define borderTilebase 128
#define LogoTileBase 256

// Palette constants
#define PAL_SPRITE 0
#define PAL_INVADER 1
#define PAL_LEMMANOID 2
#define PAL_CITY 3
#define PAL_PICTSEL 4

//Global consts
const u8 SPRITE_SCALE=7;
const u8 BITMAP_WIDTH=144;
const u8 BITMAP_HEIGHT=112;

//Game constants
#define SPRITE_MAX_WIDTH (512)
#define MAX_ASTEROID (18)
#define MAX_SPRITE (63)


typedef struct SpritePoint
{
	u16 x;
	u16 y;
} SPRITEPOINT;

//Direction is affected by sprite type
//Lemmanoids will be left/right - possibly up/down if they are floaters or being kidnapped?
//Invaders are TBD
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
} SPRITE;

// Direction constants
#define DIR_EAST 0
#define DIR_WEST 3
#define DIR_SOUTH 7
#define DIR_NORTH 11

typedef struct Level
{
	char LevelName[30];
	u8 InvaderCount;
	u8 LemmanoidCount;
	u8 AsteroidCount;
} LEVEL;

// Function templates
u8 DefenderoidsLogo();
void DrawGameScreen();
void DefenderoidsMain();

#endif

