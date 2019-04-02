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
	u16 RugBitmap[252][8];
	u8 iPoint;
	s16 iStartX;
	s16 iStartY;
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
	u16 iCounter;
	u8 iEngineLoop;
	VECTOROBJECT Asteroid[] = {
									{{6,6},{12192,8192},{128,3},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,-1},
									{{6,6},{12096,10096},{5,23},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+3},
									{{6,6},{2048,10192},{8,9},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},1,0,+3},
									{{6,6},{12192,40098},{64,-23},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,-2},
									{{6,6},{8192,2048},{-45,23},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},1,0,+3},
									{{6,6},{2048,2048},{-12,-12},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+1},
									{{6,6},{10432,2048},{-12,-12},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+1},
									{{6,6},{2048,7634},{-12,-12},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+1},
									{{6,6},{2048,7763},{-12,-12},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+1},
									{{6,6},{2048,2048},{-12,-12},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+1},
									{{6,6},{2048,10048},{-12,-12},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+1},
									{{6,6},{12432,2048},{-12,-12},15,{{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1},{0,0,1}},2,0,+1}
								};
	VECTOROBJECT PlayerOne;

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
	for (iLoopAsteroid=0;iLoopAsteroid<4;iLoopAsteroid++)
	{
		for (iLoopAsteroidPoint=0;iLoopAsteroidPoint<Asteroid[iLoopAsteroid].Points-1;iLoopAsteroidPoint++)
		{
			Asteroid[iLoopAsteroid].VectorList[iLoopAsteroidPoint] = AsteroidTemplate[iLoopAsteroidPoint][QRandom()>>7];
		}
		// Make sure the asteroid is closed...
		Asteroid[iLoopAsteroid].VectorList[iLoopAsteroidPoint] = Asteroid[iLoopAsteroid].VectorList[0];
		Asteroid[iLoopAsteroid].Scale=3;
		Asteroid[iLoopAsteroid].RotationSpeed=(QRandom()>>4)+1;
		Asteroid[iLoopAsteroid].RotationAngle=Cos(QRandom());
		//Asteroid[iLoopAsteroid].Position.x=((u16)QRandom())<<4;
		//Asteroid[iLoopAsteroid].Position.y=((u16)QRandom())<<4;
		Asteroid[iLoopAsteroid].MovementVector.x=QRandom();
		Asteroid[iLoopAsteroid].MovementVector.y=QRandom();


	}

	// Set up the player
	PlayerOne.Position.x = 72;
	PlayerOne.Position.y = 66;
	PlayerOne.MovementVector.x = 0;
	PlayerOne.MovementVector.y = 0;
	PlayerOne.Scale = 2;
	PlayerOne.Origin.x = (PlayerOne.Scale * 4);
	PlayerOne.Origin.y = (PlayerOne.Scale * 8);
	PlayerOne.Points = 40;
	PlayerOne.RotationAngle = 0;
	PlayerOne.RotationSpeed = 0;
	iLoopX=0;
	while (iLoopX++<PlayerOne.Points)
	{
		PlayerOne.VectorList[iLoopX].x = PlayerSprite[iLoopX].x;
		PlayerOne.VectorList[iLoopX].y = PlayerSprite[iLoopX].y;
		PlayerOne.VectorList[iLoopX].colour = PlayerSprite[iLoopX].colour;
	}

	iLoopX=0;
	iLoopY=0;
	iLoopQix=0;
	iCounter=0;
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

		//Qix.MovementVector.x++;
		//Qix.MovementVector.y++;

		// Asteroid
		// This hangs after "exactly" 77 frames? Every time?
		// Regardless of the number of asteroids, or the rotation speeds or anything?
		for (iLoopAsteroid=0;iLoopAsteroid<4;iLoopAsteroid++)
		{
			DrawVectorObject((u16*)RugBitmap,Asteroid[iLoopAsteroid]);
			Asteroid[iLoopAsteroid].RotationAngle+=Asteroid[iLoopAsteroid].RotationSpeed;
			// Need to do some bounds checking here...
			Asteroid[iLoopAsteroid].Position.x += Asteroid[iLoopAsteroid].MovementVector.x;
			if (Asteroid[iLoopAsteroid].Position.x < 2000)
			{
				Asteroid[iLoopAsteroid].MovementVector.x = Asteroid[iLoopAsteroid].MovementVector.x*-1;
			}
			if (Asteroid[iLoopAsteroid].Position.x > 14000)
			{
				Asteroid[iLoopAsteroid].MovementVector.x = Asteroid[iLoopAsteroid].MovementVector.x*-1;
			}
			//Asteroid[iLoopAsteroid].Position.y += Asteroid[iLoopAsteroid].MovementVector.y;
			if (Asteroid[iLoopAsteroid].Position.y < 2000)
			{
				Asteroid[iLoopAsteroid].MovementVector.y = Asteroid[iLoopAsteroid].MovementVector.y*-1;
			}
			if (Asteroid[iLoopAsteroid].Position.y > 12000)
			{
				Asteroid[iLoopAsteroid].MovementVector.y = Asteroid[iLoopAsteroid].MovementVector.y*-1;
			}
		}

		// Ship Sprite
		if (JOYPAD & J_LEFT) PlayerOne.RotationAngle-=8;
		if (JOYPAD & J_RIGHT) PlayerOne.RotationAngle+=8;
		if (JOYPAD & J_UP)
		{
			// Modify the movement vector by the angle.
			PlayerOne.MovementVector.x += (Cos(PlayerOne.RotationAngle+192));
			PlayerOne.MovementVector.y += (Sin(PlayerOne.RotationAngle+192));
		}
		// Bounds checking? How do I constrain the player within the visible screen without breaking the immersion?
		// Bitmap address [0] contains the bitmap width, and address[1] the bitmap height. So we should be able to use that?
		PlayerOne.Position.x += PlayerOne.MovementVector.x>>7;
		if (PlayerOne.Position.x<0-(PlayerOne.Scale * PlayerOne.Origin.x)) PlayerOne.Position.x=((u16*)RugBitmap)[0]+(PlayerOne.Scale * PlayerOne.Origin.x);
		if (PlayerOne.Position.x>(((u16*)RugBitmap)[0])+(PlayerOne.Scale * PlayerOne.Origin.x)) PlayerOne.Position.x=0-(PlayerOne.Scale * PlayerOne.Origin.x);
		PlayerOne.Position.y += PlayerOne.MovementVector.y>>7;
		if (PlayerOne.Position.y<0-(PlayerOne.Scale * PlayerOne.Origin.y)) PlayerOne.Position.y=((u16*)RugBitmap)[1]+(PlayerOne.Scale * PlayerOne.Origin.y);
		if (PlayerOne.Position.y>((u16*)RugBitmap)[1]+(PlayerOne.Scale * PlayerOne.Origin.y)) PlayerOne.Position.y=0-(PlayerOne.Scale * PlayerOne.Origin.y);
		// Draw some random engine noise...
		iEngineLoop=PlayerOne.Points-5;
		while (iEngineLoop<PlayerOne.Points)
		{
			PlayerOne.VectorList[iEngineLoop+1].colour = 0;
			if (QRandom()>128) PlayerOne.VectorList[iEngineLoop+1].colour = 3;
			iEngineLoop++;
		}
		// And then draw the sprite
		DrawVectorSprite((u16*)RugBitmap, PlayerOne);

		// Then copy the bitmap back into tile memory...
		CopyBitmap((u16*)RugBitmap, bgTileBase);

		// How many frames has all of this taken...
		PrintString(SCR_1_PLANE, 0, 0, 18, "FPS:");
		PrintDecimal(SCR_1_PLANE, 0, 4, 18, 60/(VBCounter-iStartFrame), 2);

		PrintDecimal(SCR_1_PLANE, 0, 8, 18, PlayerOne.Position.x, 3);
		PrintDecimal(SCR_1_PLANE, 0, 12, 18, PlayerOne.Position.y, 8);

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
		//if (iStartX<0) iStartX=0;
		iStartY = (VectorObject.Position.y>>7)+iTempY+VectorObject.Origin.y;
		//if (iStartY<0) iStartY=0;

		iEndX = (VectorObject.VectorList[iPoint].x*VectorObject.Scale)-VectorObject.Origin.x;
		iEndY = (VectorObject.VectorList[iPoint].y*VectorObject.Scale)-VectorObject.Origin.y;

		// rotate point
		iTempX = ((iEndX * cCos)>>7) - ((iEndY * cSin)>>7);
		iTempY = ((iEndX * cSin)>>7) + ((iEndY * cCos)>>7);

		// translate point back:
		iEndX = (VectorObject.Position.x>>7)+iTempX+VectorObject.Origin.x;
		//if (iEndX<0) iEndX=0;
		iEndY = (VectorObject.Position.y>>7)+iTempY+VectorObject.Origin.y;
		//if (iEndY<0) iEndY=0;

		DrawLine((u16*)BitmapAddress,(u8)(iStartX),(u8)(iStartY),(u8)(iEndX),(u8)(iEndY),VectorObject.VectorList[iPoint].colour);

		iStartX = VectorObject.VectorList[iPoint].x;
		iStartY = VectorObject.VectorList[iPoint].y;

	}

}

