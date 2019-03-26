#define bgTileBase 256

typedef struct Point
{
	u16 x;
	u16 y;
} POINT;

typedef struct ColourPoint
{
	u16 x;
	u16 y;
	u8 colour;
} COLOURPOINT;

typedef struct VectorObject
{
	POINT Origin;
	POINT Position;
	u8 Points;
	COLOURPOINT VectorList[128];
	u8 Scale;
	u8 RotationAngle;
	s8 RotationSpeed;
} VECTOROBJECT;

void DefenderoidsMain();

void DrawVectorObject(u16 * BitmapAddress, VECTOROBJECT VectorObject);

const POINT Box[5] = {{0,0},{0,1},{1,1},{1,0},{0,0}};

//const VECTOROBJECT Asteroid = {{6,6},{24,87},11,{{2,2,1},{4,0,1},{6,2,1},{10,0,1},{12,4,1},{8,8,1},{10,10,1},{6,12,1},{4,8,1},{0,6,1},{4,4,1},{2,2,1}},2};

const COLOURPOINT PlayerSprite[] = {
							                        {3,0,1},
							{0,1,0},{1,1,0},{2,1,0},{3,1,1},{4,1,0},{5,1,0},{6,1,0},
							{0,2,0},{1,2,0},{2,2,0},{3,2,1},{4,2,0},{5,2,0},{6,2,0},
							{0,3,2},{1,3,0},{2,3,0},{3,3,1},{4,3,0},{5,3,0},{6,3,2},
							{0,4,2},{1,4,0},{2,4,1},{3,4,1},{4,4,1},{5,4,0},{6,4,2},
							{0,5,1},{1,5,0},{2,5,1},{3,5,2},{4,5,1},{5,5,0},{6,5,1},
							{0,6,0},{1,6,1},{2,6,1},{3,6,2},{4,6,1},{5,6,1},{6,6,0},
							{0,7,0},{1,7,1},{2,7,3},{3,7,1},{4,7,3},{5,7,1},{6,7,0},
							{0,8,1},{1,8,0},{2,8,3},{3,8,0},{4,8,3},{5,8,0},{6,8,1},
							{0,9,0},{1,9,0},{2,9,0},{3,9,0},{4,9,0},{5,9,0},{6,9,0}
						};