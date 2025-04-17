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

typedef struct VectorObject
{
	POINT Origin;
	POINT Position;
	POINT MovementVector;
	u8 Points;
	COLOURPOINT VectorList[64];
	u8 Scale;
	u8 RotationAngle;
	s8 RotationSpeed;
} VECTOROBJECT;

void DrawVectorObjectAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject);
void DrawVectorObject(u16 * BitmapAddress, VECTOROBJECT VectorObject,u16 iHorizontalOffset);
void DrawVectorSpriteAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject);
void DrawVectorSprite(u16 * BitmapAddress, VECTOROBJECT VectorObject,u16 iHorizontalOffset);
bool LineIntersect(POINT l1, POINT l2, POINT b1, POINT b2);

#endif

