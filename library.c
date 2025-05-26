#include "ngpc.h"
#include "library.h"
#include "vgmplayer.h"


volatile u8 VBCounter;

volatile u16 RandomNumberCounter;

// dummy Interrupt function used for interrupt that are currently
// unused. To add an interrupt simply supply your own routine in the
// InitNGPC routine in the interrupt vector table initialisation
void __interrupt DummyFunction(void) {}


//////////////////////////////////////////////////////////////////////////////
// VBInterrupt
// Handles the default work required by the VBInterrupt (watchdog,
// shutdown and a standard counter)
//////////////////////////////////////////////////////////////////////////////
void SysShutdown(); // forward reference
void __interrupt VBInterrupt(void)
{
   // clear the watchdog
   WATCHDOG = WATCHDOG_CLEAR;

   // check for power down
   if (USR_SHUTDOWN)
   {
	  SysShutdown();
	  while (1);
   }

   // increment a counter
   VBCounter++;

   if (VGM_IsInstalled())
   	VGM_SendData();

   // TODO: add any other VBI code here.
}

//////////////////////////////////////////////////////////////////////////////
// InitNGPC
// This should be the first function called in main() it initialises the
// NGPC hardware (interrupt vector tables etc)
//////////////////////////////////////////////////////////////////////////////
void InitNGPC(void)
{
   // Set NGP or NGPC mode
   if (OS_VERSION == 0)
   {
	  (*(u8*)0x6f83) &= ~8; // res 3
	  (*(u8*)0x6da0) = 0;
   }

   // set user answer
   USR_ANSWER |= 64; // set 6

   // install user interrupt vectors
   SWI3_INT = DummyFunction;
   SWI4_INT = DummyFunction;
   SWI5_INT = DummyFunction;
   SWI6_INT = DummyFunction;
   RTCI_INT = DummyFunction;
   VBL_INT = VBInterrupt;
   Z80_INT = DummyFunction;
   HBL_INT = DummyFunction;
   TI0_INT = DummyFunction;
   TI1_INT = DummyFunction;
   TI2_INT = DummyFunction;
   TI3_INT = DummyFunction;
   STX_INT = DummyFunction;
   SRX_INT = DummyFunction;
   DMA0_INT = DummyFunction;
   DMA1_INT = DummyFunction;
   DMA2_INT = DummyFunction;
   DMA3_INT = DummyFunction;

   ENABLE_INTERRUPTS;

   // set screen size
   WIN_X = 0;
   WIN_Y = 0;
   WIN_W = 160;
   WIN_H = 152;

}


//////////////////////////////////////////////////////////////////////////////
// SysShutdown                                                              //
//////////////////////////////////////////////////////////////////////////////
void SysShutdown()
{
   __asm(" ld rw3,0"); // 0 = VECT_SHUTDOWN

   // do the system call
   __asm(" ldf 3");
   __asm(" add w,w");
   __asm(" add w,w");
   __asm(" ld xix,0xfffe00");
   __asm(" ld xix,(xix+w)");
   __asm(" call xix");
}


//////////////////////////////////////////////////////////////////////////////
// SysSetSystemFont                                                         //
//////////////////////////////////////////////////////////////////////////////
void SysSetSystemFont()
{
   __asm(" ld ra3,3");
   __asm(" ld rw3,5"); // VECT_SYSFONTSET

   // do the system call
   __asm(" ldf 3");
   __asm(" add w,w");
   __asm(" add w,w");
   __asm(" ld xix, 0xfffe00");
   __asm(" ld xix,(xix+w)");
   __asm(" call xix");
}

//////////////////////////////////////////////////////////////////////////////
// ClearScreen
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//////////////////////////////////////////////////////////////////////////////
void ClearScreen(u8 Plane)
{
   int i;
   u16 * Screen;

   switch(Plane)
   {
	  case SCR_1_PLANE:
		 Screen = SCROLL_PLANE_1;
		 break;
	  case SCR_2_PLANE:
		 Screen = SCROLL_PLANE_2;
		 break;
	  default:
		 return;
   }

   for (i = 0; i < 32*32; i ++)
	  Screen[i] = 0;
}

//////////////////////////////////////////////////////////////////////////////
// SetPalette
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//		PalleteNo - 0-15 the palette no to set
//      Col0-Col3 - The RGB values for the 4 colours within the palette
//////////////////////////////////////////////////////////////////////////////
void SetPalette(u8 Plane, u8 PaletteNo, u16 Col0, u16 Col1, u16 Col2, u16 Col3)
{
   u16 * thePalette;
   u8 Offset = PaletteNo * 4;

   switch (Plane)
   {
	  case SCR_1_PLANE:
		 thePalette = SCROLL_1_PALETTE;
		 break;
	  case SCR_2_PLANE:
		 thePalette = SCROLL_2_PALETTE;
		 break;
      case SPRITE_PLANE:
         thePalette = SPRITE_PALETTE;
         break;
	  default:
		 return;
   }

   thePalette[Offset] = Col0;
   thePalette[Offset+1] = Col1;
   thePalette[Offset+2] = Col2;
   thePalette[Offset+3] = Col3;
}


//////////////////////////////////////////////////////////////////////////////
// PutTile
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//		PalleteNo - 0-15 the palette number to set
//      XPos - X Position (0 to 19)
//      YPos - Y Position (0 to 18)
//      TileNo - Tile Number (0 to 511)
//////////////////////////////////////////////////////////////////////////////
void PutTile(u8 Plane, u8 Palette, u8 XPos, u8 YPos, u16 TileNo)
{
   u16 * ScreenPlane1 = SCROLL_PLANE_1;
   u16 * ScreenPlane2 = SCROLL_PLANE_2;

   u16 Offset = ((u16)YPos * 32) + XPos;
   u16 Value = TileNo + ((u16)Palette << 9);

   switch(Plane)
   {
	  case SCR_1_PLANE:
		 ScreenPlane1[Offset] = Value;
		 break;
	  case SCR_2_PLANE:
		 ScreenPlane2[Offset] = Value;
		 break;
	  default:
		 break;
   }
}


