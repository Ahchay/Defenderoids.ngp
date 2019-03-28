#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"

/*

So, this started as a defender/asteroids mash-up concept, but has now turned into a bitmap playground...

Not sure where I'm going with it yet, but it's entertaining enough

*/

void DefenderoidsMain()
{
	// Define variables
	u8 iLoopX;
	u8 iLoopY;
	u16 iTile;
	u8 iMainLoop;
	u8 iDirection;
	u16 RugBitmap[252][8];
	u8 iPoint;
	s16 iStartX;
	s16 iStartY;
	u8 iScale;
	u8 iAngle;
	s16 iEndX;
	s16 iEndY;
	s16 iTempX;
	s16 iTempY;
	u16 iStartFrame;
	s8 cSin;
	s8 cCos;
	u8 iSpriteScale;
	VECTOROBJECT Qix;
	u8 iLoopQix;
	u8 iLoopAsteroid;
	u8 iLoopAsteroidPoint;
	VECTOROBJECT Asteroid[] = {
									{{6,6},{8192,8192},{128,3},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,-1},
									{{6,6},{4096,4096},{5,23},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+3},
									{{6,6},{256,8192},{8,9},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},1,0,+3},
									{{6,6},{8192,4098},{64,-23},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,-2},
									{{6,6},{4098,256},{-45,23},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},1,0,+3},
									{{6,6},{256,256},{-12,-12},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+1}
								};

	InitNGPC();
	SysSetSystemFont();
	InitialiseQRandom();

	// So, create a bitmap...

	SetBackgroundColour(RGB(0,0,15));

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

	CreateBitmap((u16*)RugBitmap, 144, 112);
	CopyBitmap((u16*)RugBitmap, bgTileBase);

	iTile=0;
	//Copy the bitmap to SCR_1_PLANE
	// Watch the order...
	for (iLoopY=0;iLoopY<14;iLoopY++)
	{
		for (iLoopX=0;iLoopX<18;iLoopX++)
		{
			PutTile(SCR_1_PLANE, 0, 2 + iLoopX, 1 + iLoopY, bgTileBase+iTile);
			iTile++;
		}
	}

	// Setup the qix object
	// So, the Qix itself is a lot simpler than it first appears.
	// Essentially, it's a list of lines, each starting from the end position of the last
	// where the "first" line falls off the stack and is replaced at the end with a new one
	// with a random(?) length and direction.
	// Changing the length would essentially control the speed and overall size of the Qix
	Qix.Origin.x=0;
	Qix.Origin.y=0;
	Qix.Position.x=0;
	Qix.Position.y=0;
	Qix.MovementVector.x=32;
	Qix.MovementVector.y=32;
	Qix.Points=12;
	Qix.Scale=1;
	Qix.RotationAngle=0;
	Qix.RotationSpeed=0;
	for (iLoopQix=0;iLoopQix<Qix.Points;iLoopQix++)
	{
		Qix.VectorList[iLoopQix].x = 0;
		Qix.VectorList[iLoopQix].y = 0;
		Qix.VectorList[iLoopQix].colour = 3;
	}

	// Set up the asteroids
	for (iLoopAsteroid=0;iLoopAsteroid<6;iLoopAsteroid++)
	{
		for (iLoopAsteroidPoint=0;iLoopAsteroidPoint<Asteroid[iLoopAsteroid].Points-1;iLoopAsteroidPoint++)
		{
			Asteroid[iLoopAsteroid].VectorList[iLoopAsteroidPoint] = AsteroidTemplate[iLoopAsteroidPoint][QRandom()>>7];
		}
		// Make sure the asteroid is closed...
		Asteroid[iLoopAsteroid].VectorList[iLoopAsteroidPoint] = Asteroid[iLoopAsteroid].VectorList[0];
		Asteroid[iLoopAsteroid].Scale=(QRandom()>>6)+2;
		Asteroid[iLoopAsteroid].RotationSpeed=(QRandom()>>5)+1;
		Asteroid[iLoopAsteroid].RotationAngle=QRandom();
	}

	iLoopX=0;
	iLoopY=0;
	iDirection=0;
	iScale=1;
	iAngle=0;
	iLoopQix=0;
	while (1)
	{

		iStartFrame=VBCounter;

		CreateBitmap((u16*)RugBitmap, 144, 112);

		DrawVectorObject((u16*)RugBitmap,Qix);

		// rotate the points up the Qix list...
		Qix.VectorList[iLoopQix].x = (((s16)QRandom())>>2)+(Qix.MovementVector.x>>5);
		Qix.VectorList[iLoopQix].y = (((s16)QRandom())>>2)+(Qix.MovementVector.y>>5);
		Qix.VectorList[iLoopQix].colour = 3;
		if (++iLoopQix >= Qix.Points)
		{
			iLoopQix = 0;
		}

		Qix.MovementVector.x++;
		Qix.MovementVector.y++;

		// Asteroid
		for (iLoopAsteroid=0;iLoopAsteroid<6;iLoopAsteroid++)
		{
			DrawVectorObject((u16*)RugBitmap,Asteroid[iLoopAsteroid]);
			Asteroid[iLoopAsteroid].RotationAngle+=Asteroid[iLoopAsteroid].RotationSpeed;
			// Need to do some bounds checking here...
			Asteroid[iLoopAsteroid].Position.x += Asteroid[iLoopAsteroid].MovementVector.x;
			if (Asteroid[iLoopAsteroid].Position.x < 128)
			{
				Asteroid[iLoopAsteroid].MovementVector.x = Asteroid[iLoopAsteroid].MovementVector.x*-1;
			}
			if (Asteroid[iLoopAsteroid].Position.x > 15700)
			{
				Asteroid[iLoopAsteroid].MovementVector.x = Asteroid[iLoopAsteroid].MovementVector.x*-1;
			}
			Asteroid[iLoopAsteroid].Position.y += Asteroid[iLoopAsteroid].MovementVector.y;
			if (Asteroid[iLoopAsteroid].Position.y < 128)
			{
				Asteroid[iLoopAsteroid].MovementVector.y = Asteroid[iLoopAsteroid].MovementVector.y*-1;
			}
			if (Asteroid[iLoopAsteroid].Position.y > 13672)
			{
				Asteroid[iLoopAsteroid].MovementVector.y = Asteroid[iLoopAsteroid].MovementVector.y*-1;
			}
		}

		// Ship Sprite
		iPoint = 0;
		iSpriteScale = 2;
		cSin = Sin(iAngle);
		cCos = Cos(iAngle);

		while (iPoint++<40)
		{

			if (PlayerSprite[iPoint].colour != 0)
			{

				iStartX = (PlayerSprite[iPoint].x*iSpriteScale)-8;
				iStartY = (PlayerSprite[iPoint].y*iSpriteScale)-8;

				// rotate point.
				iTempX = ((iStartX * cCos)>>7) - ((iStartY * cSin)>>7);
				iTempY = ((iStartX * cSin)>>7) + ((iStartY * cCos)>>7);

				// translate point back to it's original position:
				iStartX = 80+iTempX+8;
				iStartY = 32+iTempY+8;

				// Could we do something to scale the points
				// i.e. draw a 3 x 3 box instead of a single point?
				// oh shit. That actually works. Bit sloppy round the edges, but recognisable as the sprite...
				// Or we can try rotating each point seperately?
				// Which  actually looks slightly worse...
				// I guess the "ultimate" would be to express each pixel in the sprite as a circle of size iSpriteScale
				// Which also looks slightly worse. Who'd have thought, clunky and quick looks better than accurate and slow.
				// Overdrawing the individual pixels (by changing the iSpriteScale condition to iLoopX<=iSpriteScale looks a bit better for smaller scales, but not for larger
				// Ah. Overdrawing in one axis only actually looks pretty good at most scales...
				// Might be better if we overdraw vertically between NW and NE and SW and SE and horizontally between NE and SE and SW and NW?

				for (iLoopX=0;iLoopX<iSpriteScale;iLoopX++)
				{
					for (iLoopY=0;iLoopY<iSpriteScale;iLoopY++)
					{
						SetPixel((u16*)RugBitmap,(u8)(iStartX+iLoopX),(u8)(iStartY+iLoopY),PlayerSprite[iPoint].colour);
					}
				}
			}

		}

		// Then copy the bitmap back into tile memory...
		CopyBitmap((u16*)RugBitmap, bgTileBase);

		if (iDirection==0)
		{
			iScale++;
			if (iScale==64)
			{
				iDirection=1;
			}
		}
		if (iDirection==1)
		{
			iScale--;
			if (iScale==1)
			{
				iDirection=0;
			}
		}

		iAngle++;

		// How many frames has all of this taken...
		PrintString(SCR_1_PLANE, 0, 0, 18, "FPS:");
		PrintDecimal(SCR_1_PLANE, 0, 4, 18, 60/(VBCounter-iStartFrame), 2);

	}
}

