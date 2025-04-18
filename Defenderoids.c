#include "ngpc.h"
#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"
#include "veclib.h"
#include "Tiles\Sprites.c"
#include "Tiles\Logo.c"
#include "Tiles\Border.c"
#include "VectorObjects.h"

#define SPRITE_MAX_WIDTH (512)

//Helper functions

/////////////////////////////////////////////////////
// Create a unique asteroid object
/////////////////////////////////////////////////////
VECTOROBJECT CreateAsteroid(u16 x, u16 y)
{
	VECTOROBJECT vReturn;
	u8 iLoopAsteroidPoint;
	u8 iTemplateID;

	VECTOROBJECT AsteroidTemplate[] = {
		{{3,3},{0,0},{0,0},4,{{0,6,1},{6,6,1},{6,0,1},{0,6,1}},3,0,0},
		{{3,3},{0,0},{0,0},7,{{2,0,1},{5,1,1},{4,4,1},{1,3,1},{2,2,1},{0,1,1},{2,0,1}},3,0,0}
	};

	//InitialiseQRandom();

	iTemplateID=1;
	vReturn=AsteroidTemplate[iTemplateID];

	vReturn.Position.x=x;
	vReturn.Position.y=y;

	// Set scale, rotation and speed randomly
	vReturn.Scale=5;
	vReturn.RotationSpeed=QRandom()>>4;
	vReturn.RotationAngle=QRandom();
	//Asteroid[iLoopAsteroid].Position.x=((u16)QRandom())<<4;
	//Asteroid[iLoopAsteroid].Position.y=((u16)QRandom())<<4;
	vReturn.MovementVector.x=QRandom()>>1;
	vReturn.MovementVector.y=QRandom()>>2;

	return vReturn;
}

/////////////////////////////////////////////////////
// Sprite creator function
// Creates an extended sprite type includes:
// - Position
// - ID
// - Type - index into sprite tiles enum (Invader and Lemmanoid)
// - Direction - index animation direction
// - Frame - index for animation frame
//
// Sprite tile can then be returned using
// 		Type * 16
// 		+
// 		Direction * 4
// 		+
// 		Frame
// or more effeciently:
// 		Type<<4 + Direction<<2 + Frame
/////////////////////////////////////////////////////
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

	// Initialise sprites with the nullsprite - animation frames are applied in the main loop
	CopyAnimationFrame(Sprites, sprReturn.BaseTile, 1, (u16)((EmptySprite << 4) + (sprReturn.Direction << 2) + sprReturn.Frame));

	//Create the sprite pointer - again, initialised at (0,0) and sprite plane position applied in the main loop
	SetSprite(sprReturn.SpriteID, sprReturn.BaseTile , 0, 0, 0, (u8)(PAL_SPRITE + sprReturn.SpriteType));

	return sprReturn;
}

/////////////////////////////////////////////////////
// Calculate sprite position relative to horizontal offset
// and whether visible on the screen.
/////////////////////////////////////////////////////
DrawSprite(SPRITE sprSprite, u16 iHorizontalOffset)
{
	u16 iRelativeX;
	u8 iRelativeY;

	///////////////////////////////////////////////////////////
	// Even though the sprite plane is a (u8) (256 pixels)
	// we still need to calculate the relative position as (u16)
	// as the game world is 512 pixels.
	///////////////////////////////////////////////////////////
	iRelativeX = (sprSprite.Position.x>>SPRITE_SCALE);

	if (iRelativeX>=iHorizontalOffset) {
		iRelativeX=iRelativeX-iHorizontalOffset;
	} else {
		iRelativeX=SPRITE_MAX_WIDTH-iHorizontalOffset+iRelativeX;
	}
	iRelativeY = (u8)(sprSprite.Position.y>>SPRITE_SCALE)+8;

	if(iRelativeX>144||iRelativeY>112||iRelativeX<8||iRelativeY<8) {
		CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, EmptySprite-1);
	}
	else {
		CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, (sprSprite.SpriteType << 4) + (sprSprite.Direction << 2) + sprSprite.Frame -1);
	}

	// Cast the relativeX position to (u8) once the bounds checking has been completed
	// The "duplicate" sprite that gets created won't show on screen as the sprite tile will be set to the empty tile
	SetSpritePosition(sprSprite.SpriteID, (u8)iRelativeX, iRelativeY);

}

/////////////////////////////////////////////////////
// Create an explosion object
////////////////////////////////////////////////////
VECTOROBJECT CreateExplosion(SPRITEPOINT spPosition, u8 iDirection)
{
	VECTOROBJECT voReturn;
	u8 iPointLoop;

	voReturn.Scale = 1;
	// Copy the Explosion object from the template
	voReturn.Points = 16;
	iPointLoop=0;
	for(iPointLoop=0;iPointLoop<voReturn.Points;iPointLoop++)
	{
		voReturn.VectorList[iPointLoop].x = 4;
		voReturn.VectorList[iPointLoop].y = 4;
		voReturn.VectorList[iPointLoop].colour = QRandom()>>6;
	}
	voReturn.Origin.x = 0;
	voReturn.Origin.y = 0;
	voReturn.Position.x = spPosition.x + 4;
	voReturn.Position.y = spPosition.y + 4;
	voReturn.RotationAngle = iDirection;

	// We'll use RotationSpeed to control the life of the Explosion. Kill it when it hits a limit...
	voReturn.RotationSpeed=0;

	return voReturn;
}

