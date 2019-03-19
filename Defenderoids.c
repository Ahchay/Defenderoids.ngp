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
	// Should be sort of asteroid-shipy shaped

	iLoopX=0;
	iLoopY=0;
	iDirection=0;
	iScale=1;
	while (1)
	{
		CreateBitmap((u16*)RugBitmap, 128, 128);

		iStartX=DefenderoidShip[0][0];
		iStartY=DefenderoidShip[0][1];
		iPoint = 0;

		while (iPoint++<4)
		{
			//PrintDecimal(SCR_1_PLANE, 0, 0, iPoint + 12, iPoint, 4);
			//PrintDecimal(SCR_1_PLANE, 0, 6, iPoint + 12, iStartX, 4);
			//PrintDecimal(SCR_1_PLANE, 0, 12, iPoint + 12, iStartY, 4);

			// Can we scale this easily?
			DrawLine((u16*)RugBitmap,(u8)(1+((126-iScale)>>1)+(iStartX)*iScale),(u8)(1+((126-iScale)>>1)+(iStartY)*iScale),(u8)(1+((126-iScale)>>1)+(DefenderoidShip[iPoint][0])*iScale),(u8)(1+((126-iScale)>>1)+(DefenderoidShip[iPoint][1])*iScale),2);

			DrawLine((u16*)RugBitmap,(u8)((iStartX)*127),(u8)((iStartY)*127),(u8)((DefenderoidShip[iPoint][0])*127),(u8)((DefenderoidShip[iPoint][1])*127),1);

			// Or rotate?

			iStartX = DefenderoidShip[iPoint][0];
			iStartY = DefenderoidShip[iPoint][1];

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

		PrintDecimal(SCR_1_PLANE, 0, 0, 18, iScale, 3);

		//Sleep(1);

		//DrawBitmap((u16 *)bmpBackground, 160, 96, 0, 2);
	}
}