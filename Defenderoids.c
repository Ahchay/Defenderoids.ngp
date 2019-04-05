#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"

/*

So, this started as a defender/asteroids mash-up concept, but has now turned into a bitmap playground...

Not sure where I'm going with it yet, but it's entertaining enough

*/

bool DefenderoidsLogo()
{
	bool bGameStart;
	// MUST be big enough for the bitmap (x*y) and 16 bytes for the bitmap header.
	u16 bmpLogo[2032];
	u8 iLoopY;
	u8 iLoopX;
	u8 iLoopLetter;
	u8 iTile;
	u8 iSourceLetter;
	u8 iSourcePoint;
	VECTOROBJECT GameLogo[] = {
								{{0,0},{4000,4000},{0,0},0,0,1,0,0}, //D
								{{0,0},{5000,4000},{0,0},1,0,1,0,0}, //e
								{{0,0},{6000,4000},{0,0},2,0,1,0,0}, //f
								{{0,0},{7000,4000},{0,0},1,0,1,0,0}, //e
								{{0,0},{8000,4000},{0,0},3,0,1,0,0}, //n
								{{0,0},{9000,4000},{0,0},4,0,1,0,0}, //d
								{{0,0},{10000,4000},{0,0},1,0,1,0,0}, //e
								{{0,0},{11000,4000},{0,0},5,0,1,0,0}, //r
								{{0,0},{12000,4000},{0,0},6,0,1,0,0}, //o
								{{0,0},{13000,4000},{0,0},7,0,1,0,0}, //i
								{{0,0},{14000,4000},{0,0},4,0,1,0,0}, //d
								{{0,0},{15000,4000},{0,0},8,0,1,0,0} //s
							};

	// Display a logo animation and wait for the player to push the start button.
	// Can also use this to init() the randomiser so that the player gets different starting objects when they do
	// start the game.

	//Copy my template "alphabet" objects into the GameLogo array
	//The "points" member will contain the soruce object for the letter in the template array

	for (iLoopLetter=0;iLoopLetter<12;iLoopLetter++)
	{
		iSourceLetter=GameLogo[iLoopLetter].Points;
		// Copy the relevant bits (Origin, Points and Vectorlist from the template
		GameLogo[iLoopLetter].Origin.x = Alphabet[iSourceLetter].Origin.x;
		GameLogo[iLoopLetter].Origin.y = Alphabet[iSourceLetter].Origin.y;
		GameLogo[iLoopLetter].Points = Alphabet[iSourceLetter].Points;
		for (iSourcePoint=0;iSourcePoint<Alphabet[iSourceLetter].Points;iSourcePoint++)
		{
			GameLogo[iLoopLetter].VectorList[iSourcePoint].x = Alphabet[iSourceLetter].VectorList[iSourcePoint].x;
			GameLogo[iLoopLetter].VectorList[iSourcePoint].y = Alphabet[iSourceLetter].VectorList[iSourcePoint].y;
			GameLogo[iLoopLetter].VectorList[iSourcePoint].colour = Alphabet[iSourceLetter].VectorList[iSourcePoint].colour;
		}
		GameLogo[iLoopLetter].Scale=2;
		GameLogo[iLoopLetter].RotationSpeed=(Sin(QRandom())>>7)+1;
		GameLogo[iLoopLetter].RotationAngle=0;
		GameLogo[iLoopLetter].MovementVector.x=QRandom()>>4;
		GameLogo[iLoopLetter].MovementVector.y=QRandom()>>4;

	}


	// Wait for the "A" button to be released
	while (JOYPAD & J_A);

	bGameStart=0;

	CreateBitmap((u16*)bmpLogo, 144, 112);
	CopyBitmap((u16*)bmpLogo, bgTileBase);

	iTile=0;
	for (iLoopY=0;iLoopY<14;iLoopY++)
	{
		for (iLoopX=0;iLoopX<18;iLoopX++)
		{
			PutTile(SCR_1_PLANE, 0, 1 + iLoopX, 1 + iLoopY, bgTileBase+iTile);
			iTile++;
		}
	}

	while (!bGameStart)
	{

		CreateBitmap((u16*)bmpLogo, 144, 112);

		// do stuff?
		/*
		for (iLoopY=0;iLoopY<112;iLoopY+=3)
		{
			for (iLoopX=0;iLoopX<144;iLoopX+=3)
			{
				SetPixel((u16*)bmpLogo,(u8)(iLoopX),(u8)(iLoopY),QRandom()>>6);
			}
		}
		*/

		for (iLoopLetter=0;iLoopLetter<12;iLoopLetter++)
		{
			DrawVectorObject((u16*)bmpLogo,GameLogo[iLoopLetter]);
			GameLogo[iLoopLetter].RotationAngle+=GameLogo[iLoopLetter].RotationSpeed;
			// Need to do some bounds checking here...
			GameLogo[iLoopLetter].Position.x += GameLogo[iLoopLetter].MovementVector.x;
			if (GameLogo[iLoopLetter].Position.x < 3000)
			{
				GameLogo[iLoopLetter].MovementVector.x = GameLogo[iLoopLetter].MovementVector.x*-1;
			}
			if (GameLogo[iLoopLetter].Position.x > 14000)
			{
				GameLogo[iLoopLetter].MovementVector.x = GameLogo[iLoopLetter].MovementVector.x*-1;
			}
			//Asteroid[iLoopAsteroid].Position.y += Asteroid[iLoopAsteroid].MovementVector.y;
			if (GameLogo[iLoopLetter].Position.y < 3000)
			{
				GameLogo[iLoopLetter].MovementVector.y = GameLogo[iLoopLetter].MovementVector.y*-1;
			}
			if (GameLogo[iLoopLetter].Position.y > 12000)
			{
				GameLogo[iLoopLetter].MovementVector.y = GameLogo[iLoopLetter].MovementVector.y*-1;
			}

			if (JOYPAD & J_A)
			{
				bGameStart=1;
				break;
			}
		}

		// Then copy the bitmap back into tile memory...
		CopyBitmap((u16*)bmpLogo, bgTileBase);

		// Start the game if the player pushes the "A" button
		if (JOYPAD & J_A)
		{
			bGameStart=1;
		}
	}

	// Just wait for them to let go of the button...
	while (JOYPAD & J_A);

	ClearScreen(SCR_1_PLANE);

	//Just exiting this function causes the emulator to crash?
	//Do I need to clear up memory or something?

	return 1;

}