//////////////////////////////////////////////////////////////////////////////
// GetTile
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//      XPos - X Position (0 to 19)
//      YPos - Y Position (0 to 18)
// Outputs:
//		PalleteNo - 0-15 the palette number to set
//      TileNo - Tile Number (0 to 511)
//////////////////////////////////////////////////////////////////////////////
void GetTile(u8 Plane, u8 *Palette, u8 XPos, u8 YPos, u16 *TileNo)
{
   u16 * ScreenPlane1 = SCROLL_PLANE_1;
   u16 * ScreenPlane2 = SCROLL_PLANE_2;

   u16 Offset = ((u16)YPos * 32) + XPos;

   switch(Plane)
   {
      case SCR_1_PLANE:
	 *Palette = (u8)(ScreenPlane1[Offset] >> 9);
	 *TileNo = (ScreenPlane1[Offset] & 511);
	 break;
      case SCR_2_PLANE:
	 *Palette = (u8)(ScreenPlane2[Offset] >> 9);
	 *TileNo = (ScreenPlane2[Offset] & 511);
	 break;
      default:
	 break;
   }
}


//////////////////////////////////////////////////////////////////////////////
// Sleep
// Waits for specified number of VBlanks (60ths of a second)
//////////////////////////////////////////////////////////////////////////////
void Sleep(u8 VBlanks)
{
   VBCounter = 0;
   while (VBCounter < VBlanks)
      ;
}


//////////////////////////////////////////////////////////////////////////////
// PrintDecimal
// Displays a decimal number of the screen padded with leading zeros
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//		PalleteNo - 0-15 the palette number to set
//      x - X Position (0 to 19)
//      y - Y Position (0 to 18)
//      Value - The number to display
//      Len - The number of tiles to fill
//            e.g. Value = 15, Len = 4 will display 0015
//////////////////////////////////////////////////////////////////////////////
void PrintDecimal(u8 Plane, u8 PaletteNo, u8 x, u8 y, u16 Value, u8 Len)
{
   u8 i;

   for (i = Len; i > 0; i--)
   {
	  PutTile(Plane, PaletteNo, x+i-1, y, '0' + (Value % 10));
	  Value /= 10;
   }
}

void PrintHex(u8 Plane, u8 PaletteNo, u8 x, u8 y, u16 Value, u8 Len)
{
// Similar to PrintDecimal, but prints in Hexadecimal
// works well with the BCD values returned by GetTime

   u8 i;

   for (i = Len; i > 0; i--)
   {
      if ((Value & 15) < 10)
         PutTile(Plane, PaletteNo, x+i-1, y, '0' + (Value & 15));
      else
         PutTile(Plane, PaletteNo, x+i-1, y, '7' + (Value & 15));

      Value = Value >> 4;
   }
}

void PrintBinary(u8 Plane, u8 PaletteNo, u8 x, u8 y, u16 Value, u8 Len)
{
// Similar to PrintDecimal, but prints in Hexadecimal
// works well with the BCD values returned by GetTime

   u8 i;

   for (i = Len; i > 0; i--)
   {
      if ((Value & 1))
         PutTile(Plane, PaletteNo, x+i-1, y, '1');
      else
         PutTile(Plane, PaletteNo, x+i-1, y, '0');

      Value = Value >> 1;
   }
}



//////////////////////////////////////////////////////////////////////////////
// InstallTileSet
// Copies your tile data to the NGPC tileram. Takes a 2D array and a
// length. The length field will equal "sizeof(Tiles)/2" which is the
// number of words in the 2D array.
// This style array can be exported directly from Steve Robb's NeoTile
// NGPC Tile Editing tool
//////////////////////////////////////////////////////////////////////////////
void InstallTileSet(const unsigned short Tiles[][8], u16 Len)
{
   u16 i;
   u16 * TileRam = TILE_RAM;
   u16 * theTiles = (u16 *)Tiles;

   for (i = 0; i < Len; i ++)
   {
	  TileRam[i] = *theTiles++;
   }
}

// similar to InstallTiles but allows you to
// adds tiles to a non-zero location in tile RAM
// useful if a font is installed in the lower tiles
void InstallTileSetAt(const u16 Tiles[][8], u16 Len, u16 Offset)
{
   u16 i;
   u16 * TileRam = TILE_RAM + (Offset * 8);
   u16 * theTiles = (u16 *)Tiles;

   for (i = 0; i < Len; i ++)
   {
      TileRam[i] = *theTiles++;
   }
}



//////////////////////////////////////////////////////////////////////////////
// SetBackgroundColour
// Changes the background colour of NGPC screen.
// The macro RGB() can be used to specify Col.
// e.g SetBackgroundColour(RGB(15,0,0)); will set the background red
//////////////////////////////////////////////////////////////////////////////
void SetBackgroundColour(u16 Col)
{
   BG_PAL = Col;
   BG_COL = 0x80;
}


//////////////////////////////////////////////////////////////////////////////
// PrintString
// Displays a string on the screen at the specified location
// Inputs:
//		Plane - Scroll Plane to clear SCR_1_PLANE or SCR_2_PLANE
//		PalleteNo - 0-15 the palette number to set
//      XPos - X Position (0 to 19)
//      YPos - Y Position (0 to 18)
//      theString - The string to be displayed
//////////////////////////////////////////////////////////////////////////////
void PrintString(u8 Plane, u8 Palette, u8 XPos, u8 YPos, const char * theString)
{
   u16 * Screen;

   switch (Plane)
   {
	  case SCR_1_PLANE:
		 Screen = SCROLL_PLANE_1;
		 break;

	  case SCR_2_PLANE:
		 Screen = SCROLL_PLANE_2;
		 break;

	  default:
		 return;
   }

   while (*theString)
   {
	  u16 Offset = ((u16)YPos * 32) + XPos;
	  u16 Value = *theString + ((u16)Palette << 9);
	  Screen[Offset] = Value;

	  theString++;
	  XPos++;
   }
}



