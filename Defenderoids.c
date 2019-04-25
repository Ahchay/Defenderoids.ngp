#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"
#include "Tiles\Sprites.c"
#include "VectorObjects.h"
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
	u8 iLoopStar;
	u8 iTile;
	u8 iSourceLetter;
	u8 iSourcePoint;
	VECTOROBJECT GameLogo[] = {
								{{0,0},{2000,2000},{0,0},0,0,3,0,0}, //D
								{{0,0},{3500,2000},{0,0},1,0,3,0,0}, //e
								{{0,0},{4500,2000},{0,0},2,0,3,0,0}, //f
								{{0,0},{6000,2000},{0,0},1,0,3,0,0}, //e
								{{0,0},{7500,2000},{0,0},3,0,3,0,0}, //n
								{{0,0},{9000,2000},{0,0},4,0,3,0,0}, //d
								{{0,0},{10500,2000},{0,0},1,0,3,0,0}, //e
								{{0,0},{12000,2000},{0,0},5,0,3,0,0}, //r
								{{0,0},{13000,2000},{0,0},6,0,3,0,0}, //o
								{{0,0},{14000,2000},{0,0},7,0,3,0,0}, //i
								{{0,0},{14750,2000},{0,0},4,0,3,0,0}, //d
								{{0,0},{16000,2000},{0,0},8,0,3,0,0}, //s
								{{0,0},{3000,7500},{0,0},9,0,2,0,0}, //P
								{{0,0},{4500,7500},{0,0},5,0,2,0,0}, //r
								{{0,0},{6000,7500},{0,0},1,0,2,0,0}, //e
								{{0,0},{7000,7500},{0,0},8,0,2,0,0}, //s
								{{0,0},{8000,7500},{0,0},8,0,2,0,0}, //s
								{{0,0},{9500,7000},{0,0},12,0,2,0,0}, //'A'
								{{0,0},{3000,10000},{0,0},10,0,2,0,0}, //t
								{{0,0},{4500,10000},{0,0},6,0,2,0,0}, //o
								{{0,0},{7000,10000},{0,0},8,0,2,0,0}, //s
								{{0,0},{8000,10000},{0,0},10,0,2,0,0}, //t
								{{0,0},{9000,10000},{0,0},11,0,2,0,0}, //a
								{{0,0},{10250,10000},{0,0},5,0,2,0,0}, //r
								{{0,0},{11250,10000},{0,0},10,0,2,0,0}, //t
							};

	// Display a logo animation and wait for the player to push the start button.
	// Can also use this to init() the randomiser so that the player gets different starting objects when they do
	// start the game.

	//Copy my template "alphabet" objects into the GameLogo array
	//The "points" member will contain the soruce object for the letter in the template array

	for (iLoopLetter=0;iLoopLetter<25;iLoopLetter++)
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
		GameLogo[iLoopLetter].MovementVector.x=((s8)QRandom() - (s8)QRandom())>>3;
		GameLogo[iLoopLetter].MovementVector.y=((s8)QRandom() - (s8)QRandom())>>3;

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

		// Rubbish star field...
		for (iLoopStar=0;iLoopStar<25;iLoopStar++)
		{
			iLoopY = QRandom()>>1;
			iLoopX = QRandom()>>1;
			SetPixel((u16*)bmpLogo,(u8)(iLoopX),(u8)(iLoopY),QRandom()>>7);
		}

		// Game logo and instructions...
		for (iLoopLetter=0;iLoopLetter<25;iLoopLetter++)
		{
			DrawVectorObject((u16*)bmpLogo,GameLogo[iLoopLetter],0);
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
				// Force the loop to terminate...
				iLoopLetter=255;
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
	u8 iSpriteLoop;
	u16 iCounter;
	u8 iEngineLoop;
	s16 iVelocityX;
	s16 iVelocityY;
	u8 iHeightMapLoop;
	u8 iHorizontalOffset;
	u8 iCurrentLevel;
	u8 iLives;
	u8 iLoopShot;
	bool bShoot;
	VECTOROBJECT Asteroid[] = {
									{{6,6},{2000,8192},0,0,0,0,0,0},
									{{6,6},{5000,10096},0,0,0,0,0,0},
									{{6,6},{8000,10192},0,0,0,0,0,0},
									{{6,6},{11000,40098},0,0,0,0,0,0},
									{{6,6},{14000,2048},0,0,0,0,0,0},
									{{6,6},{19000,2048},0,0,0,0,0,0},
									{{6,6},{25000,2048},0,0,0,0,0,0},
									{{6,6},{30000,7634},0,0,0,0,0,0},
									{{6,6},{36000,7763},0,0,0,0,0,0},
									{{6,6},{43000,2048},0,0,0,0,0,0},
									{{6,6},{51000,10048},0,0,0,0,0,0},
									{{6,6},{58000,2048},0,0,0,0,0,0}
								};

	VECTOROBJECT Shots[] = {
								{{1,1},{0,0},{0,0},1,{{1,1}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{1,1}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{1,1}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{1,1}},0,0,0}
							};

	LEVEL DefenderoidsLevels[] = {
									{"Start me up",5,5,2},
									{"Getting Harder",12,12,3}
								};

	SPRITE SpriteList[16];

	VECTOROBJECT PlayerOne;

	iCurrentLevel=0;
	iLives=3;

	// Set up the test sprite
	SetPalette(SPRITE_PLANE, PAL_SPRITE + 0, RGB(0,0,0), RGB(0,15,0), RGB(15,15,0), RGB(15,0,0));
	SetPalette(SPRITE_PLANE, PAL_SPRITE + 1, RGB(0,0,0), RGB(8,8,0), RGB(0,0,15), RGB(0,15,0));

	// So, create a bitmap...
	SetBackgroundColour(RGB(0,0,4));

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

	CreateBitmap((u16*)bmpPlayField, 144, 112);
	CopyBitmap((u16*)bmpPlayField, bgTileBase);

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

	// Main lives/game loop
	while ((!(JOYPAD & J_OPTION)) && iLives>0)
	{
		// Create the level

		for (iSpriteLoop=0;iSpriteLoop<DefenderoidsLevels[iCurrentLevel].InvaderCount;iSpriteLoop++)
		{
			SpriteList[iSpriteLoop].Position.x = ((u16)QRandom())<<10;
			SpriteList[iSpriteLoop].Position.y = 0;
			SpriteList[iSpriteLoop].SpriteID = iSpriteLoop;
			SpriteList[iSpriteLoop].SpriteType = 0; // 0 or 1
			SpriteList[iSpriteLoop].Direction = DIR_SOUTH;
			SpriteList[iSpriteLoop].BaseTile = spTileBase + SpriteList[iSpriteLoop].SpriteID;
			SpriteList[iSpriteLoop].Frame = 0;

			SetSprite(SpriteList[iSpriteLoop].SpriteID, SpriteList[iSpriteLoop].BaseTile , 0, (u8)(SpriteList[iSpriteLoop].Position.x>>8), (u8)(SpriteList[iSpriteLoop].Position.y>>8), PAL_SPRITE + SpriteList[iSpriteLoop].SpriteType);
		}

		for (;iSpriteLoop<DefenderoidsLevels[iCurrentLevel].InvaderCount+DefenderoidsLevels[iCurrentLevel].LemmanoidCount;iSpriteLoop++)
		{
			SpriteList[iSpriteLoop].Position.x = ((u16)QRandom())<<10;
			SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>8))+4]+4)<<8;
			SpriteList[iSpriteLoop].SpriteID = iSpriteLoop;
			SpriteList[iSpriteLoop].SpriteType = 1;
			SpriteList[iSpriteLoop].Direction = 1 + ((QRandom()>>7)<<1); // NORTH or SOUTH for SpriteType 0, EAST/WEST for Sprite Type 1
			SpriteList[iSpriteLoop].BaseTile = spTileBase + SpriteList[iSpriteLoop].SpriteID;
			SpriteList[iSpriteLoop].Frame = 0;

			SetSprite(SpriteList[iSpriteLoop].SpriteID, SpriteList[iSpriteLoop].BaseTile , 0, (u8)(SpriteList[iSpriteLoop].Position.x>>8), (u8)(SpriteList[iSpriteLoop].Position.y>>8), PAL_SPRITE + SpriteList[iSpriteLoop].SpriteType);
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
		Qix.MovementVector.x=376;
		Qix.MovementVector.y=32;
		Qix.Points=12;
		Qix.Scale=1;
		Qix.RotationAngle=0;
		Qix.RotationSpeed=0;
		for (iLoopQix=0;iLoopQix<Qix.Points;iLoopQix++)
		{
			Qix.VectorList[iLoopQix].x = 6000;
			Qix.VectorList[iLoopQix].y = 0;
			Qix.VectorList[iLoopQix].colour = 3;
		}

		// Set up the asteroids
		for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
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
		PlayerOne.MovementVector.x = 0; // Use 256 to set up an initial drift...;
		PlayerOne.MovementVector.y = 0;
		PlayerOne.Scale = 2;
		PlayerOne.Origin.x = 4;
		PlayerOne.Origin.y = 8;
		PlayerOne.Points = 40;
		PlayerOne.RotationAngle = 64;
		PlayerOne.RotationSpeed = 0;
		iLoopX=0;
		while (iLoopX++<PlayerOne.Points)
		{
			PlayerOne.VectorList[iLoopX].x = PlayerSprite[iLoopX].x;
			PlayerOne.VectorList[iLoopX].y = PlayerSprite[iLoopX].y;
			PlayerOne.VectorList[iLoopX].colour = PlayerSprite[iLoopX].colour;
		}

		// Set up the horizontal offset.
		iHorizontalOffset=128;

		iLoopX=0;
		iLoopY=0;
		iLoopQix=0;
		iCounter=0;
		iVelocityX=0;
		iVelocityY=0;
		bShoot=(JOYPAD & J_A);

		// Main level loop
		while ((!(JOYPAD & J_OPTION)) && iLives>0 && DefenderoidsLevels[iCurrentLevel].InvaderCount>0)
		{

			iStartFrame=VBCounter;

			//Reset the bitmap for every frame.
			CreateBitmap((u16*)bmpPlayField, 144, 112);

			// Draw the height map
			for (iHeightMapLoop=0;iHeightMapLoop<143;iHeightMapLoop++)
			{
				SetPixel((u16*)bmpPlayField,iHeightMapLoop,HeightMap[iHorizontalOffset+iHeightMapLoop],3);
			}

			// Draw and animate the qix.
			// This will be an "evil otto" style enemy that cannot be destroyed...
			// I have to draw the Qix with an "absolute" position within the bitmap, as the offset doesn't work?
			// Presumably, because the position is (0,0) and does not move.
			// Anyway, I'm keeping this in here because I like it - not really sure what it's going to do in terms of gameplay yet.
			/*
			DrawVectorObjectAbsolute((u16*)bmpPlayField,Qix);
			// Overwrite the "oldest" point in the list with a new random point.
			Qix.VectorList[iLoopQix].x = (((s16)QRandom())>>3)+(Qix.MovementVector.x>>5);
			Qix.VectorList[iLoopQix].y = (((s16)QRandom())>>3)+(Qix.MovementVector.y>>5);
			Qix.VectorList[iLoopQix].colour = 3;
			if (++iLoopQix >= Qix.Points)
			{
				iLoopQix = 0;
			}
			// Move the Qix towards the player
			Qix.MovementVector.x++;
			Qix.MovementVector.y++;
			*/

			// Ship Sprite
			if (JOYPAD & J_LEFT) PlayerOne.RotationAngle-=8;
			if (JOYPAD & J_RIGHT) PlayerOne.RotationAngle+=8;
			if (JOYPAD & J_B)
			{
				// Also, need to constrain the Movement Vector to a maximum velocity.
				// I could do that in seperately in the x or y directions easily enough,
				// Length of a vector is SQRT(x^2 + y^2)
				// I don't need to calculate the square root, as I can work on the squared result.
				//if (PlayerOne.MovementVector.x < 0)
				//{
				//	iVelocityX = (PlayerOne.MovementVector.x<<48 * -1);
				//}
				//else
				//{
				//	iVelocityX = (PlayerOne.MovementVector.x<<48);
				//}
				//iVelocityX = ((iVelocityX + (Cos(PlayerOne.RotationAngle+192))>>2) * (iVelocityX + (Cos(PlayerOne.RotationAngle+192))>>2));
				//iVelocityY = ((PlayerOne.MovementVector.y + (Sin(PlayerOne.RotationAngle+192)))* (PlayerOne.MovementVector.y + (Sin(PlayerOne.RotationAngle+192))));
				// This seems to jump rapidly to huge numbers? Which means it doesn't work at all.
				// (which makes sense Iguess given that I'm using a 16 bit scale so it rapidly gets out of control)
				//iVelocity = (u16)(iVelocityX>>8) + (u16)(iVelocityY>>8);

				// Movement vector should grow or shrink by +/- 127 in any given frame?
				if (PlayerOne.MovementVector.x + Cos(PlayerOne.RotationAngle+192) < 0)
				{
					iVelocityX = (u16)((PlayerOne.MovementVector.x + Cos(PlayerOne.RotationAngle+192)) * -1)>>8;
				}
				else
				{
					iVelocityX = (u16)((PlayerOne.MovementVector.x + Cos(PlayerOne.RotationAngle+192)))>>8;
				}

				if (PlayerOne.MovementVector.y + Sin(PlayerOne.RotationAngle+192) < 0)
				{
					iVelocityY = (u16)((PlayerOne.MovementVector.y + Sin(PlayerOne.RotationAngle+192)) * -1)>>8;
				}
				else
				{
					iVelocityY = (u16)((PlayerOne.MovementVector.y + Sin(PlayerOne.RotationAngle+192)))>>8;
				}

				// Bugger it. We can have separate maximum vertical and horizontal velocities. We can call it a gravity effect...

				if (iVelocityX<8)
				{
					// Modify the movement vector by the angle.
					// Because "zero" degrees is at right angles to "up", we need to rotate this by 270 degrees
					// which on a 256 byte sine table is 192.
					PlayerOne.MovementVector.x += (Cos(PlayerOne.RotationAngle+192));
				}
				if (iVelocityY<3)
				{
					PlayerOne.MovementVector.y += (Sin(PlayerOne.RotationAngle+192));
				}
			}
			if (JOYPAD & J_A && bShoot)
			{
				// Fire a shot.
				// Essentially, find an "empty" shot in the array, and copy the template "shot" vector into it.
				// MovementVector will be based on tr whe current ship vector with a multiplier to give the velocity. Hopefully one that never out-runs the ship.
				// Set the shot flag so that we don't get continuous fire. Got to make the player work for it...
				// Find the first "empty" shot - scale will be set to zero if it's not valid.
				for (iLoopShot=0;iLoopShot<4;iLoopShot++)
				{
					if (Shots[iLoopShot].Scale == 0)
					{
						Shots[iLoopShot].Scale = 1;
						// Copy the shot object from the template
						Shots[iLoopShot].Points = 10;
						iLoopX=0;
						while (iLoopX++<Shots[iLoopShot].Points)
						{
							Shots[iLoopShot].VectorList[iLoopX].x = Shot[iLoopX].x;
							Shots[iLoopShot].VectorList[iLoopX].y = Shot[iLoopX].y;
							Shots[iLoopShot].VectorList[iLoopX].colour = Shot[iLoopX].colour;
						}
						Shots[iLoopShot].Origin.x = 3;
						Shots[iLoopShot].Origin.y = 3;
						Shots[iLoopShot].Position.x = PlayerOne.Position.x + iHorizontalOffset;
						Shots[iLoopShot].Position.y = PlayerOne.Position.y;
						Shots[iLoopShot].RotationAngle = PlayerOne.RotationAngle;
						// Give the shots a bit of speed. Should be faster than the ship but in the same direction as the ship is "facing" (rather than moving)
						Shots[iLoopShot].MovementVector.x = ((s16)Cos(Shots[iLoopShot].RotationAngle+192))<<3;
						Shots[iLoopShot].MovementVector.y = ((s16)Sin(Shots[iLoopShot].RotationAngle+192))<<3;

						iLoopShot = 5;
					}
				}
				// Might make it a timer rather than a single yes/no flag...
				bShoot=0;
			}
			if (!(JOYPAD & J_A))
			{
				bShoot=1;
			}
			// Bounds checking? How do I constrain the player within the visible screen without breaking the immersion?
			// Bitmap address [0] contains the bitmap width, and address[1] the bitmap height.
			// So we can use that and just loop around to the opposite edge when we reach the side
			// I don't need to worry about horizontal position as the player is now fixed to the centre of the screen.

			iHorizontalOffset += PlayerOne.MovementVector.x>>7;
			// Display the shots first
			for (iLoopShot=0;iLoopShot<4;iLoopShot++)
			{
				if (Shots[iLoopShot].Scale == 1)
				{
					// Destroy the shot when it leaves the playfield.
					if (Shots[iLoopShot].Position.x < 0 | Shots[iLoopShot].Position.x > bmpPlayField[0] | Shots[iLoopShot].Position.y < 0 | Shots[iLoopShot].Position.y > bmpPlayField[1])
					{
						Shots[iLoopShot].Scale = 0;
					}
					else
					{
						Shots[iLoopShot].Position.x += Shots[iLoopShot].MovementVector.x >> 7;
						Shots[iLoopShot].Position.y += Shots[iLoopShot].MovementVector.y >> 7;
						iEngineLoop=Shots[iLoopShot].Points-5;
						// Only do this if the player is currently pressing thrust
						while (iEngineLoop<Shots[iLoopShot].Points)
						{
							Shots[iLoopShot].VectorList[iEngineLoop+1].colour = (((QRandom()>>7) && (JOYPAD & J_B)) * 3);
							iEngineLoop++;
						}
					}
					DrawVectorSprite((u16*)bmpPlayField, Shots[iLoopShot], iHorizontalOffset);
				}
			}

			// And then the player ship
			if (PlayerOne.Position.y<0) PlayerOne.Position.y=((u16*)bmpPlayField)[1];
			if (PlayerOne.Position.y>((u16*)bmpPlayField)[1]) PlayerOne.Position.y=0;
			PlayerOne.Position.y += PlayerOne.MovementVector.y>>7;
			// Draw some random engine noise...
			iEngineLoop=PlayerOne.Points-5;
			// Only do this if the player is currently pressing thrust
			while (iEngineLoop<PlayerOne.Points)
			{
				PlayerOne.VectorList[iEngineLoop+1].colour = (((QRandom()>>7) && (JOYPAD & J_B)) * 3);
				iEngineLoop++;
			}
			DrawVectorSpriteAbsolute((u16*)bmpPlayField, PlayerOne);

			// And finally the asteroids
			// Move and rotate the asteroids
			for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
			{
				// Is this causing the hanging?
				DrawVectorObject((u16*)bmpPlayField,Asteroid[iLoopAsteroid],iHorizontalOffset);
				Asteroid[iLoopAsteroid].RotationAngle+=Asteroid[iLoopAsteroid].RotationSpeed;
				// Need to do some bounds checking here...
				Asteroid[iLoopAsteroid].Position.x += Asteroid[iLoopAsteroid].MovementVector.x;
				if (Asteroid[iLoopAsteroid].Position.x < 4000)
				{
					Asteroid[iLoopAsteroid].MovementVector.x = Asteroid[iLoopAsteroid].MovementVector.x*-1;
				}
				if (Asteroid[iLoopAsteroid].Position.x > 60000)
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

			// And any explosions

			// Then copy the bitmap back into tile memory...
			CopyBitmap((u16*)bmpPlayField, bgTileBase);

			// Show the sprites...
			for (iSpriteLoop=0;iSpriteLoop<(DefenderoidsLevels[iCurrentLevel].InvaderCount+DefenderoidsLevels[iCurrentLevel].LemmanoidCount);iSpriteLoop++)
			{
				CopyAnimationFrame(Sprites, SpriteList[iSpriteLoop].BaseTile, 1, (SpriteList[iSpriteLoop].SpriteType << 4) + (SpriteList[iSpriteLoop].Direction << 2) + SpriteList[iSpriteLoop].Frame);
				SetSpritePosition(SpriteList[iSpriteLoop].SpriteID, (u8)(SpriteList[iSpriteLoop].Position.x>>8)-iHorizontalOffset+4, (u8)(SpriteList[iSpriteLoop].Position.y>>8));
				if (++SpriteList[iSpriteLoop].Frame>3) SpriteList[iSpriteLoop].Frame=0;
				switch(SpriteList[iSpriteLoop].Direction)
				{
					case DIR_SOUTH:
						SpriteList[iSpriteLoop].Position.y+=64;
						break;
					case DIR_NORTH:
						SpriteList[iSpriteLoop].Position.y-=64;
						break;
					case DIR_WEST:
						SpriteList[iSpriteLoop].Position.x-=128;
						SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>8))+4]+4)<<8;
						break;
					case DIR_EAST:
						SpriteList[iSpriteLoop].Position.x+=128;
						SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>8))+4]+4)<<8;
					default:
						break;
				}
			}

			// How many frames has all of this taken...
			PrintString(SCR_1_PLANE, 0, 0, 18, "HZL:");
			PrintDecimal(SCR_1_PLANE, 0, 4, 18, 60/(VBCounter-iStartFrame), 2);
			PrintDecimal(SCR_1_PLANE, 0, 4, 17, iHorizontalOffset, 3);
			PrintDecimal(SCR_1_PLANE, 0, 8, 17, Shots[0].Position.x, 8);

		} // Level Loop

		// Need to clear down any still open sprites between levels
		for (iSpriteLoop=0;iSpriteLoop<64;iSpriteLoop++)
		{
			SetSprite(iSpriteLoop, 0, 0, 0, 0, PAL_SPRITE);
		}

	} // Lives Loop
}

void DrawVectorObjectAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject)
{
	DrawVectorObject(BitmapAddress, VectorObject, 0);
}

void DrawVectorObject(u16 * BitmapAddress, VECTOROBJECT VectorObject, u8 iHorizontalOffset)
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

	if (!((((VectorObject.Position.x>>7)-iHorizontalOffset) > BitmapAddress[0]) && ((VectorObject.Position.x>>7)<iHorizontalOffset)))
	{

		cSin = Sin(VectorObject.RotationAngle);
		cCos = Cos(VectorObject.RotationAngle);

		iStartX = VectorObject.VectorList[0].x;
		iStartY = VectorObject.VectorList[0].y;

		while (iPoint++<VectorObject.Points)
		{

			// Modifier here is to find the centre of rotation
			iStartX = (iStartX-VectorObject.Origin.x)*VectorObject.Scale;
			iStartY = (iStartY-VectorObject.Origin.y)*VectorObject.Scale;

			// rotate point.
			iTempX = ((iStartX * cCos)>>7) - ((iStartY * cSin)>>7);
			iTempY = ((iStartX * cSin)>>7) + ((iStartY * cCos)>>7);

			// translate point back to it's original position:
			// Modify back from the centre of rotation above, and then add the X & Y co-ordinates
			iStartX = (VectorObject.Position.x>>7)+iTempX-iHorizontalOffset;
			//if (iStartX<0) iStartX=0;
			iStartY = (VectorObject.Position.y>>7)+iTempY;
			//if (iStartY<0) iStartY=0;

			iEndX = (VectorObject.VectorList[iPoint].x-VectorObject.Origin.x)*VectorObject.Scale;
			iEndY = (VectorObject.VectorList[iPoint].y-VectorObject.Origin.y)*VectorObject.Scale;

			// rotate point
			iTempX = ((iEndX * cCos)>>7) - ((iEndY * cSin)>>7);
			iTempY = ((iEndX * cSin)>>7) + ((iEndY * cCos)>>7);

			// translate point back:
			iEndX = (VectorObject.Position.x>>7)-iHorizontalOffset+iTempX;
			//if (iEndX<0) iEndX=0;
			iEndY = (VectorObject.Position.y>>7)+iTempY;
			//if (iEndY<0) iEndY=0;

			// Bounds check to ensure that rotated points are still within the bitmap boundary
			if (iStartX>=0&&iStartY>=0&&iEndX>=0&&iEndY>=0&&iStartX<=BitmapAddress[0]&&iStartY<=BitmapAddress[1]&&iEndX<=BitmapAddress[0]&&iEndY<=BitmapAddress[1])
			{
				DrawLine((u16*)BitmapAddress,(u8)(iStartX),(u8)(iStartY),(u8)(iEndX),(u8)(iEndY),VectorObject.VectorList[iPoint].colour);
			}
			iStartX = VectorObject.VectorList[iPoint].x;
			iStartY = VectorObject.VectorList[iPoint].y;
		}
	}
}

void DrawVectorSpriteAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject)
{
	DrawVectorSprite(BitmapAddress, VectorObject, 0);
}
// Only works when HorizontalOffset is between 0 and 72?
// i.e. half the bitmap width. So, it must have something to do with that...
// Might be affecting the drawobject above as well...
void DrawVectorSprite(u16 * BitmapAddress, VECTOROBJECT VectorObject, u8 iHorizontalOffset)
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

	while (iPoint++<VectorObject.Points)
	{

		if (PlayerSprite[iPoint].colour != 0)
		{

			iStartX = (PlayerSprite[iPoint].x-VectorObject.Origin.x)*VectorObject.Scale;
			iStartY = (PlayerSprite[iPoint].y-VectorObject.Origin.y)*VectorObject.Scale;

			// rotate point.
			iTempX = ((iStartX * cCos)>>7) - ((iStartY * cSin)>>7);
			iTempY = ((iStartX * cSin)>>7) + ((iStartY * cCos)>>7);

			// translate point back to it's original position:
			iStartX = VectorObject.Position.x+iTempX-iHorizontalOffset;
			iStartY = VectorObject.Position.y+iTempY;

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