void DefenderoidsMain()
{
	// Define variables
	u8 iLoopX;
	u8 iLoopY;
	u16 iTile;
	u8 iMainLoop;
	u16 bmpPlayField[2032];
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
	u32 iVelocityX;
	u32 iVelocityY;
	u16 iVelocity;
	bool bShoot;
	VECTOROBJECT Asteroid[] = {
									{{6,6},{12192,8192},0,0,0,0,0,0},
									{{6,6},{12096,10096},0,0,0,0,0,0},
									{{6,6},{2048,10192},0,0,0,0,0,0},
									{{6,6},{12192,40098},0,0,0,0,0,0},
									{{6,6},{8192,2048},0,0,0,0,0,0},
									{{6,6},{2048,2048},0,0,0,0,0,0},
									{{6,6},{10432,2048},0,0,0,0,0,0},
									{{6,6},{2048,7634},0,0,0,0,0,0},
									{{6,6},{2048,7763},0,0,0,0,0,0},
									{{6,6},{2048,2048},0,0,0,0,0,0},
									{{6,6},{2048,10048},0,0,0,0,0,0},
									{{6,6},{12432,2048},0,0,0,0,0,0}
								};

	VECTOROBJECT Shot[] = {
								{{1,1},{0,0},{0,0},1,{{1,1}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{1,1}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{1,1}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{1,1}},0,0,0}
							};

	VECTOROBJECT PlayerOne;

	// So, create a bitmap...

	SetBackgroundColour(RGB(0,0,4));

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

	InitialiseQRandom();

	CreateBitmap((u16*)bmpPlayField, 144, 112);
	CopyBitmap((u16*)bmpPlayField, bgTileBase);

	iTile=0;
	bShoot=0;
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
		Asteroid[iLoopAsteroid].Points = 7 + (QRandom()>>5);
		for (iLoopAsteroidPoint=0;iLoopAsteroidPoint<Asteroid[iLoopAsteroid].Points-1;iLoopAsteroidPoint++)
		{
			Asteroid[iLoopAsteroid].VectorList[iLoopAsteroidPoint] = AsteroidTemplate[iLoopAsteroidPoint][QRandom()>>7];
		}
		// Make sure the asteroid is closed...
		Asteroid[iLoopAsteroid].VectorList[iLoopAsteroidPoint] = Asteroid[iLoopAsteroid].VectorList[0];
		Asteroid[iLoopAsteroid].Scale=3;
		Asteroid[iLoopAsteroid].RotationSpeed=(Sin(QRandom())>>4)+1;
		Asteroid[iLoopAsteroid].RotationAngle=Sin(QRandom());
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
	iVelocity=0;
	while (!(JOYPAD & J_B))
	{

		iStartFrame=VBCounter;

		//Reset the bitmap for every frame.
		CreateBitmap((u16*)bmpPlayField, 144, 112);

		// Draw and animate the qix.
		// This will be an "evil otto" style enemy that cannot be destroyed...
		DrawVectorObject((u16*)bmpPlayField,Qix);
		// Overwrite the "oldest" point in the list with a new random point.
		Qix.VectorList[iLoopQix].x = (((s16)QRandom())>>3)+(Qix.MovementVector.x>>5);
		Qix.VectorList[iLoopQix].y = (((s16)QRandom())>>3)+(Qix.MovementVector.y>>5);
		Qix.VectorList[iLoopQix].colour = 3;
		if (++iLoopQix >= Qix.Points)
		{
			iLoopQix = 0;
		}

		// Move the Qix towards the player
		//Qix.MovementVector.x++;
		//Qix.MovementVector.y++;

		// Move and rotate the asteroids
		for (iLoopAsteroid=0;iLoopAsteroid<4;iLoopAsteroid++)
		{
			DrawVectorObject((u16*)bmpPlayField,Asteroid[iLoopAsteroid]);
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
			// Also, need to constrain the Movement Vector to a maximum velocity.
			// I could do that in seperately in the x or y directions easily enough,
			// Length of a vector is SQRT(x^2 + y^2)
			// I don't need to calculate the square root, as I can work on the squared result.
			iVelocityX = ((PlayerOne.MovementVector.x + (Cos(PlayerOne.RotationAngle+192))) * (PlayerOne.MovementVector.x + (Cos(PlayerOne.RotationAngle+192))));
			iVelocityY = ((PlayerOne.MovementVector.y + (Sin(PlayerOne.RotationAngle+192)))* (PlayerOne.MovementVector.y + (Sin(PlayerOne.RotationAngle+192))));
			// This seems to jump rapidly to huge numbers? Which means it doesn't work at all.
			iVelocity = (u16)(iVelocityX>>8) + (u16)(iVelocityY>>8);

			//if (iVelocity<32768)
			//{
				// Modify the movement vector by the angle.
				// Because "zero" degrees is at right angles to "up", we need to rotate this by 270 degrees
				// which on a 256 byte sine table is 192.
				PlayerOne.MovementVector.x += (Cos(PlayerOne.RotationAngle+192));
				PlayerOne.MovementVector.y += (Sin(PlayerOne.RotationAngle+192));
			//}

		}
		if (JOYPAD & J_A && bShoot)
		{
			// Fire a shot.
			// Set the shot flag so that we don't get continuous fire. Got to make the player work for it...
			// Might make it a timer rather than a single yes/no flag...
			bShoot=1;
		}
		if (!(JOYPAD & J_A))
		{
			bShoot=0;
		}
		// Bounds checking? How do I constrain the player within the visible screen without breaking the immersion?
		// Bitmap address [0] contains the bitmap width, and address[1] the bitmap height.
		// So we can use that and just loop around to the opposite edge when we reach the side
		if (PlayerOne.Position.x<0) PlayerOne.Position.x=((u16*)bmpPlayField)[0];
		if (PlayerOne.Position.x>(((u16*)bmpPlayField)[0])) PlayerOne.Position.x=0;
		PlayerOne.Position.x += PlayerOne.MovementVector.x>>7;
		if (PlayerOne.Position.y<0) PlayerOne.Position.y=((u16*)bmpPlayField)[1];
		if (PlayerOne.Position.y>((u16*)bmpPlayField)[1]) PlayerOne.Position.y=0;
		PlayerOne.Position.y += PlayerOne.MovementVector.y>>7;
		// Draw some random engine noise...
		iEngineLoop=PlayerOne.Points-5;
		while (iEngineLoop<PlayerOne.Points)
		{
			PlayerOne.VectorList[iEngineLoop+1].colour = (QRandom()>>7) * 3;
			iEngineLoop++;
		}
		// And then draw the sprite
		DrawVectorSprite((u16*)bmpPlayField, PlayerOne);

		// Then copy the bitmap back into tile memory...
		CopyBitmap((u16*)bmpPlayField, bgTileBase);

		// How many frames has all of this taken...
		PrintString(SCR_1_PLANE, 0, 0, 18, "FPS:");
		PrintDecimal(SCR_1_PLANE, 0, 4, 18, 60/(VBCounter-iStartFrame), 2);

		PrintString(SCR_1_PLANE, 0, 8, 18, "VEL:");
		PrintDecimal(SCR_1_PLANE, 0, 12, 18, iVelocity, 8);
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