//////////////////////////////////////////////////////////////////////////////
// SetSprite
// Initialise a sprite by mapping a tile number and sprite palette no
// to a sprite number and allowing chaining to be set up.
// Inputs:
//		SpriteNo - 0-63 the sprite to move
//      XPos - X Position (0 to 255)
//      YPos - Y Position (0 to 255)
//////////////////////////////////////////////////////////////////////////////
void SetSprite(u8 SpriteNo, u16 TileNo, u8 Chain, u8 XPos, u8 YPos, u8 PaletteNo)
{
	u16 SprCtrlReg;
	u8 * theSprite = SPRITE_RAM;
	u8 * theSpriteCol = SPRITE_COLOUR;

	theSprite += (SpriteNo * 4);
	theSpriteCol += SpriteNo;

	SprCtrlReg = 24; // topmost priority (16 + 8)
	if (Chain)
		SprCtrlReg += 6; // v and h chaining (4 + 2)

	*(theSprite)   = TileNo;
	*(theSprite+1) = SprCtrlReg+(TileNo>>8);
	*(theSprite+2) = XPos;
	*(theSprite+3) = YPos;

	*theSpriteCol = PaletteNo;
}

//////////////////////////////////////////////////////////////////////////////
// SetSpritePalette
// Changes the palette for a given sprite
// Parameters:
// - SpriteNo
// - PaletteNo
// 
//////////////////////////////////////////////////////////////////////////////
void SetSpritePalette(u8 SpriteNo, u8 PaletteNo)
{
	u8 * theSpriteCol = SPRITE_COLOUR;

	theSpriteCol += SpriteNo;
	*theSpriteCol = PaletteNo;

}

//////////////////////////////////////////////////////////////////////////////
// SetSpriteOptions
// Applies sprite parameters to the 2nd bit
// Inputs:
//		SpriteNo - 0-63 the sprite to flip
//      HorizontalFlip - Apply horizontal flip
//      VerticalFlip - Apply vertical flip
// Possible extensions:
//		Priority
//		Horizontal/Vertical Chain
//		Palette Code
//
//		Sprites are in four bytes starting at SPRITE_RAM (0x8800)
//		+0	C.C = Character Tile Number (lower 8 bits of 9)
//		+1	Bit 7 - H.F = Horizontal Flip (0=normal,1=flipped)
//			Bit 6 - V.F = Vertical Flip   (0=normal,1=flipped
//			Bit 5 - P.C = Palette Code (Valid for K1GE upper palette compatible mode ie BW hardware)
//			Bit 4 - PR.C = Priority Code MSB  (00=Hide,01=Furthest,10=Middle,11=Front)
//			Bit 3 - PR.C = Priority Code LSB
//			Bit 2 - H.ch = H Position Chain (0 = Normal, 1 = Offset coordinates) (Value defined becomes the offset value with respect to the previous character.)
//			Bit 1 - V.ch = V Position Chain (0 = Normal, 1 = Offset coordinates) (Value defined becomes the offset value with respect to the previous character.)
//			Bit 0 - C.C = Character Tile Number (Upper 1 bit of 9)
//		+2	H.P = Horizontal Position
//		+3	V.P = Vertical Position
//	
//		Sprite palette memory starts at SPRITE_COLOUR (0x8c00)
//		
//////////////////////////////////////////////////////////////////////////////
void SetSpriteOptions(u8 SpriteNo, bool HorizontalFlip, bool VerticalFlip, bool Chain)
{
	u16 SprCtrlReg;
	u8 * theSprite = SPRITE_RAM;

	theSprite += (SpriteNo * 4);

	// First remove any existing attributes
	SprCtrlReg&=57;
	// Apply flips if required
	if (VerticalFlip==1)
		SprCtrlReg|=64;
	if (HorizontalFlip==1)
		SprCtrlReg|=128;
	if (Chain==1)
		SprCtrlReg|=6;

	*(theSprite+1) = SprCtrlReg;

}


//////////////////////////////////////////////////////////////////////////////
// SetSpritePosition
// Moves a already initialise sprite
// Inputs:
//		SpriteNo - 0-63 the sprite to move
//      XPos - X Position (0 to 255)
//      YPos - Y Position (0 to 255)
//////////////////////////////////////////////////////////////////////////////
void SetSpritePosition(u8 SpriteNo, u8 XPos, u8 YPos)
{
   u8 * theSprite = SPRITE_RAM;
   theSprite += (SpriteNo * 4);

   *(theSprite+2) = XPos;
   *(theSprite+3) = YPos;
}

void BlockCopy(u8 * Dest, const u8 * Source, u16 n)
{
   u8 *p1, *p2;

   p1 = Dest;
   p2 = (u8 *)Source;

   while (n--)
	  *p1++ = *p2++;
}

