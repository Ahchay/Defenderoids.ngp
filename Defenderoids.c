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
 *  Different enemy types
 *   Other attackeroid types?
 *    Different behaviours
 *    - Sine Wave
 *    - Invader march/Galaxian swoop?
 * 		Need a rank object
 * 		New invaders will be added to the rank based on their sub-type (direction - so, 0 top rank, 1 middle and 2 bottom)
 * 		The rank will need:
 * 		- overall position (probably centred over the city?, starting at y-pos 0, x-pos never changes?)
 * 		- overall direction (left/right)
 *      - overall direction will change every $foo frames
 * 		- next invader pointer (for the shuffle)
 * 		Invaders will be added to the rank from left->right
 * 		Warp in from the top of the screen until they reach their alloted slot
 * 		Only one invader moves in each frame if they're in rank
 * 		What to do when the rank reaches ground level? Potential game/level over condition?
 *    - Missile - Done
 *    - Bomber (attack the city)
 *      Want to try and get a missile command-y vibe
 * Pause Mode
 *  Option to bring up a menu
 *  - Resume
 *  - Quit game
 *  - Other options?
 *    - Enable/Disable music
 *    - Help
 *    - About
 *    - How to play
 * Sounds
 *  Music
 *  Sound effects:
 *  - Shot
 *  - Explosion
 *    Different types for:
 *    - Invader
 *    - Planet
 *    - Qix
 *    - Asteroid
 *  - Help!
 *  - Yippee!
 *  
 * 
 * DONE List
 * Ship collisions
 * 	Asteroids - Done
 *  Invaders - Done
 * 	Pictcells - Done
 *  Qix - Done
 * Energy Gauge
 *  Add bounds checking so that energy doesn't go >DEFAULT or <0
 * QRandom() eventually tends towards zero? - Done
 * Qix
 *  Implement Qix object - Done
 *  Qix can only be harmed by ramming with the Defenderoid - Done
 *  Ramming will deplete ~50% of your energy guage over 2 seconds (or something) - Done
 *  Each 50% of energy will destroy one leg of the Qix - Done
 *  Shoot asteroids to release Pictcells as before - Done
 *  Pictcells will replenish energy - Done
 * Asteroids
 *  Sizing etc -- Might have been sorted with the QRandom() fix -- Done
 *   Still get occasional perfectly square asteroids
 *   Still get occasional too small asteroids
 *   Still a bit prone to not moving, check for minimum vector sizes?
 * 	Asteroid collision - Done
 *  Asteroid split into smaller - Done
 *  Asteroid drop pictcell - Done
 * 	Rejig Asteroid creation to get more variety - Done
 *  Asteroids can sometimes get weird shapes (flat lines, points, boxes etc) - Done
 *  Asteroids sometimes don't move/rotate after splitting - Done
 *  Asteroids always seem to rotate/move in the same direction? - Done
 * Add city - Done
 * Collect resources
 * 	Pictcell base movement - Done
 * 	Lemmanoids pickup - Done
 * 	Add to city - Done
 * Lemmanoids
 * 	Umbrellas - Done
 *  Go home - Done
 *   Check home condition - Done
 *   Destroy Lemmanoid sprite - Done
 *   Animation - Done
 *  Change to Mutanoid - Done
 *  Mutanoid behaviour - Chase player or bomb city?
 *   Rudimentary homing behaviour - Done
 *   Needs to be a bit more aggressive
 *   And actually home in on the player rather than horizontal offset
 * Aliens
 *  Different enemy types
 *   Rammer (attack the player) - use the Mutanoid - Done
 * 	Capture Lemmanoids - Done
 *   Need to refine the capture distance - Lemmanoids can be scooped up from too far - Done
 *   Seems to be triggering at opposite end of the map (spr.x - 256?) - Done (I think)
 *  Release Lemmanoids - Done
 *  Spawn on a timer
 *   If active invaders <= level invader count
 *    x% chance of creating an invader
 *    always create invaders off screen (so player position - 256)
 * Shot collisions
 *  Proximity collision - Done
 *  Explosions
 * 		Still a bit rubbish really. Often not at the right place either - Done.
 * All collisions
 *  Fine tune collision - Might actually leave this alone - Have set different granularities which seems to work well enough (see Defenderoids.h)...
 * Level success/failure modes
 * 	Success: City built, Lemmanoids rescued - Done
 *  Failure: Lemmanoids destroyed/captured (Qix space level) - Done
 *  Both:
 *   If LemmanoidCount==0
 *    Test for *any* Lemmanoids in city - then success
 *    else failure
 *  On failure:
 *   Next level will be a Qix Level
 *  Game Over: Energy gauge goes to zero
 * Transitions
 * 	Needs more pizazz - fireworks/explosions/sortathing - Done
 *   Level Success - fireworks + explosion objects - Done
 *   All Lemmanoids destroyed - spawn asteroids along heightmap - Done
 *   Ship destroyed - Explosions everywhere - Done
 * 
 ************************************************/


//Helper functions
/////////////////////////////////////////////////////
// Create a unique asteroid object
/////////////////////////////////////////////////////
SMALLVECTOROBJECT CreateAsteroid(s16 x, s16 y, u8 Scale, u8 Colour)
{
	SMALLVECTOROBJECT vReturn;
	u8 iLoopAsteroidPoint;
	u8 iTemplateID;
	u8 iPointScaling;
	u8 iScale;

	SMALLVECTOROBJECT AsteroidTemplate= {VEC_ASTEROID,{0,0},{0,0},{0,0},9,{{0,1,1},{1,1,1},{1,0,1},{1,-1,1},{0,-1,1},{-1,-1,1},{-1,0,1},{-1,1,1},{0,1,1}},0,0,0,0};

	//InitialiseQRandom();

	iTemplateID=2;
	vReturn=AsteroidTemplate;
	for (iLoopAsteroidPoint=0;iLoopAsteroidPoint<vReturn.Points;iLoopAsteroidPoint++)
	{
		iPointScaling=QRandom();
		iScale=0;
		if (iPointScaling>82)
		{
			iScale=1;
		}
		if (iPointScaling>164)
		{
			iScale=2;
		}
		vReturn.PointList[iLoopAsteroidPoint].x=vReturn.PointList[iLoopAsteroidPoint].x<<iScale;
		vReturn.PointList[iLoopAsteroidPoint].y=vReturn.PointList[iLoopAsteroidPoint].y<<iScale;
		vReturn.PointList[iLoopAsteroidPoint].colour=Colour;
	}
	// First and Last points in the asteroid have to be the same
	vReturn.PointList[vReturn.Points-1].x=vReturn.PointList[0].x;
	vReturn.PointList[vReturn.Points-1].y=vReturn.PointList[0].y;

	vReturn.Position.x=x;
	vReturn.Position.y=y;

	//Counter unused for asteroids
	vReturn.Counter=0;

	// Set scale, rotation and speed randomly
	vReturn.Scale=Scale;
	vReturn.RotationSpeed=((s8)QRandom()>>4)-((s8)QRandom()>>4);
	vReturn.RotationAngle=((s8)QRandom()-(s8)QRandom());
	//Asteroid[iLoopAsteroid].Position.x=((u16)QRandom())<<4;
	//Asteroid[iLoopAsteroid].Position.y=((u16)QRandom())<<4;
	vReturn.MovementVector.x=((s8)QRandom()-(s8)QRandom());
	vReturn.MovementVector.y=QRandom()>>2;

	return vReturn;
}

/////////////////////////////////////////////////////
// Create the Qix object
/////////////////////////////////////////////////////
SMALLVECTOROBJECT CreateQix()
{
	SMALLVECTOROBJECT vReturn;
	u8 iPointLoop;

	//Qix will start as a point.
	vReturn.ObjectType = VEC_QIX;
	vReturn.Origin.x = 8;
	vReturn.Origin.y = 8;
	vReturn.Position.x = (u16)32<<SPRITE_SCALE;
	vReturn.Position.y = (u16)64<<SPRITE_SCALE;
	switch(QRandom()>>6)
	{
		case 0:
			vReturn.MovementVector.x=-256;
			break;
		case 1:
			vReturn.MovementVector.x=-128;
			break;
		case 2:
			vReturn.MovementVector.x=-128;
			break;
		case 3:
			vReturn.MovementVector.x=256;
			break;
	}
	switch(QRandom()>>7)
	{
		case 0:
			vReturn.MovementVector.y=-128;
			break;
		case 1:
			vReturn.MovementVector.x=128;
			break;
	}
	vReturn.Points=9;
	for(iPointLoop=0;iPointLoop<vReturn.Points;iPointLoop++)
	{
		vReturn.PointList[iPointLoop].x = 8;
		vReturn.PointList[iPointLoop].y = 8;
		vReturn.PointList[iPointLoop].colour = 1;
	}
	vReturn.Scale=1;
	vReturn.RotationAngle=0;
	vReturn.RotationSpeed=0;
	// Use counter for Qix health
	vReturn.Counter=vReturn.Points<<4;
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
	u8 iChain;

	SPRITE sprReturn;
	sprReturn.Position.x = x;
	sprReturn.Position.y = y;
	sprReturn.SpriteID = ID;
	sprReturn.SpriteType = Type;
	sprReturn.Direction = Direction;
	sprReturn.BaseTile = spTileBase + ID;
	sprReturn.Frame = Frame;
	sprReturn.RelatedSpriteID = rsNone;
	sprReturn.RankColumn = 0;
	sprReturn.Initiated = 1;

	iChain=0;

	// map the sprites to their selected Palette
	switch (sprReturn.SpriteType)
	{
		case sprCatcher:
			iPalette=(u8)(PAL_INVADER);
			break;
		case sprSpacie:
		case sprSpacie+1:
		case sprSpacie+2:
			iPalette=(u8)(PAL_SPACIE+Direction);
			break;
		case sprMissile:
			iPalette=(u8)(PAL_MISSILE);
			break;
		case sprBomber+1:
			// Bomber sprites are chained...
			iChain=1;
		case sprBomber:
			iPalette=(u8)(PAL_BOMBER);
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
		case sprFirework:
		case sprFirework+4:
			iPalette=(u8)(PAL_FIREWORK+(ID%4));
			break;
		default:
			iPalette=(u8)(PAL_SPRITE);
			break;
	}

	// Sprites are created with a Null sprite tile and an empty NGPC sprite object
	// The sprite animation and object properties are both maintained in the main
	// sprite animation loop which checks to see if the sprite is on screen etc

	CopyAnimationFrame(Sprites, sprReturn.BaseTile, 1, sprMisc);
	SetSprite(sprReturn.SpriteID, sprReturn.BaseTile , iChain, 0, 0, iPalette);

	return sprReturn;
}

/////////////////////////////////////////////////////
// CreateInvader()
//	x.pos
//  y.pos
//  Sprite ID
//
// Function to create a random Invader/Enemy:
// - sprCatcher
// - sprMissile
// - sprSpacie
// - sprBomber
// 
/////////////////////////////////////////////////////
SPRITE CreateInvader(u16 x, u16 y, u8 ID)
{
	u8 iInvaderType;
	u8 iDirection;
	u8 iFrame;
	SPRITE spReturn;

	iInvaderType=QRandom()>>6;
	switch (iInvaderType)
	{
		case 2:
			// Missile type
			// Two frames
			iFrame=(QRandom()>>7);
			// Should be 0-1 (LEFT/RIGHT)
			iDirection=(QRandom()>>7)<<1;
			iInvaderType=sprMissile;
			break;
		case 3:
			// Spacie type
			// Four frames
			iFrame=(QRandom()>>6);
			// Should be 0-2 (first, second, third rank)
			// Bit of a palaver - should have gone with four spacie types...
			iDirection=QRandom();
			if (iDirection>170)
			{
				iDirection=2;
			}
			else if (iDirection>86)
			{
				iDirection=1;
			}
			else 
			{
				iDirection=0;
			}
			iInvaderType=sprSpacie;
			break;
		default:
			// Base invader/catcher type
			// Four frames
			iFrame=(QRandom()>>6);
			// Always spawn going south
			iDirection=DIR_SOUTH;
			iInvaderType=sprCatcher;
			break;
	}

	spReturn=CreateSprite(x,y,ID,iInvaderType,iDirection,iFrame);

	return spReturn;
}

