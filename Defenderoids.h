#ifndef _DEFENDEROIDS
#define _DEFENDEROIDS

// Tile constants
#define bgTileBase 256
#define spTileBase 137
#define borderTilebase 128
#define LogoTileBase 256

// Palette constants

// SCR_2_PLANE
#define PAL_BITMAP 0
#define PAL_SHIP 1

// SPR_PLANE
#define PAL_SPRITE 0
#define PAL_INVADER 1
#define PAL_LEMMANOID 2
#define PAL_CITY 3
#define PAL_PICTSEL 4
#define PAL_ANGRYINVADER 5
#define PAL_UMBRELLA 6
#define PAL_BOMBER 7
#define PAL_MISSILE 8
#define PAL_SPACIE 9 // Actuall 9-11 for the three Spacie types
#define PAL_FIREWORK 12 // Actually 12-15 for firework palettes


// SPR_PLANE/SCR1_PLANE (Lemmanoid/City)
// SCR_1_PLANE
// Logo palettes
#define PAL_LOGO 0
#define PAL_STARFIELD 1
// Main game palettes
#define PAL_SCORE 0
#define PAL_DEBUG 1
#define PAL_CITY_STATUS 2
#define PAL_BORDER 3
#define PAL_STATUS 4 // Actually 4-14 for individual Lemmanoid status tiles

// Macro to translate Player position+Horizontal Offset to the sprite position
// Needs to cater for the wrap point I think?
#define PLAYER_X ((u16)iHorizontalOffset<<SPRITE_SCALE)+((vShip.Position.x+vShip.Origin.x)<<SPRITE_SCALE)
#define PLAYER_Y ((vShip.Position.y+vShip.Origin.y)<<SPRITE_SCALE)

//Global consts
const u8 SPRITE_SCALE=7;
const u8 BITMAP_WIDTH=144;
const u8 BITMAP_HEIGHT=112;

//Game constants
#define SPRITE_MAX_WIDTH (512)
#define MAX_SPRITE (64)
#define MAX_VECTOR 48
#define MAX_ENERGY 96

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
#define DIR_NONE 99
#define DIR_RIGHT 0
#define DIR_LEFT 2
#define DIR_EAST 0
#define DIR_WEST 4
#define DIR_SOUTH 8
#define DIR_NORTH 12
#define DIR_HOME 16
#define DIR_MUTANOID 20
#define DIR_HEADSTONE 26
#define DIR_SAFE 24

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

// City position

#define MIN_CITY 1024
#define MAX_CITY 4096

// Misc sprite definitions
#define sprUmbrella sprMisc+1
#define sprHodWest sprMisc+2
#define sprHodEast sprMisc+3

// Collision distances
#define COLLISION_QIX 4096
#define COLLISION_PLAYER 1536
#define COLLISION_SHOT 1024
#define COLLISION_ASTEROID 2048


typedef struct Level
{
	char LevelName[20];
	u8 InvaderCount;
	u8 LemmanoidCount;
	u8 AsteroidCount;
	u8 QixCount;
	u8 CityStatus;
	u8 Saved;
	u8 Died;
} LEVEL;

const LEVEL DefenderoidsLevels[] = {
	{"ERADICATE EVIL QIX",0,0,0,1,0,0,0},
	{"THE FALL OF LEMMAN",16,1,1,0,0,0,0},
	{" THE LAST OF THEM ",1,3,1,0,12,0,0},
	{"COLLECT PICTCELLS ",3,5,2,0,8,0,0},
	{"BUILD LEMMANOPOLIS",8,6,2,0,4,0,0},
	{"    MINED OUT!    ",0,8,3,0,0,0,0},
	{"THE LAST LEMMANOID",4,1,4,0,0,0,0},
	{"WHOS DEFENDING WHO",6,6,3,0,0,0,0}
};

#define MAX_LEVEL 7

// Function templates
u8 DefenderoidsLogo();
void DrawGameScreen();
void DefenderoidsMain();

#endif