// ----------------------------------------------
// QRandom Numbers - Chris Ahchay
// ----------------------------------------------
//Most of these aren't used... I'll get the correctly shaped tuit one of these days....
const unsigned char QuickRandomNumbers[1024]={63, 254, 102, 147, 252, 158, 83, 102, 90, 162, 89, 211, 146, 181,
33, 188, 239, 26, 52, 113, 168, 22, 140, 238, 236, 139, 163, 63, 92, 140, 49, 59, 196, 187, 155, 179, 77, 194,
203, 66, 226, 67, 239, 131, 193, 41, 115, 237, 253, 231, 230, 26, 201, 211, 37, 120, 217, 200, 147, 142, 88,
251, 156, 158, 223, 88, 255, 196, 160, 75, 19, 82, 24, 129, 212, 130, 118, 66, 20, 176, 87, 69, 79, 213, 195,
71, 109, 232, 48, 169, 93, 43, 206, 8, 52, 207, 92, 6, 47, 123, 153, 158, 256, 219, 247, 0, 48, 27, 140, 194,
74, 196, 206, 104, 78, 142, 13, 82, 65, 5, 220, 215, 72, 235, 122, 240, 166, 14, 149, 116, 233, 254, 159, 164,
52, 236, 252, 231, 84, 189, 158, 45, 1, 75, 130, 202, 89, 229, 136, 194, 61, 253, 91, 19, 1, 101, 34, 80, 74,
114, 243, 79, 172, 125, 101, 91, 42, 176, 31, 15, 171, 213, 127, 132, 256, 168, 190, 42, 100, 80, 78, 17, 104,
98, 142, 245, 158, 95, 111, 155, 131, 132, 220, 38, 158, 217, 186, 12, 29, 246, 31, 157, 19, 197, 228, 237, 248,
89, 62, 232, 247, 167, 92, 127, 100, 168, 200, 50, 113, 248, 144, 132, 91, 216, 57, 132, 140, 13, 251, 214, 30,
177, 252, 61, 1, 125, 90, 84, 161, 118, 71, 146, 74, 2, 27, 168, 158, 93, 199, 78, 201, 181, 229, 132, 226, 178,
24, 206, 158, 153, 29, 87, 93, 210, 226, 149, 206, 133, 69, 163, 53, 250, 195, 132, 5, 131, 219, 213, 35, 72, 150,
107, 26, 146, 64, 30, 39, 223, 105, 172, 109, 245, 11, 12, 20, 33, 223, 13, 40, 94, 104, 218, 238, 230, 26, 196,
255, 229, 107, 40, 155, 35, 218, 178, 115, 154, 120, 86, 163, 120, 235, 94, 44, 201, 164, 8, 80, 250, 25, 30, 256,
84, 24, 36, 177, 26, 180, 208, 190, 255, 143, 66, 34, 135, 185, 187, 248, 172, 242, 186, 177, 232, 180, 30, 206,
210, 214, 224, 152, 176, 13, 89, 30, 242, 156, 31, 138, 74, 239, 43, 151, 179, 60, 24, 186, 81, 141, 106, 133, 131,
58, 31, 221, 164, 141, 175, 57, 204, 63, 82, 52, 161, 106, 93, 124, 69, 17, 79, 113, 139, 195, 57, 103, 45, 225, 45,
51, 59, 251, 28, 30, 38, 204, 237, 198, 63, 217, 106, 109, 109, 63, 30, 114, 179, 117, 139, 102, 209, 186, 104, 202,
165, 165, 125, 222, 116, 161, 219, 179, 37, 156, 115, 4, 80, 10, 195, 214, 219, 211, 211, 49, 197, 151, 159, 35, 155,
49, 120, 157, 165, 182, 99, 189, 18, 204, 204, 218, 21, 239, 214, 90, 24, 69, 186, 218, 44, 154, 142, 149, 230, 253,
16, 98, 46, 163, 121, 25, 9, 61, 95, 225, 150, 149, 22, 199, 253, 19, 139, 246, 167, 28, 183, 238, 126, 37, 231, 66,
77, 21, 117, 40, 227, 155, 236, 62, 244, 227, 14, 244, 172, 71, 149, 174, 4, 205, 9, 18, 123, 175, 37, 147, 255, 27,
128, 228, 36, 70, 152, 60, 1, 13, 43, 7, 67, 131, 178, 8, 203, 67, 183, 66, 221, 120, 208, 48, 45, 197, 99, 159, 169,
91, 20, 219, 82, 245, 56, 127, 147, 136, 80, 201, 201, 161, 24, 212, 200, 253, 53, 141, 188, 247, 62, 147, 185, 212,
36, 253, 175, 238, 205, 147, 20, 154, 160, 74, 180, 169, 37, 85, 97, 12, 169, 195, 114, 215, 219, 83, 251, 25, 248,
140, 200, 179, 26, 175, 128, 136, 219, 182, 86, 215, 30, 71, 250, 159, 206, 7, 79, 173, 200, 194, 212, 244, 14, 49,
42, 241, 157, 239, 70, 63, 84, 85, 44, 190, 9, 18, 209, 24, 246, 169, 176, 232, 194, 54, 136, 132, 84, 148, 199, 12,
95, 155, 38, 124, 236, 88, 141, 222, 196, 221, 181, 162, 12, 154, 146, 122, 189, 138, 142, 137, 188, 48, 15, 161, 16,
52, 70, 24, 63, 56, 99, 216, 174, 95, 3, 59, 247, 235, 160, 8, 239, 57, 241, 58, 36, 163, 19, 24, 244, 80, 213, 93,
208, 223, 150, 59, 234, 212, 169, 65, 130, 250, 212, 49, 160, 114, 127, 202, 57, 244, 210, 252, 109, 191, 60, 102,
9, 51, 223, 28, 62, 30, 219, 224, 198, 37, 102, 167, 254, 111, 32, 33, 62, 212, 144, 128, 81, 97, 62, 63, 106, 48,
8, 55, 84, 134, 248,112, 211, 134, 3, 53, 35, 143, 58, 159, 219, 114, 56, 142, 100, 100, 97, 136, 216, 202, 217, 122,
233, 47, 81, 29, 10, 18, 179, 180, 44, 236, 186, 102, 4, 186, 83, 225, 130, 100, 179, 166, 190, 252, 39, 8, 85, 86,
226, 174, 158, 116, 214, 50, 187, 39, 40, 227, 176, 125, 79, 129, 85, 236, 165, 26, 97, 127, 117, 190, 161, 100, 183,
123, 27, 116, 86, 149, 226, 76, 178, 12, 154, 67, 67, 89, 228, 180, 206, 216, 74, 97, 222, 9, 159, 19, 130, 191, 120,
94, 7, 93, 233, 43, 251, 151, 148, 55, 28, 79, 43, 33, 8, 139, 243, 171, 189, 104, 19, 95, 252, 123, 130, 69, 16, 84,
191, 48, 159, 77, 196, 246, 34, 217, 153, 152, 216, 53, 36, 153, 205, 246, 52, 142, 115, 244, 45, 120, 171, 205, 219,
229, 179, 239, 93, 130, 126, 244, 19, 189, 184, 34, 247, 101, 176, 76, 94, 118, 56, 19, 240, 20, 197, 143, 219, 38,
42, 158, 9, 223, 173, 149, 211, 54, 255, 136, 11, 194, 26, 172, 73, 109, 14, 178, 9, 114, 121, 157, 117, 119, 91, 248,
29, 156, 194, 35, 156, 158, 8, 18, 170, 243, 145, 51, 93, 128, 92, 237, 192, 77, 238, 250, 173, 109, 41, 51, 43, 80,
241, 28, 172, 46, 99, 183, 35, 133, 170, 9, 120, 192, 44, 110, 182, 135, 193, 45, 161, 105, 151, 191, 170, 226, 53,
70, 31, 182, 2};

