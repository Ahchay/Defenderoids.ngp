#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"

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

	InitNGPC();
	SysSetSystemFont();

	// So, create a bitmap...

	SetBackgroundColour(0);

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,0,0), RGB(0,15,0), RGB(0,0,15));

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

		iStartX=DefenderoidShip[0][0];
		iStartY=DefenderoidShip[0][1];
		iPoint = 0;

		while (iPoint++<4)
		{
			//PrintDecimal(SCR_1_PLANE, 0, 0, iPoint + 12, iPoint, 4);
			//PrintDecimal(SCR_1_PLANE, 0, 6, iPoint + 12, iStartX, 4);
			//PrintDecimal(SCR_1_PLANE, 0, 12, iPoint + 12, iStartY, 4);

			// Start with a border
			DrawLine((u16*)RugBitmap,(u8)((iStartX)*63),(u8)((iStartY)*63),(u8)((DefenderoidShip[iPoint][0])*63),(u8)((DefenderoidShip[iPoint][1])*63),1);

			// Can we scale this easily?
			DrawLine((u16*)RugBitmap,(u8)(1+(iStartX)*iScale),(u8)(1+(iStartY)*iScale),(u8)(1+(DefenderoidShip[iPoint][0])*iScale),(u8)(1+(DefenderoidShip[iPoint][1])*iScale),2);

			// Or rotate? Using integer maths?
			// Library C functions return a signed integer range, not a "true" sin/cos
			cSin = Sin(iAngle);
			cCos = Cos(iAngle);

			// translate point back to origin:
			iStartX = (iStartX<<3)-8;
			iStartY = (iStartY<<3)-8;

			// rotate point.
			// The calculation here is xpos = (xpos * cos(theta)) - (ypos * sin(theta))
			// Because our sin/cos table is an integer table and not the "true" sin/cos
			// we need to do each side of that calculation and then divide down by 128 (or right shift 7 points)
			// before adding/subtracting the two results
			iTempX = ((iStartX * cCos)>>7) - ((iStartY * cSin)>>7);
			iTempY = ((iStartX * cSin)>>7) + ((iStartY * cCos)>>7);

			// translate point back:
			iStartX = iTempX + 8;
			iStartY = iTempY + 8;

			// translate point back to origin:
			iEndX = (DefenderoidShip[iPoint][0]<<3)-8;
			iEndY = (DefenderoidShip[iPoint][1]<<3)-8;

			// rotate point
			iTempX = ((iEndX * cCos)>>7) - ((iEndY * cSin)>>7);
			iTempY = ((iEndX * cSin)>>7) + ((iEndY * cCos)>>7);

			// translate point back:
			iEndX = iTempX + 8;
			iEndY = iTempY + 8;

			DrawLine((u16*)RugBitmap,(u8)(1+((126-16)>>1)+(iStartX)),(u8)(1+((126-16)>>1)+(iStartY)),(u8)(1+((126-16)>>1)+(iEndX)),(u8)(1+((126-16)>>1)+(iEndY)),3);

			iStartX = DefenderoidShip[iPoint][0];
			iStartY = DefenderoidShip[iPoint][1];

		}

		// Then copy the bitmap back into tile memory...
		CopyBitmap((u16*)RugBitmap, bgTileBase);

		if (iDirection==0)
		{
			iScale++;
			if (iScale==62)
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
		PrintDecimal(SCR_1_PLANE, 0, 0, 18, VBCounter-iStartFrame, 6);
		// Five or six with one static box, one scaled box and one rotated box
		// Interestingly, creating with only a static box only takes it down to 4 or five blanks per frame?
		// Implying that maybe it's the "create and copy bitmap" that's taking a significant amount of that time.
		// We know that PutPixel is a bit slow as well, so the longer the line the longer it will take...

		//DrawBitmap((u16 *)bmpBackground, 160, 96, 0, 2);
	}
}