/////////////////////////////////////////////////////
// CreatePlayer()
// Sets up the player object
/////////////////////////////////////////////////////
VECTOROBJECT CreatePlayer()
{
	VECTOROBJECT PlayerOne;
	u8 iPointLoop;
	PlayerOne.Position.x = 72; // Fixed to the centre of the bitmap
	PlayerOne.Position.y = 66;
	PlayerOne.MovementVector.x = 0; // Use 256 to set up an initial drift...;
	PlayerOne.MovementVector.y = 0;
	PlayerOne.Scale = 1;
	PlayerOne.Origin.x = 3;
	PlayerOne.Origin.y = 8;
	PlayerOne.Points = 50;
	PlayerOne.RotationAngle = 64;
	PlayerOne.RotationSpeed = 0;
	iPointLoop=0;
	while (iPointLoop<PlayerOne.Points)
	{
		PlayerOne.VectorList[iPointLoop].x = PlayerSprite[iPointLoop].x;
		PlayerOne.VectorList[iPointLoop].y = PlayerSprite[iPointLoop].y;
		PlayerOne.VectorList[iPointLoop].colour = PlayerSprite[iPointLoop].colour;
		iPointLoop++;
	}

	return PlayerOne;
}

/////////////////////////////////////////////////////
// CreateShot()
/////////////////////////////////////////////////////
VECTOROBJECT CreateShot(u16 iHorizontalOffset,VECTOROBJECT PlayerOne, bool bShotType)
{
	VECTOROBJECT vecShot;
	u8 iPointLoop;
	u8 iShotSide;

	vecShot.Scale = 1;

	// Alternate shots between left and right sides
	iShotSide=0;
	if(bShotType) iShotSide=8;

	// Copy the shot object from the template
	vecShot.Points = SHOT_POINTS;
	for(iPointLoop=0;iPointLoop<=vecShot.Points;iPointLoop++)
	{
		vecShot.VectorList[iPointLoop].x = Shot[iPointLoop].x+iShotSide;
		vecShot.VectorList[iPointLoop].y = Shot[iPointLoop].y;
		vecShot.VectorList[iPointLoop].colour = Shot[iPointLoop].colour;
	}
	vecShot.Origin.x = 3;
	vecShot.Origin.y = 0;
	vecShot.Position.x = (PlayerOne.Position.x<<7)+(iHorizontalOffset<<7);
	vecShot.Position.y = (PlayerOne.Position.y<<7);
	vecShot.RotationAngle = PlayerOne.RotationAngle;
	// Give the shots a bit of speed. Should be faster than the ship but in the same direction as the ship is "facing" (rather than moving)
	vecShot.MovementVector.x = ((s16)Cos(vecShot.RotationAngle+192))<<4;
	vecShot.MovementVector.y = ((s16)Sin(vecShot.RotationAngle+192))<<4;

	// We'll use RotationSpeed to control the life of the shot. Kill it when it hits a limit...
	vecShot.RotationSpeed=0;

	return vecShot;

}

/////////////////////////////////////////////////////
// Print the game border and score sheet etc
/////////////////////////////////////////////////////

void DrawGameScreen()
{
	u8 iLoopX;
	u8 iLoopY;

	ClearScreen(SCR_1_PLANE);
	ClearScreen(SCR_2_PLANE);

	// Create the border
	InstallTileSetAt(Border,sizeof(Border),borderTilebase);

	// Border tiles layout
	// 0=[Top Left] 1=[Top]     2=[Top Right]
	// 3=[Left]     4=[Empty]   5=[Right]
	// 6=[Bottom L] 7=[Bottom]  8=[Bottom R]
	//

	//Corners
	PutTile(SCR_1_PLANE,0,0,0,borderTilebase);
	PutTile(SCR_1_PLANE,0,19,0,borderTilebase+2);
	PutTile(SCR_1_PLANE,0,0,15,borderTilebase+6);
	PutTile(SCR_1_PLANE,0,19,15,borderTilebase+8);

	//Top/Bottom edges
	for(iLoopX=1;iLoopX<=18;iLoopX++){
		PutTile(SCR_1_PLANE,0,iLoopX,0,borderTilebase+1);
		PutTile(SCR_1_PLANE,0,iLoopX,15,borderTilebase+7);
	}
	//Left/Right edges
	for(iLoopY=1;iLoopY<15;iLoopY++){
		PutTile(SCR_1_PLANE,0,0,iLoopY,borderTilebase+3);
		PutTile(SCR_1_PLANE,0,19,iLoopY,borderTilebase+5);
	}

	//Other window dressing (debug info and scorecard/lives count etc)
	PrintString(SCR_1_PLANE, 0, 0, 18, "FPS:");
	PrintString(SCR_2_PLANE, 0, 0, 17, "HZL:");

}