// Initialises QRandom counter based on the clock
void InitialiseQRandom()
{
   TIME tmCur;

   GetTime(&tmCur);

   RandomNumberCounter=tmCur.Second;
}

// Returns the next random number in the array
unsigned char QRandom()
{
	u8 iRandom;
	iRandom=QuickRandomNumbers[RandomNumberCounter++];
	return iRandom;
}


//////////////////////////////////////////////////////////////////////////////
// Sine and Cosine
//////////////////////////////////////////////////////////////////////////////
const signed char SineTable256[256]=
{
   0,	3,	6,	9,	12,	16,	19,	22,	25,	28,	31,	34,	37,	40,	43,	46,	49,	51,	54,	57,	60,	63,	65,	68,	71,	73,	76,	78,	81,	83,	85,	88,	90,	92,	94,	96,	98,	100,	102,	104,	106,	107,	109,	111,	112,	113,	115,	116,	117,	118,	120,	121,	122,	122,	123,	124,	125,	125,	126,	126,	126,	127,	127,	127,	127,
   127,	127,	127,	126,	126,	126,	125,	125,	124,	123,	122,	122,	121,	120,	118,	117,	116,	115,	113,	112,	111,	109,	107,	106,	104,	102,	100,	98,	96,	94,	92,	90,	88,	85,	83,	81,	78,	76,	73,	71,	68,	65,	63,	60,	57,	54,	51,	49,	46,	43,	40,	37,	34,	31,	28,	25,	22,	19,	16,	12,	9,	6,	3,
   0,	-3,	-6,	-9,	-12,	-16,	-19,	-22,	-25,	-28,	-31,	-34,	-37,	-40,	-43,	-46,	-49,	-51,	-54,	-57,	-60,	-63,	-65,	-68,	-71,	-73,	-76,	-78,	-81,	-83,	-85,	-88,	-90,	-92,	-94,	-96,	-98,	-100,	-102,	-104,	-106,	-107,	-109,	-111,	-112,	-113,	-115,	-116,	-117,	-118,	-120,	-121,	-122,	-122,	-123,	-124,	-125,	-125,	-126,	-126,	-126,	-127,	-127,	-127,	-127,	-127,
   -127,	-127,	-126,	-126,	-126,	-125,	-125,	-124,	-123,	-122,	-122,	-121,	-120,	-118,	-117,	-116,	-115,	-113,	-112,	-111,	-109,	-107,	-106,	-104,	-102,	-100,	-98,	-96,	-94,	-92,	-90,	-88,	-85,	-83,	-81,	-78,	-76,	-73,	-71,	-68,	-65,	-63,	-60,	-57,	-54,	-51,	-49,	-46,	-43,	-40,	-37,	-34,	-31,	-28,	-25,	-22,	-19,	-16,	-12,	-9,	-6,	-3
};

signed char Sin(unsigned char dAngle)
{
   return SineTable256[dAngle];
}

signed char Cos(unsigned char dAngle)
{
   return SineTable256[dAngle+64];
}

//////////////////////////////////////////////////////////////////////////////
// WrapDistance
// Calculates the distance between two points with a wrap around (max)
//////////////////////////////////////////////////////////////////////////////
u16 WrapDistance(u16 First, u16 Second, u16 Wrap)
{
	u16 iDistance;
	u16 iWrapDistance;
	u16 retval;

	//Start by calculating Difference between First and Second
	//both directly (x1-x2) and indirectly (wrap-x1)+x2
	if (First>=Second)
	{
		iDistance=First-Second;
		iWrapDistance=(Wrap-First)+Second;
	}
	else
	{
		iDistance=Second-First;
		iWrapDistance=(Wrap-Second)+First;
	}

	//Return the smaller of the two distances
	if (iDistance<iWrapDistance)
		retval=iDistance;
	else
		retval=iWrapDistance;

	return retval;
}


//////////////////////////////////////////////////////////////////////////////
// Time and Alarm functions
//////////////////////////////////////////////////////////////////////////////
const char DayNames[7][10] = {
   "Sunday", "Monday", "Tuesday", "Wednesday",
   "Thursday", "Friday", "Saturday"
};


