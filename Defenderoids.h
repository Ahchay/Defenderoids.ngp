#define bgTileBase 256
#define spTileBase 128
#define LogoTileBase 256

#define PAL_SPRITE 0

typedef struct SpritePoint
{
	u16 x;
	u16 y;
} SPRITEPOINT;

typedef struct Sprite
{
	SPRITEPOINT Position;
	u8 SpriteID;
	u16 SpriteType;
	u16 BaseTile;
	u16 Frame;
	u16 Direction; // 1 North 2 East 3 South 4 West
} SPRITE;

#define DIR_NORTH 0
#define DIR_EAST 1
#define DIR_SOUTH 2
#define DIR_WEST 3

typedef struct Level
{
	char LevelName[30];
	u8 InvaderCount;
	u8 LemmanoidCount;
	u8 AsteroidCount;
} LEVEL;

bool DefenderoidsLogo();
void DefenderoidsMain();

