#include "ngpc.h"
#include "ngpc.h"
#include "library.h"
#include "Defenderoids.h"
#include "veclib.h"
#include "Tiles\Sprites.c"
#include "Tiles\Logo.c"
#include "Tiles\Border.c"
#include "VectorObjects.h"
#include "vgmplayer.h"
#include "Sound\noise.c"
#include "Sound\shoot.c"
#include "Sound\bgm1.c"
#include "Sound\zap.c"

/*************************************************
 * TODO List
 * 
 * Asteroids
 * 	Asteroid collision - Done
 *  Asteroid split into smaller - Done
 *  Asteroid drop pictcell - Done
 * 	Rejig Asteroid creation to get more variety - Done
 *  Asteroids can sometimes get weird shapes (flat lines, points etc)
 * Add city - Done
 * Collect resources
 * 	Pictcell base movement - Done
 * 	Lemmanoids pickup - Done
 * 	Ship pickup
 * 	Add to city - Done
 * Aliens
 *  Different enemy types
 * 	Capture Lemmanoids
 *  Spawn on a timer
 * Ship collisions
 * Shot collisions
 *  Proximity collision - Done
 *  Fine tune collision
 * Level success/failure modes
 * Bring the Qix back?
 * 
 ************************************************/


//Helper functions
/////////////////////////////////////////////////////
// Create a unique asteroid object
/////////////////////////////////////////////////////
VECTOROBJECT CreateAsteroid(s16 x, s16 y, u8 Scale)
{
	VECTOROBJECT vReturn;
	u8 iLoopAsteroidPoint;
	u8 iTemplateID;
	u8 iPointScaling;
	u8 iScale;

	VECTOROBJECT AsteroidTemplate= {{0,0},{0,0},{0,0},9,{{0,1,1},{1,1,1},{1,0,1},{1,-1,1},{0,-1,1},{-1,-1,1},{-1,0,1},{-1,1,1},{0,1,1}},0,0,0};

	//InitialiseQRandom();

	iTemplateID=2;
	vReturn=AsteroidTemplate;
	for (iLoopAsteroidPoint=0;iLoopAsteroidPoint<vReturn.Points;iLoopAsteroidPoint++)
	{
		iPointScaling=QRandom();
		iScale=1;
		if (iPointScaling>64)
		{
			iScale=2;
		}
		if (iPointScaling>128)
		{
			iScale=3;
		}
		if (iPointScaling>192)
		{
			iScale=4;
		}
		vReturn.VectorList[iLoopAsteroidPoint].x=vReturn.VectorList[iLoopAsteroidPoint].x*iScale;
		vReturn.VectorList[iLoopAsteroidPoint].y=vReturn.VectorList[iLoopAsteroidPoint].y*iScale;
	}
	// First and Last points in the asteroid have to be the same
	vReturn.VectorList[vReturn.Points-1].x=vReturn.VectorList[0].x;
	vReturn.VectorList[vReturn.Points-1].y=vReturn.VectorList[0].y;

	vReturn.Position.x=x;
	vReturn.Position.y=y;

	// Set scale, rotation and speed randomly
	vReturn.Scale=Scale;
	vReturn.RotationSpeed=QRandom()>>4;
	vReturn.RotationAngle=QRandom();
	//Asteroid[iLoopAsteroid].Position.x=((u16)QRandom())<<4;
	//Asteroid[iLoopAsteroid].Position.y=((u16)QRandom())<<4;
	vReturn.MovementVector.x=QRandom();
	vReturn.MovementVector.y=QRandom()>>2;

	return vReturn;
}