void GetTime(TIME * pTime)
{
   __asm(" ld rw3, 2");		// VECT_RTCGET
   __asm(" ld xde, (xsp+4)");	// get ptr off stack
   __asm(" ld xhl3, xde");	// pass in this register

   // do the system call
   __asm(" ldf 3");
   __asm(" add w,w");
   __asm(" add w,w");
   __asm(" ld xix, 0xfffe00");
   __asm(" ld xix,(xix+w)");
   __asm(" call xix");
}

void SetWake(ALARM * pAlarm)
{
// sets system to wakeup if powered off
// pass in a pointer to a alarm structure

   __asm(" ld rw3, 11");	// VECT_ALARMDOWNSET
   __asm(" ld xiy, (xsp+4)");	// get ptr
   __asm(" ldf 3");		// params go in reg bank 3
                // could probably do a 32 bit copy
                // to xbc but this looks better
   __asm(" ld h, (xiy +)");	// day of month
   __asm(" ld qc, h");
   __asm(" ld b, (xiy +)");	// hour
   __asm(" ld c, (xiy +)");	// min

   // do the system call
   __asm(" add w,w");
   __asm(" add w,w");
   __asm(" ld xix,0xfffe00");
   __asm(" ld xix,(xix+w)");
   __asm(" call xix");

   // returns success in A3 someday
//   __asm(" ld (xiy), a");
}

void SetAlarm(ALARM * pAlarm)
{
// sets an alarm while powered up
// I don't see much use in this one, but...
// need to setup interrupt first to catch this one
// and do something with it
// pass in a pointer to a alarm structure

   __asm(" ld rw3, 9");		// VECT_ALARMSET
   __asm(" ld xiy, (xsp+4)");	// get ptr
   __asm(" ldf 3");		// params go in reg bank 3
                // could probably do a 32 bit copy
                // to xbc but this looks better
   __asm(" ld h, (xiy +)");	// day of month
   __asm(" ld qc, h");
   __asm(" ld b, (xiy +)");	// hour
   __asm(" ld c, (xiy +)");	// min

   // do the system call
   __asm(" add w,w");
   __asm(" add w,w");
   __asm(" ld xix,0xfffe00");
   __asm(" ld xix,(xix+w)");
   __asm(" call xix");

// return success in A3 someday
//   __asm(" ld (xiy+3), a3");
}


void CpuSpeed(u8 spd)
{
// changes system clock speed
// pass 0 (full) to 4 (slowest)


   __asm("ld rw3, 1");		// VECT_CLOCKGEARSET
   __asm("ld xde, (xsp+4)");	// get spd off stack
   __asm("ld b, e");
   __asm("ld c, 0");		// no speedup on joypad

   // do the system call
   __asm(" ldf 3");
   __asm(" add w,w");
   __asm(" add w,w");
   __asm(" ld xix,0xfffe00");
   __asm(" ld xix,(xix+w)");
   __asm(" call xix");
};

void ResumeOff(void)
{
// turns off system after setting resume bit
// this skips the intro and can be detected to
// resume the game where it left off if programmed
// properly

   USR_ANSWER |= 128;
   SysShutdown();
};

//////////////////////////////////////////////////////////////////////////////
// Bitmap Mode support - Chris Ahchay
//////////////////////////////////////////////////////////////////////////////

//Bit of a rethink here. This is a vanilla bitmap function.
//It works, up to a point, but there are a ton of problems with it
//for real world use:
//1, You can only create a single bitmap.
//2. It's slow (probably not much I can do about that)
//3. Blt'ing bits is painful.
//So...
//Retool this to...

//Pass a memory address into the functions. This replaces the top/left
//co-ordinates.
//Everything works relative to this address, rather than fixed to BitmapTileBase
//Write a third function (or, more accurately, redo CopySingleTile()) to blt
//one bitmap into another (i.e. move from an array to the screen)

//The bitmap passed must be big enough to store (Width*Height)+8) elements
//defined as (to store a 160 by 96 size bitmap (160/8)*(96/8)=240 for the
//bitmap tiles and eight elements for the bitmap properties.
//u16 Bitmap[241][8]
//or
//u16 Bitmap[1928]
//Doesn't matter which, but the first is the easiest to use IMHO...

//Bitmap masks
const unsigned short BitmapMask[8] = {0x3fff, 0xcfff, 0xf3ff, 0xfcff, 0xff3f, 0xffcf, 0xfff3, 0xfffc};

const unsigned short BitmapNewMask[4][8] =
{
   {0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000},
   {0x4000, 0x1000, 0x0400, 0x0100, 0x0040, 0x0010, 0x0004, 0x0001},
   {0x8000, 0x2000, 0x0800, 0x0200, 0x0080, 0x0020, 0x0008, 0x0002},
   {0xc000, 0x3000, 0x0c00, 0x0300, 0x00c0, 0x0030, 0x000c, 0x0003}
};

const unsigned short OffsetValues[8]={0x1, 0x4, 0x10, 0x40, 0x100, 0x400, 0x1000, 0x4000};


void CreateBitmap(u16 * BitmapAddress, u8 Width, u8 Height)
{
	u8 iLoopX;
	u8 iLoopY;
	u16 * MyAddress= BitmapAddress;
	u8 iFillLoop;

	//Create an array of tiles for a bitmap operation thang

	//Save the bitmap dimensions for later
	//Use the *first* element of the bitmap array to store the properties of the
	//bitmap.
	MyAddress[0]=Width;
	MyAddress[1]=Height;
	//Can put other stuff in here
	//MyAddress[2]=BackgroundColour
	//MyAddress[3]=Colour 1
	//MyAddress[4]=Colour 2
	//MyAddress[5]=Colour 3
	//MyAddress[6]=X Position
	//MyAddress[7]=Y Position
	MyAddress+=8;

	for (iLoopX=0;iLoopX<(Width/8);iLoopX++)
	{
		for (iLoopY=0;iLoopY<(Height/8);iLoopY++)
		{
			//Copies a blank tile into the bitmap
			for (iFillLoop=0;iFillLoop<8;iFillLoop++)
			{
				MyAddress++[0]=0;
			}

		}
	}
}

