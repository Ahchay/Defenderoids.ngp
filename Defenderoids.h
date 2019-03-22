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

void DefenderoidsMain();

const POINT DefenderoidShip[5] = {{0,0},{0,1},{1,1},{1,0},{0,0}};

const COLOURPOINT PrettyPattern[12] = {{0,0,1},{3,3,1},{5,5,1},{7,7,1},{12,12,3},{10,10,3},{8,8,3},{6,6,3},{0,8,2},{2,6,2},{4,4,2},{6,2,2}};