/////////////////////////////////////////////////////
// Calculate sprite position relative to horizontal offset
// and whether visible on the screen.
/////////////////////////////////////////////////////
DrawSprite(SPRITE sprSprite, u16 iHorizontalOffset)
{
	u16 iRelativeX;
	u8 iRelativeY;
	u8 iSpriteTile;

	if(sprSprite.Initiated==1)
	{

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

		if(iRelativeX>147||iRelativeY>115||iRelativeX<5||iRelativeY<5) {
			//Replace tile with the empty sprite if it's outside the visible area
			iSpriteTile = sprMisc;
		}
		else
		{
			//Sprite Tile selection will differ depending on SpriteType
			//Spacie -- sprSpacie + Type + 4 frames
			switch (sprSprite.SpriteType)
			{
				case sprCatcher:
				case sprPictcell:
				case sprFirework:
				case sprFirework+4:
					// Single char animated sprites (typically four frames) - no direction element so just SpriteType + Frame
					iSpriteTile = (sprSprite.SpriteType) + sprSprite.Frame;
					break;
				case sprSpacie:
					//Spacie -- sprSpacie + sub-type/Direction (0,1,2)*4 + frame (0-3)
					iSpriteTile = (sprSprite.SpriteType) + (sprSprite.Direction<<2) + sprSprite.Frame;
					break;
				case sprLemmanoid:
				case sprCity:
				case sprMissile:
					// Direction relative sprites
					// Both for when a literal "direction" or "Frame" is used, or when the Direction attribute represents
					// something else (City Block no and Age, Spacies sub-type)
					//
					//Lemmanoid -- sprLemmanoid + Direction (East/West 0, 4) + Frame (0-3)
					//Missile -- sprMissile + Right/Left (0,2) + Frame (0,1)
					//City -- sprCity + blockid + age
					// Use Direction as BlockID (0-3)
					// Use Frame as age (0,4,18,12)
					// Net result -- sprCity + Direction + Frame (0-3)
					iSpriteTile = (sprSprite.SpriteType) + (sprSprite.Direction) + sprSprite.Frame;
					break;
				case sprBomber:
				case sprBomber+1:
					// Two frames, left or right tile depending on type
					// SpriteType (60 or 61) + Frame*2 (0 or 2)
					// (Sprite 61 position will be relative to Sprite 60 - 61.position.y=60.position.y and 61.position.x=60.position.x+(8<<SPRITE_SCALE))
					// I could chain them, but that might cause problems when I reuse sprites
					iSpriteTile = (sprSprite.SpriteType) + (sprSprite.Frame<<1);
					break;
				default:
					//Fixed, no animation, sprites
					//Empty, Umbrella, Builders Hat, Hod, etc
					//Default -- No animation, so just use SpriteType
					iSpriteTile = sprSprite.SpriteType;
					break;
			}
			// Cast the relativeX position to (u8) once the bounds checking has been completed
			SetSpritePosition(sprSprite.SpriteID, (u8)iRelativeX, iRelativeY);
		}
		CopyAnimationFrame(Sprites, sprSprite.BaseTile, 1, iSpriteTile);
	}
}

bool CheckVectorSpriteCollision(POINT object1, SPRITEPOINT object2, u16 iCollisionDistance)
{
	bool bReturn;
	u16 iHorizontalDistance;
	u16 iWrapDistance;
	u16 iVerticalDistance;
	const u16 iWrapPoint=65535;
	bReturn=0;

	// Test to the centre of the sprite
	// Vector object "origin" is already the centre point

	if (object1.x>=(object2.x+((u16)4<<SPRITE_SCALE)))
	{
		iHorizontalDistance=object1.x-(object2.x+((u16)4<<SPRITE_SCALE));
		iWrapDistance=(iWrapPoint-object1.x)+(object2.x+((u16)4<<SPRITE_SCALE));
	}
	else
	{
		iHorizontalDistance=(object2.x+((u16)4<<SPRITE_SCALE))-object1.x;
		iWrapDistance=(iWrapPoint-(object2.x+((u16)4<<SPRITE_SCALE)))+object1.x;
	}
	if (object1.y>=(object2.y+((u16)4<<SPRITE_SCALE)))
	{
		iVerticalDistance=object1.y-(object2.y+((u16)4<<SPRITE_SCALE));
	}
	else
	{
		iVerticalDistance=(object2.y+((u16)4<<SPRITE_SCALE))-object1.y;
	}

	if ((iHorizontalDistance<=iCollisionDistance||iWrapDistance<=iCollisionDistance)&&iVerticalDistance<=iCollisionDistance)
	{
		// Our objects are within 1024 units (8 pixels give or take) of centre
		// Can now do finer detail checks if needed
		bReturn=1;
	}

	return bReturn;
}