/////////////////////////////////////////////////////
// Sprite creator function
//
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
//
/////////////////////////////////////////////////////
SPRITE CreateSprite(u16 x, u16 y, u8 ID, u8 Type, u8 Direction, u8 Frame)
{
	u8 iPalette;

	SPRITE sprReturn;
	sprReturn.Position.x = x;
	sprReturn.Position.y = y;
	sprReturn.SpriteID = ID;
	sprReturn.SpriteType = Type;
	sprReturn.Direction = Direction;
	sprReturn.BaseTile = spTileBase + ID;
	sprReturn.Frame = Frame;
	sprReturn.RelatedSpriteID = 0;

	// map the sprites to their selected Palette
	switch (sprReturn.SpriteType)
	{
		case sprInvader:
			iPalette=(u8)(PAL_INVADER);
			break;
		case sprLemmanoid:
			iPalette=(u8)(PAL_LEMMANOID);
			break;
		case sprCity:
			iPalette=(u8)(PAL_CITY);
			break;
		case sprPictcell:
			iPalette=(u8)(PAL_PICTSEL);
			break;
		default:
			iPalette=(u8)(PAL_SPRITE);
			break;
	}

	// Sprites are created with a Null sprite tile and an empty NGPC sprite object
	// The sprite animation and object properties are both maintained in the main
	// sprite animation loop which checks to see if the sprite is visible etc

	CopyAnimationFrame(Sprites, sprReturn.BaseTile, 1, sprMisc);
	SetSprite(sprReturn.SpriteID, sprReturn.BaseTile , 0, 0, 0, iPalette);

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
		//Replace tile with the empty sprite if it's outside the visible area
		CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, sprMisc);
	}
	else {
		//Sprite Tile selection will differ depending on SpriteType
		//Sprite Misc -- sprMisc + type (Empty, Umbrella, Builders Hat, Hod, etc)
		//Invader -- sprInvader + 4 frames
		//Lemmanoid -- sprLemmanoid + Direction + 4 frames
		//Spacie -- sprSpacie + Type + 4 frames
		//Pictcell -- sprPictCell + 4 frames
		//City -- sprCity + blockid + age
		//Default -- No animation
		switch (sprSprite.SpriteType)
		{
			case sprInvader:
				CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, (sprSprite.SpriteType) + sprSprite.Frame);
				break;
			case sprPictcell:
				CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, (sprSprite.SpriteType) + sprSprite.Frame);
				break;
			case sprLemmanoid:
				// Flipped sprites behave weirdly - are their x/y positions also flipped?
				//if(sprSprite.Direction==DIR_WEST) {
				//	FlipSprite(sprSprite.SpriteID,1,0);
				//}
				CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, (sprSprite.SpriteType) + (sprSprite.Direction) + sprSprite.Frame);
				break;
			case sprCity:
				// Use Direction as BlockID (0-3)
				// Use Frame as age (0,4,18,12)
				CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, (sprSprite.SpriteType) + (sprSprite.Direction) + sprSprite.Frame);
				break;
			default:
				CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, (sprSprite.SpriteType));
				break;
		}

	}

	// Cast the relativeX position to (u8) once the bounds checking has been completed
	SetSpritePosition(sprSprite.SpriteID, (u8)iRelativeX, iRelativeY);

}

bool CheckSpriteCollision(POINT object1, SPRITEPOINT object2)
{
	bool bReturn;
	u16 iHorizontalDistance;
	u16 iWrapDistance;
	u16 iVerticalDistance;
	const u16 iCollisionDistance=1024;
	const u16 iWrapPoint=65535;
	bReturn=0;

	if (object1.x>=object2.x)
	{
		iHorizontalDistance=object1.x-object2.x;
		iWrapDistance=(iWrapPoint-object1.x)+object2.x;
	}
	else
	{
		iHorizontalDistance=object2.x-object1.x;
		iWrapDistance=(iWrapPoint-object2.x)+object1.x;
	}
	if (object1.y>=object2.y)
	{
		iVerticalDistance=object1.y-object2.y;
	}
	else
	{
		iVerticalDistance=object2.y-object1.y;
	}

	if ((iHorizontalDistance<=iCollisionDistance||iWrapDistance<=iCollisionDistance)&&iVerticalDistance<=iCollisionDistance)
	{
		// Our objects are within 1024 units (8 pixels give or take)
		// Can now do finer detail checks if needed
		bReturn=1;
	}

	return bReturn;
}

bool CheckAsteroidCollision(POINT object1, POINT object2)
{
	bool bReturn;
	u16 iHorizontalDistance;
	u16 iWrapDistance;
	u16 iVerticalDistance;
	const u16 iCollisionDistance=1024;
	const u16 iWrapPoint=65535;
	bReturn=0;

	if (object1.x>=object2.x)
	{
		iHorizontalDistance=object1.x-object2.x;
		iWrapDistance=(iWrapPoint-object1.x)+object2.x;
	}
	else
	{
		iHorizontalDistance=object2.x-object1.x;
		iWrapDistance=(iWrapPoint-object2.x)+object1.x;
	}
	if (object1.y>=object2.y)
	{
		iVerticalDistance=object1.y-object2.y;
	}
	else
	{
		iVerticalDistance=object2.y-object1.y;
	}

	if ((iHorizontalDistance<=iCollisionDistance||iWrapDistance<=iCollisionDistance)&&iVerticalDistance<=iCollisionDistance)
	{
		// Our objects are within 1024 units (8 pixels give or take)
		// Can now do finer detail checks if needed
		bReturn=1;
	}

	return bReturn;
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
		voReturn.VectorList[iPointLoop].x = 0;
		voReturn.VectorList[iPointLoop].y = 0;
		voReturn.VectorList[iPointLoop].colour = QRandom()>>6;
	}
	voReturn.Origin.x = 0;
	voReturn.Origin.y = 0;
	voReturn.MovementVector.x = 0;
	voReturn.MovementVector.y = 0;
	voReturn.Position.x = spPosition.x>>SPRITE_SCALE;
	voReturn.Position.y = spPosition.y>>SPRITE_SCALE;
	voReturn.RotationAngle = iDirection+128;

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
	PrintString(SCR_1_PLANE, 0, 0, 16, "ENERGY:");

}