void DrawVectorObject(u16 * BitmapAddress, VECTOROBJECT VectorObject)
{
	s16 iStartX;
	s16 iStartY;
	s16 iEndX;
	s16 iEndY;
	s16 iTempX;
	s16 iTempY;
	u8 iPoint = 0;
	s8 cSin;
	s8 cCos;

	cSin = Sin(VectorObject.RotationAngle);
	cCos = Cos(VectorObject.RotationAngle);

	iStartX = VectorObject.VectorList[0].x;
	iStartY = VectorObject.VectorList[0].y;

	while (iPoint++<VectorObject.Points)
	{

		// Modifier here is to find the centre of rotation
		iStartX = (iStartX*VectorObject.Scale)-VectorObject.Origin.x;
		iStartY = (iStartY*VectorObject.Scale)-VectorObject.Origin.y;

		// rotate point.
		iTempX = ((iStartX * cCos)>>7) - ((iStartY * cSin)>>7);
		iTempY = ((iStartX * cSin)>>7) + ((iStartY * cCos)>>7);

		// translate point back to it's original position:
		// Modify back from the centre of rotation above, and then add the X & Y co-ordinates
		iStartX = (VectorObject.Position.x>>7)+iTempX+VectorObject.Origin.x;
		if (iStartX<0) iStartX=0;
		iStartY = (VectorObject.Position.y>>7)+iTempY+VectorObject.Origin.y;
		if (iStartY<0) iStartY=0;

		iEndX = (VectorObject.VectorList[iPoint].x*VectorObject.Scale)-VectorObject.Origin.x;
		iEndY = (VectorObject.VectorList[iPoint].y*VectorObject.Scale)-VectorObject.Origin.y;

		// rotate point
		iTempX = ((iEndX * cCos)>>7) - ((iEndY * cSin)>>7);
		iTempY = ((iEndX * cSin)>>7) + ((iEndY * cCos)>>7);

		// translate point back:
		iEndX = (VectorObject.Position.x>>7)+iTempX+VectorObject.Origin.x;
		if (iEndX<0) iEndX=0;
		iEndY = (VectorObject.Position.y>>7)+iTempY+VectorObject.Origin.y;
		if (iEndY<0) iEndY=0;

		DrawLine((u16*)BitmapAddress,(u8)(iStartX),(u8)(iStartY),(u8)(iEndX),(u8)(iEndY),VectorObject.VectorList[iPoint].colour);

		iStartX = VectorObject.VectorList[iPoint].x;
		iStartY = VectorObject.VectorList[iPoint].y;

	}

}