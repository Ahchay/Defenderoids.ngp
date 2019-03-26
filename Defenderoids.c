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
	u8 iTile;
	u8 iMainLoop;
	u8 iDirection;
	u16 RugBitmap[257][8];
	u8 iPoint;
	u16 iStartX;
	u16 iStartY;
	u8 iScale;
	u8 iAngle;
	u16 iEndX;
	u16 iEndY;
	u16 iTempX;
	u16 iTempY;
	u16 iStartFrame;
	s8 cSin;
	s8 cCos;
	u8 iSpriteScale;
	VECTOROBJECT Qix;
	u8 iLoopQix;
	VECTOROBJECT Asteroid = {{6,6},{24,87},11,{{2,2,1},{4,0,1},{6,2,1},{10,0,1},{12,4,1},{8,8,1},{10,10,1},{6,12,1},{4,8,1},{0,6,1},{4,4,1},{2,2,1}},2,0,-12};

	InitNGPC();
	SysSetSystemFont();
	InitialiseQRandom();

	// So, create a bitmap...

	SetBackgroundColour(0);

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

	CreateBitmap((u16*)RugBitmap, 128, 128);
	CopyBitmap((u16*)RugBitmap, bgTileBase);

	iTile=0;

	//Copy the bitmap to SCR_1_PLANE
	// Watch the order...
	for (iLoopY=0;iLoopY<16;iLoopY++)
	{
		for (iLoopX=0;iLoopX<16;iLoopX++)
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
		Qix.VectorList[iLoopQix].x = (QRandom()>>2);
		Qix.VectorList[iLoopQix].y = (QRandom()>>2);
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

		CreateBitmap((u16*)RugBitmap, 128, 128);

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
		Qix.VectorList[iLoopQix].x = (QRandom()>>2);
		Qix.VectorList[iLoopQix].y = (QRandom()>>2);
		Qix.VectorList[iLoopQix].colour = 3;


		// Line version...
		/*
		iStartX=Box[0].x;
		iStartY=Box[0].y;
		iPoint = 1;

		cSin = Sin(iAngle);
		cCos = Cos(iAngle);

		while (iPoint++<4)
		{
			//PrintDecimal(SCR_1_PLANE, 0, 0, iPoint + 12, iPoint, 4);
			//PrintDecimal(SCR_1_PLANE, 0, 6, iPoint + 12, iStartX, 4);
			//PrintDecimal(SCR_1_PLANE, 0, 12, iPoint + 12, iStartY, 4);

			// Start with a border
			DrawLine((u16*)RugBitmap,(u8)((iStartX)*127),(u8)((iStartY)*127),(u8)((Box[iPoint].x)*127),(u8)((Box[iPoint].y)*127),1);

			// Scale is relatively easy
			DrawLine((u16*)RugBitmap,(u8)(1+(iStartX)*iScale),(u8)(1+(iStartY)*iScale),(u8)(1+(Box[iPoint].x)*iScale),(u8)(1+(Box[iPoint].y)*iScale),2);

			// Rotation needs a bit more calculation
			// Library C functions return a signed integer range, not a "true" sin/cos
			// translate point to an arbitary origin:
			// This acts as the centre of rotation, scaling done here too.
			iStartX = (iStartX<<3)-16;
			iStartY = (iStartY<<3)-16;

			// rotate point.
			// The calculation here should be xpos = (xpos * cos(theta)) - (ypos * sin(theta))
			// Because our sin/cos table is an integer table and not the "true" sin/cos
			// we need to do each side of that calculation and then divide down by 128 (or right shift 7 points)
			// before adding/subtracting the two results

			iTempX = ((iStartX * cCos)>>7) - ((iStartY * cSin)>>7);
			iTempY = ((iStartX * cSin)>>7) + ((iStartY * cCos)>>7);

			// translate point back to it's original position:
			iStartX = iTempX + 16;
			iStartY = iTempY + 16;

			// translate point back to origin:
			iEndX = (Box[iPoint].x<<3)-16;
			iEndY = (Box[iPoint].y<<3)-16;

			// rotate point
			iTempX = ((iEndX * cCos)>>7) - ((iEndY * cSin)>>7);
			iTempY = ((iEndX * cSin)>>7) + ((iEndY * cCos)>>7);

			// translate point back:
			iEndX = iTempX + 16;
			iEndY = iTempY + 16;

			DrawLine((u16*)RugBitmap,(u8)(1+((126-16)>>1)+(iStartX)),(u8)(1+((126-16)>>1)+(iStartY)),(u8)(1+((126-16)>>1)+(iEndX)),(u8)(1+((126-16)>>1)+(iEndY)),3);


			iStartX = Box[iPoint].x;
			iStartY = Box[iPoint].y;
		}
		*/

		// Asteroid
		DrawVectorObject((u16*)RugBitmap,Asteroid);
		Asteroid.RotationAngle+=Asteroid.RotationSpeed;

		// Ship Sprite
		iPoint = 0;
		iSpriteScale = 2;
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
				iStartX = 87+iTempX+8;
				iStartY = 87+iTempY+8;

				// Could we do something to scale the points
				// i.e. draw a 3 x 3 box instead of a single point?
				// oh shit. That actually works. Bit sloppy round the edges, but recognisable as the sprite...
				// Or we can try rotating each point seperately?
				// Which  actually looks slightly worse...
				// I guess the "ultimate" would be to express each pixel in the sprite as a circle of size iSpriteScale
				// Which also looks slightly worse. Who'd have thought, clunky and quick looks better than accurate and slow.

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

		// Five or six with one static box, one scaled box and one rotated box
		// Interestingly, creating with only a static box only takes it down to 4 or five blanks per frame?
		// Implying that maybe it's the "create and copy bitmap" that's taking a significant amount of that time.
		// We know that PutPixel is a bit slow as well, so the longer the line the longer it will take...
		// 1 or 2 frames if we use individual pixel maps?
		// Could I do "proper" rotatable sprites like this?

		//DrawBitmap((u16 *)bmpBackground, 160, 96, 0, 2);
	}
}

void DrawVectorObject(u16 * BitmapAddress, VECTOROBJECT VectorObject)
{
	u16 iStartX;
	u16 iStartY;
	u16 iEndX;
	u16 iEndY;
	u16 iTempX;
	u16 iTempY;
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
		iStartY = VectorObject.Position.y+iTempY+VectorObject.Origin.y;

		iEndX = (VectorObject.VectorList[iPoint].x*VectorObject.Scale)-VectorObject.Origin.x;
		iEndY = (VectorObject.VectorList[iPoint].y*VectorObject.Scale)-VectorObject.Origin.y;

		// rotate point
		iTempX = ((iEndX * cCos)>>7) - ((iEndY * cSin)>>7);
		iTempY = ((iEndX * cSin)>>7) + ((iEndY * cCos)>>7);

		// translate point back:
		iEndX = VectorObject.Position.x+iTempX+VectorObject.Origin.x;
		iEndY = VectorObject.Position.y+iTempY+VectorObject.Origin.y;

		DrawLine((u16*)BitmapAddress,(u8)(iStartX),(u8)(iStartY),(u8)(iEndX),(u8)(iEndY),VectorObject.VectorList[iPoint].colour);

		iStartX = VectorObject.VectorList[iPoint].x;
		iStartY = VectorObject.VectorList[iPoint].y;

	}

}