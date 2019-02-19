#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"
u16 bmpBackground[240][8];

void DefenderoidsMain()
{
	// Define variables
	u8 iLoopX;
	u8 iLoopY;

	// So, create a bitmap...

	CreateBitmap((u16 *)bmpBackground, 160,96);
	//Copy the bitmap tiles into tile memory and blatt them onto the screen.
	for (iLoopX=0;iLoopX<20;iLoopX++)
	{
		for (iLoopY=0;iLoopY<12;iLoopY++)
		{
			PutBitmapTile((u16 *)bmpBackground, (iLoopY*20)+iLoopX, bgTileBase+(iLoopY*20)+iLoopX);
			PutTile(SCR_1_PLANE, 0, iLoopX, iLoopY+4, bgTileBase+(iLoopY*20)+iLoopX);
		}
	}

	while (1);
}