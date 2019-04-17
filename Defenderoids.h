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
	u16 Direction;
} SPRITE;


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

bool DefenderoidsLogo();
void DefenderoidsMain();

void DrawVectorObjectAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject);
void DrawVectorObject(u16 * BitmapAddress, VECTOROBJECT VectorObject,u8 iHorizontalOffset);
void DrawVectorSprite(u16 * BitmapAddress, VECTOROBJECT VectorObject);

