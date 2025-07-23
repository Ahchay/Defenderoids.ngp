#ifndef _VECLIB
#define _VECLIB

typedef struct Point
{
	s16 x;
	s16 y;
} POINT;

typedef struct ColourPoint
{
	s16 x;
	s16 y;
	u8 colour;
} COLOURPOINT;

// total 337 bytes
typedef struct VectorObject
{
	u8 ObjectType; // 1 byte
	POINT Origin; // 4 bytes
	POINT Position; // 4 bytes
	POINT MovementVector; //4 bytes
	u8 Points; // 1 byte
	COLOURPOINT PointList[64]; //64 x 5 bytes (320 bytes)
	u8 Scale; // 1 byte
	u8 RotationAngle; // 1 byte
	s8 RotationSpeed; // 1 byte
	u8 Counter;
} VECTOROBJECT;

// 67 bytes
typedef struct SmallVectorObject
{
	u8 ObjectType; // 1 byte
	POINT Origin; // 4 bytes
	POINT Position; // 4 bytes
	POINT MovementVector; //4 bytes
	u8 Points; // 1 byte
	COLOURPOINT PointList[10]; //10 x 5 bytes (50 bytes)
	u8 Scale; // 1 byte
	u8 RotationAngle; // 1 byte
	s8 RotationSpeed; // 1 byte
	u8 Counter; // 1 byte
} SMALLVECTOROBJECT;

void DrawVectorObjectAbsolute(u16 * BitmapAddress, SMALLVECTOROBJECT VectorObject);
void DrawVectorObject(u16 * BitmapAddress, SMALLVECTOROBJECT VectorObject,u16 iHorizontalOffset);
void DrawVectorSpriteAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject);
void DrawVectorSprite(u16 * BitmapAddress, VECTOROBJECT VectorObject,u16 iHorizontalOffset);
void DrawSmallVectorSprite(u16 * BitmapAddress, SMALLVECTOROBJECT VectorObject, u16 iHorizontalOffset);
bool LineIntersect(POINT l1, POINT l2, POINT b1, POINT b2);

#endif

