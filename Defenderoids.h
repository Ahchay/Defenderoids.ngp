#define bgTileBase 256

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
							                           {3, 0,1},
							                           {3, 1,2},
											           {3, 2,1},
							                           {3, 3,1},
							                           {3, 4,1},
							{0, 5,2},                  {3, 5,1},                  {6, 5,2},
							{0, 6,2},{1, 6,0},{2, 6,1},{3, 6,1},{4, 6,1},{5, 6,0},{6, 6,2},
							{0, 7,1},{1, 7,0},{2, 7,1},{3, 7,2},{4, 7,1},{5, 7,0},{6, 7,1},
							{0, 8,1},         {2, 8,1},{3, 8,2},{4, 8,1},         {6, 8,1},
							         {1, 9,1},{2, 9,3},{3, 9,1},{4, 9,3},{5, 9,1},
							{0,10,1},{1,10,1},{2,10,3},{3,10,0},{4,10,3},{5,10,1},{6,10,1},
							{0,11,3},                                             {6,11,3}
						};