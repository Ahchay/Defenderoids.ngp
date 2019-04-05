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
	s8 RotationAngle;
	s8 RotationSpeed;
} VECTOROBJECT;

bool DefenderoidsLogo();
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

// VECTOROBJECT Constructor
// {{ORIGINX,ORIGINY},{POSITIONX,POSITIONY},{MOVEMENTVECTORX,MOVEMENTVECTORY},POINTS,{colourpoints},SCALE,ROTATIONANGLE,ROTATIONSPEED}
const VECTOROBJECT Alphabet[] = {
	// "D" 0
	{
			{5,10},
			{0,0},
			{0,0},
			11,
			{
				{0,0,1},
				{0,20,1},
				{7,20,1},
				{9,19,1},
				{9,18,1},
				{10,17,1},
				{10,3,1},
				{9,2,1},
				{9,1,1},
				{8,0,1},
				{0,0,1}
			},
			1,
			0,
			0
	},
	// "e" 1
	{
			{3,7},
			{0,0},
			{0,0},
			9,
			{
				{0,12,1},
				{5,12,1},
				{4,10,1},
				{3,9,1},
				{1,10,1},
				{0,12,1},
				{1,14,1},
				{3,15,1},
				{5,14,1}
			},
			1,
			0,
			0
	},
	// "f" 2
	{
			{3,10},
			{0,0},
			{0,0},
			10,
			{
				{0,12,1},
				{5,12,1},
				{5,10,0},
				{5,10,1},
				{4,9,1},
				{2,10,1},
				{1,12,1},
				{1,14,1},
				{2,17,1},
				{3,18,1}
			},
			1,
			0,
			0
	},
	// "n" 3
	{
			{3,10},
			{0,0},
			{0,0},
			9,
			{
				{0,15,1},
				{0,9,1},
				{0,10,1},
				{1,9,1},
				{1,9,1},
				{2,9,1},
				{4,10,1},
				{5,11,1},
				{5,15,1}
			},
			1,
			0,
			0
	},
	// "d" 4
	{
			{3,7},
			{0,0},
			{0,0},
			11,
			{
				{5,14,1},
				{5,12,1},
				{4,10,1},
				{3,9,1},
				{1,10,1},
				{0,12,1},
				{1,14,1},
				{3,15,1},
				{5,14,1},
				{5,15,1},
				{5,3,1}
			},
			1,
			0,
			0
	},
	// "r" 5
	{
			{3,7},
			{0,0},
			{0,0},
			7,
			{
				{0,15,1},
				{0,9,1},
				{0,10,1},
				{1,9,1},
				{1,10,1},
				{2,9,1},
				{5,10,1}
			},
			1,
			0,
			0
	},
	// "o" 6
	{
			{3,7},
			{0,0},
			{0,0},
			9,
			{
				{5,12,1},
				{4,10,1},
				{3,9,1},
				{1,10,1},
				{0,12,1},
				{1,14,1},
				{3,15,1},
				{4,14,1},
				{5,12,1}
			},
			1,
			0,
			0
	},
	// "i" 7
	{
			{2,7},
			{0,0},
			{0,0},
			10,
			{
				{1,5,1},
				{2,6,1},
				{0,7,1},
				{1,7,1},
				{1,5,1},
				{1,9,0},
				{1,9,1},
				{1,13,1},
				{2,14,1},
				{3,13,1}
			},
			1,
			0,
			0
	},
	// "s" 8
	{
			{3,7},
			{0,0},
			{0,0},
			6,
			{
				{5,10,1},
				{3,9,1},
				{0,10,1},
				{3,12,1},
				{5,14,1},
				{3,15,1},
				{0,15,1}
			},
			1,
			0,
			0
	},
	// "P" 9
	{
			{3,10},
			{0,0},
			{0,0},
			11,
			{
				{0,20,1},
				{0,0,1},
				{3,0,1},
				{5,0,1},
				{6,1,1},
				{8,4,1},
				{8,6,1},
				{6,9,1},
				{5,10,1},
				{0,10,1},
				{0,0,1}
			},
			1,
			0,
			0
	},
	// "t" 10
	{
			{5,10},
			{0,0},
			{0,0},
			10,
			{
				{5,4,1},
				{5,13,1},
				{6,13,1},
				{7,14,1},
				{8,13,1},
				{8,13,1},
				{0,20,0},
				{3,7,0},
				{3,7,1},
				{8,7,1}
			},
			1,
			0,
			0
	},
	// "a" 11
	{
			{3,6},
			{0,0},
			{0,0},
			12,
			{
				{0,2,1},
				{2,0,1},
				{5,0,1},
				{7,7,1},
				{7,12,1},
				{7,8,1},
				{5,5,1},
				{3,5,1},
				{0,7,1},
				{3,9,1},
				{5,9,1},
				{7,10,1}
			},
			1,
			0,
			0
	},
	// "'A'" 12
	{
			{3,10},
			{0,0},
			{0,0},
			15,
			{
				{3,0,1},
				{3,1,1},
				{0,20,0},
				{7,0,0},
				{7,0,1},
				{7,1,1},
				{0,20,0},
				{0,20,1},
				{1,20,1},
				{5,0,1},
				{8,20,1},
				{0,20,0},
				{3,10,0},
				{3,10,1},
				{8,10,1}
			},
			1,
			0,
			0
	},
	// "b" 13
	{
			{3,8},
			{0,0},
			{0,0},
			11,
			{
				{0,14,1},
				{0,12,1},
				{1,10,1},
				{3,9,1},
				{4,10,1},
				{5,12,1},
				{4,14,1},
				{3,15,1},
				{0,14,1},
				{0,15,1},
				{0,3,1}
			},
			1,
			0,
			0
	},
	// "u" 14
	{
			{3,7},
			{0,0},
			{0,0},
			9,
			{
				{5,9,1},
				{5,15,1},
				{5,14,1},
				{5,15,1},
				{4,15,1},
				{3,15,1},
				{1,14,1},
				{0,13,1},
				{0,9,1},
			},
			1,
			0,
			0
	}
};