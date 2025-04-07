#include "ngpc.h"
#include "library.h"
#include "veclib.h"

void DrawVectorObjectAbsolute(u16 * BitmapAddress, VECTOROBJECT VectorObject)
{
	DrawVectorObject(BitmapAddress, VectorObject, 0);
}

void DrawVectorObject(u16 * BitmapAddress, VECTOROBJECT VectorObject, u16 iHorizontalOffset)
{
	s16 iStartX;
	s16 iStartY;
	s16 iEndX;
	s16 iEndY;
	s16 iTempX;
	s16 iTempY;
	s8 sPositionX;
	u8 iPositionX;
	u8 iPositionY;
	u8 iPoint = 0;
	s8 cSin;
	s8 cCos;


	sPositionX = VectorObject.Position.x>>7;
	iPositionX=sPositionX-iHorizontalOffset;

	/*
	PrintString(SCR_2_PLANE,0,0,1,"X:");
	PrintDecimal(SCR_2_PLANE,0,2,1,sPositionX,3);
	PrintString(SCR_2_PLANE,0,0,2,"A:");
	PrintDecimal(SCR_2_PLANE,0,2,2,iPositionX,3);
	//Clear some space for debug lines
	PrintString(SCR_2_PLANE,0,0,3,"                      ");
	PrintString(SCR_2_PLANE,0,0,4,"                      ");
	PrintString(SCR_2_PLANE,0,0,5,"                      ");
	PrintString(SCR_2_PLANE,0,0,6,"                      ");
	PrintString(SCR_2_PLANE,0,0,7,"                      ");
	PrintString(SCR_2_PLANE,0,0,8,"                      ");
	*/
	if (iPositionX >=0 && iPositionX<BitmapAddress[0])
	{

		//PrintString(SCR_2_PLANE,0,0,4,"PLOTTING");

		cSin = Sin(VectorObject.RotationAngle);
		cCos = Cos(VectorObject.RotationAngle);

		sPositionX = VectorObject.Position.x>>7;
		sPositionX -= iHorizontalOffset;
		iPositionX = sPositionX % 256;

		iPositionY = VectorObject.Position.y>>7;

		iStartX = VectorObject.VectorList[0].x;
		iStartY = VectorObject.VectorList[0].y;

		//Plot the centre point
		//SetPixel((u16*)BitmapAddress,iPositionX,iPositionY,3);

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
			iStartX = iPositionX+iTempX;
			//if (iStartX<0) iStartX=0;
			iStartY = (VectorObject.Position.y>>7)+iTempY;
			//if (iStartY<0) iStartY=0;

			iEndX = (VectorObject.VectorList[iPoint].x-VectorObject.Origin.x)*VectorObject.Scale;
			iEndY = (VectorObject.VectorList[iPoint].y-VectorObject.Origin.y)*VectorObject.Scale;

			// rotate point
			iTempX = ((iEndX * cCos)>>7) - ((iEndY * cSin)>>7);
			iTempY = ((iEndX * cSin)>>7) + ((iEndY * cCos)>>7);

			// translate point back:
			iEndX = iPositionX+iTempX;
			//if (iEndX<0) iEndX=0;
			iEndY = iPositionY+iTempY;
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
void DrawVectorSprite(u16 * BitmapAddress, VECTOROBJECT VectorObject, u16 iHorizontalOffset)
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
	u8 iPositionX;

	cSin = Sin(VectorObject.RotationAngle);
	cCos = Cos(VectorObject.RotationAngle);

	while (iPoint<VectorObject.Points)
	{

		if (VectorObject.VectorList[iPoint].colour != 0)
		{

			iStartX = (VectorObject.VectorList[iPoint].x-VectorObject.Origin.x)*VectorObject.Scale;
			iStartY = (VectorObject.VectorList[iPoint].y-VectorObject.Origin.y)*VectorObject.Scale;

			// rotate point.
			iTempX = ((iStartX * cCos)>>7) - ((iStartY * cSin)>>7);
			iTempY = ((iStartX * cSin)>>7) + ((iStartY * cCos)>>7);

			// translate point back to it's original position:
			// x = -32767...32767
			// iTempX=0
			// iHorizontalX=72
			// Only display when Position.x - iHorizontalOffset > 72 and <144
			// Need to restict "Position.x" to a 256 byte value?
			// Which needs to wrap around at 0/256
			// Will MOD() do that?
			iPositionX = VectorObject.Position.x % 256;
			iStartX = (iPositionX-iHorizontalOffset)+iTempX;
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
		iPoint++;
	}
}

bool LineIntersect(POINT pLineStart, POINT pLineEnd, POINT pBoxTopLeft, POINT pBoxBottomRight)
{
	// Stolen from the library function DrawLine()
	// Step through the line and test each point against the rectangle for the hit box. If they intersect, then they must be in contact.
	// Bit of a brute force method and I'm sure can be improved.
	s16 xinc1;
	s16 xinc2;
	s16 yinc1;
	s16 yinc2;
	s16 den;
	s16 num;
	s16 numadd;
	s16 numpixels;
	s16 curpixel;
	s16 deltax;
	s16 deltay;
	s16 x;
	s16 y;


	if (pLineEnd.x >= pLineStart.x)
		deltax = pLineEnd.x - pLineStart.x;        // The difference between the x's
	else
		deltax = pLineStart.x - pLineEnd.x;
	if (pLineEnd.y>=pLineStart.y)
		deltay = pLineEnd.y - pLineStart.y;        // The difference between the y's
	else
		deltay = pLineStart.y - pLineEnd.y;

	x = pLineStart.x;                       // Start x off at the first pixel
	y = pLineStart.y;                       // Start y off at the first pixel

	if (pLineEnd.x >= pLineStart.x)                 // The x-values are increasing
	{
		xinc1 = 1;
		xinc2 = 1;
	}
	else                          // The x-values are decreasing
	{
		xinc1 = -1;
		xinc2 = -1;
	}

	if (pLineEnd.y >= pLineStart.y)                 // The y-values are increasing
	{
		yinc1 = 1;
		yinc2 = 1;
	}
	else                          // The y-values are decreasing
	{
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay)         // There is at least one x-value for every y-value
	{
		xinc1 = 0;                  // Don't change the x when numerator >= denominator
		yinc2 = 0;                  // Don't change the y for every iteration
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;         // There are more x-values than y-values
	}
	else                          // There is at least one y-value for every x-value
	{
		xinc2 = 0;                  // Don't change the x for every iteration
		yinc1 = 0;                  // Don't change the y when numerator >= denominator
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;         // There are more y-values than x-values
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		// Check current point against the rectangle
		if (x >= pBoxTopLeft.x && x <=pBoxBottomRight.x && y>=pBoxTopLeft.y && y<=pBoxBottomRight.y)
		{
			// point is inside the bounds of the box. We've hit something!
			return 1;
		}

		num += numadd;              // Increase the numerator by the top of the fraction
		if (num >= den)             // Check if numerator >= denominator
		{
			num -= den;               // Calculate the new numerator value
			x += xinc1;               // Change the x as appropriate
			y += yinc1;               // Change the y as appropriate
		}
		x += xinc2;                 // Change the x as appropriate
		y += yinc2;                 // Change the y as appropriate
	}

	return 0;
}
