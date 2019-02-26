#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"
u16 bmpBackground[240][8];

void DefenderoidsMain()
{
	// Define variables
	u8 iLoopX;
	u8 iLoopY;
	u8 iTile;
	u8 iMainLoop;
	u8 iDirection;
	u16 RugBitmap[128];

	InitNGPC();
	SysSetSystemFont();

	// So, create a bitmap...

	SetBackgroundColour(0);

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,0,0), RGB(0,15,0), RGB(0,0,15));

	CreateBitmap((u16*)RugBitmap, 64, 64);
	CopyBitmap((u16*)RugBitmap, bgTileBase);

	iTile=0;

	//Copy the bitmap to SCR_1_PLANE
	// Watch the order...
	for (iLoopY=0;iLoopY<8;iLoopY++)
	{
		for (iLoopX=0;iLoopX<8;iLoopX++)
		{
			PutTile(SCR_1_PLANE, 0, iLoopX, iLoopY, bgTileBase+iTile);
			iTile++;
		}
	}

	iLoopX=0;
	iLoopY=0;
	iDirection=1;
	while (1)
	{
		CreateBitmap((u16*)RugBitmap, 64, 64);
		if (iDirection==0)
		{
			if (iLoopX==63)
			{
				iDirection=1;
			}
			else
			{
				iLoopX++;
				iLoopY++;
			}
		}
		else
		{
			if (iLoopX==0)
			{
				iDirection=0;
			}
			else
			{
				iLoopX--;
				iLoopY--;
			}
		}
		// just draw some static lines...
		DrawLine((u16*)RugBitmap,iLoopX,iLoopY,63-iLoopX,63-iLoopY,1);
		DrawLine((u16*)RugBitmap,iLoopX,0,0,iLoopY,2);
		DrawLine((u16*)RugBitmap,0,iLoopX>>1,iLoopY>>3,15,3);
		DrawLine((u16*)RugBitmap,3,5,18,28,3);
		// Then copy the bitmap back into tile memory...
		CopyBitmap((u16*)RugBitmap, bgTileBase);

		Sleep(1);

		//DrawBitmap((u16 *)bmpBackground, 160, 96, 0, 2);
	}
}