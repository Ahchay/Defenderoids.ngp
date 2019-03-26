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
	u16 RugBitmap[257][8];
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
	VECTOROBJECT Asteroid[] = {
									{{6,6},{24,87},11,{{2,2,1},{4,0,1},{6,2,1},{10,0,1},{12,4,1},{8,8,1},{10,10,1},{6,12,1},{4,8,1},{0,6,1},{4,4,1},{2,2,1}},2,0,-12},
									{{6,6},{82,12},11,{{2,2,1},{4,0,1},{6,2,1},{10,0,1},{12,4,1},{8,8,1},{10,10,1},{6,12,1},{4,8,1},{0,6,1},{4,4,1},{2,2,1}},2,0,+3},
									{{6,6},{13,16},11,{{2,2,1},{4,0,1},{6,2,1},{10,0,1},{12,4,1},{8,8,1},{10,10,1},{6,12,1},{4,8,1},{0,6,1},{4,4,1},{2,2,1}},2,0,-2},
									{{6,6},{87,87},11,{{2,2,1},{4,0,1},{6,2,1},{10,0,1},{12,4,1},{8,8,1},{10,10,1},{6,12,1},{4,8,1},{0,6,1},{4,4,1},{2,2,1}},2,0,+1}
								};

	InitNGPC();
	SysSetSystemFont();
	InitialiseQRandom();

	// So, create a bitmap...

	SetBackgroundColour(0);

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

	CreateBitmap((u16*)RugBitmap, 144, 128);
	CopyBitmap((u16*)RugBitmap, bgTileBase);

	iTile=0;

	//Copy the bitmap to SCR_1_PLANE
	// Watch the order...
	for (iLoopY=0;iLoopY<16;iLoopY++)
	{
		for (iLoopX=0;iLoopX<18;iLoopX++)
		{
			PutTile(SCR_1_PLANE, 0, 2 + iLoopX, 1 + iLoopY, bgTileBase+iTile);
			iTile++;
		}
	}

	// Setup the qix object
	Qix.Origin.x=0;
	Qix.Origin.y=0;
	Qix.Position.x=32;
	Qix.Position.y=32;
	Qix.Points=12;
	Qix.Scale=1;
	Qix.RotationAngle=0;
	Qix.RotationSpeed=0;
	for (iLoopQix=0;iLoopQix<Qix.Points;iLoopQix++)
	{
		Qix.VectorList[iLoopQix].x = (QRandom()>>3);
		Qix.VectorList[iLoopQix].y = (QRandom()>>3);
		Qix.VectorList[iLoopQix].colour = 3;
	}

	// Create a shape object
	// Should be sort of asteroid-shipy shaped, but for now, it's a box.

	iLoopX=0;
	iLoopY=0;
	iDirection=0;
	iScale=1;
	iAngle=0;
	while (1)
	{

		iStartFrame=VBCounter;

		CreateBitmap((u16*)RugBitmap, 144, 128);

		// Line version...
		/*
		iStartX=Qix[0].x;
		iStartY=Qix[0].y;
		iPoint = 0;

		//Draw a qix
		while (iPoint++<12)
		{
			DrawLine((u16*)RugBitmap,(u8)((iStartX)),(u8)((iStartY)),(u8)((Qix[iPoint].x)),(u8)((Qix[iPoint].y)),Qix[iPoint].colour);

			iStartX = Qix[iPoint].x;
			iStartY = Qix[iPoint].y;
		}
		*/

		DrawVectorObject((u16*)RugBitmap,Qix);

		// rotate the points up the Qix list...
		for (iLoopQix=0;iLoopQix<Qix.Points;iLoopQix++)
		{
			Qix.VectorList[iLoopQix].x = Qix.VectorList[iLoopQix+1].x;
			Qix.VectorList[iLoopQix].y = Qix.VectorList[iLoopQix+1].y;
			Qix.VectorList[iLoopQix].colour = 3;
		}
		Qix.VectorList[iLoopQix].x = (QRandom()>>3);
		Qix.VectorList[iLoopQix].y = (QRandom()>>3);
		Qix.VectorList[iLoopQix].colour = 3;

		// Asteroid
		for (iLoopAsteroid=0;iLoopAsteroid<4;iLoopAsteroid++)
		{
			DrawVectorObject((u16*)RugBitmap,Asteroid[iLoopAsteroid]);
			Asteroid[iLoopAsteroid].RotationAngle+=Asteroid[iLoopAsteroid].RotationSpeed;
		}

		// Ship Sprite
		iPoint = 0;
		iSpriteScale = 4;
		cSin = Sin(iAngle);
		cCos = Cos(iAngle);

		while (iPoint++<63)
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

				for (iLoopX=0;iLoopX<iSpriteScale;iLoopX++)
				{
					for (iLoopY=0;iLoopY<=iSpriteScale;iLoopY++)
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
			if (iScale==126)
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
		iStartX = VectorObject.Position.x+iTempX+VectorObject.Origin.x;
		if (iStartX<0) iStartX=0;
		iStartY = VectorObject.Position.y+iTempY+VectorObject.Origin.y;
		if (iStartY<0) iStartY=0;

		iEndX = (VectorObject.VectorList[iPoint].x*VectorObject.Scale)-VectorObject.Origin.x;
		iEndY = (VectorObject.VectorList[iPoint].y*VectorObject.Scale)-VectorObject.Origin.y;

		// rotate point
		iTempX = ((iEndX * cCos)>>7) - ((iEndY * cSin)>>7);
		iTempY = ((iEndX * cSin)>>7) + ((iEndY * cCos)>>7);

		// translate point back:
		iEndX = VectorObject.Position.x+iTempX+VectorObject.Origin.x;
		if (iEndX<0) iEndX=0;
		iEndY = VectorObject.Position.y+iTempY+VectorObject.Origin.y;
		if (iEndY<0) iEndY=0;

		DrawLine((u16*)BitmapAddress,(u8)(iStartX),(u8)(iStartY),(u8)(iEndX),(u8)(iEndY),VectorObject.VectorList[iPoint].colour);

		iStartX = VectorObject.VectorList[iPoint].x;
		iStartY = VectorObject.VectorList[iPoint].y;

	}

}