void SetPixel(u16 * BitmapAddress, s16 xPosition, s16 yPosition, u16 Colour)
{
	//Set the selected pixel in the bitmap
	u16 CurrentTile;
	//u16 TileRow;
	u16 * TileRam = BitmapAddress+8;

	s16 xbounded;
	s16 ybounded;

	// forces the pixel to draw inside the bounds of the bitmap area.
	xbounded=xPosition;
	if (xbounded<=0) xbounded=0;
	if (xbounded>=BitmapAddress[0]) xbounded=BitmapAddress[0]-1;

	ybounded=yPosition;
	if (ybounded<=0) ybounded=0;
	if (ybounded>=BitmapAddress[1]) ybounded=BitmapAddress[1]-1;

	//Fist of all, identify the tile that the specified bitmap lives inside...
	//This can be derived from xPosition and yPosition

	CurrentTile=(xbounded/8)+((ybounded/8)*(BitmapAddress[0]/8));
	//For now, copy our "rubbish" tile into the tile space.
	//This is just to prove that I'm in the right general area...
	//CopySingleTile(RubbishTile, CustomTileBase+CurrentTile);

	//Having got the tile number. We now need to calculate the *new* tile
	//to put into here.

	//Basic method.
	//Get the existing tile into an unsigned short [8] Array
	//I don't need the whole tile. Just the row (yPosition % 8)
	//Grab a pointer to the relevant tile.
	//TileRam += (BitmapTileBase * 8);
	TileRam += (CurrentTile * 8)+(ybounded%8);
	//& Then... Grab the value held in this address...
	//TileRow = *TileRam;

	//Do clever stuff with AND's and OR's with the existing tile and the BitmapMask
	//array(s?)
	//Yer basic method;
	//AND MaskRow[xPosition%8] TileRow
	//OR ColourRow[xPosition%8] TileRow
	//Where MaskRow & ColourRow are 16bit jobbies showing a "holey" bitmap and a "coloured" bitmap
	//respectively. (Load the values in bitmap.h into neotile to see what I mean)

	//Updated to work directly on the tileram. Slightly quicker...
	TileRam[0]=TileRam[0] & BitmapMask[xbounded%8];
	TileRam[0]=TileRam[0] | BitmapNewMask[Colour][xbounded%8];

	//Put the tile back...
	//TileRam[0] = TileRow;

}

void QuickSetPixel(u16 * BitmapAddress, u16 xPosition, u16 yPosition, u16 Colour)
{
	//To be used when we can guarantee that only a single pixel in a tile row is affected.
	//Set the selected pixel in the bitmap
	u16 CurrentTile;
	//u16 TileRow;
	u16 * TileRam = BitmapAddress+8;

	//Trouble is. This needs to know the width and height of the bitmap (well, probably not the height)

	//Fist of all, identify the tile that the specified bitmap lives inside...
	//This can be derived from xPosition and yPosition

	CurrentTile=(xPosition/8)+((yPosition/8)*(BitmapAddress[0]/8));
	//For now, copy our "rubbish" tile into the tile space.
	//This is just to prove that I'm in the right general area...
	//CopySingleTile(RubbishTile, CustomTileBase+CurrentTile);

	//Having got the tile number. We now need to calculate the *new* tile
	//to put into here.

	//Basic method.
	//Get the existing tile into an unsigned short [8] Array
	//I don't need the whole tile. Just the row (yPosition % 8)
	//Grab a pointer to the relevant tile.
	//TileRam += (BitmapTileBase * 8);
	TileRam += (CurrentTile * 8)+(yPosition%8);
	//& Then... Grab the value held in this address...
	//TileRow = *TileRam;


	//Do clever stuff with AND's and OR's with the existing tile and the BitmapMask
	//array(s?)
	//Yer basic method;
	//AND MaskRow[xPosition%8] TileRow
	//OR ColourRow[xPosition%8] TileRow
	//Where MaskRow & ColourRow are 16bit jobbies showing a "holey" bitmap and a "coloured" bitmap
	//respectively. (Load the values in bitmap.h into neotile to see what I mean)

	//Updated to work directly on the tileram. Slightly quicker...
	//TileRam[0]=TileRam[0] & BitmapMask[xPosition%8];
	TileRam[0]=TileRam[0] = BitmapNewMask[Colour][xPosition%8];

	//Put the tile back...
	//TileRam[0] = TileRow;

}

//Don't worry about GetPixel() yet...
/*
u8 GetPixel(u16 xPosition, u16 yPosition)
{
	//Get the selected pixel in the bitmap

	//Sort of based on SetPixel() Let's get that working first..
	u16 CurrentTile;
	u16 TileRow;
	u16 * TileRam = TILE_RAM;
	u8 iReturn;
	u8 iOffset;
	u16 lHighBit;
	u16 lLowBit;
	//Fist of all, identify the tile that the specified bitmap lives inside...
	//This can be derived from xPosition and yPosition

	CurrentTile=(xPosition/8)+((yPosition/8)*(CurrentBitmapWidth/8));

	//Basic method.
	//Get the existing tile into an unsigned short [8] Array
	//I don't need the whole tile. Just the row (yPosition % 8)
	TileRam += (BitmapTileBase * 8);
	TileRam += (CurrentTile * 8)+(yPosition%8);

	//Fetch the actual value of the tile row...
	TileRow = *TileRam;

	//Work out which *bits* are used for this pixel
	iOffset=(7-(xPosition%8));
	lLowBit = OffsetValues[iOffset];
	lHighBit = (lLowBit * 2);
	//Right then. Are any of these set?
	iReturn=0;
	if ((TileRow & (lHighBit+lLowBit))==(lHighBit+lLowBit))
		iReturn=3;
	else if ((TileRow & (lHighBit))==(lHighBit))
		iReturn=2;
	else if ((TileRow & (lLowBit))==(lLowBit))
		iReturn=1;

	//Send something back...
	return iReturn;
}
*/