/////////////////////////////////////////////////////
// Logo and game select screen
/////////////////////////////////////////////////////
u8 DefenderoidsLogo()
{
	bool iGameMode=0;
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
	PrintString(SCR_1_PLANE, 0, 2, 14, "PRESS A TO START");
	PrintString(SCR_1_PLANE, 0, 2, 15, "PRESS B TO TEST");

	iGameMode=0;

	while (iGameMode==0)
	{

		// Start the game if the player pushes either button
		if (JOYPAD & J_A)
		{
			iGameMode=1;
		}
		if (JOYPAD & J_B) {
			iGameMode=2;
		}
	}

	// Just wait for them to let go of the button...
	while (JOYPAD & J_A||JOYPAD & J_B);

	ClearScreen(SCR_1_PLANE);
	ClearScreen(SCR_2_PLANE);

	//Just exiting this function causes the emulator to crash?
	//Do I need to clear up memory or something?

	return iGameMode;

}

/////////////////////////////////////////////////////
// Main game loop
/////////////////////////////////////////////////////
void DefenderoidsMain()
{
	// Define variables
	u8 iLoopX;
	u8 iLoopY;
	u8 iPointLoop;
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
	u16 iHeightMapLoop;
	u16 iHorizontalOffset;
	u8 iCurrentLevel;
	u8 iLives;
	u8 iLoopShot;
	u8 iLoopExplosion;
	u8 iLoopExplosionPoint;
	u8 iEngineNoise;
	bool bShotType;
	bool bShoot;

	/////////////////////////////////////////////////////////
	// Template vector/sprite arrays
	/////////////////////////////////////////////////////////

	// Vector objects
	//	- Origin;
	//	- Position;
	//	- MovementVector;
	//	- Points;
	//	- VectorList[64];
	//	- Scale;
	//	- RotationAngle;
	//	- RotationSpeed;

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
								{{1,1},{0,0},{0,0},3,{{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},3,{{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},3,{{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},3,{{0,0,0},{0,0,0},{0,0,0}},0,0,0},
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

	SPRITE SpriteList[16];

	VECTOROBJECT PlayerOne;

	InitialiseQRandom();

	// Set up the palettes
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPRITE + Invader), RGB(0,0,0), RGB(0,15,0), RGB(15,15,0), RGB(15,0,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPRITE + Lemmanoid), RGB(0,0,0), RGB(15, 11, 12), RGB(0,0,15), RGB(0,15,0));

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));
	SetPalette(SCR_2_PLANE, 0, 0, RGB(15,15,15), RGB(8,8,8), RGB(4,4,4));

	////////////////////////////////////////////////////////////
	//Create game screen
	////////////////////////////////////////////////////////////

	DrawGameScreen();

	//Create a bitmap...
	SetBackgroundColour(RGB(0,0,4));

	CreateBitmap((u16*)bmpPlayField, BITMAP_WIDTH, BITMAP_HEIGHT);
	CopyBitmap((u16*)bmpPlayField, bgTileBase);

	iTile=0;
	//Copy the bitmap to SCR_1_PLANE
	// Watch the order...
	for (iLoopY=0;iLoopY<14;iLoopY++)
	{
		for (iLoopX=0;iLoopX<18;iLoopX++)
		{
			PutTile(SCR_1_PLANE, 0, 1 + iLoopX, 1 + iLoopY, bgTileBase+iTile);
			iTile++;
		}
	}

	/////////////////////////////////////////////////////////
	// Main lives/game loop
	/////////////////////////////////////////////////////////

	iCurrentLevel=0;
	iLives=3;

	while ((!(JOYPAD & J_OPTION)) && iLives>0)
	{
		////////////////////////////////////////////////////////////
		// Level setup
		////////////////////////////////////////////////////////////

		// Invaders - do I want these to all appear at the start of the level or be phased in somehow?
		for (iSpriteLoop=0;iSpriteLoop<DefenderoidsLevels[iCurrentLevel].InvaderCount;iSpriteLoop++)
		{
			//x, y, ID, Type, Direction, Frame
			//Invaders always move south on creation
			//Eventually, we'll spawn these on a timer I guess rather than just dumping them all in at the outset of the level
			SpriteList[iSpriteLoop] = CreateSprite(((u16)QRandom())<<8,((u16)QRandom())<<4,iSpriteLoop,Invader,DIR_SOUTH,0);
		}

		// Lemmanoids
		for (;iSpriteLoop<DefenderoidsLevels[iCurrentLevel].InvaderCount+DefenderoidsLevels[iCurrentLevel].LemmanoidCount;iSpriteLoop++)
		{
			//x, y, ID, Type, Direction, Frame
			//Lemmanoids move either west or east (DIR_EAST + [0|2])
			//Spawn them vertically at the median point of the terrain
			SpriteList[iSpriteLoop] = CreateSprite(((u16)QRandom())<<8,100<<8,iSpriteLoop,Lemmanoid,DIR_EAST + ((QRandom()>>7)<<1),(QRandom()>>5));
		}

		// Asteroids
		for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
		{
			Asteroid[iLoopAsteroid] = CreateAsteroid(((u16)QRandom())<<7,((u16)QRandom())<<5);
		}

		// Set up the player
		PlayerOne=CreatePlayer();

		// Set the horizontal offset.
		iHorizontalOffset=255;

		iLoopX=0;
		iLoopY=0;
		iCounter=0;
		iVelocityX=0;
		iVelocityY=0;
		bShoot=0;
		bShoot=(JOYPAD & J_A);

		// Main level loop
		while ((!(JOYPAD & J_OPTION)) && iLives>0 && DefenderoidsLevels[iCurrentLevel].InvaderCount>0)
		{

			iStartFrame=VBCounter;

			//Reset the bitmap for every frame.
			CreateBitmap((u16*)bmpPlayField, BITMAP_WIDTH, BITMAP_HEIGHT);

			//Check player movement

			// Ship Sprite
			// Movement controls:
			// Left/Right = Rotate left/right
			// B button = Thrust
			// A butotn = Fire (must be released between shots)

			if (JOYPAD & J_LEFT) PlayerOne.RotationAngle-=8;
			if (JOYPAD & J_RIGHT) PlayerOne.RotationAngle+=8;
			if (JOYPAD & J_B)
			{

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

				// Need to constrain the Movement Vector to a maximum velocity.
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
				// Find an "empty" shot in the array, and copy the template "shot" vector into it.
				// MovementVector will be based on tr whe current ship vector with a multiplier to give the velocity. Hopefully one that never out-runs the ship.
				// Set the shot flag so that we don't get continuous fire. Got to make the player work for it...
				// Find the first "empty" shot - scale will be set to zero if it's not valid.
				for (iLoopShot=0;iLoopShot<3;iLoopShot++)
				{
					if (Shots[iLoopShot].Scale == 0)
					{
						bShotType=!bShotType;
						Shots[iLoopShot]=CreateShot(iHorizontalOffset,PlayerOne,bShotType);
						
						//Terminate the loop so that only one shot is created
						iLoopShot = 5;
					}
				}
				// Reset the shoot flag to force the player to press the button again
				bShoot=0;
			}
			if (!(JOYPAD & J_A))
			{
				//Set the shoot flag to zero (they've let go of the button)
				bShoot=1;
			}

			//////////////////////////////////////////////////////
			// Draw game elements
			//////////////////////////////////////////////////////

			//////////////////////////////////////////////////////
			// Height Map
			//////////////////////////////////////////////////////
			for (iHeightMapLoop=0;iHeightMapLoop<143;iHeightMapLoop++)
			{
				// Need to wrap around at sizeof(HeightMap)-1
				if(iHorizontalOffset+iHeightMapLoop>sizeof(HeightMap)-1){
					SetPixel((u16*)bmpPlayField,iHeightMapLoop,HeightMap[iHorizontalOffset+iHeightMapLoop-sizeof(HeightMap)]-1,3);
				} else {
					SetPixel((u16*)bmpPlayField,iHeightMapLoop,HeightMap[iHorizontalOffset+iHeightMapLoop],3);
				}
			}

			//////////////////////////////////////////////////////
			// Shots
			//////////////////////////////////////////////////////
			for (iLoopShot=0;iLoopShot<3;iLoopShot++)
			{
				if (Shots[iLoopShot].Scale == 1)
				{
					// Destroy the shot when it leaves the playfield.
					Shots[iLoopShot].RotationSpeed++;
					if (Shots[iLoopShot].RotationSpeed == 6 |Shots[iLoopShot].Position.y>>7 < 0 | Shots[iLoopShot].Position.y>>7 > bmpPlayField[1])
					{
						Shots[iLoopShot].Scale = 0;
					}
					else
					{

						// Calculate any collisions...
						// VERY slow (and doesn't actually work) - leaving it for reference, but we need to sort this out
						for (iSpriteLoop=128;iSpriteLoop<(DefenderoidsLevels[iCurrentLevel].InvaderCount);iSpriteLoop++)
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
								pEndShot.x = pStartShot.x + (Shots[iLoopShot].MovementVector.x);
								pEndShot.y = pStartShot.y + (Shots[iLoopShot].MovementVector.y);
								// Offset the sprite to the centre of the box
								pEndSprite.x = (SpriteList[iSpriteLoop].Position.x);
								pEndSprite.y = (SpriteList[iSpriteLoop].Position.y);
								pStartSprite.x = pStartSprite.x - 8;
								pStartSprite.y = pStartSprite.y - 8;

								if (LineIntersect(Shots[iLoopShot].Position, pEndShot, pStartSprite, pEndSprite) == 1)
								{
									SpriteList[iSpriteLoop].SpriteType = EmptySprite;
									SetSprite(SpriteList[iSpriteLoop].SpriteID, 0, 0, 0, 0, PAL_SPRITE);
									Shots[iLoopShot].Scale = 0;
									iSpriteLoop = DefenderoidsLevels[iCurrentLevel].InvaderCount + 1;

									// Add an explosion
									for (iLoopExplosion=0;iLoopExplosion<8;iLoopExplosion++)
									{
										if (Explosions[iLoopExplosion].Scale == 0)
										{
											Explosions[iLoopExplosion]=CreateExplosion(SpriteList[iSpriteLoop].Position, Shots[iLoopShot].RotationAngle);

											iLoopExplosion = 9;
										}
									}

								}
							}
						}
						if (Shots[iLoopShot].Scale == 1)
						{
							Shots[iLoopShot].Position.x += Shots[iLoopShot].MovementVector.x;
							Shots[iLoopShot].Position.y += Shots[iLoopShot].MovementVector.y;
						}
					}
					if (Shots[iLoopShot].Scale == 1)
					{
						DrawVectorObject((u16*)bmpPlayField, Shots[iLoopShot], iHorizontalOffset);
					}
				}
			}

			//////////////////////////////////////////////////////
			// Explosions
			//////////////////////////////////////////////////////
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

			//////////////////////////////////////////////////////
			// Asteroids
			//////////////////////////////////////////////////////
			for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
			{
				// Increase rotation angle
				Asteroid[iLoopAsteroid].RotationAngle+=Asteroid[iLoopAsteroid].RotationSpeed;
				// Need to do some bounds checking here...
				if (Asteroid[iLoopAsteroid].MovementVector.y>Asteroid[iLoopAsteroid].Position.y || (Asteroid[iLoopAsteroid].Position.y + Asteroid[iLoopAsteroid].MovementVector.y)>>7 > bmpPlayField[1]){
					Asteroid[iLoopAsteroid].MovementVector.y = Asteroid[iLoopAsteroid].MovementVector.y*-1;
				}
				Asteroid[iLoopAsteroid].Position.x += Asteroid[iLoopAsteroid].MovementVector.x;
				Asteroid[iLoopAsteroid].Position.y += Asteroid[iLoopAsteroid].MovementVector.y;

				DrawVectorObject((u16*)bmpPlayField,Asteroid[iLoopAsteroid],iHorizontalOffset);
			}

			//////////////////////////////////////////////////////
			// Player
			//////////////////////////////////////////////////////

			//Update the horizontal offset (ship remains centred horizontally)
			iHorizontalOffset += PlayerOne.MovementVector.x>>7;
			//Bounds check - needs to wrap at sizeof(HeightMap)-1
			if(iHorizontalOffset>sizeof(HeightMap)-1){
				if(PlayerOne.MovementVector.x<0){
					iHorizontalOffset=sizeof(HeightMap)-1;
				} else {
					iHorizontalOffset=0;
				}
			}

			//Keep player inside the playfield...
			//Reduce vertical velocity to zero as they approach either top/bottom edge- need to decide whether to apply a bounce or just stop at the top/bottom of the screen
			if (PlayerOne.Position.y<4 && PlayerOne.MovementVector.y<127){
				PlayerOne.MovementVector.y = 127;
				PlayerOne.Position.y=4;
				//PlayerOne.Position.y=((u16*)bmpPlayField)[1];
			} else if (PlayerOne.Position.y>bmpPlayField[1]-4 && PlayerOne.MovementVector.y>127){
				PlayerOne.MovementVector.y = 0;
				PlayerOne.Position.y=bmpPlayField[1]-4;
			}

			// Apply vertical velocity
			PlayerOne.Position.y += PlayerOne.MovementVector.y>>7;

			// Add some engine noise if the player is thrusting
			for(iEngineLoop=PlayerOne.Points-4;iEngineLoop<=PlayerOne.Points;iEngineLoop++)
			{
				iEngineNoise=0;
				if (JOYPAD & J_B) iEngineNoise=(((QRandom()>>7))*3);

				PlayerOne.VectorList[iEngineLoop].colour = iEngineNoise;
			}

			//Draw the player sprite
			DrawVectorSpriteAbsolute((u16*)bmpPlayField, PlayerOne);

			//////////////////////////////////////////////////////
			// Copy the bitmap object into tile memory
			//////////////////////////////////////////////////////
			CopyBitmap((u16*)bmpPlayField, bgTileBase);

			//////////////////////////////////////////////////////
			// Display Sprite objects
			//////////////////////////////////////////////////////
			for (iSpriteLoop=0;iSpriteLoop<(DefenderoidsLevels[iCurrentLevel].InvaderCount+DefenderoidsLevels[iCurrentLevel].LemmanoidCount);iSpriteLoop++)
			{
				if (!(SpriteList[iSpriteLoop].SpriteType == EmptySprite))
				{
					// This is ridiculously simple
					// Invaders bob up and down
					// Lemmanoids lope along the mountain left and right
					switch(SpriteList[iSpriteLoop].Direction)
					{
						case DIR_SOUTH:
							SpriteList[iSpriteLoop].Position.y+=64;
							if (SpriteList[iSpriteLoop].Position.y>(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]+4)<<SPRITE_SCALE){
								SpriteList[iSpriteLoop].Direction = DIR_NORTH;
							}
							break;
						case DIR_NORTH:
							SpriteList[iSpriteLoop].Position.y-=64;
							if (SpriteList[iSpriteLoop].Position.y>(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]+4)<<SPRITE_SCALE){
								SpriteList[iSpriteLoop].Direction = DIR_SOUTH;
							}
							break;
						case DIR_WEST:
							SpriteList[iSpriteLoop].Position.x-=128;
							SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-4)<<SPRITE_SCALE;
							break;
						case DIR_EAST:
							SpriteList[iSpriteLoop].Position.x+=128;
							SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-4)<<SPRITE_SCALE;
						default:
							break;
					}

					SpriteList[iSpriteLoop].Frame++;
					if (SpriteList[iSpriteLoop].Frame>3) SpriteList[iSpriteLoop].Frame=0;

					DrawSprite(SpriteList[iSpriteLoop],iHorizontalOffset);
				}
			}

			//////////////////////////////////////////////////////
			// Score and other dressing
			// Mostly debug information at the moment
			//////////////////////////////////////////////////////
			/*
			PrintString(SCR_2_PLANE,0,0,0,"PLAYER:(     ,     )");
			PrintDecimal(SCR_2_PLANE, 0, 8, 0, PlayerOne.Position.x, 5);
			PrintDecimal(SCR_2_PLANE, 0, 14, 0, PlayerOne.Position.y, 5);
			PrintString(SCR_2_PLANE,0,0,1,"SHOT 0:(     ,     )");
			PrintString(SCR_2_PLANE,0,0,2,"SHOT 1:(     ,     )");
			PrintString(SCR_2_PLANE,0,0,3,"SHOT 2:(     ,     )");
			PrintString(SCR_2_PLANE,0,0,4,"SHOT 3:(     ,     )");
			for(iLoopShot=0;iLoopShot<4;iLoopShot++) {
				if (Shots[iLoopShot].Scale == 1)
				{
					PrintDecimal(SCR_2_PLANE, 0, 8, 1+iLoopShot,Shots[iLoopShot].Position.x, 5);
					PrintDecimal(SCR_2_PLANE, 0, 14, 1+iLoopShot, Shots[iLoopShot].Position.y, 5);
				}
			}
			PrintString(SCR_2_PLANE,0,0,5,"INVADR:(     ,     )");
			PrintString(SCR_2_PLANE,0,0,6,"FRAME:( , , ,   )");
			PrintDecimal(SCR_2_PLANE,0,8,5,SpriteList[0].Position.x,5);
			PrintDecimal(SCR_2_PLANE,0,14,5,SpriteList[0].Position.y,5);
			PrintDecimal(SCR_2_PLANE,0,7,6,SpriteList[0].SpriteType,1);
			PrintDecimal(SCR_2_PLANE,0,9,6,SpriteList[0].Direction,1);
			PrintDecimal(SCR_2_PLANE,0,11,6,SpriteList[0].Frame,1);
			PrintDecimal(SCR_2_PLANE,0,13,6,(SpriteList[0].SpriteType << 4) + (SpriteList[0].Direction << 2) + SpriteList[0].Frame,3);
			PrintString(SCR_2_PLANE,0,0,7,"LEMMNG:(     ,     )");
			PrintString(SCR_2_PLANE,0,0,8,"FRAME:( , , ,   )");
			PrintDecimal(SCR_2_PLANE,0,8,7,SpriteList[12].Position.x,5);
			PrintDecimal(SCR_2_PLANE,0,14,7,SpriteList[12].Position.y,5);
			PrintDecimal(SCR_2_PLANE,0,7,8,SpriteList[12].SpriteType,1);
			PrintDecimal(SCR_2_PLANE,0,9,8,SpriteList[12].Direction,1);
			PrintDecimal(SCR_2_PLANE,0,11,8,SpriteList[12].Frame,1);
			for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
			{
				PrintString(SCR_2_PLANE+iLoopAsteroid,0,0,10+iLoopAsteroid,"ASTER :(     ,     )");
				PrintDecimal(SCR_2_PLANE+iLoopAsteroid,0,5,10+iLoopAsteroid,iLoopAsteroid,1);
				PrintDecimal(SCR_2_PLANE, 0, 8, 10+iLoopAsteroid,Asteroid[iLoopAsteroid].Position.x, 5);
				PrintDecimal(SCR_2_PLANE, 0, 14, 10+iLoopAsteroid, Asteroid[iLoopAsteroid].Position.y, 5);
			}
			*/

			//Debug info

			//Frame counter
			PrintDecimal(SCR_1_PLANE, 0, 4, 18, 60/(VBCounter-iStartFrame), 2);
			//Current Horizontal Offset
			PrintDecimal(SCR_2_PLANE, 0, 4, 17, iHorizontalOffset, 3);
			
			// Slow it down so I can look at it...
			//Sleep(60);

		} // Level Loop

		// Close down all sprites at end of level loop
		for (iSpriteLoop=0;iSpriteLoop<64;iSpriteLoop++)
		{
			SetSprite(iSpriteLoop, 0, 0, 0, 0, PAL_SPRITE);
		}
		iPointLoop
	} // Lives Loop

	//////////////////////////////////////////////////////
	// Game Over information etc
	//////////////////////////////////////////////////////

}

