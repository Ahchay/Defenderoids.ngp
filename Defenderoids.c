#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"
#include "veclib.h"
#include "Tiles\Sprites.c"
#include "Tiles\Logo.c"
#include "VectorObjects.h"
/*

So, this started as a defender/asteroids mash-up concept, but has now turned into a bitmap playground...

Not sure where I'm going with it yet, but it's entertaining enough

*/

VECTOROBJECT CreateAsteroid()
{
	VECTOROBJECT vReturn;
	u8 iLoopAsteroidPoint;

	vReturn.Origin.x=6;
	vReturn.Origin.y=6;
	vReturn.Position.x=QRandom();
	vReturn.Position.y=QRandom()<<4;
	vReturn.Points = 7 + (QRandom()>>5);
	for (iLoopAsteroidPoint=0;iLoopAsteroidPoint<vReturn.Points-1;iLoopAsteroidPoint++)
		{
			vReturn.VectorList[iLoopAsteroidPoint] = AsteroidTemplate[iLoopAsteroidPoint][QRandom()>>7];
		}
	// Make sure the asteroid is closed...
	vReturn.VectorList[iLoopAsteroidPoint] = vReturn.VectorList[0];
	vReturn.Scale=3;
	vReturn.RotationSpeed=(Sin(QRandom())>>4)+1;
	vReturn.RotationAngle=Sin(QRandom());
	//Asteroid[iLoopAsteroid].Position.x=((u16)QRandom())<<4;
	//Asteroid[iLoopAsteroid].Position.y=((u16)QRandom())<<4;
	vReturn.MovementVector.x=QRandom();
	vReturn.MovementVector.y=QRandom();

	return vReturn;
}