void DrawVectorSprite(u16 * BitmapAddress, VECTOROBJECT VectorObject)
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
	u8 iLoopX;
	u8 iLoopY;

	cSin = Sin(VectorObject.RotationAngle);
	cCos = Cos(VectorObject.RotationAngle);

	iStartX = VectorObject.VectorList[0].x;
	iStartY = VectorObject.VectorList[0].y;

	while (iPoint++<VectorObject.Points)
	{

		if (PlayerSprite[iPoint].colour != 0)
		{

			iStartX = (PlayerSprite[iPoint].x*VectorObject.Scale)-VectorObject.Origin.x;
			iStartY = (PlayerSprite[iPoint].y*VectorObject.Scale)-VectorObject.Origin.y;

			// rotate point.
			iTempX = ((iStartX * cCos)>>7) - ((iStartY * cSin)>>7);
			iTempY = ((iStartX * cSin)>>7) + ((iStartY * cCos)>>7);

			// translate point back to it's original position:
			iStartX = VectorObject.Position.x+iTempX; //+VectorObject.Origin.x;
			iStartY = VectorObject.Position.y+iTempY; //+VectorObject.Origin.y;

			// Quick and dirty method to scale up the individual points
			for (iLoopX=0;iLoopX<VectorObject.Scale;iLoopX++)
			{
				for (iLoopY=0;iLoopY<VectorObject.Scale;iLoopY++)
				{
					SetPixel((u16*)BitmapAddress,(u8)(iStartX+iLoopX),(u8)(iStartY+iLoopY),VectorObject.VectorList[iPoint].colour);
				}
			}
		}

	}
}
