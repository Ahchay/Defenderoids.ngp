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
void DrawVectorSprite(u16 * BitmapAddress, VECTOROBJECT VectorObject);

const POINT Box[5] = {{0,0},{0,1},{1,1},{1,0},{0,0}};

//const VECTOROBJECT Asteroid = {{6,6},{24,87},11,{{2,2,1},{4,0,1},{6,2,1},{10,0,1},{12,4,1},{8,8,1},{10,10,1},{6,12,1},{4,8,1},{0,6,1},{4,4,1},{2,2,1}},2};

const COLOURPOINT AsteroidTemplate[14][2] = {{{6,0,1},{6,2,1}},{{8,1,1},{7,2,1}},{{9,2,1},{8,3,1}},{{10,4,1},{8,4,1}},{{9,6,1},{8,5,1}},{{8,7,1},{7,6,1}},{{6,8,1},{6,6,1}},{{4,8,1},{5,6,1}},{{2,7,1},{3,5,1}},{{1,6,1},{2,5,1}},{{0,4,1},{2,4,1}},{{1,2,1},{2,3,1}},{{2,1,1},{3,2,1}},{{4,0,1},{5,2,1}}};

const COLOURPOINT PlayerSprite[] = {
							                           {3, 0,1},
							                           {3, 1,2},
											           {3, 2,1},
							                           {3, 3,1},
							                           {3, 4,1},
							{0, 5,2},                  {3, 5,1},                  {6, 5,2},
							{0, 6,2},         {2, 6,1},{3, 6,1},{4, 6,1},         {6, 6,2},
							{0, 7,1},         {2, 7,1},{3, 7,2},{4, 7,1}         ,{6, 7,1},
							{0, 8,1},         {2, 8,1},{3, 8,2},{4, 8,1},         {6, 8,1},
							         {1, 9,1},{2, 9,3},{3, 9,1},{4, 9,3},{5, 9,1},
							{0,10,1},{1,10,1},{2,10,3},         {4,10,3},{5,10,1},{6,10,1},
							{0,11,3},                                             {6,11,3},
							// Use the last five elements of the array for engine noise...
							                  {2,12,1},{3,12,1},{4,12,1},
							                  {2,13,1},         {4,13,1}
						};