/////////////////////////////////////////////////////
// Test game loop
// Used to isolate individual game elements for testing
/////////////////////////////////////////////////////
void DefenderoidsTest()
{
	// Define variables
	u8 iLoopX;
	u8 iLoopY;
	u8 iPointLoop;
	u16 iTile;
	u8 iMainLoop;
	u16 bmpPlayField[2032];
	u16 iStartFrame;
	u8 iLoopAsteroid;
	u8 iLoopShot;
	u8 iSpriteLoop;
	u8 iHorizontalOffset;
	u8 iCurrentLevel;
	u8 iLives;
	u8 iMode;
	bool bShotType;
	u8 iShotSide;

	VECTOROBJECT Shots[] = {
								{{1,1},{0,0},{0,0},3,{{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},3,{{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},3,{{0,0,0},{0,0,0},{0,0,0}},0,0,0},
								{{1,1},{0,0},{0,0},3,{{0,0,0},{0,0,0},{0,0,0}},0,0,0},
							};

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

	LEVEL DefenderoidsLevels[] = {
									{"Asteroids Testing",0,0,1},
									{"Getting Harder",12,12,3}
								};

	VECTOROBJECT PlayerOne;

	SPRITE SpriteList[16];

	iCurrentLevel=0;
	iLives=3;

	// Set up the test sprite
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPRITE + Invader), RGB(0,0,0), RGB(0,15,0), RGB(15,15,0), RGB(15,0,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPRITE + Lemmanoid), RGB(0,0,0), RGB(15, 11, 12), RGB(0,0,15), RGB(0,15,0));

	// So, create a bitmap...
	SetBackgroundColour(RGB(0,0,4));

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));
	SetPalette(SCR_2_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

	CreateBitmap((u16*)bmpPlayField, BITMAP_WIDTH, BITMAP_HEIGHT);
	CopyBitmap((u16*)bmpPlayField, bgTileBase);

	iTile=0;
	//Copy the bitmap to SCR_1_PLANE
	// Watch the order...
	for (iLoopY=0;iLoopY<14;iLoopY++)
	{
		for (iLoopX=0;iLoopX<18;iLoopX++)
		{
			PutTile(SCR_1_PLANE, 0, 1 + iLoopX, 1 + iLoopY, bgTileBase+iTile);
			iTile++;
		}
	}

	InitialiseQRandom();

	//Mode:
	//0 = Asteroids
	//1 = Shots
	iMode=1;

	// Main lives/game loop
	while ((!(JOYPAD & J_OPTION)) && iLives>0)
	{
		// Create the level
		//////////////////////////////////////////////////////////////
		// Set up the player (just as a focus point really)
		//////////////////////////////////////////////////////////////
		PlayerOne.Position.x = 72;
		PlayerOne.Position.y = 66;
		PlayerOne.MovementVector.x = 0; // Use 256 to set up an initial drift...;
		PlayerOne.MovementVector.y = 0;
		PlayerOne.Scale = 1;
		PlayerOne.Origin.x = 3;
		PlayerOne.Origin.y = 8;
		PlayerOne.Points = 50;
		PlayerOne.RotationAngle = 0;
		PlayerOne.RotationSpeed = 0;
		iPointLoop=0;
		while (iPointLoop<PlayerOne.Points)
		{
			PlayerOne.VectorList[iPointLoop].x = PlayerSprite[iPointLoop].x;
			PlayerOne.VectorList[iPointLoop].y = PlayerSprite[iPointLoop].y;
			PlayerOne.VectorList[iPointLoop].colour = PlayerSprite[iPointLoop].colour;
			iPointLoop++;
		}

		//////////////////////////////////////////////////////////////
		// Create test asteroid objects
		//////////////////////////////////////////////////////////////
		for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
		{
			//Create an asteroid every 64 pixels...
			//Asteroid[iLoopAsteroid] = CreateAsteroid(((u16)iLoopAsteroid)<<12,(1024+((u16)iLoopAsteroid))<<10);
			//Create a single asteroid
			Asteroid[iLoopAsteroid] = CreateAsteroid(12384,12384);
		}


		//////////////////////////////////////////////////////////////
		// Create test shot object
		//////////////////////////////////////////////////////////////
		iLoopShot=0;
		Shots[iLoopShot].Scale = 1;
		// Copy the shot object from the template

		bShotType=!bShotType;
		iShotSide=0;
		if(bShotType) iShotSide=8;

		Shots[iLoopShot].Points = sizeof(Shot);
		for(iLoopX=0;iLoopX<=Shots[iLoopShot].Points;iLoopX++)
		{
			Shots[iLoopShot].VectorList[iLoopX].x = iShotSide;
			Shots[iLoopShot].VectorList[iLoopX].y = Shot[iLoopX].y;
			Shots[iLoopShot].VectorList[iLoopX].colour = Shot[iLoopX].colour;
		}
		Shots[iLoopShot].Origin.x = 3;
		Shots[iLoopShot].Origin.y = 0;
		Shots[iLoopShot].Position.x = PlayerOne.Position.x+iHorizontalOffset;
		Shots[iLoopShot].Position.y = PlayerOne.Position.y;
		Shots[iLoopShot].RotationAngle = PlayerOne.RotationAngle;
		// Give the shots a bit of speed. Should be faster than the ship but in the same direction as the ship is "facing" (rather than moving)
		Shots[iLoopShot].MovementVector.x = ((s16)Cos(Shots[iLoopShot].RotationAngle+192))<<4;
		Shots[iLoopShot].MovementVector.y = ((s16)Sin(Shots[iLoopShot].RotationAngle+192))<<4;

		// We'll use RotationSpeed to control the life of the shot. Kill it when it hits a limit...
		Shots[iLoopShot].RotationSpeed=0;

		// Set up the horizontal offset.
		iHorizontalOffset=0;

		// Main level loop
		while ((!(JOYPAD & J_OPTION)) && iLives>0)
		{

			iStartFrame=VBCounter;

			//Reset the bitmap for every frame.
			CreateBitmap((u16*)bmpPlayField, BITMAP_WIDTH, BITMAP_HEIGHT);

			//Check player movement

			// Asteroids testing - just scroll left/right
			if (iMode==0){
				if (JOYPAD & J_LEFT) iHorizontalOffset--;
				if (JOYPAD & J_RIGHT) iHorizontalOffset++;
			}

			// Shot testing - move the test shot along it's vector with the up/down
			// rotate the player with left/right
			// fire new shot with A
			if (iMode==1){



				if (JOYPAD & J_UP) {
					for (iLoopShot=0;iLoopShot<4;iLoopShot++)
					{
						Shots[iLoopShot].Position.x += Shots[iLoopShot].MovementVector.x >> 7;
						Shots[iLoopShot].Position.y += Shots[iLoopShot].MovementVector.y >> 7;
					}
				}
				if (JOYPAD & J_DOWN) {
					for (iLoopShot=0;iLoopShot<4;iLoopShot++)
					{
						Shots[iLoopShot].Position.x -= Shots[iLoopShot].MovementVector.x >> 7;
						Shots[iLoopShot].Position.y -= Shots[iLoopShot].MovementVector.y >> 7;
					}
				}

			}


			// Draw the test objects
			// Draw some markers
			DrawLine((u16*)bmpPlayField,0,0,10,0,2);
			DrawLine((u16*)bmpPlayField,0,7,10,7,2);
			DrawLine((u16*)bmpPlayField,0,15,10,15,2);
			DrawLine((u16*)bmpPlayField,0,23,10,23,2);
			DrawLine((u16*)bmpPlayField,0,31,10,31,2);
			DrawLine((u16*)bmpPlayField,0,39,10,39,2);
			DrawLine((u16*)bmpPlayField,0,47,10,47,2);
			DrawLine((u16*)bmpPlayField,0,55,10,55,2);
			DrawLine((u16*)bmpPlayField,0,63,10,63,2);
			DrawLine((u16*)bmpPlayField,0,71,10,71,2);
			DrawLine((u16*)bmpPlayField,0,79,10,79,2);
			DrawLine((u16*)bmpPlayField,0,87,10,87,2);
			DrawLine((u16*)bmpPlayField,0,95,10,95,2);
			DrawLine((u16*)bmpPlayField,0,103,10,103,2);
			DrawLine((u16*)bmpPlayField,0,111,10,111,2);

			DrawLine((u16*)bmpPlayField,iHorizontalOffset,0,iHorizontalOffset,112,2);

			// Draw the test asteroid objects
			if (iMode==0)
			{
				PrintString(SCR_2_PLANE, 0, 0, 0, "ASTEROIDS:");
				PrintDecimal(SCR_2_PLANE, 0, 11, 0, DefenderoidsLevels[iCurrentLevel].AsteroidCount ,2);
				for (iLoopAsteroid=0;iLoopAsteroid<DefenderoidsLevels[iCurrentLevel].AsteroidCount;iLoopAsteroid++)
				{
					// Draw asteroids at fixed points
					DrawVectorObject((u16*)bmpPlayField,Asteroid[iLoopAsteroid],iHorizontalOffset);
				}
			}

			//////////////////////////////////////////////////////
			// Draw shot objects
			/////////////////////////////////////////////////////
			if (iMode==1)
			{
				PrintString(SCR_2_PLANE, 0, 0, 0, "SHOTS:");
				for (iLoopShot=0;iLoopShot<4;iLoopShot++)
				{
					PrintDecimal(SCR_2_PLANE,0,0,1+iLoopShot,iLoopShot,1);
					PrintString(SCR_2_PLANE,0,1,1+iLoopShot,":(   ,   )");
					if (Shots[iLoopShot].Scale == 1)
					{
						PrintDecimal(SCR_2_PLANE,0,3,1+iLoopShot,Shots[iLoopShot].Position.x,3);
						PrintDecimal(SCR_2_PLANE,0,7,1+iLoopShot,Shots[iLoopShot].Position.y,3);
						DrawVectorSprite((u16*)bmpPlayField, Shots[iLoopShot], iHorizontalOffset);
					}
					else{
						PrintString(SCR_2_PLANE,0,3,1+iLoopShot,"---");
						PrintString(SCR_2_PLANE,0,7,1+iLoopShot,"---");
					}
				}
			}

			//Player Object
			DrawVectorSpriteAbsolute((u16*)bmpPlayField, PlayerOne);

			// Then copy the bitmap back into tile memory...
			CopyBitmap((u16*)bmpPlayField, bgTileBase);

			// Print debug information
			PrintString(SCR_2_PLANE, 0, 0, 18, "FPS:");
			PrintDecimal(SCR_2_PLANE, 0, 4, 18, 60/(VBCounter-iStartFrame), 2);
			PrintString(SCR_2_PLANE, 0, 0, 17, "HZL:");
			PrintDecimal(SCR_2_PLANE, 0, 4, 17, iHorizontalOffset, 3);
			PrintString(SCR_2_PLANE, 0, 8, 17, "X/Y:");
			PrintDecimal(SCR_2_PLANE, 0, 12, 17, PlayerOne.Position.x, 3);
			PrintString(SCR_2_PLANE, 0, 15, 17, "/:");
			PrintDecimal(SCR_2_PLANE, 0, 16, 17, PlayerOne.Position.y, 3);
			//PrintDecimal(SCR_2_PLANE, 0, 8, 17, Shots[0].Position.x, 8);

		} // Level Loop

	} // Lives Loop
}