//Try it this way...
//Blts a single "bitmap" tile into TILE_RAM (one way?)
//Could we use CopySpriteTile()? Looks pretty similar to me.
void PutBitmapTile(u16 * BitmapAddress, u16 InputTile, u16 OutputTile)
{
	//A generic loop variable
	u16 i;
	//TileRam is a pointer to the beginning of tile memory
	u16 * TileRam = TILE_RAM;
	//theTiles is a pointer to the beginning of our Tiles array.
	//Copy a single tile into the specified tile number
	u16 * theTiles = (u16 *)BitmapAddress;

	TileRam += (OutputTile * 8);
	theTiles += (InputTile * 8);

	for (i = 0; i < 8; i ++)
	{
		//Copy the tile data
		TileRam[i] = *theTiles++;
	}
}

void CopyBitmap(u16 * BitmapAddress, u16 TileNumber)
{
	u8 iLoopX;
	u8 iLoopY;
	u8 TileWidth;
	u8 TileHeight;
	u16 * MyAddress= BitmapAddress+8;

	TileWidth=BitmapAddress[0]/8;
	TileHeight=BitmapAddress[1]/8;
	//Copy the bitmap to the tile ram.
	//Does not put it on the screen.
	for (iLoopX=0;iLoopX<TileWidth;iLoopX++)
	{
		for (iLoopY=0;iLoopY<TileHeight;iLoopY++)
		{
			PutBitmapTile((u16 *)MyAddress, (iLoopY*TileWidth)+iLoopX, TileNumber+(iLoopY*TileWidth)+iLoopX);
		}
	}
}

void DrawBitmap(u16 * BitmapAddress, u8 xPosition, u8 yPosition, u16 TileNumber)
{
	u8 iLoopX;
	u8 iLoopY;
	u8 TileWidth;
	u8 TileHeight;

	TileWidth=BitmapAddress[0]>>3;
	TileHeight=BitmapAddress[0]>>3;

	//Copy the bitmap tiles into tile memory and blatt them onto the screen.
	for (iLoopX=0;iLoopX<TileWidth;iLoopX++)
	{
		for (iLoopY=0;iLoopY<TileHeight;iLoopY++)
		{
			PutTile(SCR_1_PLANE, 0, iLoopX+xPosition, iLoopY+yPosition, TileNumber+(iLoopY*TileWidth)+iLoopX);
		}
	}

}
void DrawLine(u16 * BitmapAddress, u8 x1, u8 y1, u8 x2, u8 y2, u8 Colour)
{
	//Stolen wholesale from gamedev...
	u8 xinc1;
	u8 xinc2;
	u8 yinc1;
	u8 yinc2;
	u8 den;
	u8 num;
	u8 numadd;
	u8 numpixels;
	u8 curpixel;
	u8 deltax;
	u8 deltay;
	u8 x;
	u8 y;

	//There appears to be an absence of an abs() function.
	if (x2==x1)
	{
		DrawLineVertical(BitmapAddress, x1, y1, y2, Colour);
	}
	else
	{
		if (y2==y1)
		{
			DrawLineHorizontal(BitmapAddress, x1, x2, y1, Colour);
		}
		else
		{
			if (x2>x1)
				deltax = x2 - x1;        // The difference between the x's
			else
				deltax = x1 - x2;
			if (y2>y1)
				deltay = y2 - y1;        // The difference between the y's
			else
				deltay = y1 - y2;

			x = x1;                       // Start x off at the first pixel
			y = y1;                       // Start y off at the first pixel

			if (x2 >= x1)                 // The x-values are increasing
			{
				xinc1 = 1;
				xinc2 = 1;
			}
			else                          // The x-values are decreasing
			{
				xinc1 = -1;
				xinc2 = -1;
			}

			if (y2 >= y1)                 // The y-values are increasing
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
				SetPixel((u16 *)BitmapAddress, x, y, Colour);             // Draw the current pixel
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
		}
	}
}


//Two specific cases for vertical and horizontal lines
void DrawLineVertical(u16 * BitmapAddress, u16 x, u16 y1, u16 y2, u16 Colour)
{
	u16 y;
	s8 yinc;

	if (y1>y2)
		yinc=-1;
	else
		yinc=1;

	for (y=y1;y!=y2;y+=yinc)
	{
		SetPixel((u16 *)BitmapAddress, x, y, Colour);
	}
}


void DrawLineHorizontal(u16 * BitmapAddress, u16 x1, u16 x2, u16 y, u16 Colour)
{
	u16 x;
	s8 xinc;

	if (x1>x2)
		xinc=-1;
	else
		xinc=1;

	for (x=x1;x!=x2;x+=xinc)
	{
		SetPixel((u16 *)BitmapAddress, x, y, Colour);
	}
}

// Helper function to streamline copying animation frames between a buffer and the tile memory

// copies a new animation frame into tile memory.
void CopyAnimationFrame(const u16 AnimTiles[][8], u16 TileNumber, u16 TileCount, u16 AnimNumber)
{
	u8 i;
	u16 * TileRam = TILE_RAM;
	//Our input parameter. NOT a global constant
	u16 * theAnim = (u16 *) AnimTiles;
	theAnim += (TileCount * 8);

	TileRam += (TileNumber * 8);
	theAnim += (AnimNumber * 8);
	theAnim-=8;

	// Copy the specified tiles into tile ram
	for (i = 0; i < (TileCount * 8); i ++)
	{
		TileRam[i] = *theAnim++;
	}
}