void DrawGameOverScreen()
{
	u8 iLoopX;
	u8 iLoopY;

	//Corners
	PutTile(SCR_1_PLANE,0,2,7,borderTilebase);
	PutTile(SCR_1_PLANE,0,17,7,borderTilebase+2);
	PutTile(SCR_1_PLANE,0,2,11,borderTilebase+6);
	PutTile(SCR_1_PLANE,0,17,11,borderTilebase+8);

	//Top/Bottom edges
	for(iLoopX=3;iLoopX<=16;iLoopX++){
		PutTile(SCR_1_PLANE,0,iLoopX,7,borderTilebase+1);
		PutTile(SCR_1_PLANE,0,iLoopX,11,borderTilebase+7);
	}
	//Left/Right edges
	for(iLoopY=8;iLoopY<=10;iLoopY++){
		PutTile(SCR_1_PLANE,0,2,iLoopY,borderTilebase+3);
		PutTile(SCR_1_PLANE,0,17,iLoopY,borderTilebase+5);
	}

	//Other window dressing (debug info and scorecard/lives count etc)
	PrintString(SCR_1_PLANE, 0, 3, 8, "  GAME OVER");
	PrintString(SCR_1_PLANE, 0, 3, 9, "The Lemmanoids");
	PrintString(SCR_1_PLANE, 0, 3, 10, "  are doomed");

	Sleep(120);

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
	u8 iPalette;
	u8 iStar;
	u16 iStarColour;

	// Wait for the "A" button to be released
	while (JOYPAD & J_A);

	//Install the logo tileset
	InstallTileSetAt(Logo, sizeof(Logo)/2, LogoTileBase);

	//Clear the screen

	SetBackgroundColour(0);

	ClearScreen(SCR_1_PLANE);
	ClearScreen(SCR_2_PLANE);

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,0,0), RGB(15,15,0), RGB(15,15,15));
	SetPalette(SCR_1_PLANE, 1, 0, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0));
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
		iPalette=1;
		for (iLoopY=0;iLoopY<=9;iLoopY++)
		{
			//Top 2 and bottom 2 lines of the logo are the star field
			if (iLoopY>=2) {iPalette=0;}
			if (iLoopY>=8) {iPalette=1;}
			PutTile(SCR_1_PLANE, iPalette, iLoopX, iLoopY+4, LogoTileBase +(iLoopY*20)+iLoopX);
		}
	}

	PrintString(SCR_1_PLANE, 0, 2, 1, "  PRESENTED BY");
	PrintString(SCR_1_PLANE, 0, 2, 2, "   AHCHAY.COM");
	PrintString(SCR_1_PLANE, 0, 2, 14, "PRESS A TO START");

	iGameMode=0;

	iStarColour=0;

	while (iGameMode==0)
	{
		// Just a quick palette shift for the starfield
		// Select a star to cycle (0 - no stars, 1-3 palette colours)
		if(iStarColour==0)
		{
			iStar=(QRandom()>>6)+1;
			iStarColour=RGB(15,15,15);
		}
		// Set the selected star to a random colour
		// This might be a bit blinkenlichten but let's see what it looks like
		switch (iStar)
		{
			case 0:
				SetPalette(SCR_1_PLANE, 1, 0, 0, 0, 0);
				break;
			case 1:
				SetPalette(SCR_1_PLANE, 1, 0, iStarColour, 0, 0);
				break;
			case 2:
				SetPalette(SCR_1_PLANE, 1, 0, 0, iStarColour, 0);
				break;
			case 3:
				SetPalette(SCR_1_PLANE, 1, 0, 0, 0, iStarColour);
				break;
		}
		iStarColour--;

		// Start the game if the player pushes either button
		if (JOYPAD & J_A)
		{
			iGameMode=1;
			//VGM_PlaySFX((u8*)noise,1);
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
	u8 iPictcellLoop;
	u16 iCounter;
	u8 iEngineLoop;
	s16 iVelocityX;
	s16 iVelocityY;
	u16 iHeightMapLoop;
	u16 iHorizontalOffset;
	u8 iCurrentLevel;
	LEVEL lvCurrent;
	u8 iLives;
	u8 iLoopShot;
	u8 iLoopExplosion;
	u8 iLoopExplosionPoint;
	u8 iEngineNoise;
	bool bShotType;
	bool bShoot;
	u8 iEnergyGauge;
	u8 iEnergyLoop;
	u8 iGaugePalette;
	u8 iCityBlock;
	u8 iMinAge;
	u8 iCityLoop;
	u8 iUmbrellaLoop;
	u8 iLemmanoidLoop;

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

	VECTOROBJECT Asteroid[MAX_ASTEROID+1];

	VECTOROBJECT Shots[4];

	VECTOROBJECT Explosions[8];

	SPRITE SpriteList[MAX_SPRITE+1];

	VECTOROBJECT PlayerOne;

	InitialiseQRandom();

	// Set up the palettes
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPRITE), RGB(0,0,0), RGB(15,15,15), RGB(11,11,11), RGB(5,5,5));
	SetPalette(SPRITE_PLANE, (u8)(PAL_INVADER), RGB(0,0,0), RGB(0,15,0), RGB(15,15,0), RGB(15,0,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_LEMMANOID), RGB(0,0,0), RGB(15, 11, 12), RGB(0,0,15), RGB(0,15,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_PICTSEL), RGB(0,0,0), RGB(9, 9, 9), RGB(15,0,0), RGB(0,0,15));
	SetPalette(SPRITE_PLANE, (u8)(PAL_CITY), RGB(0,0,0), RGB(0, 0, 15), RGB(15,0,0), RGB(0,15,0));
	// Lemmanoid palettes for debugging if I need to distinguish between individuals
	// SpriteID:
	// 	4 = White
	//	5 = Red
	//	6 = Green
	//	7 = Blue			
	//SetPalette(SPRITE_PLANE, (u8)(PAL_LEMMANOID), RGB(0,0,0), RGB(15, 15, 15), RGB(15,15,15), RGB(15,15,15));
	//SetPalette(SPRITE_PLANE, (u8)(PAL_LEMMANOID+1), RGB(0,0,0), RGB(15, 0, 0), RGB(15, 0, 0), RGB(15, 0, 0));
	//SetPalette(SPRITE_PLANE, (u8)(PAL_LEMMANOID+2), RGB(0,0,0), RGB(0, 15, 0), RGB(0, 15, 0), RGB(0, 15, 0));
	//SetPalette(SPRITE_PLANE, (u8)(PAL_LEMMANOID+3), RGB(0,0,0), RGB(0, 0, 15), RGB(0,0,15), RGB(0,0,15));

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));
	SetPalette(SCR_2_PLANE, 0, 0, RGB(15,15,15), RGB(8,8,8), RGB(4,4,4));
	SetPalette(SCR_2_PLANE, 1, 0, RGB(15,0,0), RGB(8,8,8), RGB(4,4,4));

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

	VGM_PlayBGM_Loop((u8*)bgm1, bgm1_loop_point);

	iEnergyGauge=96;

	while ((!(JOYPAD & J_OPTION)) && iEnergyGauge>0)
	{
		////////////////////////////////////////////////////////////
		// Level setup
		////////////////////////////////////////////////////////////

		lvCurrent=DefenderoidsLevels[iCurrentLevel];

		// City (create as sprites 0-3 so I can find them easier later)
		SpriteList[0] = CreateSprite(16<<SPRITE_SCALE,92<<SPRITE_SCALE,1,sprCity,CITYBLOCK2,0);
		SpriteList[1] = CreateSprite(24<<SPRITE_SCALE,92<<SPRITE_SCALE,2,sprCity,CITYBLOCK3,0);
		SpriteList[2] = CreateSprite(8<<SPRITE_SCALE,92<<SPRITE_SCALE,0,sprCity,CITYBLOCK1,0);
		SpriteList[3] = CreateSprite(32<<SPRITE_SCALE,92<<SPRITE_SCALE,3,sprCity,CITYBLOCK4,0);

		// Invaders - do I want these to all appear at the start of the level or be phased in somehow?
		for (iSpriteLoop=4;iSpriteLoop<lvCurrent.InvaderCount+4;iSpriteLoop++)
		{
			//x, y, ID, Type, Direction, Frame
			//Invaders always move south on creation
			//Eventually, we'll spawn these on a timer rather than just dumping them all in at the outset of the level
			SpriteList[iSpriteLoop] = CreateSprite(((u16)QRandom())<<8,((u16)QRandom())<<4,iSpriteLoop,sprInvader,DIR_SOUTH,0);
		}

		// Lemmanoids
		for (;iSpriteLoop<lvCurrent.InvaderCount+lvCurrent.LemmanoidCount+4;iSpriteLoop++)
		{
			//x, y, ID, Type, Direction, Frame
			//Lemmanoids move either west or east (DIR_EAST + [0|1]*DIR_WEST)
			//Spawn them vertically at the median point of the terrain
			SpriteList[iSpriteLoop] = CreateSprite(((u16)QRandom())<<8,100<<8,iSpriteLoop,sprLemmanoid,DIR_EAST + ((QRandom()>>7)*DIR_WEST),(QRandom()>>5));
		}

		// Asteroids
		for (iLoopAsteroid=0;iLoopAsteroid<lvCurrent.AsteroidCount;iLoopAsteroid++)
		{
			Asteroid[iLoopAsteroid] = CreateAsteroid(((u16)QRandom())<<8,((u16)QRandom())<<5,4);
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
		while ((!(JOYPAD & J_OPTION)) && iEnergyGauge>0 && lvCurrent.InvaderCount>0)
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
						
						VGM_PlaySFX((u8*)zap,1);

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

						/*
							Okay, so collision detection part deux

							Check the X Position against other visible objects, no need to go further if this differs by 
							a significant amount

							In fact, is a simple x/y check enough?

						*/

						for (iSpriteLoop=0;iSpriteLoop<MAX_SPRITE;iSpriteLoop++)
						{
							if (SpriteList[iSpriteLoop].SpriteType == sprInvader)
							{
								if (CheckSpriteCollision(Shots[iLoopShot].Position,SpriteList[iSpriteLoop].Position))
								{
									// Add an explosion
									for (iLoopExplosion=0;iLoopExplosion<8;iLoopExplosion++)
									{
										if (Explosions[iLoopExplosion].Scale == 0)
										{
											Explosions[iLoopExplosion]=CreateExplosion(SpriteList[iSpriteLoop].Position, Shots[iLoopShot].RotationAngle);

											// Need a better 'splode noise
											//VGM_PlaySFX((u8*)noise,1);

											iLoopExplosion = 9;
										}
									} 
									SpriteList[iSpriteLoop].SpriteType = sprMisc;
									if (SpriteList[iSpriteLoop].RelatedSpriteID!=0)
									{
										// Invader has a captured Lemmanoid, give 'em an umbrella and let them go
										for(iUmbrellaLoop=0;iUmbrellaLoop<=MAX_SPRITE;iUmbrellaLoop++)
										{
											//Search for the first "empty" sprite
											if (SpriteList[iUmbrellaLoop].SpriteType==sprMisc+1)
											{
												SpriteList[iUmbrellaLoop]=CreateSprite(0,0,iUmbrellaLoop,sprPictcell,DIR_SOUTH,0);
												SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].RelatedSpriteID=iUmbrellaLoop;
												SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Direction=DIR_SOUTH;
												iUmbrellaLoop=MAX_SPRITE+1;
											}
										}

									}
									SetSprite(SpriteList[iSpriteLoop].SpriteID, 0, 0, 0, 0, PAL_SPRITE);
									Shots[iLoopShot].Scale = 0;
									iSpriteLoop = lvCurrent.InvaderCount + 1;


								}
							}

						}

						// Check for Asteroid collision as well
						// On collision: destroy asteroid and create some smaller ones...
						// When smallest asteroid is destroyed, create a Pictcell sprite

						if (Shots[iLoopShot].Scale == 1)
						{
						for (iLoopAsteroid=0;iLoopAsteroid<(lvCurrent.AsteroidCount);iLoopAsteroid++)
							{
								if (CheckAsteroidCollision(Shots[iLoopShot].Position,Asteroid[iLoopAsteroid].Position))
								{
									// Use the Asteroid Scale object to determine what happens next
									if (Asteroid[iLoopAsteroid].Scale>1)
									{
										//Split asteroid into smaller chunks
										//First reduce the size of the current asteroid
										Asteroid[iLoopAsteroid]=CreateAsteroid(Asteroid[iLoopAsteroid].Position.x,Asteroid[iLoopAsteroid].Position.y,Asteroid[iLoopAsteroid].Scale-1);
										//Then spawn new ones (up to the MAX_ASTEROID limit)
										if(lvCurrent.AsteroidCount<MAX_ASTEROID)
										{
											Asteroid[lvCurrent.AsteroidCount]=CreateAsteroid(Asteroid[iLoopAsteroid].Position.x,Asteroid[iLoopAsteroid].Position.y,Asteroid[iLoopAsteroid].Scale);
											lvCurrent.AsteroidCount++;
										}
									}
									else
									{
										//Smallest size asteroid destroyed
										//First create a Pictcell
										for(iSpriteLoop=0;iSpriteLoop<=MAX_SPRITE;iSpriteLoop++)
										{
											//Search for the first "empty" sprite
											if (SpriteList[iSpriteLoop].SpriteType==sprMisc)
											{
												SpriteList[iSpriteLoop]=CreateSprite(Asteroid[iLoopAsteroid].Position.x,Asteroid[iLoopAsteroid].Position.y,iSpriteLoop,sprPictcell,DIR_SOUTH,0);
												iSpriteLoop=MAX_SPRITE+1;
											}
										}
										//Then shuffle asteroids up to fill the gap and reduce lvCurrent.AsteroidCount
										while (iLoopAsteroid<lvCurrent.AsteroidCount)
										{
											Asteroid[iLoopAsteroid]=Asteroid[++iLoopAsteroid];
										}
										lvCurrent.AsteroidCount--;
									}
									iLoopAsteroid=MAX_ASTEROID;
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
					if (Explosions[iLoopExplosion].Scale != 0)
					{
						//Increase the scale for later frames
						Explosions[iLoopExplosion].Scale+=(Explosions[iLoopExplosion].RotationSpeed>>2);
						for (iLoopExplosionPoint=0;iLoopExplosionPoint<Explosions[iLoopExplosion].Points;iLoopExplosionPoint++)
						{
							// Why not just a few QRandom() calls within the rotation speed area?
							// Looks ace. Need to figure out the rotation angle to get the direction right is all
							Explosions[iLoopExplosion].VectorList[iLoopExplosionPoint].x +=(QRandom()>>(8-Explosions[iLoopExplosion].RotationSpeed));
							Explosions[iLoopExplosion].VectorList[iLoopExplosionPoint].y +=(QRandom()>>(8-Explosions[iLoopExplosion].RotationSpeed));
						}

						DrawVectorSprite((u16*)bmpPlayField, Explosions[iLoopExplosion], iHorizontalOffset);

						// Destroy the Explosion after a few frames
						Explosions[iLoopExplosion].RotationSpeed++;
						if (Explosions[iLoopExplosion].RotationSpeed == 7)
						{
							Explosions[iLoopExplosion].Scale = 0;
						}
					}
				}
			}

			//////////////////////////////////////////////////////
			// Asteroids
			//////////////////////////////////////////////////////
			for (iLoopAsteroid=0;iLoopAsteroid<lvCurrent.AsteroidCount;iLoopAsteroid++)
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
			for (iSpriteLoop=0;iSpriteLoop<MAX_SPRITE;iSpriteLoop++)
			{
				if (!(SpriteList[iSpriteLoop].SpriteType == sprMisc))
				{
					switch(SpriteList[iSpriteLoop].SpriteType)
					{
						case sprInvader:
							// Invaders bob up and down

							// Need to home in on a Lemmanoid if there's one nearby
							// So, if Y-position is <$foo from the heightmap, and a Lemmanoid is within $bar horizontal units, start adjusting the trajectory towards the Lemmanoid
							// On capture. Change the Lemmanoid direction to North and switch the related flag to the capturing alien
							// Aliens with cargo will move half as slowly as normal, and maybe wobble a bit on the way up or something
							switch(SpriteList[iSpriteLoop].Direction)
							{
								case DIR_SOUTH:
									// Check for any nearby Lemmanoids
									if (SpriteList[iSpriteLoop].Position.y>(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-32)<<SPRITE_SCALE)
									{
										for(iLemmanoidLoop=0;iLemmanoidLoop<MAX_SPRITE;iLemmanoidLoop++)
										{
											if (WrapDistance(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE,SpriteList[iLemmanoidLoop].Position.x>>SPRITE_SCALE,SPRITE_MAX_WIDTH)<32)
											{
												if(SpriteList[iSpriteLoop].Position.x>SpriteList[iLemmanoidLoop].Position.x)
													SpriteList[iSpriteLoop].Position.x-=32;
												else
													SpriteList[iSpriteLoop].Position.x+=32;
												iLemmanoidLoop=MAX_SPRITE;
											}
										}
									}
									SpriteList[iSpriteLoop].Position.x=SpriteList[iSpriteLoop].Position.x+QRandom()-QRandom();
									SpriteList[iSpriteLoop].Position.y+=64;
									if (SpriteList[iSpriteLoop].Position.y>(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]+4)<<SPRITE_SCALE)
									{
										// We only need to check for Lemmanoid capture at the southern-most point
										SpriteList[iSpriteLoop].Direction = DIR_NORTH;
										for(iLemmanoidLoop=0;iLemmanoidLoop<MAX_SPRITE;iLemmanoidLoop++)
										{
											if (WrapDistance(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE,SpriteList[iLemmanoidLoop].Position.x>>SPRITE_SCALE,SPRITE_MAX_WIDTH)<8)
											{
												//Gotcha
												PrintString(SCR_2_PLANE,0,0,18,"Gotcha!");
												SpriteList[iSpriteLoop].RelatedSpriteID=SpriteList[iLemmanoidLoop].RelatedSpriteID;
												SpriteList[iLemmanoidLoop].RelatedSpriteID=SpriteList[iSpriteLoop].RelatedSpriteID;
												SpriteList[iLemmanoidLoop].Direction=DIR_NORTH;
												iLemmanoidLoop=MAX_SPRITE;
											}
										}
									}
									break;
								case DIR_NORTH:
									// Slow down if carrying anything
									SpriteList[iSpriteLoop].Position.x=SpriteList[iSpriteLoop].Position.x+QRandom()-QRandom();
									if (SpriteList[iSpriteLoop].RelatedSpriteID == 0)
										SpriteList[iSpriteLoop].Position.y-=64;
									else
										SpriteList[iSpriteLoop].Position.y-=32;

									if (SpriteList[iSpriteLoop].Position.y>(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]+4)<<SPRITE_SCALE)
									{
										SpriteList[iSpriteLoop].Direction = DIR_SOUTH;
									}
									break;
							}
							SpriteList[iSpriteLoop].Frame++;
							if (SpriteList[iSpriteLoop].Frame>3) SpriteList[iSpriteLoop].Frame=0;
							break;
						case sprLemmanoid:
							// Lemmanoids lope along the mountain left and right, unless they've been captured
							switch(SpriteList[iSpriteLoop].Direction)
							{
							case DIR_WEST:
									SpriteList[iSpriteLoop].Position.x-=128;
									SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-4)<<SPRITE_SCALE;
									break;
								case DIR_EAST:
									SpriteList[iSpriteLoop].Position.x+=128;
									SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-4)<<SPRITE_SCALE;
									break;
								case DIR_NORTH:
									// Captured, so movement is tied to the capturing alien (RelatedSpriteID)
									SpriteList[iSpriteLoop].Position.x=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.x;
									SpriteList[iSpriteLoop].Position.y=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.y+8<<SPRITE_SCALE;
									break;
								case DIR_SOUTH:
									// Floating back to earth
									// Give 'em a little wobble
									SpriteList[iSpriteLoop].Position.x=SpriteList[iSpriteLoop].Position.x+QRandom()-QRandom();
									SpriteList[iSpriteLoop].Position.y+=32;
									if (SpriteList[iSpriteLoop].Position.y>(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]+4)<<SPRITE_SCALE){
										// Resume walking
										if (QRandom()<128)
											SpriteList[iSpriteLoop].Direction = DIR_EAST;
										else
											SpriteList[iSpriteLoop].Direction = DIR_WEST;
									}
									break;
								default:
									break;
							}
							// Pickup Pictcell?
							// Iterate through the sprite list again? Any way to avoid that?
							// Pickup Pictcell when pictcell.Direction==0 && pictcell.Position.x=Lemmanoid.Position.x (don't need to check for y)
							// Need to also make sure that each Lemmanoid *only* picks up one Pictcell
							if (SpriteList[iSpriteLoop].RelatedSpriteID==0)
							{
								for(iPictcellLoop=0;iPictcellLoop<MAX_SPRITE;iPictcellLoop++)
								{
									// Never actually get picked up?
									if (SpriteList[iPictcellLoop].SpriteType==sprPictcell && SpriteList[iPictcellLoop].Direction==0 && WrapDistance(SpriteList[iPictcellLoop].Position.x,SpriteList[iSpriteLoop].Position.x,SPRITE_MAX_WIDTH)<1024 && SpriteList[iPictcellLoop].RelatedSpriteID==0)
									{
										// Bingo. Attach this sprite to the Lemmanoid and change the type to the left/right carrying Hod
										// Use MasterSpriteID to chain it to another Sprite
										SpriteList[iPictcellLoop].Direction=SpriteList[iSpriteLoop].Direction;
										SpriteList[iPictcellLoop].RelatedSpriteID=SpriteList[iSpriteLoop].SpriteID;
										SpriteList[iSpriteLoop].RelatedSpriteID=SpriteList[iPictcellLoop].SpriteID;
										if(SpriteList[iSpriteLoop].Direction==DIR_EAST)
											SpriteList[iPictcellLoop].SpriteType=sprMisc+3;
										else
											SpriteList[iPictcellLoop].SpriteType=sprMisc+2;
										iPictcellLoop=MAX_SPRITE;
									}
								}
							}
							SpriteList[iSpriteLoop].Frame++;
							if (SpriteList[iSpriteLoop].Frame>3) SpriteList[iSpriteLoop].Frame=0;
							break;
						case sprMisc+2:
						case sprMisc+3:
							// Carried Pictcells will move along with their associated Lemmanoid
							// Check for being near a city, add to the city and remove the link to the Lemmanoid
							if (SpriteList[iSpriteLoop].Position.x>MIN_CITY&&SpriteList[iSpriteLoop].Position.x<MAX_CITY)
							{
								// Clear from Lemmanoid
								SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].RelatedSpriteID=0;
								// Destroy pictcell
								SpriteList[iSpriteLoop].SpriteType=sprMisc;
								SpriteList[iSpriteLoop].RelatedSpriteID=0;
								// Add to city
								// Find "first" city block with minimum age (frame)
								// Start at city block 5 - so we don't keep adding once the city is complete
								iCityBlock=4;
								iMinAge=CITYAGE4;
								for(iCityLoop=0;iCityLoop<4;iCityLoop++)
								{
									if(SpriteList[iCityLoop].Frame<iMinAge)
									{
										iMinAge=SpriteList[iCityLoop].Frame;
										iCityBlock=iCityLoop;
									}
								}
								if (iCityBlock<4)
								{
									SpriteList[iCityBlock].Frame+=4;
									PrintDecimal(SCR_2_PLANE,0,iCityBlock<<2,18,SpriteList[iCityBlock].Frame,3);
								}
							}
							
							SpriteList[iSpriteLoop].Position.x=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.x;
							SpriteList[iSpriteLoop].Position.y=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.y-256;

							break;
						case sprPictcell:
							// Pictcells will float to the ground level and then wait to be picked up
							if (SpriteList[iSpriteLoop].Direction== DIR_SOUTH)
							{
								SpriteList[iSpriteLoop].Position.y+=64;
								if (SpriteList[iSpriteLoop].Position.y>((u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]+4)<<SPRITE_SCALE)-1024)
								{
									SpriteList[iSpriteLoop].Direction = 0;
									SpriteList[iSpriteLoop].RelatedSpriteID = 0;
								}
							}
							SpriteList[iSpriteLoop].Frame++;
							if (SpriteList[iSpriteLoop].Frame>3) SpriteList[iSpriteLoop].Frame=0;
							break;
						default:
							break;
					}

					DrawSprite(SpriteList[iSpriteLoop],iHorizontalOffset);
				}
			}

			//////////////////////////////////////////////////////
			// Score and other dressing
			// Mostly debug information at the moment
			//////////////////////////////////////////////////////

			// Energy gauge
			iGaugePalette=1;
			for(iEnergyLoop=1;iEnergyLoop<=(iEnergyGauge>>3);iEnergyLoop++)
			{
				PutTile(SCR_2_PLANE, iGaugePalette, 6+iEnergyLoop, 16, 8);
				if(iEnergyLoop==3) iGaugePalette=0;
			}
			PutTile(SCR_2_PLANE,iGaugePalette,6+iEnergyLoop,16,(iEnergyGauge%8));
			//iEnergyGauge--;
			//Need to add some spaces after the last tile just in case energy has reduced by more than 8 units
			//Also, remember to put bounds checking in when reducing the Energy level so that it doesn't wrap
			for(iEnergyLoop++;iEnergyLoop<=12;iEnergyLoop++)
			{
				PutTile(SCR_2_PLANE, iGaugePalette, 6+iEnergyLoop, 16, 0);
			}
			
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
			//PrintDecimal(SCR_1_PLANE, 0, 4, 18, 60/(VBCounter-iStartFrame), 2);
			//Current Horizontal Offset
			//PrintDecimal(SCR_2_PLANE, 0, 4, 17, iHorizontalOffset, 3);
			
			// Slow it down so I can look at it...
			//Sleep(60);

		} // Level Loop

		iCurrentLevel++;
		lvCurrent=DefenderoidsLevels[iCurrentLevel];

		// Close down all sprites at end of level loop
		for (iSpriteLoop=0;iSpriteLoop<64;iSpriteLoop++)
		{
			SetSprite(iSpriteLoop, 0, 0, 0, 0, PAL_SPRITE);
		}
	} // Lives Loop

	VGM_StopBGM();

	DrawGameOverScreen();

	//////////////////////////////////////////////////////
	// Game Over information etc
	//////////////////////////////////////////////////////

}