SPRITE CreateSprite(u16 x, u16 y, u8 ID, u8 Type, u8 Direction, u8 Frame)
{
	SPRITE sprReturn;
	sprReturn.Position.x = x;
	sprReturn.Position.y = y;
	sprReturn.SpriteID = ID;
	sprReturn.SpriteType = Type;
	sprReturn.Direction = Direction;
	sprReturn.BaseTile = spTileBase + ID;
	sprReturn.Frame = Frame;

	SetSprite(sprReturn.SpriteID, sprReturn.BaseTile , 0, (u8)(sprReturn.Position.x>>8), (u8)(sprReturn.Position.y>>8), (u8)(PAL_SPRITE + sprReturn.SpriteType));

	return sprReturn;
}

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
	u8 iSpriteLoop;

	// Wait for the "A" button to be released
	while (JOYPAD & J_A);

	//Install the logo tileset
	InstallTileSetAt(Logo, sizeof(Logo)/2, LogoTileBase);
	//Clear the screen

	SetBackgroundColour(0);

	ClearScreen(SCR_1_PLANE);
	ClearScreen(SCR_2_PLANE);

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,0,0), RGB(15,15,0), RGB(15,15,15));
	SetPalette(SCR_2_PLANE, 0, 0, RGB(15,15,15), RGB(11,11,11), RGB(7,7,7));

	//Also, clear the sprites....
	for (iSpriteLoop=0;iSpriteLoop<64;iSpriteLoop++)
	{
		SetSprite(iSpriteLoop, 0, 0, 0, 0, 0);
	}

	while (JOYPAD & J_A);

	//Paint the logo.
	for (iLoopX=0;iLoopX<=19;iLoopX++)
	{
		for (iLoopY=0;iLoopY<=9;iLoopY++)
		{
			PutTile(SCR_1_PLANE, 0, iLoopX, iLoopY+4, LogoTileBase +(iLoopY*20)+iLoopX);
		}
	}

	PrintString(SCR_1_PLANE, 0, 2, 1, "  PRESENTED BY");
	PrintString(SCR_1_PLANE, 0, 2, 2, "   AHCHAY.COM");
	PrintString(SCR_1_PLANE, 0, 2, 13, "PRESS A TO START");

	bGameStart=0;

	while (!bGameStart)
	{

		// Start the game if the player pushes the "A" button
		if (JOYPAD & J_A)
		{
			bGameStart=1;
		}
	}

	// Just wait for them to let go of the button...
	while (JOYPAD & J_A);

	ClearScreen(SCR_1_PLANE);
	ClearScreen(SCR_2_PLANE);

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
	u8 iLoopAsteroid;
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
	u8 iLoopExplosion;
	u8 iLoopExplosionPoint;
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
									{{6,6},{23000,7763},0,0,0,0,0,0},
									{{6,6},{31000,2048},0,0,0,0,0,0},
									{{6,6},{12000,10048},0,0,0,0,0,0},
									{{6,6},{8000,2048},0,0,0,0,0,0}
								};

	VECTOROBJECT Shots[] = {
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0}
							};

	VECTOROBJECT Explosions[] = {
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},1,{{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}},0,0,0}
								};

	LEVEL DefenderoidsLevels[] = {
									{"Start me up",12,5,7},
									{"Getting Harder",12,12,3}
								};

	SPRITE SpriteList[16];

	VECTOROBJECT PlayerOne;

	iCurrentLevel=0;
	iLives=3;

	// Set up the test sprite
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPRITE + Invader), RGB(0,0,0), RGB(0,15,0), RGB(15,15,0), RGB(15,0,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPRITE + Lemmanoid), RGB(0,0,0), RGB(15, 11, 12), RGB(0,0,15), RGB(0,15,0));

	// So, create a bitmap...
	SetBackgroundColour(RGB(0,0,4));

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));
	SetPalette(SCR_2_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

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
		// Invaders
		for (iSpriteLoop=0;iSpriteLoop<DefenderoidsLevels[iCurrentLevel].InvaderCount;iSpriteLoop++)
		{
			SpriteList[iSpriteLoop] = CreateSprite(((u16)QRandom())<<10,0,iSpriteLoop,Invader,DIR_SOUTH,0);
		}
		// Lemmanoids
		for (;iSpriteLoop<DefenderoidsLevels[iCurrentLevel].InvaderCount+DefenderoidsLevels[iCurrentLevel].LemmanoidCount;iSpriteLoop++)
		{
			SpriteList[iSpriteLoop] = CreateSprite(((u16)QRandom())<<10,(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>8))+4]+4)<<8,iSpriteLoop,Lemmanoid,1 + ((QRandom()>>7)<<1),0);
		}

		// Set up the asteroids
		for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
		{
			Asteroid[iLoopAsteroid] = CreateAsteroid();
		}

		// Set up the player
		PlayerOne.Position.x = 72;
		PlayerOne.Position.y = 66;
		PlayerOne.MovementVector.x = 0; // Use 256 to set up an initial drift...;
		PlayerOne.MovementVector.y = 0;
		PlayerOne.Scale = 1;
		PlayerOne.Origin.x = 3;
		PlayerOne.Origin.y = 8;
		PlayerOne.Points = 40;
		PlayerOne.RotationAngle = 64;
		PlayerOne.RotationSpeed = 0;
		iLoopX=0;
		while (iLoopX<PlayerOne.Points)
		{
			PlayerOne.VectorList[iLoopX].x = PlayerSprite[iLoopX].x;
			PlayerOne.VectorList[iLoopX].y = PlayerSprite[iLoopX].y;
			PlayerOne.VectorList[iLoopX].colour = PlayerSprite[iLoopX].colour;
			iLoopX++;
		}

		// Set up the horizontal offset.
		iHorizontalOffset=128;

		iLoopX=0;
		iLoopY=0;
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

			//Check player movement

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

				// We have separate max velocities for horizontal and vertical movement

				if (iVelocityX<3)
				{
					// Modify the movement vector by the angle.
					// Because "zero" degrees is at right angles to "up", we need to rotate this by 270 degrees
					// which on a 256 byte sine table is 192.
					PlayerOne.MovementVector.x += (Cos(PlayerOne.RotationAngle+192));
				}
				if (iVelocityY<2)
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
						Shots[iLoopShot].Points = 16;
						iLoopX=0;
						while (iLoopX<Shots[iLoopShot].Points)
						{
							Shots[iLoopShot].VectorList[iLoopX].x = Shot[iLoopX].x;
							Shots[iLoopShot].VectorList[iLoopX].y = Shot[iLoopX].y;
							Shots[iLoopShot].VectorList[iLoopX].colour = Shot[iLoopX].colour;
							iLoopX++;
						}
						Shots[iLoopShot].Origin.x = 2;
						Shots[iLoopShot].Origin.y = 3;
						Shots[iLoopShot].Position.x = PlayerOne.Position.x + iHorizontalOffset;
						Shots[iLoopShot].Position.y = PlayerOne.Position.y;
						Shots[iLoopShot].RotationAngle = PlayerOne.RotationAngle;
						// Give the shots a bit of speed. Should be faster than the ship but in the same direction as the ship is "facing" (rather than moving)
						Shots[iLoopShot].MovementVector.x = ((s16)Cos(Shots[iLoopShot].RotationAngle+192))<<4;
						Shots[iLoopShot].MovementVector.y = ((s16)Sin(Shots[iLoopShot].RotationAngle+192))<<4;

						// We'll use RotationSpeed to control the life of the shot. Kill it when it hits a limit...
						Shots[iLoopShot].RotationSpeed=0;

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
					Shots[iLoopShot].RotationSpeed++;
					if (Shots[iLoopShot].RotationSpeed == 15 |Shots[iLoopShot].Position.y < 0 | Shots[iLoopShot].Position.y > bmpPlayField[1])
					{
						Shots[iLoopShot].Scale = 0;
					}
					else
					{

						// Calculate any collisions...
						for (iSpriteLoop=0;iSpriteLoop<(DefenderoidsLevels[iCurrentLevel].InvaderCount);iSpriteLoop++)
						{
							// If any sprite exists along the bullet's movement vector, then it's dead.
							// Shots and sprites are in different scales, so need to be translated to match
							if (SpriteList[iSpriteLoop].SpriteType == Invader)
							{
								POINT pStartShot;
								POINT pEndShot;
								POINT pStartSprite;
								POINT pEndSprite;
								// Offset the shot by the line origin. Hang on, that won't work without applying the rotation...
								// So the position is about as much as we can hope for. The Origin "should" be in the centre of the object anyway,
								// so this will still go centre to centre
								pStartShot.x = Shots[iLoopShot].Position.x; // - Shots[iLoopShot].Origin.x;
								pStartShot.y = Shots[iLoopShot].Position.y; // - Shots[iLoopShot].Origin.y;
								pEndShot.x = pStartShot.x + (Shots[iLoopShot].MovementVector.x >> 7);
								pEndShot.y = pStartShot.y + (Shots[iLoopShot].MovementVector.y >> 7);
								// Offset the sprite to the centre of the box
								pStartSprite.x = (SpriteList[iSpriteLoop].Position.x >> 8) - 8;
								pStartSprite.y = (SpriteList[iSpriteLoop].Position.y >> 8) - 8;
								pEndSprite.x = pStartSprite.x + 8;
								pEndSprite.y = pStartSprite.y + 8;

								/*
								PrintString(SCR_1_PLANE, 0, 5, 15, "START   END");
								PrintString(SCR_1_PLANE, 0, 0, 16, "SHOT");
								PrintDecimal(SCR_1_PLANE, 0, 5, 16, pStartShot.x,3);
								PrintString(SCR_1_PLANE, 0, 8, 16, "/");
								PrintDecimal(SCR_1_PLANE, 0, 9, 16, pStartShot.y,3);
								PrintDecimal(SCR_1_PLANE, 0, 13, 16, pEndShot.x,3);
								PrintString(SCR_1_PLANE, 0, 16, 16, "/");
								PrintDecimal(SCR_1_PLANE, 0, 17, 16, pEndShot.y,3);
								PrintString(SCR_1_PLANE, 0, 0, 17, "SPRT");
								PrintDecimal(SCR_1_PLANE, 0, 5, 17, pStartSprite.x,3);
								PrintString(SCR_1_PLANE, 0, 8, 17, "/");
								PrintDecimal(SCR_1_PLANE, 0, 9, 17, pStartSprite.y,3);
								PrintDecimal(SCR_1_PLANE, 0, 13, 17, pEndSprite.x,3);
								PrintString(SCR_1_PLANE, 0, 16, 17, "/");
								PrintDecimal(SCR_1_PLANE, 0, 17, 17, pEndSprite.y,3);
								*/
								if (LineIntersect(Shots[iLoopShot].Position, pEndShot, pStartSprite, pEndSprite) == 1)
								{
									SpriteList[iSpriteLoop].SpriteType = NullSprite;
									SetSprite(SpriteList[iSpriteLoop].SpriteID, 0, 0, 0, 0, PAL_SPRITE);
									Shots[iLoopShot].Scale = 0;
									iSpriteLoop = DefenderoidsLevels[iCurrentLevel].InvaderCount + 1;

									// Add an explosion
									for (iLoopExplosion=0;iLoopExplosion<8;iLoopExplosion++)
									{
										if (Explosions[iLoopExplosion].Scale == 0)
										{
											Explosions[iLoopExplosion].Scale = 1;
											// Copy the Explosion object from the template
											Explosions[iLoopExplosion].Points = 16;
											iLoopX=0;
											while (iLoopX<Explosions[iLoopExplosion].Points)
											{
												Explosions[iLoopExplosion].VectorList[iLoopX].x = 4;
												Explosions[iLoopExplosion].VectorList[iLoopX].y = 4;
												Explosions[iLoopExplosion].VectorList[iLoopX].colour = QRandom()>>6;
												iLoopX++;
											}
											Explosions[iLoopExplosion].Origin.x = 0;
											Explosions[iLoopExplosion].Origin.y = 0;
											Explosions[iLoopExplosion].Position.x = pStartSprite.x;
											Explosions[iLoopExplosion].Position.y = pStartSprite.y;
											Explosions[iLoopExplosion].RotationAngle = Shots[iLoopShot].RotationAngle+128;

											// We'll use RotationSpeed to control the life of the Explosion. Kill it when it hits a limit...
											Explosions[iLoopExplosion].RotationSpeed=0;

											iLoopExplosion = 9;
										}
									}

								}
							}
						}
						if (Shots[iLoopShot].Scale == 1)
						{
							Shots[iLoopShot].Position.x += Shots[iLoopShot].MovementVector.x >> 7;
							Shots[iLoopShot].Position.y += Shots[iLoopShot].MovementVector.y >> 7;
							iEngineLoop=Shots[iLoopShot].Points-5;
							// Only do this if the player is currently pressing thrust
							while (iEngineLoop<Shots[iLoopShot].Points)
							{
								Shots[iLoopShot].VectorList[iEngineLoop].colour = (((QRandom()>>7)) * 3);
								iEngineLoop++;
							}
						}
					}
					if (Shots[iLoopShot].Scale == 1)
					{
						if (Shots[iLoopShot].Position.x < 0)
						{
							Shots[iLoopShot].Position.x = 255;
						}
						else
						{
							if (Shots[iLoopShot].Position.x > 255)
							{
								Shots[iLoopShot].Position.x = 0;
							}
						}

						DrawVectorSprite((u16*)bmpPlayField, Shots[iLoopShot], iHorizontalOffset);
					}
				}
			}

			// Move and animate the explosions (points heading off in 16 directions basically)
			for (iLoopExplosion=0;iLoopExplosion<8;iLoopExplosion++)
			{
				if (Explosions[iLoopExplosion].Scale != 0)
				{
					// Destroy the Explosion after a few frames
					Explosions[iLoopExplosion].RotationSpeed++;
					if (Explosions[iLoopExplosion].RotationSpeed == 7)
					{
						Explosions[iLoopExplosion].Scale = 0;
					}
					if (Explosions[iLoopExplosion].Scale != 0)
					{
						for (iLoopExplosionPoint=0;iLoopExplosionPoint<Explosions[iLoopExplosion].Points;iLoopExplosionPoint++)
						{
							// Wrong, but still manages to look quite good. No idea what's going on...
							// Why not just a few QRandom() calls within the rotation speed area?
							// Looks ace. Need to figure out the rotation angle to get the direction right is all
							// Maybe increase the scale for later frames?
							Explosions[iLoopExplosion].VectorList[iLoopExplosionPoint].x = (QRandom()>>(8-Explosions[iLoopExplosion].RotationSpeed));
							Explosions[iLoopExplosion].VectorList[iLoopExplosionPoint].y = (QRandom()>>(8-Explosions[iLoopExplosion].RotationSpeed));
						}
						DrawVectorSprite((u16*)bmpPlayField, Explosions[iLoopExplosion], iHorizontalOffset);
					}
				}
			}

			// And then the player ship
			//PrintDecimal(SCR_2_PLANE, 0, 0, 0, PlayerOne.Position.y, 16);
			//Constrain player within bounds
			if (PlayerOne.Position.y<4 && PlayerOne.MovementVector.y<127){
				//Keep player inside the playfield...
				//Reduce vertical velocity to zero
				PlayerOne.MovementVector.y = 127;
				PlayerOne.Position.y=4;
				//PlayerOne.Position.y=((u16*)bmpPlayField)[1];
			} else if (PlayerOne.Position.y>bmpPlayField[1]-4 && PlayerOne.MovementVector.y>127){
				PlayerOne.MovementVector.y = 0;
				PlayerOne.Position.y=bmpPlayField[1]-4;
			}

			PlayerOne.Position.y += PlayerOne.MovementVector.y>>7;
			// Draw some random engine noise...
			iEngineLoop=PlayerOne.Points-4;
			// Only do this if the player is currently pressing thrust
			while (iEngineLoop<=PlayerOne.Points)
			{
				PlayerOne.VectorList[iEngineLoop].colour = (((QRandom()>>7) && (JOYPAD & J_B)) * 3);
				iEngineLoop++;
			}
			DrawVectorSpriteAbsolute((u16*)bmpPlayField, PlayerOne);

			// And finally the asteroids
			// Move and rotate the asteroids
			for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
			{
				// Is this causing the hanging?
				Asteroid[iLoopAsteroid].RotationAngle+=Asteroid[iLoopAsteroid].RotationSpeed;
				// Need to do some bounds checking here...
				Asteroid[iLoopAsteroid].Position.x += Asteroid[iLoopAsteroid].MovementVector.x;
				if (Asteroid[iLoopAsteroid].Position.x < 256)
				{
					Asteroid[iLoopAsteroid].MovementVector.x = Asteroid[iLoopAsteroid].MovementVector.x*-1;
				}
				if (Asteroid[iLoopAsteroid].Position.x > (((u16)bmpPlayField[0])-256)<<7)
				{
					Asteroid[iLoopAsteroid].MovementVector.x = Asteroid[iLoopAsteroid].MovementVector.x*-1;
				}
				Asteroid[iLoopAsteroid].Position.y += Asteroid[iLoopAsteroid].MovementVector.y;
				if (Asteroid[iLoopAsteroid].Position.y < 256)
				{
					Asteroid[iLoopAsteroid].MovementVector.y = Asteroid[iLoopAsteroid].MovementVector.y*-1;
				}
				if (Asteroid[iLoopAsteroid].Position.y > (((u16)bmpPlayField[1])-256)<<7)
				{
					Asteroid[iLoopAsteroid].MovementVector.y = Asteroid[iLoopAsteroid].MovementVector.y*-1;
				}
				DrawVectorObject((u16*)bmpPlayField,Asteroid[iLoopAsteroid],iHorizontalOffset);
			}

			// And any explosions

			// Then copy the bitmap back into tile memory...
			CopyBitmap((u16*)bmpPlayField, bgTileBase);

			// Show the sprites...
			for (iSpriteLoop=0;iSpriteLoop<(DefenderoidsLevels[iCurrentLevel].InvaderCount+DefenderoidsLevels[iCurrentLevel].LemmanoidCount);iSpriteLoop++)
			{
				if (!(SpriteList[iSpriteLoop].SpriteType == NullSprite))
				{
					//PrintString(SCR_2_PLANE, 0, 0, iSpriteLoop, "S  :");
					//PrintDecimal(SCR_2_PLANE, 0, 1, iSpriteLoop, iSpriteLoop, 2);
					//PrintBinary(SCR_2_PLANE, 0, 4, iSpriteLoop, (SpriteList[iSpriteLoop].SpriteType << 3) + (SpriteList[iSpriteLoop].Direction << 2) + SpriteList[iSpriteLoop].Frame, 16);
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
			}

			// How many frames has all of this taken...
			//PrintString(SCR_1_PLANE, 0, 0, 18, "FPS:");
			//PrintDecimal(SCR_1_PLANE, 0, 4, 18, 60/(VBCounter-iStartFrame), 2);
			PrintString(SCR_2_PLANE, 0, 0, 17, "HZL:");
			PrintDecimal(SCR_2_PLANE, 0, 4, 17, iHorizontalOffset, 3);
			//PrintDecimal(SCR_2_PLANE, 0, 8, 17, Shots[0].Position.x, 8);

		} // Level Loop

		// Need to clear down any still open sprites between levels
		for (iSpriteLoop=0;iSpriteLoop<64;iSpriteLoop++)
		{
			SetSprite(iSpriteLoop, 0, 0, 0, 0, PAL_SPRITE);
		}

	} // Lives Loop
}
