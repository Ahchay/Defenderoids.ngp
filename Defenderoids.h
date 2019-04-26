#define bgTileBase 256
#define spTileBase 128

#define PAL_SPRITE 0

typedef struct Point
{
	s16 x;
	s16 y;
} POINT;

typedef struct SpritePoint
{
	u16 x;
	u16 y;
} SPRITEPOINT;

typedef struct Sprite
{
	SPRITEPOINT Position;
	u8 SpriteID;
	u8 SpriteType;
	u16 BaseTile;
	u16 Frame;
	u16 Direction; // 1 North 2 East 3 South 4 West
} SPRITE;

#define DIR_NORTH 0
#define DIR_EAST 1
#define DIR_SOUTH 2
#define DIR_WEST 3

typedef struct ColourPoint
{
	s16 x;
	s16 y;
	u8 colour;
} COLOURPOINT;

typedef struct VectorObject
{
	POINT Origin;
	POINT Position;
	POINT MovementVector;
	u8 Points;
	COLOURPOINT VectorList[128];
	u8 Scale;
	s8 RotationAngle;
	s8 RotationSpeed;
} VECTOROBJECT;

typedef struct Level
{
	char LevelName[30];
	u8 InvaderCount;
	u8 LemmanoidCount;
	u8 AsteroidCount;
} LEVEL;

bool DefenderoidsLogo();
void DefenderoidsMain();

void DrawVectorObjectAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject);
void DrawVectorObject(u16 * BitmapAddress, VECTOROBJECT VectorObject,u8 iHorizontalOffset);
void DrawVectorSpriteAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject);
void DrawVectorSprite(u16 * BitmapAddress, VECTOROBJECT VectorObject,u8 iHorizontalOffset);
bool LineIntersect(POINT l1, POINT l2, POINT b1, POINT b2);