bool CheckVectorCollision(POINT object1, POINT object2, u16 iCollisionDistance)
{
	bool bReturn;
	u16 iHorizontalDistance;
	u16 iWrapDistance;
	u16 iVerticalDistance;
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
SMALLVECTOROBJECT CreateExplosion(SPRITEPOINT spPosition)
{
	SMALLVECTOROBJECT voReturn;
	u8 iPointLoop;

	voReturn.ObjectType = VEC_EXPLOSION;
	voReturn.Scale = 2;
	// Copy the Explosion object from the template
	voReturn.Points = 8;
	iPointLoop=0;
	for(iPointLoop=0;iPointLoop<voReturn.Points;iPointLoop++)
	{
		voReturn.PointList[iPointLoop].x = 0;
		voReturn.PointList[iPointLoop].y = 0;
		voReturn.PointList[iPointLoop].colour = QRandom()>>6;
	}
	voReturn.Origin.x = 0;
	voReturn.Origin.y = 0;
	voReturn.MovementVector.x = 0;
	voReturn.MovementVector.y = 0;
	voReturn.Position.x = spPosition.x>>SPRITE_SCALE;
	voReturn.Position.y = spPosition.y>>SPRITE_SCALE;
	voReturn.RotationAngle = QRandom();

	// We'll use RotationSpeed to control the life of the Explosion. Kill it when it hits a limit...
	voReturn.RotationSpeed=7;

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
	PlayerOne.ObjectType = VEC_SHIP;
	PlayerOne.Position.x = 72; // Fixed to the centre of the bitmap
	PlayerOne.Position.y = 66;
	PlayerOne.MovementVector.x = 0; // Use 256 to set up an initial drift...;
	PlayerOne.MovementVector.y = 0;
	PlayerOne.Scale = 1;
	PlayerOne.Origin.x = 4;
	PlayerOne.Origin.y = 9;
	PlayerOne.Points = 50;
	PlayerOne.RotationAngle = 64;
	PlayerOne.RotationSpeed = 0;
	PlayerOne.Counter = 0;
	iPointLoop=0;
	while (iPointLoop<PlayerOne.Points)
	{
		PlayerOne.PointList[iPointLoop].x = PlayerSprite[iPointLoop].x;
		PlayerOne.PointList[iPointLoop].y = PlayerSprite[iPointLoop].y;
		PlayerOne.PointList[iPointLoop].colour = PlayerSprite[iPointLoop].colour;
		iPointLoop++;
	}

	return PlayerOne;
}

/////////////////////////////////////////////////////
// CreateShot()
/////////////////////////////////////////////////////
SMALLVECTOROBJECT CreateShot(u16 iHorizontalOffset,VECTOROBJECT PlayerOne, bool bShotType)
{
	SMALLVECTOROBJECT vecShot;
	u8 iPointLoop;
	u8 iShotSide;

	vecShot.ObjectType=VEC_SHOT;
	vecShot.Scale = 1;

	// Alternate shots between left and right sides
	iShotSide=0;
	if(bShotType) iShotSide=8;

	// Copy the shot object from the template
	vecShot.Points = SHOT_POINTS;
	for(iPointLoop=0;iPointLoop<=vecShot.Points;iPointLoop++)
	{
		vecShot.PointList[iPointLoop].x = Shot[iPointLoop].x+iShotSide;
		vecShot.PointList[iPointLoop].y = Shot[iPointLoop].y;
		vecShot.PointList[iPointLoop].colour = Shot[iPointLoop].colour;
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
// Set up the palettes for us in the game
/////////////////////////////////////////////////////
void DefinePalettes()
{
	//Sprite palettes
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPRITE), RGB(0,0,0), RGB(15,15,15), RGB(11,11,11), RGB(5,5,5));
	SetPalette(SPRITE_PLANE, (u8)(PAL_INVADER), RGB(0,0,0), RGB(0,15,0), RGB(15,15,0), RGB(15,0,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_LEMMANOID), RGB(0,0,0), RGB(15, 11, 12), RGB(0,0,15), RGB(0,15,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_CITY), RGB(0,0,0), RGB(0, 0, 15), RGB(15,0,0), RGB(0,15,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_PICTSEL), RGB(0,0,0), RGB(9, 9, 9), RGB(15,0,0), RGB(0,0,15));
	SetPalette(SPRITE_PLANE, (u8)(PAL_ANGRYINVADER), RGB(0,0,0), RGB(15,0,0), RGB(15,15,0), RGB(0,0,15));
	SetPalette(SPRITE_PLANE, (u8)(PAL_UMBRELLA), RGB(0,0,0), RGB(15, 0, 0), RGB(15,15,15), RGB(10,5,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_BOMBER), RGB(0,0,0), RGB(0,15,0), RGB(15,15,0), RGB(15,0,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_MISSILE), RGB(0,0,0), RGB(15,15,15), RGB(15,0,0), RGB(0,0,15));
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPACIE), RGB(0,0,0), RGB(0,15,0), RGB(15,15,0), RGB(4,0,15));
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPACIE+1), RGB(0,0,0), RGB(15,15,0), RGB(15,15,15), RGB(0,0,15));
	SetPalette(SPRITE_PLANE, (u8)(PAL_SPACIE+2), RGB(0,0,0), RGB(15,0,0), RGB(0,15,0), RGB(0,0,15));
	SetPalette(SPRITE_PLANE, (u8)(PAL_FIREWORK), RGB(0,0,0), RGB(15, 15, 15), RGB(15,15,0), RGB(15,0,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_FIREWORK+1), RGB(0,0,0), RGB(7, 15, 3), RGB(15,7,0), RGB(0,0,15));
	SetPalette(SPRITE_PLANE, (u8)(PAL_FIREWORK+2), RGB(0,0,0), RGB(3, 15, 7), RGB(7,15,0), RGB(0,15,0));
	SetPalette(SPRITE_PLANE, (u8)(PAL_FIREWORK+3), RGB(0,0,0), RGB(15, 3, 7), RGB(0,7,15), RGB(15,15,15));

	// Scroll Plane 1 palettes (PAL_STATUS set in game loop)
	SetPalette(SCR_1_PLANE, PAL_SCORE, 0, RGB(15,15,15), RGB(8,8,8), RGB(4,4,4));
	SetPalette(SCR_1_PLANE, PAL_DEBUG, 0, RGB(15,0,0), RGB(8,8,8), RGB(4,4,4));
	SetPalette(SCR_1_PLANE, PAL_BORDER, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));
	SetPalette(SCR_1_PLANE, PAL_CITY_STATUS, RGB(0,0,0), RGB(0, 0, 15), RGB(15,0,0), RGB(0,15,0));

	// Scroll Plane 2 palette
	SetPalette(SCR_2_PLANE, PAL_SHIP, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));
	SetPalette(SCR_2_PLANE, PAL_BITMAP, 0, RGB(15,15,15), RGB(15,15,0), RGB(15,0,0));
	

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
	PutTile(SCR_1_PLANE,PAL_BORDER,0,0,borderTilebase);
	PutTile(SCR_1_PLANE,PAL_BORDER,19,0,borderTilebase+2);
	PutTile(SCR_1_PLANE,PAL_BORDER,0,15,borderTilebase+6);
	PutTile(SCR_1_PLANE,PAL_BORDER,19,15,borderTilebase+8);

	//Top/Bottom edges
	for(iLoopX=1;iLoopX<=18;iLoopX++){
		PutTile(SCR_1_PLANE,PAL_BORDER,iLoopX,0,borderTilebase+1);
		PutTile(SCR_1_PLANE,PAL_BORDER,iLoopX,15,borderTilebase+7);
	}
	//Left/Right edges
	for(iLoopY=1;iLoopY<15;iLoopY++){
		PutTile(SCR_1_PLANE,PAL_BORDER,0,iLoopY,borderTilebase+3);
		PutTile(SCR_1_PLANE,PAL_BORDER,19,iLoopY,borderTilebase+5);
	}

	//Other window dressing (debug info and scorecard/lives count etc)
	PrintString(SCR_1_PLANE, PAL_SCORE, 0, 16, "ENERGY:");
	PrintString(SCR_1_PLANE, PAL_SCORE, 0, 17, "STATUS:");
	// Use the unprintable char(10) through to char(30) for status bar updates
	// Replace with the Lemmanoid animation or city status in the game loop
	for(iLoopX=0;iLoopX<10;iLoopX++){
		SetPalette(SCR_1_PLANE,PAL_STATUS+iLoopX, RGB(0,0,0), RGB(15, 11, 12), RGB(0,0,15), RGB(0,15,0));
		PutTile(SCR_1_PLANE,PAL_STATUS+iLoopX,iLoopX+7,17, iLoopX+10); //Lemmanoids Status
		PutTile(SCR_1_PLANE,PAL_CITY_STATUS,iLoopX+7,18, iLoopX+20); //City Status
	}

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

	SetPalette(SCR_1_PLANE, PAL_LOGO, 0, RGB(15,0,0), RGB(15,15,0), RGB(15,15,15));
	SetPalette(SCR_1_PLANE, PAL_STARFIELD, 0, RGB(0,0,0), RGB(0,0,0), RGB(0,0,0));

	//Also, clear the sprites....
	for (iSpriteLoop=0;iSpriteLoop<64;iSpriteLoop++)
	{
		SetSprite(iSpriteLoop, 0, 0, 0, 0, 0);
	}

	while (JOYPAD & J_A);

	//Paint the logo.
	for (iLoopX=0;iLoopX<=19;iLoopX++)
	{
		iPalette=PAL_STARFIELD;
		for (iLoopY=0;iLoopY<=9;iLoopY++)
		{
			//Top 2 and bottom 2 lines of the logo are the star field
			if (iLoopY>=2) {iPalette=PAL_LOGO;}
			if (iLoopY>=8) {iPalette=PAL_STARFIELD;}
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
				SetPalette(SCR_1_PLANE, PAL_STARFIELD, 0, 0, 0, 0);
				break;
			case 1:
				SetPalette(SCR_1_PLANE, PAL_STARFIELD, 0, iStarColour, 0, 0);
				break;
			case 2:
				SetPalette(SCR_1_PLANE, PAL_STARFIELD, 0, 0, iStarColour, 0);
				break;
			case 3:
				SetPalette(SCR_1_PLANE, PAL_STARFIELD, 0, 0, 0, iStarColour);
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
// Change Energy Gauge
/////////////////////////////////////////////////////
u8 AddEnergy(u8 EnergyLevel, u8 UpperBound,s8 EnergyChange)
{
	u8 retval;
	retval=EnergyLevel;
	if(EnergyChange<0)
	{
		if((EnergyChange*-1)>EnergyLevel)
			retval=0;
		else
			retval+=EnergyChange;
	}
	else
	{
		if (EnergyLevel+EnergyChange>UpperBound)
			retval=UpperBound;
		else
			retval+=EnergyChange;
	}
	return retval;
}


/////////////////////////////////////////////////////
// Main game loop
/////////////////////////////////////////////////////
void DefenderoidsMain()
{
	// Define variables
	u8 iLoopX;
	u8 iLoopY;
	u16 iTile;
	u16 bmpPlayField[2032];
	u8 iLoopAsteroid;
	u8 iSpriteLoop;
	u8 iPictcellLoop;
	u8 iEngineLoop;
	u8 iCityLoop;
	u8 iLemmanoidLoop;
	u8 iNewVectorLoop;
	u8 iLoopShot;
	u8 iLoopExplosion;
	u8 iPointLoop;
	s16 iVelocityX;
	s16 iVelocityY;
	u16 iHeightMapLoop;
	u16 iHorizontalOffset;
	u8 iCurrentLevel;
	u8 iInvaderLoop;
	LEVEL lvCurrent;
	u8 iEngineNoise;
	bool bShotType;
	bool bShoot;
	u8 iEnergyGauge;
	u8 iEnergyLoop;
	u8 iGaugePalette;
	u8 iCityBlock;
	u8 iMinAge;
	u16 iCaptureDistance;
	u8 iCaptureHeight;
	u8 iVectorLoop;
	bool bProcessControls;
	bool bLevelComplete;
	bool bQixLevel;
	u8 iTransitionCounter;
	u8 iTransitionFrame;
	u8 iTransitionPalette;
	POINT ptPlayer;
	SPRITEPOINT ptExplosion;
	INVADERRANK rkSpacies;
	u8 freecol[7];

	/////////////////////////////////////////////////////////
	// Vector/sprite arrays
	/////////////////////////////////////////////////////////
	VECTOROBJECT vShip;
	SMALLVECTOROBJECT VectorList[MAX_VECTOR + 1];
	SPRITE SpriteList[MAX_SPRITE];

	// Set up the random number generator
	InitialiseQRandom();

	// Set up the palettes
	DefinePalettes();

	////////////////////////////////////////////////////////////
	//Create game screen
	////////////////////////////////////////////////////////////

	DrawGameScreen();

	//Create a bitmap...
	SetBackgroundColour(RGB(0,0,4));

	CreateBitmap((u16*)bmpPlayField, BITMAP_WIDTH, BITMAP_HEIGHT);
	CopyBitmap((u16*)bmpPlayField, bgTileBase);

	iTile=0;
	//Copy the bitmap to SCR_2_PLANE
	// Watch the order...
	for (iLoopY=0;iLoopY<14;iLoopY++)
	{
		for (iLoopX=0;iLoopX<18;iLoopX++)
		{
			// Switch palettes for the central zone Red/White stay the same, switch the third colour to blue (ship) and yellow (rest of screen)
			if (iLoopX>=8&&iLoopX<=10)
				PutTile(SCR_2_PLANE, PAL_SHIP, 1 + iLoopX, 1 + iLoopY, bgTileBase+iTile);
			else
				PutTile(SCR_2_PLANE, PAL_BITMAP, 1 + iLoopX, 1 + iLoopY, bgTileBase+iTile);
			iTile++;
		}
	}

	/////////////////////////////////////////////////////////
	// Main lives/game loop
	// Level zero is the "planet destroyed" Qix level and not normally
	// accessible in the main game loop
	/////////////////////////////////////////////////////////
	iCurrentLevel=1;
	
	VGM_PlayBGM_Loop((u8*)bgm1, bgm1_loop_point);

	iEnergyGauge=MAX_ENERGY;
	bQixLevel=false;

	// Overall game loop
	while (iEnergyGauge>0)
	{
		////////////////////////////////////////////////////////////
		// Level setup
		////////////////////////////////////////////////////////////

		// Clear down any existing vector objects
		for(iVectorLoop=0;iVectorLoop<MAX_VECTOR;iVectorLoop++)
		{
			VectorList[iVectorLoop].ObjectType=VEC_NONE;
		}
		// And any sprites

		if (bQixLevel)
		{
			lvCurrent=DefenderoidsLevels[0];
		}
		else
		{
			lvCurrent=DefenderoidsLevels[iCurrentLevel];
		}


		// City (need to create at end of sprite list so they end up at back of priority queue)
		// Need to use the city age (0-12) to determine each individual block frame
		SpriteList[0] = CreateSprite(8<<SPRITE_SCALE,92<<SPRITE_SCALE,60,sprCity,CITYBLOCK1,lvCurrent.CityStatus);
		SpriteList[1] = CreateSprite(16<<SPRITE_SCALE,92<<SPRITE_SCALE,61,sprCity,CITYBLOCK2,lvCurrent.CityStatus);
		SpriteList[2] = CreateSprite(24<<SPRITE_SCALE,92<<SPRITE_SCALE,62,sprCity,CITYBLOCK3,lvCurrent.CityStatus);
		SpriteList[3] = CreateSprite(32<<SPRITE_SCALE,92<<SPRITE_SCALE,63,sprCity,CITYBLOCK4,lvCurrent.CityStatus);

		// Set up the player
		vShip = CreatePlayer();

		// Setup the Invader Rank object
		rkSpacies.Direction=DIR_RIGHT;
		rkSpacies.Position.x=0;
		rkSpacies.Position.y=0;
		rkSpacies.InvaderCount[0]=0;
		rkSpacies.InvaderCount[1]=0;
		rkSpacies.InvaderCount[2]=0;
		rkSpacies.NextInvader=0;
		// Reset the spacies sprite link
		for(iInvaderLoop=0;iInvaderLoop<24;iInvaderLoop++)
		{
			rkSpacies.Invaders[iInvaderLoop]=rsNone;
		}

		// Invaders
		for (iSpriteLoop=4;iSpriteLoop<lvCurrent.InvaderCount+4;iSpriteLoop++)
		{
			//x, y, ID, Type, Direction, Frame
			//Invaders always move south on creation
			SpriteList[iSpriteLoop] = CreateInvader(((u16)QRandom())<<8,((u16)QRandom())<<4,iSpriteLoop);
			// Add spacies to the rank
			if (SpriteList[iSpriteLoop].SpriteType==sprSpacie)
			{
				rkSpacies.Invaders[rkSpacies.NextInvader++]=SpriteList[iSpriteLoop].SpriteID;
				SpriteList[iSpriteLoop].RelatedSpriteID=rsRank;
				while(rkSpacies.InvaderCount[SpriteList[iSpriteLoop].Direction]>=8)
				{
					//Row is full - try the next one...
					// Assumed that total invader count must always be less than 24 (three rows of 8)
					SpriteList[iSpriteLoop].Direction++;
					if (SpriteList[iSpriteLoop].Direction>2)
						SpriteList[iSpriteLoop].Direction=0;
				}
				// Might need different invader counts per row?
				SpriteList[iSpriteLoop].RankColumn=rkSpacies.InvaderCount[SpriteList[iSpriteLoop].Direction]++;
				SpriteList[iSpriteLoop].Position.x=rkSpacies.Position.x+((u16)(SpriteList[iSpriteLoop].RankColumn)<<4)<<SPRITE_SCALE;
				SpriteList[iSpriteLoop].Position.y=rkSpacies.Position.y+((SpriteList[iSpriteLoop].Direction<<4)<<SPRITE_SCALE);
			}
		}

		// Lemmanoids
		for (;iSpriteLoop<lvCurrent.InvaderCount+lvCurrent.LemmanoidCount+4;iSpriteLoop++)
		{
			//x, y, ID, Type, Direction, Frame
			//Lemmanoids move either west or east (DIR_EAST + [0|1]*DIR_WEST)
			//Spawn them vertically at the median point of the terrain
			SpriteList[iSpriteLoop] = CreateSprite(((u16)QRandom())<<8,100<<8,iSpriteLoop,sprLemmanoid,DIR_EAST + ((QRandom()>>7)*DIR_WEST),(QRandom()>>5));
		}

		// Clear any other sprites
		for (;iSpriteLoop<MAX_SPRITE-4;iSpriteLoop++)
		{
			SpriteList[iSpriteLoop] = CreateSprite(0,0,iSpriteLoop,sprMisc,0,0);
		}

		// Asteroids
		for (iVectorLoop=1;iVectorLoop<=lvCurrent.AsteroidCount;iVectorLoop++)
		{
			VectorList[iVectorLoop] = CreateAsteroid(((u16)QRandom())<<8,((u16)QRandom())<<5,4,1);
		}

		// Qix
		if(lvCurrent.QixCount>0)
		{
			VectorList[0] = CreateQix();
		}
		
		// Set the horizontal offset.
		iHorizontalOffset=255;

		iLoopX=0;
		iLoopY=0;
		iVelocityX=0;
		iVelocityY=0;
		bShoot=0;
		bShoot=(JOYPAD & J_A);
		bProcessControls=false;
		bLevelComplete=false;

		iTransitionCounter=0;
		iTransitionFrame=0;

		//Reset game palettes
		DefinePalettes();

		// Level loop
		while (!bLevelComplete)
		{

			// Translate the player position to a point (for use in collision detection)
			ptPlayer.x=PLAYER_X;
			ptPlayer.y=PLAYER_Y;

			//Reset the bitmap for every frame.
			CreateBitmap((u16*)bmpPlayField, BITMAP_WIDTH, BITMAP_HEIGHT);

			//Check player movement

			// Ship Sprite
			// Movement controls:
			// Left/Right = Rotate left/right
			// B button = Thrust
			// A butotn = Fire (must be released between shots)

			// Don't process controls during transition phases
			if (bProcessControls)
			{
				if (JOYPAD & J_LEFT) vShip.RotationAngle-=8;
				if (JOYPAD & J_RIGHT) vShip.RotationAngle+=8;
				if (JOYPAD & J_B)
				{

					// Movement vector should grow or shrink by +/- 127 in any given frame?
					if (vShip.MovementVector.x + Cos(vShip.RotationAngle+192) < 0)
					{
						iVelocityX = (u16)((vShip.MovementVector.x + Cos(vShip.RotationAngle+192)) * -1)>>8;
					}
					else
					{
						iVelocityX = (u16)((vShip.MovementVector.x + Cos(vShip.RotationAngle+192)))>>8;
					}

					if (vShip.MovementVector.y + Sin(vShip.RotationAngle+192) < 0)
					{
						iVelocityY = (u16)((vShip.MovementVector.y + Sin(vShip.RotationAngle+192)) * -1)>>8;
					}
					else
					{
						iVelocityY = (u16)((vShip.MovementVector.y + Sin(vShip.RotationAngle+192)))>>8;
					}

					// Need to constrain the Movement Vector to a maximum velocity.
					// We have separate max velocities for horizontal and vertical movement
					if (iVelocityX<3)
					{
						// Modify the movement vector by the angle.
						// Because "zero" degrees is at right angles to "up", we need to rotate this by 270 degrees
						// which on a 256 byte sine table is 192.
						vShip.MovementVector.x += (Cos(vShip.RotationAngle+192));
					}
					if (iVelocityY<2)
					{
						vShip.MovementVector.y += (Sin(vShip.RotationAngle+192));
					}
				}
				if (JOYPAD & J_A && bShoot)
				{
					// Fire a shot.
					// Find an "empty" shot in the array, and copy the template "shot" vector into it.
					// MovementVector will be based on tr whe current ship vector with a multiplier to give the velocity. Hopefully one that never out-runs the ship.
					// Set the shot flag so that we don't get continuous fire. Got to make the player work for it...
					// Find the first "empty" shot - scale will be set to zero if it's not valid.
					for (iVectorLoop=1;iVectorLoop<(MAX_VECTOR);iVectorLoop++)
					{
						if (VectorList[iVectorLoop].ObjectType == 0)
						{
							
							VGM_PlaySFX((u8*)zap,1);

							bShotType=!bShotType;
							VectorList[iVectorLoop]=CreateShot(iHorizontalOffset,vShip,bShotType);
							
							//Terminate the loop so that only one shot is created
							iVectorLoop = MAX_VECTOR;
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
			}
			else 
			{
				// Hands off mode engaged

				// (lvCurrent.Died==0)&&(iEnergyGauge!=0)&&(lvCurrent.Saved==0)
				// Level Start Transition
				// Display the objective at least
				if (lvCurrent.Died==0&&iEnergyGauge!=0&&lvCurrent.Saved==0&&!bQixLevel)
				{
					switch (iTransitionCounter)
					{
						case 0:
							PrintString(SCR_1_PLANE,PAL_DEBUG,5,2,"DEFEND THE");
							PrintString(SCR_1_PLANE,PAL_DEBUG,5,3,"LEMMANOIDS");
							iTransitionCounter++;
						case 1:
							if(iTransitionFrame++>=16)
							{
								iTransitionCounter++;
							}
							break;
						case 2:
							if (!bQixLevel)
							{
								PrintString(SCR_1_PLANE,PAL_SCORE,6,5,"LEVEL ");
								PrintDecimal(SCR_1_PLANE,PAL_SCORE,12,5,iCurrentLevel,1);
							}
							PrintString(SCR_1_PLANE,PAL_SCORE,1,6,lvCurrent.LevelName);
							if(iTransitionFrame++>=64)
							{
								iTransitionCounter++;
							}
							break;
						default:
							bProcessControls=true;
							iTransitionCounter=0;
							iTransitionFrame=0;
							PrintString(SCR_1_PLANE,PAL_SCORE,5,2,"          ");
							PrintString(SCR_1_PLANE,PAL_SCORE,5,3,"          ");
							PrintString(SCR_1_PLANE,PAL_SCORE,6,5,"          ");
							PrintString(SCR_1_PLANE,PAL_SCORE,1,6,"                  ");
							break;
					// When animation is complete, set the Level Complete flag...
					}
				}

				// lvCurrent.LemmanoidCount==lvCurrent.Died
				// No Lemmanoids saved - planet blows up!
				// Palette shift on the bitmap plane - colour 1 flash red/white for a couple of seconds and
				// the ground colour fading to black. Maybe a bunch of asteroids or something in the ground colour?
				if(bQixLevel&&iEnergyGauge!=0)
				{
					if(VectorList[0].Points<=3)
					{
						// Explosions. Lot's of them. Random position on the screen
						// Stop player movement
						// Spawn a bunch of umbrella lemmanoids
						iTransitionPalette=(iTransitionPalette++)>>7;
						switch (iTransitionCounter)
						{
							case 0:
								// Start by disabling any movement velocity
								vShip.MovementVector.x=0;
								vShip.MovementVector.y=0;
								iTransitionCounter++;
								iTransitionFrame=0;
								break;
							case 1:
								PrintString(SCR_1_PLANE,iTransitionPalette,4,7,"CONGRATURATION");
								PrintString(SCR_1_PLANE,iTransitionPalette,3,8,"YOU BIG SUCCESS!");
								//Create Explosion
								for (iLoopExplosion=1;iLoopExplosion<MAX_VECTOR;iLoopExplosion++)
								{
									if (VectorList[iLoopExplosion].ObjectType==VEC_NONE)
									{
										ptExplosion.x=PLAYER_X+QRandom()-QRandom();
										ptExplosion.y=PLAYER_Y+QRandom()-QRandom();
										VectorList[iLoopExplosion]=CreateExplosion(ptExplosion);

										// Need a better 'splode noise
										//VGM_PlaySFX((u8*)noise,1);

										iLoopExplosion = MAX_VECTOR;
									}
								} 
							
								//Create Lemmanoid/Umbrella at/near top of screen#
								iLemmanoidLoop=0;
								if(iTransitionFrame%4==0)
								{
									for(iSpriteLoop=0;iSpriteLoop<=MAX_SPRITE;iSpriteLoop++)
									{
										if (SpriteList[iSpriteLoop].SpriteType==sprMisc)
										{
											// Create a falling Lemmanoid
											if(iLemmanoidLoop==0)
											{
												SpriteList[iSpriteLoop]=CreateSprite(((u16)iHorizontalOffset<<SPRITE_SCALE)+((vShip.Position.x+QRandom()-QRandom())<<SPRITE_SCALE),(u16)(QRandom()>>3)<<SPRITE_SCALE,iSpriteLoop,(u8)sprLemmanoid,DIR_SOUTH,0);
												iLemmanoidLoop=iSpriteLoop;
											}
											else
											{
												SpriteList[iSpriteLoop]=CreateSprite((u16)0,(u16)0,iSpriteLoop,(u8)sprUmbrella,DIR_SOUTH,0);
												SetSpritePalette(SpriteList[iSpriteLoop].SpriteID,PAL_UMBRELLA);
												SpriteList[iSpriteLoop].RelatedSpriteID=iLemmanoidLoop;
												iSpriteLoop=MAX_SPRITE+1;
											}
										}
									}
								}
								if(iTransitionFrame++>64)
								{
									iTransitionCounter++;
								}
								break;
							case 2:
								PrintString(SCR_1_PLANE,PAL_SCORE,4,7,"              ");
								PrintString(SCR_1_PLANE,PAL_SCORE,3,8,"                ");
								//Make sure the Qix object doesn't hang around to the next level
								VectorList[0].ObjectType=VEC_NONE;
								//Set level complete and turn off the QixLevel flag so that the normal level loop reinstates
								bLevelComplete=true;
								break;
						// When animation is complete, set the Level Complete flag...
						}
					}
					else
					{
						switch (iTransitionCounter)
						{
							case 0:
								PrintString(SCR_1_PLANE,PAL_DEBUG,5,2,"RESCUE THE");
								PrintString(SCR_1_PLANE,PAL_DEBUG,5,3,"LEMMANOIDS");
								iTransitionCounter++;
							case 1:
								if(iTransitionFrame++>=16)
								{
									iTransitionCounter++;
								}
								break;
							case 2:
								PrintString(SCR_1_PLANE,PAL_SCORE,1,6,lvCurrent.LevelName);
								if(iTransitionFrame++>=64)
								{
									iTransitionCounter++;
								}
								break;
							default:
								bProcessControls=true;
								iTransitionCounter=0;
								iTransitionFrame=0;
								PrintString(SCR_1_PLANE,PAL_SCORE,5,2,"          ");
								PrintString(SCR_1_PLANE,PAL_SCORE,5,3,"          ");
								PrintString(SCR_1_PLANE,PAL_SCORE,6,5,"          ");
								PrintString(SCR_1_PLANE,PAL_SCORE,1,6,"                  ");
								break;
								// When animation is complete, set the Level Complete flag...
						}
					}
				}
				else
				{
					/////////////////////////////////////////////////////
					// All Lemmanoids destroyed, kill the planet
					/////////////////////////////////////////////////////
					if (lvCurrent.LemmanoidCount==lvCurrent.Died)
					{

						iTransitionPalette=PAL_SCORE;
						if (iTransitionFrame++%2==0) iTransitionPalette=PAL_DEBUG;
						PrintString(SCR_1_PLANE,iTransitionPalette,7,3,"MISSION");
						PrintString(SCR_1_PLANE,iTransitionPalette,7,4,"FAILED!");
						// Add a screen shake by moving SCR_2_PLANE around a bit
						if (iTransitionFrame%4==0)
							SCR2_X++;
						else if (iTransitionFrame%3==0)
							SCR2_X--;
						else if (iTransitionFrame%2==0)
							SCR2_Y++;
						else
							SCR2_Y--;
						//SCR1_Y+=((s8)(128-QRandom())>>6);
						if (iTransitionPalette==PAL_DEBUG)
						{
							SetPalette(SCR_2_PLANE, PAL_BITMAP, 0, RGB(15,0,0), RGB(15,0,0), RGB(15,0,0));
						}
						else
						{
							SetPalette(SCR_2_PLANE,PAL_BITMAP,RGB(15,0,0),RGB(15,15,15),RGB(15,15,15),RGB(15,15,15));
						}
						switch (iTransitionCounter)
						{
							case 0:
								if(iTransitionFrame>=4)
								{
									iTransitionCounter++;
								}
								break;
							case 1:
								if(iTransitionFrame%4==0)
								{
									for (iNewVectorLoop=1;iNewVectorLoop<MAX_VECTOR;iNewVectorLoop++)
									{
										if (VectorList[iNewVectorLoop].ObjectType==VEC_NONE)
										{
											// Limit asteroid creation to current horizontal offset+8 to offset+136
											iHeightMapLoop=QRandom()>>1;
											iHeightMapLoop+=iHorizontalOffset+8;
											VectorList[iNewVectorLoop]=CreateAsteroid((iHeightMapLoop)<<7,((u16)HeightMap[iHeightMapLoop])<<7,3,3);
											iNewVectorLoop=MAX_VECTOR;
										}
											
									}
								}
								if (iTransitionFrame==64) iTransitionCounter++;
								break;
							default:
								bLevelComplete=true;
								SCR2_X=0;
								SCR2_Y=0;
								PrintString(SCR_1_PLANE,iTransitionPalette,7,3,"       ");
								PrintString(SCR_1_PLANE,iTransitionPalette,7,4,"       ");
								break;
								// When animation is complete, set the Level Complete flag...
						}
					}
					// lvCurrent.Saved>0 - warp to new planet
					// Autopilot the ship back to the city and then warp vertically up the screen
					// Fire off a bunch of firework sprites as we go (can clear down any pictcell or invader sprites that are still lurking)
					// Use iTransitionCounter
					// 0 - Display "Auto-pilot engaged"
					// 1 - Navigate towards home (firework type 1 every few frames)
					// 2 - Rotate to north
					// 3 - Kick off a bunch of firework type 2
					// 4 - Thrust north off the screen
					// Could do with going a bit quicker
					if (lvCurrent.Saved>0&&lvCurrent.LemmanoidCount==lvCurrent.Died+lvCurrent.Saved)
					{
						// Display autopilot message
						iTransitionPalette=PAL_SCORE;
						if (iTransitionFrame++%2==0) iTransitionPalette=PAL_DEBUG;
						PrintString(SCR_1_PLANE,iTransitionPalette,6,3,"AUTOPILOT");
						PrintString(SCR_1_PLANE,iTransitionPalette,7,4,"ENGAGED");
						// Create Firework sprite
						for(iSpriteLoop=0;iSpriteLoop<=MAX_SPRITE;iSpriteLoop++)
						{
							//Search for the first "empty" sprite
							if (SpriteList[iSpriteLoop].SpriteType==sprMisc)
							{
								// Create one of two firework types
								if(iTransitionFrame%2==0)
								{
									SpriteList[iSpriteLoop]=CreateSprite(((u16)iHorizontalOffset<<SPRITE_SCALE)+((vShip.Position.x+QRandom()-QRandom())<<SPRITE_SCALE),(u16)(100)<<SPRITE_SCALE,iSpriteLoop,sprFirework,DIR_NORTH,0);
								}
								else
								{
									SpriteList[iSpriteLoop]=CreateSprite(((u16)iHorizontalOffset<<SPRITE_SCALE)+((vShip.Position.x+QRandom()-QRandom())<<SPRITE_SCALE),(u16)(100)<<SPRITE_SCALE,iSpriteLoop,(u8)sprFirework+4,DIR_NORTH,0);
								}

								iSpriteLoop=MAX_SPRITE+1;
							}
						}
						switch (iTransitionCounter)
						{
							case 0:
								// Start by disabling any movement velocity
								vShip.MovementVector.x=0;
								vShip.MovementVector.y=0;
								iTransitionCounter++;
								iTransitionFrame=0;
								break;
							case 1:
								//Then shuffle asteroids up to fill the gap and reduce lvCurrent.AsteroidCount
								// Rotate the ship towards home (always faces right for now)
								if (iHorizontalOffset>456||iHorizontalOffset<200)
								{
									if (vShip.RotationAngle>192|vShip.RotationAngle<=64)
										vShip.RotationAngle-=4;
									else
										vShip.RotationAngle+=4;
									if (vShip.RotationAngle>188&&vShip.RotationAngle<196)
									{
										//Rotate the ship to Horizontal (angle=195)
										vShip.RotationAngle=192;
										iTransitionCounter++;
									}
								}
								else
								{
									if (vShip.RotationAngle>192|vShip.RotationAngle<=64)
										vShip.RotationAngle+=4;
									else
										vShip.RotationAngle-=4;
									if (vShip.RotationAngle>60&&vShip.RotationAngle<68)
									{
										//Rotate the ship to Horizontal (angle=64)
										vShip.RotationAngle=64;
										iTransitionCounter++;
									}
								}
								break;
							case 2:
								// Apply thrust and kick off a firework every 8 frames or so
								// Until ship is directly over the city
								// Adjust vertical thrust as well to drag it into the centre of the screen
								if (vShip.RotationAngle==64)
									vShip.MovementVector.x=512;
								else
									vShip.MovementVector.x=-512;
								if (iHorizontalOffset>=453&&iHorizontalOffset<=459)
								{
									iHorizontalOffset=456;
									iTransitionCounter++;
									vShip.MovementVector.x=0;
								}
								break;
							case 3:
								// Rotate to vertical (angle=0)
								if (vShip.RotationAngle>=192)
									vShip.RotationAngle+=4;
								else
									vShip.RotationAngle-=4;

								if (vShip.RotationAngle<4||vShip.RotationAngle>252)
								{
									vShip.RotationAngle=0;
									iTransitionCounter++;
								}
								break;
							case 4:
								// Apply thrust until ship is off the screen
								vShip.MovementVector.y=-256;
								// Bounds checking will bounce the ship at y=4, so we need to finish before then...
								if (vShip.Position.y<=6)
								{
									iTransitionCounter++;
								}
								break;
							default:
								PrintString(SCR_1_PLANE,PAL_DEBUG,6,3,"         ");
								PrintString(SCR_1_PLANE,PAL_DEBUG,7,4,"       ");
								bLevelComplete=true;
								break;
						}
					}
				}
				// Theoretically possible to run out of Pictcell's and therefore to not be able to completely build
				// the city and save any Lemmanoids? Do I need to trap this as an end-of-level condition?
				// I might spawn bonus asteroids in this situation (if(AsteroidCount==0))
				// Spawn a Qix if 
				
				// Energy Gauge==0
				// Player ship explodees
				// Pallete shift the white elements of the bitmap to red/white and fade to black
				// Add a bunch of particle explosions
				// Will then fall out of the loop to the Game Over screen
				if (iEnergyGauge==0)
				{
					iTransitionPalette=PAL_SCORE;
					if (iTransitionFrame++%2==0) iTransitionPalette=PAL_DEBUG;
					PrintString(SCR_1_PLANE,iTransitionPalette,7,3,"MISSION");
					PrintString(SCR_1_PLANE,iTransitionPalette,7,4,"FAILED!");
					PrintString(SCR_1_PLANE,iTransitionPalette,4,8,"LEMMANKIND IS");
					PrintString(SCR_1_PLANE,iTransitionPalette,7,9,"DOOMED");
					// Add a screen shake by moving SCR_2_PLANE around a bit
					if (iTransitionFrame%4==0)
						SCR2_X++;
					else if (iTransitionFrame%3==0)
						SCR2_X--;
					else if (iTransitionFrame%2==0)
						SCR2_Y++;
					else
						SCR2_Y--;
					//SCR1_Y+=((s8)(128-QRandom())>>6);
					if (iTransitionPalette==PAL_DEBUG)
					{
						SetPalette(SCR_2_PLANE, PAL_BITMAP, 0, RGB(15,0,0), RGB(15,0,0), RGB(15,0,0));
					}
					else
					{
						SetPalette(SCR_2_PLANE,PAL_BITMAP,RGB(15,0,0),RGB(15,15,15),RGB(15,15,15),RGB(15,15,15));
					}
					switch (iTransitionCounter)
					{
						case 0:
							if(iTransitionFrame>=4)
							{
								iTransitionCounter++;
							}
							break;
						case 1:
							for (iNewVectorLoop=1;iNewVectorLoop<MAX_VECTOR;iNewVectorLoop++)
							{
								if (VectorList[iNewVectorLoop].ObjectType==VEC_NONE)
								{
									ptExplosion.x=PLAYER_X + QRandom()-QRandom();
									ptExplosion.y=PLAYER_Y + QRandom()-QRandom();
									VectorList[iNewVectorLoop]=CreateExplosion(ptExplosion);
									iNewVectorLoop=MAX_VECTOR;
								}
									
							}
							if (iTransitionFrame==64) iTransitionCounter++;
							break;
						default:
							bLevelComplete=true;
							SCR2_X=0;
							SCR2_Y=0;
							PrintString(SCR_1_PLANE,iTransitionPalette,7,3,"       ");
							PrintString(SCR_1_PLANE,iTransitionPalette,7,4,"       ");
							PrintString(SCR_1_PLANE,iTransitionPalette,4,8,"             ");
							PrintString(SCR_1_PLANE,iTransitionPalette,7,9,"      ");
							break;
							// When animation is complete, set the Level Complete flag...
					}
				}

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
			// Player
			//////////////////////////////////////////////////////

			//Update the horizontal offset (ship remains centred horizontally)
			iHorizontalOffset += vShip.MovementVector.x>>7;
			//Bounds check - needs to wrap at sizeof(HeightMap)-1
			if(iHorizontalOffset>sizeof(HeightMap)-1){
				if(vShip.MovementVector.x<0){
					iHorizontalOffset=sizeof(HeightMap)-(65535-iHorizontalOffset);
				} else {
					iHorizontalOffset=0;
				}
			}

			//Keep player inside the playfield...
			//Reduce vertical velocity to zero as they approach either top/bottom edge- need to decide whether to apply a bounce or just stop at the top/bottom of the screen
			if (vShip.Position.y<4 && vShip.MovementVector.y<127){
				vShip.MovementVector.y = 127;
				vShip.Position.y=4;
				//vShip.Position.y=((u16*)bmpPlayField)[1];
			} else if (vShip.Position.y>bmpPlayField[1]-4 && vShip.MovementVector.y>127){
				vShip.MovementVector.y = 0;
				vShip.Position.y=bmpPlayField[1]-4;
			}

			// Apply vertical velocity
			vShip.Position.y += vShip.MovementVector.y>>7;

			// Add some engine noise if the player is thrusting
			for(iEngineLoop=vShip.Points-4;iEngineLoop<=vShip.Points;iEngineLoop++)
			{
				iEngineNoise=0;
				if (JOYPAD & J_B) iEngineNoise=(((QRandom()>>7))*3);

				vShip.PointList[iEngineLoop].colour = iEngineNoise;
			}

			DrawVectorSpriteAbsolute((u16*)bmpPlayField, vShip);

			//////////////////////////////////////////////////////
			// Vector objects
			//////////////////////////////////////////////////////
			for (iVectorLoop=0;iVectorLoop<=MAX_VECTOR;iVectorLoop++)
			{
				switch (VectorList[iVectorLoop].ObjectType)
				{
					
					case VEC_SHOT:
						// Shots will exist for 6 frames (based on RotationSpeed) or if they leave the playfield vertically, whichever is true first
						VectorList[iVectorLoop].RotationSpeed++;
						if (VectorList[iVectorLoop].RotationSpeed == 6 |VectorList[iVectorLoop].Position.y>>7 < 0 | VectorList[iVectorLoop].Position.y>>7 > bmpPlayField[1])
						{
							VectorList[iVectorLoop].ObjectType = VEC_NONE;
						}
						else
						{	
							/*
								Okay, so collision detection part deux

								Check the X Position against other visible objects, no need to go further if this differs by 
								a significant amount

								In fact, is a simple x/y check enough?
							*/
							// Check against all sprite objects
							for (iSpriteLoop=0;iSpriteLoop<MAX_SPRITE;iSpriteLoop++)
							{
								// Only Invaders and Mutated Lemmanoids can be shot
								if (SpriteList[iSpriteLoop].SpriteType == sprCatcher||SpriteList[iSpriteLoop].SpriteType == sprMissile||SpriteList[iSpriteLoop].SpriteType == sprSpacie||(SpriteList[iSpriteLoop].SpriteType == sprLemmanoid&&SpriteList[iSpriteLoop].Direction == DIR_MUTANOID))
								{
									if (CheckVectorSpriteCollision(VectorList[iVectorLoop].Position,SpriteList[iSpriteLoop].Position,COLLISION_SHOT))
									{	
										// Add an explosion
										for (iLoopExplosion=1;iLoopExplosion<MAX_VECTOR;iLoopExplosion++)
										{
											if (VectorList[iLoopExplosion].ObjectType==VEC_NONE)
											{
												VectorList[iLoopExplosion]=CreateExplosion(SpriteList[iSpriteLoop].Position);

												// Need a better 'splode noise
												//VGM_PlaySFX((u8*)noise,1);

												iLoopExplosion = MAX_VECTOR;
											}
										} 
										// Check affected sprite for other sprites to handle (again, should only apply to sprCatcher)
										switch(SpriteList[iSpriteLoop].RelatedSpriteID)
										{
											case rsNone:
												SpriteList[iSpriteLoop].SpriteType = sprMisc;
												break;
											case rsRank:
												SpriteList[iSpriteLoop].SpriteType = sprMisc;
												SpriteList[iSpriteLoop].RelatedSpriteID = rsNone;
												break;
											default:
												// Invader has a captured Lemmanoid, give 'em an umbrella and let them go
												// No need to check for RelatedSpriteID.SpriteType as it can only be sprLemmanoid
												// We can recycle the invader sprite ID for the umbrella and
												// keep the RelatedSpriteID connections active
												SpriteList[iSpriteLoop].SpriteType=sprUmbrella;
												SetSpritePalette(SpriteList[iSpriteLoop].SpriteID,PAL_UMBRELLA);

												// Set the Lemmanoid movement direction
												SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Direction=DIR_SOUTH;
												break;
										}
										lvCurrent.InvaderCount--;
										// Destroy the shot object
										VectorList[iVectorLoop].ObjectType=VEC_NONE;
										// Stop checking for collisions
										iSpriteLoop = MAX_SPRITE + 1;
									}
								}

							}

							// Check for Asteroid collision as well
							// On collision: destroy asteroid and create some smaller ones...
							// When smallest asteroid is destroyed, create a Pictcell sprite
							// Check that shot hasn't already been deactivated
							if (VectorList[iVectorLoop].ObjectType == VEC_SHOT)
							{
								for (iLoopAsteroid=0;iLoopAsteroid<MAX_VECTOR;iLoopAsteroid++)
								{
									if (VectorList[iLoopAsteroid].ObjectType==VEC_ASTEROID)
									{
										if (CheckVectorCollision(VectorList[iVectorLoop].Position,VectorList[iLoopAsteroid].Position,COLLISION_ASTEROID))
										{
											// Use the Asteroid Scale object to determine what happens next
											switch (VectorList[iLoopAsteroid].Scale)
											{
												case 0:
													break;
												case 1:
													//Smallest size asteroid destroyed
													//First create a Pictcell
													for(iSpriteLoop=0;iSpriteLoop<=MAX_SPRITE;iSpriteLoop++)
													{
														//Search for the first "empty" sprite
														if (SpriteList[iSpriteLoop].SpriteType==sprMisc)
														{
															SpriteList[iSpriteLoop]=CreateSprite(VectorList[iLoopAsteroid].Position.x,VectorList[iLoopAsteroid].Position.y,iSpriteLoop,sprPictcell,DIR_SOUTH,0);
															iSpriteLoop=MAX_SPRITE+1;
														}
													}
													//Then shuffle asteroids up to fill the gap and reduce lvCurrent.AsteroidCount
													VectorList[iLoopAsteroid].ObjectType=VEC_NONE;
													break;
												default:
													//Split asteroid into smaller chunks
													//First reduce the size of the current asteroid
													VectorList[iLoopAsteroid]=CreateAsteroid(VectorList[iLoopAsteroid].Position.x,VectorList[iLoopAsteroid].Position.y,VectorList[iLoopAsteroid].Scale-1,1);
													//Then spawn new ones (up to the MAX_ASTEROID limit)
													// Find new asteroid id
													for (iNewVectorLoop=1;iNewVectorLoop<MAX_VECTOR;iNewVectorLoop++)
													{
														if (VectorList[iNewVectorLoop].ObjectType==VEC_NONE)
														{
															VectorList[iNewVectorLoop]=CreateAsteroid(VectorList[iLoopAsteroid].Position.x,VectorList[iLoopAsteroid].Position.y,VectorList[iLoopAsteroid].Scale,1);
															iNewVectorLoop=MAX_VECTOR;
														}
															
													}
													break;
											}
											iLoopAsteroid=MAX_VECTOR;
											VectorList[iVectorLoop].ObjectType=VEC_NONE;

										}
									}
								}
							}

							// Lastly, if the shot still hasn't been destroyed, then move it
							if (VectorList[iVectorLoop].ObjectType == VEC_SHOT)
							{
								VectorList[iVectorLoop].Position.x += VectorList[iVectorLoop].MovementVector.x;
								VectorList[iVectorLoop].Position.y += VectorList[iVectorLoop].MovementVector.y;
							}
						}
						break;
					case VEC_EXPLOSION:
						//////////////////////////////////////////////////////
						// Explosions
						//////////////////////////////////////////////////////
						//Increase the scale for later frames

						//VectorList[iVectorLoop].Scale+=(VectorList[iVectorLoop].RotationSpeed>>2);
						for (iPointLoop=0;iPointLoop<VectorList[iVectorLoop].Points;iPointLoop++)
						{
							// Why not just a few QRandom() calls within the rotation speed area?
							VectorList[iVectorLoop].PointList[iPointLoop].x += QRandom()>>4 - QRandom()>>4;
							VectorList[iVectorLoop].PointList[iPointLoop].y += QRandom()>>4 - QRandom()>>4;
						}

						// Destroy the Explosion after a few frames
						VectorList[iVectorLoop].RotationSpeed--;
						if (VectorList[iVectorLoop].RotationSpeed == 0)
						{
							VectorList[iVectorLoop].ObjectType = VEC_NONE;
						}
						break;
					case VEC_ASTEROID:
						//////////////////////////////////////////////////////
						// Asteroids
						//////////////////////////////////////////////////////

						// Check Player Collision (bounce the player away)
						if (CheckVectorCollision(VectorList[iVectorLoop].Position,ptPlayer,COLLISION_ASTEROID))
						{
							//Only bounce if the asteroid vector and ship vector are going in the same direction
							//If either isn't moving, then leave it alone
							// Can get stuck if more than one asteroid is in a small area...
							if (vShip.Counter==0)
							{
								vShip.Counter=8;
								if ((vShip.MovementVector.x<0&&VectorList[iVectorLoop].MovementVector.x<0)||(vShip.MovementVector.x>0&&VectorList[iVectorLoop].MovementVector.x>0))
									vShip.MovementVector.x=vShip.MovementVector.x*-1;
								if ((vShip.MovementVector.y<0&&VectorList[iVectorLoop].MovementVector.y<0)||(vShip.MovementVector.y>0&&VectorList[iVectorLoop].MovementVector.y>0))
									vShip.MovementVector.y=vShip.MovementVector.y*-1;
								if(vShip.RotationAngle>64&&vShip.RotationAngle<196)
									vShip.RotationAngle=vShip.RotationAngle+128;
								else
									vShip.RotationAngle=vShip.RotationAngle-128;
							}
						}

						// Increase rotation angle
						VectorList[iVectorLoop].RotationAngle+=VectorList[iVectorLoop].RotationSpeed;
						// Need to do some bounds checking here...
						if (VectorList[iVectorLoop].MovementVector.y>VectorList[iVectorLoop].Position.y || (VectorList[iVectorLoop].Position.y + VectorList[iVectorLoop].MovementVector.y)>>7 > bmpPlayField[1]){
							VectorList[iVectorLoop].MovementVector.y = VectorList[iVectorLoop].MovementVector.y*-1;
						}
						VectorList[iVectorLoop].Position.x += VectorList[iVectorLoop].MovementVector.x;
						VectorList[iVectorLoop].Position.y += VectorList[iVectorLoop].MovementVector.y;
						break;
					case VEC_QIX:
						//Shuffle the points up the list
						for(iPointLoop=0;iPointLoop<VectorList[iVectorLoop].Points-1;iPointLoop++)
						{
							VectorList[iVectorLoop].PointList[iPointLoop] = VectorList[iVectorLoop].PointList[iPointLoop+1];
							// Adjust by the movement vector - looks a bit weird with the whole Qix object moving
							// All previous lines should stay still, and only the new line should go in a new direction?
							VectorList[iVectorLoop].PointList[iPointLoop].x += (VectorList[iVectorLoop].MovementVector.x*-1)>>SPRITE_SCALE;
							VectorList[iVectorLoop].PointList[iPointLoop].y += (VectorList[iVectorLoop].MovementVector.y*-1)>>SPRITE_SCALE;
						}
						// Randomise the last point on the list
						VectorList[iVectorLoop].PointList[VectorList[iVectorLoop].Points-1].x = (s16)(QRandom()>>3)-(s16)(QRandom()>>3);
						VectorList[iVectorLoop].PointList[VectorList[iVectorLoop].Points-1].y = (s16)(QRandom()>>3)-(s16)(QRandom()>>3);
						// Qix needs to move around as well...
						// Add a modifier based on player position, or on Qix movement vector?
						// Bounce the Qix off the walls - only switch directions if it's heading off the screen
						if (VectorList[iVectorLoop].Position.y>>SPRITE_SCALE<16 && VectorList[iVectorLoop].MovementVector.y<1)
						{
							VectorList[iVectorLoop].MovementVector.y = VectorList[iVectorLoop].MovementVector.y*-1;
						}
						
						if (VectorList[iVectorLoop].Position.y>>SPRITE_SCALE > bmpPlayField[1]-16 && VectorList[iVectorLoop].MovementVector.y>1)
						{
							VectorList[iVectorLoop].MovementVector.y = VectorList[iVectorLoop].MovementVector.y*-1;
						}
						VectorList[iVectorLoop].Position.x += VectorList[iVectorLoop].MovementVector.x;
						VectorList[iVectorLoop].Position.y += VectorList[iVectorLoop].MovementVector.y;
						// And every so often switch up the movement vector
						if (QRandom()>248)
						{
							switch(QRandom()>>6)
							{
								case 0:
									VectorList[iVectorLoop].MovementVector.x=-256;
									break;
								case 1:
									VectorList[iVectorLoop].MovementVector.x=-128;
									break;
								case 2:
									VectorList[iVectorLoop].MovementVector.x=-128;
									break;
								case 3:
									VectorList[iVectorLoop].MovementVector.x=256;
									break;
							}
							switch(QRandom()>>7)
							{
								case 0:
									VectorList[iVectorLoop].MovementVector.y=-128;
									break;
								case 1:
									VectorList[iVectorLoop].MovementVector.x=128;
									break;
							}
						}
						//Stop checking if we're in a game/level over loop
						if(bProcessControls)
						{
							// Check for ship collision
							PrintString(SCR_1_PLANE,PAL_SCORE,0,17,"QIXERGY:");
							PrintDecimal(SCR_1_PLANE,PAL_SCORE,9,17,VectorList[iVectorLoop].Counter,3);
							PrintDecimal(SCR_1_PLANE,PAL_SCORE,14,17,VectorList[iVectorLoop].Points,2);
							if (CheckVectorCollision(VectorList[iVectorLoop].Position,ptPlayer,COLLISION_QIX))
							{
								for (iLoopExplosion=1;iLoopExplosion<MAX_VECTOR;iLoopExplosion++)
								{
									if (VectorList[iLoopExplosion].ObjectType==VEC_NONE)
									{
										ptExplosion.x = ptPlayer.x;
										ptExplosion.y = ptPlayer.y;
										VectorList[iLoopExplosion]=CreateExplosion(ptExplosion);
										iLoopExplosion = MAX_VECTOR;
									}
								} 
								iEnergyGauge=AddEnergy(iEnergyGauge,MAX_ENERGY,-4);
								//Reduce the Qix energy (every 16 points will kill one leg)
								//The Qix itself will explode when there is only one leg left
								VectorList[iVectorLoop].Counter--;
								if (VectorList[iVectorLoop].Counter%16==0)
								{
									// Create a few small asteroids (four?) and then reduce the size of the Qix by one
									for (iNewVectorLoop=1;iNewVectorLoop<MAX_VECTOR;iNewVectorLoop++)
									{
										if (VectorList[iNewVectorLoop].ObjectType==VEC_NONE)
										{
											VectorList[iNewVectorLoop]=CreateAsteroid(VectorList[iVectorLoop].Position.x,VectorList[iVectorLoop].Position.y,2,1);
											iNewVectorLoop=MAX_VECTOR;
										}
									}
									// Each asteroid can be killed for 1 pictcell, 1 pictcell will restore 25% of your health.
									VectorList[iVectorLoop].Points--;
								}
							}
						}
						break;
				}
					
				switch (VectorList[iVectorLoop].ObjectType)
				{
					case VEC_ASTEROID:
					case VEC_SHOT:
					case VEC_QIX:
						DrawVectorObject((u16*)bmpPlayField, VectorList[iVectorLoop], iHorizontalOffset);
						break;
					case VEC_EXPLOSION:
						DrawSmallVectorSprite((u16*)bmpPlayField, VectorList[iVectorLoop], iHorizontalOffset);
						//DrawVectorObject((u16*)bmpPlayField, VectorList[iVectorLoop], iHorizontalOffset);
						break;
				}

			}

			//////////////////////////////////////////////////////
			// Copy the bitmap object into tile memory
			//////////////////////////////////////////////////////
			CopyBitmap((u16*)bmpPlayField, bgTileBase);

			//////////////////////////////////////////////////////
			// Display Sprite objects
			//////////////////////////////////////////////////////
			for (iSpriteLoop=0;iSpriteLoop<MAX_SPRITE;iSpriteLoop++)
			{
				// Check for player collision for all enemy sprite types
				if (SpriteList[iSpriteLoop].SpriteType==sprCatcher||SpriteList[iSpriteLoop].SpriteType==sprMissile||SpriteList[iSpriteLoop].SpriteType==sprSpacie)
				{
					if (bProcessControls&&CheckVectorSpriteCollision(ptPlayer,SpriteList[iSpriteLoop].Position,COLLISION_PLAYER))
					{

						for (iLoopExplosion=1;iLoopExplosion<MAX_VECTOR;iLoopExplosion++)
						{
							if (VectorList[iLoopExplosion].ObjectType==VEC_NONE)
							{
								VectorList[iLoopExplosion]=CreateExplosion(SpriteList[iSpriteLoop].Position);

								// Need a better 'splode noise
								//VGM_PlaySFX((u8*)noise,1);

								iLoopExplosion = MAX_VECTOR;
							}
						} 
						// Check affected sprite for other sprites to handle (should only matter for sprCatcher)
						if (SpriteList[iSpriteLoop].RelatedSpriteID!=rsNone)
						{
							// Drop the Lemmanoid - do we need to check for sprite type?
							// Invader has a captured Lemmanoid, give 'em an umbrella and let them go
							// No need for another loop - we can recycle the invader sprite ID for the umbrella?
							// Keep the RelatedSpriteID connections active
							SpriteList[iSpriteLoop].SpriteType=sprUmbrella;
							SetSpritePalette(SpriteList[iSpriteLoop].SpriteID,PAL_UMBRELLA);

							// Set the Lemmanoid movement direction
							SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Direction=DIR_SOUTH;

						}
						else
						{
							SpriteList[iSpriteLoop].SpriteType = sprMisc;
						}
						lvCurrent.InvaderCount--;
						iEnergyGauge=AddEnergy(iEnergyGauge,MAX_ENERGY,-2);
					}
				}
				switch(SpriteList[iSpriteLoop].SpriteType)
				{
					case sprCatcher:
						// Invaders bob up and down
						//PrintDecimal(SCR_2_PLANE,0,3,2+iSpriteLoop-4,SpriteList[iSpriteLoop].Position.x,5);
						// Need to home in on a Lemmanoid if there's one nearby
						// So, if Y-position is <$foo from the heightmap, and a Lemmanoid is within $bar horizontal units, start adjusting the trajectory towards the Lemmanoid
						// On capture. Change the Lemmanoid direction to North and switch the related flag to the capturing alien
						// Aliens with cargo will move half as slowly as normal, and maybe wobble a bit on the way up or something
						switch(SpriteList[iSpriteLoop].Direction)
						{
							case DIR_SOUTH:
								//PrintString(SCR_2_PLANE,0,9,2+iSpriteLoop-4,"S           ");
								// Check for any nearby Lemmanoids
								// Don't check if already carrying a hostage
								iCaptureHeight=((u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-4))-(SpriteList[iSpriteLoop].Position.y>>SPRITE_SCALE);
								if (iCaptureHeight<32&&SpriteList[iSpriteLoop].RelatedSpriteID==rsNone&&bProcessControls)
								{
									for(iLemmanoidLoop=0;iLemmanoidLoop<MAX_SPRITE;iLemmanoidLoop++)
									{
										// Leave any Lemmanoid alone if it is already captured (or, as it currently stands, is holding anything - might put that into the plot "The mystical power of the Pictcells can temporarily protect the Lemmanoids")
										if(SpriteList[iLemmanoidLoop].SpriteType==sprLemmanoid&&SpriteList[iLemmanoidLoop].RelatedSpriteID==rsNone&&SpriteList[iLemmanoidLoop].Direction<=DIR_WEST)
										{
											//PrintDecimal(SCR_2_PLANE,0,10,3+iSpriteLoop-4,SpriteList[iLemmanoidLoop].Position.x,5);
											//PrintDecimal(SCR_2_PLANE,0,10,2+iSpriteLoop-4,WrapDistance(SpriteList[iSpriteLoop].Position.x,SpriteList[iLemmanoidLoop].Position.x,65535),5);
											iCaptureDistance=WrapDistance(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE,SpriteList[iLemmanoidLoop].Position.x>>SPRITE_SCALE,SPRITE_MAX_WIDTH);
											
											if (iCaptureDistance<8&&iCaptureHeight<8)
											{
													//Gotcha
													//PrintString(SCR_2_PLANE,0,11,2+iSpriteLoop-4,"C");
													//PrintDecimal(SCR_2_PLANE,0,12,2+iSpriteLoop-4,iLemmanoidLoop,2);
													SpriteList[iSpriteLoop].RelatedSpriteID=SpriteList[iLemmanoidLoop].SpriteID;
													SpriteList[iLemmanoidLoop].RelatedSpriteID=SpriteList[iSpriteLoop].SpriteID;
													SpriteList[iLemmanoidLoop].Direction=DIR_NORTH;
													iLemmanoidLoop=MAX_SPRITE;
											}
											else if (iCaptureDistance<64)
											{
													//PrintString(SCR_2_PLANE,0,14,2+iSpriteLoop-4,"H");
													//PrintDecimal(SCR_2_PLANE,0,15,2+iSpriteLoop-4,iLemmanoidLoop,2);
													SetSprite(SpriteList[iSpriteLoop].SpriteID, SpriteList[iSpriteLoop].BaseTile , 0, 0, 0, PAL_ANGRYINVADER);
													if(SpriteList[iSpriteLoop].Position.x>SpriteList[iLemmanoidLoop].Position.x)
														SpriteList[iSpriteLoop].Position.x-=256;
													else
														SpriteList[iSpriteLoop].Position.x+=256;
												// Fixate on a single Lemmanoid
												iLemmanoidLoop=MAX_SPRITE;
											}
										}
									}
								}
								SpriteList[iSpriteLoop].Position.x=SpriteList[iSpriteLoop].Position.x+QRandom()-QRandom();
								SpriteList[iSpriteLoop].Position.y+=64;
								// We only need to check for Lemmanoid capture as we approach the ground
								if (SpriteList[iSpriteLoop].Position.y>(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-8)<<SPRITE_SCALE)
								{
									SpriteList[iSpriteLoop].Direction = DIR_NORTH;
									SetSprite(SpriteList[iSpriteLoop].SpriteID, SpriteList[iSpriteLoop].BaseTile , 0, 0, 0, PAL_INVADER);
								}
								break;
							case DIR_NORTH:
								//PrintString(SCR_2_PLANE,0,9,2+iSpriteLoop-4,"N");
								// Slow down if carrying anything
								SpriteList[iSpriteLoop].Position.x=SpriteList[iSpriteLoop].Position.x+QRandom()-QRandom();
								if (SpriteList[iSpriteLoop].RelatedSpriteID == rsNone)
								{
									SpriteList[iSpriteLoop].Position.y-=64;
									if (SpriteList[iSpriteLoop].Position.y<64)
									{
										SpriteList[iSpriteLoop].Direction = DIR_SOUTH;
									}
								}
								else
								{
									SpriteList[iSpriteLoop].Position.y-=32;
									if (SpriteList[iSpriteLoop].Position.y<32)
									{
										SpriteList[iSpriteLoop].Direction = DIR_SOUTH;
										//Drop and mutate the Lemmanoid
										SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Direction=DIR_MUTANOID;
										SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].RelatedSpriteID=rsNone;
										SpriteList[iSpriteLoop].RelatedSpriteID=rsNone;
										//lvCurrent.LemmanoidCount--;
									}
								}
								break;
						}
						SpriteList[iSpriteLoop].Frame++;
						if (SpriteList[iSpriteLoop].Frame>3) SpriteList[iSpriteLoop].Frame=0;
						break;
					case sprMissile:
						// Lemmanoids buzz the ground the mountain left and right, unless they've been captured
						switch(SpriteList[iSpriteLoop].Direction)
						{
							case DIR_LEFT:
								SpriteList[iSpriteLoop].Position.x-=256;
								break;
							case DIR_RIGHT:
								SpriteList[iSpriteLoop].Position.x+=256;
								break;
							default:
								break;
						}
						SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-64)<<SPRITE_SCALE;
						SpriteList[iSpriteLoop].Frame++;
						if (SpriteList[iSpriteLoop].Frame>1) SpriteList[iSpriteLoop].Frame=0;
						break;
					case sprSpacie:
						// Spacies will move towards their position in the rank (animation within the rank will be dealt with separately)
						// Set RelatedSpriteID to rsRank when in position
						if (SpriteList[iSpriteLoop].RelatedSpriteID!=rsRank)
						{
							//Find next available position in rank
							for(iInvaderLoop=0;iInvaderLoop<8;iInvaderLoop++)
							{
								if (SpriteList[iSpriteLoop].Position.x<=rkSpacies.Position.x+((u16)(SpriteList[iSpriteLoop].RankColumn)<<4)<<SPRITE_SCALE)
								{
									SpriteList[iSpriteLoop].Position.x+=512;
								}
								else
								{
									SpriteList[iSpriteLoop].Position.x-=512;
								}

								if(SpriteList[iSpriteLoop].Position.y<=rkSpacies.Position.y+((SpriteList[iSpriteLoop].Direction<<4)<<SPRITE_SCALE))
								{
									SpriteList[iSpriteLoop].Position.y+=128;
								}
								else
								{
									SpriteList[iSpriteLoop].Position.y-=128;
								}
							}


						}
						break;
					case sprLemmanoid:
						// Lemmanoids lope along the mountain left and right, unless they've been captured
						//PrintDecimal(SCR_2_PLANE,0,3,9+iSpriteLoop-5,SpriteList[iSpriteLoop].Position.x,5);
						switch(SpriteList[iSpriteLoop].Direction)
						{
							case DIR_WEST:
							case DIR_EAST:
								if (SpriteList[iSpriteLoop].Direction==DIR_WEST)
									{SpriteList[iSpriteLoop].Position.x-=128;}
								else
									{SpriteList[iSpriteLoop].Position.x+=128;}
								SpriteList[iSpriteLoop].Position.y = (u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]-4)<<SPRITE_SCALE;
								// Check for go home condition
								if ((SpriteList[iSpriteLoop].Position.x>MIN_CITY&&SpriteList[iSpriteLoop].Position.x<MAX_CITY&&lvCurrent.CityStatus==CITY_COMPLETE)&&bProcessControls)
								{
									//Yippee!
									// Drop anything that the Lemmanoid is carrying
									if (SpriteList[iSpriteLoop].RelatedSpriteID!=rsNone)
									{
										SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].SpriteType=sprMisc;
										SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].RelatedSpriteID=rsNone;
										SpriteList[iSpriteLoop].RelatedSpriteID=rsNone;
									}
									SpriteList[iSpriteLoop].Direction=DIR_HOME;
									SpriteList[iSpriteLoop].Frame=0;
								}
								break;
							case DIR_NORTH:
								// Captured, so movement is tied to the capturing alien (RelatedSpriteID)
								SpriteList[iSpriteLoop].Position.x=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.x;
								SpriteList[iSpriteLoop].Position.y=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.y+(8<<SPRITE_SCALE);
								break;
							case DIR_SOUTH:
								// Floating back to earth
								// Give 'em a little wobble
								SpriteList[iSpriteLoop].Position.x=SpriteList[iSpriteLoop].Position.x+QRandom()-QRandom();
								SpriteList[iSpriteLoop].Position.y+=32;
								if (SpriteList[iSpriteLoop].Position.y>(u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]+4)<<SPRITE_SCALE){
									// Drop the umbrella
									SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].SpriteType=sprMisc;
									SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].RelatedSpriteID=rsNone;
									SpriteList[iSpriteLoop].RelatedSpriteID=rsNone;
									// Resume walking
									if (QRandom()<128)
										SpriteList[iSpriteLoop].Direction = DIR_EAST;
									else
										SpriteList[iSpriteLoop].Direction = DIR_WEST;
								}
								break;
							case DIR_HOME:
								// Check the frame and destroy the sprite after the animation has completed
								if (SpriteList[iSpriteLoop].Frame==3) 
								{
									SpriteList[iSpriteLoop].SpriteType=sprMisc;
									lvCurrent.Saved++;
								}
								break;
							case DIR_MUTANOID:
								// Hunt down the player
								if(SpriteList[iSpriteLoop].Position.x<ptPlayer.x)
								{
									SpriteList[iSpriteLoop].Position.x+=256;
								}
								else
								{
									SpriteList[iSpriteLoop].Position.x-=256;
								}
								if(SpriteList[iSpriteLoop].Position.y<ptPlayer.y)
								{
									SpriteList[iSpriteLoop].Position.y+=256;
								}
								else
								{
									SpriteList[iSpriteLoop].Position.y-=256;
								}
								break;
							default:
								break;
						}
						// Pickup Pictcell?
						// Iterate through the sprite list again? Any way to avoid that?
						// Pickup Pictcell when pictcell.Direction==0 && pictcell.Position.x=Lemmanoid.Position.x (don't need to check for y)
						// Need to also make sure that each Lemmanoid *only* picks up one Pictcell
						if (SpriteList[iSpriteLoop].RelatedSpriteID==rsNone)
						{
							for(iPictcellLoop=0;iPictcellLoop<MAX_SPRITE;iPictcellLoop++)
							{
								// Never actually get picked up?
								if (SpriteList[iPictcellLoop].SpriteType==sprPictcell && SpriteList[iPictcellLoop].Direction==0 && WrapDistance(SpriteList[iPictcellLoop].Position.x,SpriteList[iSpriteLoop].Position.x,SPRITE_MAX_WIDTH)<1024 && SpriteList[iPictcellLoop].RelatedSpriteID==rsNone)
								{
									// Bingo. Attach this sprite to the Lemmanoid and change the type to the left/right carrying Hod
									// Use MasterSpriteID to chain it to another Sprite
									SpriteList[iPictcellLoop].Direction=SpriteList[iSpriteLoop].Direction;
									SpriteList[iPictcellLoop].RelatedSpriteID=SpriteList[iSpriteLoop].SpriteID;
									SpriteList[iSpriteLoop].RelatedSpriteID=SpriteList[iPictcellLoop].SpriteID;
									if(SpriteList[iSpriteLoop].Direction==DIR_EAST)
										SpriteList[iPictcellLoop].SpriteType=sprHodEast;
									else
										SpriteList[iPictcellLoop].SpriteType=sprHodWest;
									iPictcellLoop=MAX_SPRITE;
								}
							}
						}
						SpriteList[iSpriteLoop].Frame++;
						if (SpriteList[iSpriteLoop].Frame>3) SpriteList[iSpriteLoop].Frame=0;
						break;
					case sprUmbrella:
						//Umbrella - spawn just above the associated Lemmanoid
						SpriteList[iSpriteLoop].Position.x=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.x-378;
						SpriteList[iSpriteLoop].Position.y=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.y-(786);
						break;
					case sprHodWest:
					case sprHodEast:
						// Carried Pictcells will move along with their associated Lemmanoid
						// Check for being near a city, add to the city and remove the link to the Lemmanoid
						if (SpriteList[iSpriteLoop].Position.x>MIN_CITY&&SpriteList[iSpriteLoop].Position.x<MAX_CITY)
						{
							// Clear from Lemmanoid
							SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].RelatedSpriteID=rsNone;
							// Destroy pictcell
							SpriteList[iSpriteLoop].SpriteType=sprMisc;
							SpriteList[iSpriteLoop].RelatedSpriteID=rsNone;
							// Add to city
							// Find "first" city block with minimum age (frame)
							// Start at city age MAX/block 4 - so we don't keep adding once the city is complete
							iCityBlock=4;
							iMinAge=CITY_COMPLETE;
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
								lvCurrent.CityStatus++;
							}
							// else - City already complete, don't need to do anything, just destroy the pictcell sprite and let the Lemmanoid go home
						}
						else
						{
							SpriteList[iSpriteLoop].Position.x=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.x;
							SpriteList[iSpriteLoop].Position.y=SpriteList[SpriteList[iSpriteLoop].RelatedSpriteID].Position.y-256;
						}

						break;
					case sprPictcell:
						// Pictcells will float to the ground level and then wait to be picked up
						if (SpriteList[iSpriteLoop].Direction== DIR_SOUTH)
						{
							SpriteList[iSpriteLoop].Position.y+=64;
							if (SpriteList[iSpriteLoop].Position.y>((u16)(HeightMap[((u8)(SpriteList[iSpriteLoop].Position.x>>SPRITE_SCALE))+4]+4)<<SPRITE_SCALE)-1024)
							{
								SpriteList[iSpriteLoop].Direction = DIR_NONE;
								SpriteList[iSpriteLoop].RelatedSpriteID = rsNone;
							}
						}
						//Player collision?
						if ((SpriteList[iSpriteLoop].Direction== DIR_SOUTH||SpriteList[iSpriteLoop].Direction== DIR_NONE)&&bProcessControls)
						{
							if(CheckVectorSpriteCollision(ptPlayer,SpriteList[iSpriteLoop].Position,COLLISION_PLAYER))
							{
								SpriteList[iSpriteLoop].SpriteType = sprMisc;
								SpriteList[iSpriteLoop].Direction = DIR_NONE;
								SpriteList[iSpriteLoop].RelatedSpriteID = rsNone;
								iEnergyGauge=AddEnergy(iEnergyGauge,MAX_ENERGY,32);
							}
						}
						SpriteList[iSpriteLoop].Frame++;
						if (SpriteList[iSpriteLoop].Frame>3) SpriteList[iSpriteLoop].Frame=0;
						break;
					case sprMisc:
						//////////////////////////////////////////////////////
						// Spawn new invaders if invader count < default
						//////////////////////////////////////////////////////
						if(lvCurrent.InvaderCount<DefenderoidsLevels[iCurrentLevel].InvaderCount&&!bQixLevel)
						{
							lvCurrent.InvaderCount++;
							// Spawn invaders off screen, keep the player on their toes
							if(iHorizontalOffset<255)
							{
								SpriteList[iSpriteLoop]=CreateInvader(((u16)iHorizontalOffset-255)<<SPRITE_SCALE,0,iSpriteLoop);
							}
							else
							{
								SpriteList[iSpriteLoop]=CreateInvader(((u16)iHorizontalOffset+255)<<SPRITE_SCALE,0,iSpriteLoop);
							}
							if (SpriteList[iSpriteLoop].SpriteType==sprSpacie)
							{
								// Spacies will spawn at a random position, but will move to their position in the rank over time
								// Find a blank space for the new invader
								for(iInvaderLoop=0;iInvaderLoop<24;iInvaderLoop++)
								{
									if(rkSpacies.Invaders[iInvaderLoop]==rsNone)
									{
										rkSpacies.Invaders[iInvaderLoop]=SpriteList[iSpriteLoop].SpriteID;
										iInvaderLoop=24;
									}
								}

								// Limit each row to 8 invaders
								while(rkSpacies.InvaderCount[SpriteList[iSpriteLoop].Direction]>=8)
								{
									//Row is full - try the next one...
									rkSpacies.InvaderCount[SpriteList[iSpriteLoop].Direction]--;
									SpriteList[iSpriteLoop].Direction++;
									if (SpriteList[iSpriteLoop].Direction>2)
										SpriteList[iSpriteLoop].Direction=0;
								}
								// Need to backfill into any gaps, first fill an array with 1's
								for(iInvaderLoop=0;iInvaderLoop<=7;iInvaderLoop++)
								{
									freecol[iInvaderLoop]=1;
								}
								// Then loop through all spacies to work out which columns are free
								for(iInvaderLoop=0;iInvaderLoop<24;iInvaderLoop++)
								{
									// Make sure we don't match to the current sprite
									if(rkSpacies.Invaders[iInvaderLoop]!=SpriteList[iSpriteLoop].SpriteID)
									{
										// Only check spacies in the same row
										if(SpriteList[rkSpacies.Invaders[iInvaderLoop]].Direction==SpriteList[iSpriteLoop].Direction)
										{
											// Set the column "free" indicator to zero
											freecol[SpriteList[rkSpacies.Invaders[iInvaderLoop]].RankColumn]=0;
										}
									}
								}
								// Search for the first free column
								for(iInvaderLoop=0;iInvaderLoop<=7;iInvaderLoop++)
								{
									if(freecol[iInvaderLoop]==1)
									{
										SpriteList[iSpriteLoop].RankColumn=iInvaderLoop;
										iInvaderLoop=8;
									}
								}
							}

						}
						break;
					default:
						break;
				}
					
				DrawSprite(SpriteList[iSpriteLoop],iHorizontalOffset);

				// For sprites that I do want to process during the hands-off mode (fireworks etc)
				switch(SpriteList[iSpriteLoop].SpriteType)
				{
					case sprFirework:
					case sprFirework+4:
						switch(SpriteList[iSpriteLoop].Direction)
						{
							case DIR_NORTH:
								// Wobble up the screen a bit...
								SpriteList[iSpriteLoop].Position.x+=(s16)(QRandom()-128);
								SpriteList[iSpriteLoop].Position.y-=2048;
								if(SpriteList[iSpriteLoop].Position.y<(32<<SPRITE_SCALE)-(((u16)QRandom()-128)<<4))
								{
									// Trigger the final sprite animation
									SpriteList[iSpriteLoop].Direction=DIR_SOUTH;
									// Create an explosion object as well
									for (iLoopExplosion=1;iLoopExplosion<MAX_VECTOR;iLoopExplosion++)
									{
										if (VectorList[iLoopExplosion].ObjectType==VEC_NONE)
										{
											VectorList[iLoopExplosion]=CreateExplosion(SpriteList[iSpriteLoop].Position);

											// Need a better 'splode noise
											//VGM_PlaySFX((u8*)noise,1);

											iLoopExplosion = MAX_VECTOR;
										}
									} 

								}
								
								break;
							case DIR_SOUTH:
								// And then explode
								SpriteList[iSpriteLoop].Frame++;
								break;
						}
						if (SpriteList[iSpriteLoop].Frame>3)
						{
							// Reset the sprite after four frames
							SpriteList[iSpriteLoop].SpriteType=sprMisc;
						}
						break;
					default:
						break;
				}
			}

			//////////////////////////////////////////////////////
			// Score and other dressing
			//////////////////////////////////////////////////////

			// Debug info
			// Energy gauge
			iGaugePalette=PAL_DEBUG;
			for(iEnergyLoop=1;iEnergyLoop<=(iEnergyGauge>>3);iEnergyLoop++)
			{
				PutTile(SCR_1_PLANE, iGaugePalette, 6+iEnergyLoop, 16, 8);
				if(iEnergyLoop==3) iGaugePalette=PAL_SCORE;
			}
			PutTile(SCR_1_PLANE,iGaugePalette,6+iEnergyLoop,16,(iEnergyGauge%8));
			//iEnergyGauge--;
			//Need to add some spaces after the last tile just in case energy has reduced by more than 8 units
			//Also, remember to put bounds checking in when reducing the Energy level so that it doesn't wrap
			for(iEnergyLoop++;iEnergyLoop<=12;iEnergyLoop++)
			{
				PutTile(SCR_1_PLANE, iGaugePalette, 6+iEnergyLoop, 16, 0);
			}

			// Lemmanoid and city status
			// Use the unprintable CHAR(10) through to CHAR(19) for the Lemmanoid status
			// Use CHAR(20) through to CHAR(30) for the city status (only actually need CHAR(20)-CHAR(23))
			// Can also use this to check for end of level conditions
			// - All Lemmanoids Saved
			// - All Lemmanoids Dead (possibly captured)
			iLoopX=0;
			iLoopY=0;
			for(iLemmanoidLoop=0;iLemmanoidLoop<MAX_SPRITE;iLemmanoidLoop++)
			{
				switch(SpriteList[iLemmanoidLoop].SpriteType)
				{
					case sprLemmanoid:
						SetPalette(SCR_1_PLANE,PAL_STATUS+iLoopX, RGB(0,0,0), RGB(15, 11, 12), RGB(0,0,15), RGB(0,15,0));
						CopyAnimationFrame(Sprites, 10+iLoopX++, 1, (SpriteList[iLemmanoidLoop].SpriteType) + (SpriteList[iLemmanoidLoop].Direction) + SpriteList[iLemmanoidLoop].Frame);
						break;
								
					case sprCity:
						CopyAnimationFrame(Sprites, 20+iLoopY++, 1, (SpriteList[iLemmanoidLoop].SpriteType) + (SpriteList[iLemmanoidLoop].Direction) + SpriteList[iLemmanoidLoop].Frame);
						break;
				}
			}
			// Need different palettes for saved/dead Lemmanoids
			// Saved Lemmanoids get a little house
			for(iLemmanoidLoop=0;iLemmanoidLoop<lvCurrent.Saved;iLemmanoidLoop++)
			{
				SetPalette(SCR_1_PLANE,PAL_STATUS+iLoopX,RGB(0,0,0),RGB(15,15,15),RGB(15,0,0),RGB(0,15,0));
				CopyAnimationFrame(Sprites, 10+iLoopX++, 1, sprLemmanoid+DIR_SAFE);
			}
			// Dead ones get a tombstone
			lvCurrent.Died=0;
			for(;iLoopX<lvCurrent.LemmanoidCount;iLoopX++)
			{
				SetPalette(SCR_1_PLANE,PAL_STATUS+iLoopX,RGB(0,0,0),RGB(15,15,15),RGB(3,3,3),RGB(0,15,0));
				CopyAnimationFrame(Sprites, 10+iLoopX, 1, sprLemmanoid+DIR_HEADSTONE);
				lvCurrent.Died++;

			}
			// Everything else gets a null sprite
			for(;iLoopX<10;iLoopX++)
			{
				CopyAnimationFrame(Sprites, 10+iLoopX, 1, sprMisc);
			}

			// Switch to hands-off mode if end of level reached for any reason
			// On the Qix level, Vector object [0] will always be the Qix
			if (iEnergyGauge==0 || ((lvCurrent.LemmanoidCount==(lvCurrent.Saved+lvCurrent.Died))&&!bQixLevel)||(bQixLevel&&VectorList[0].Points<=3))
			{
				bProcessControls=false;
			}

			// Reduce the ship "bounce" counter if>0
			if (vShip.Counter>0)
			{
				vShip.Counter--;
			}

		} // Level Loop

		// Clear all sprites
		for (iSpriteLoop=0;iSpriteLoop<MAX_SPRITE;iSpriteLoop++)
		{
			SpriteList[iSpriteLoop].SpriteID=rsNone;
			SpriteList[iSpriteLoop].RelatedSpriteID=rsNone;
			SpriteList[iSpriteLoop].SpriteType=sprMisc;
			SetSprite(iSpriteLoop, 0, 0, 0, 0, PAL_SPRITE);
		}

		// Reset the Lemmanoid count and City status tiles too

		if(!bQixLevel&&lvCurrent.LemmanoidCount==lvCurrent.Died)
		{
			bQixLevel=true;
		}
		else
		{
			bQixLevel=false;
			iCurrentLevel++;
			// Setup next level
			if(iCurrentLevel>MAX_LEVEL) iCurrentLevel=1;
		}


	} // Player Energy Loop

	VGM_StopBGM();

	//////////////////////////////////////////////////////
	// Game Over information etc
	//////////////////////////////////////////////////////

}