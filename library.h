/*****************************************************************
 *
 * C Library of useful, generic NGPC routines
 *
 *****************************************************************/
#ifndef _LIBRARY_H
#define _LIBRARY_H

/*
 * Function Prototypes contained within the library
 */
void InitNGPC(void);
void InstallTileSet(const unsigned short Tiles[][8], u16 Len);
void InstallTileSetAt(const u16 Tiles[][8], u16 Len, u16 Offset);

void ClearScreen(u8 ScrollPlane);
void SetBackgroundColour(u16 Col);
void SysSetSystemFont(void);
void SetPalette(u8 ScrollPlane, u8 PaletteNo, u16 Col0, u16 Col1, u16 Col2, u16 Col3);
void Sleep(u8 VBLanks);

void GetTile(u8 ScrollPlane, u8 *PaletteNo, u8 XPos, u8 YPos, u16 *TileNo);
void PutTile(u8 ScrollPlane, u8 PaletteNo, u8 XPos, u8 YPos, u16 TileNo);

void PrintDecimal(u8 Plane, u8 PaletteNo, u8 x, u8 y, u16 Value, u8 Len);
void PrintHex(u8 Plane, u8 PaletteNo, u8 x, u8 y, u16 Value, u8 Len);
void PrintString(u8 Plane, u8 Palette, u8 XPos, u8 YPos, const char * theString);
void PrintBinary(u8 Plane, u8 PaletteNo, u8 x, u8 y, u16 Value, u8 Len);

void SetSprite(u8 SpriteNo, u16 TileNo, u8 Chain, u8 XPos, u8 YPos, u8 PaletteNo);
void SetSpritePalette(u8 SpriteNo, u8 PaletteNo);
void SetSpritePosition(u8 SpriteNo, u8 XPos, u8 YPos);
void SetSpriteOptions(u8 SpriteNo, bool HorizontalFlip, bool VerticalFlip, bool Chain);

void SeedRandom(void);
u16 GetRandom(u16 Value);
void InitialiseQRandom();
unsigned char QRandom();

signed char Sin(unsigned char dAngle);
signed char Cos(unsigned char dAngle);

u16 WrapDistance(u16 First, u16 Second, u16 Wrap);

typedef struct tagTIME
{
   u8 Year;
   u8 Month;
   u8 Day;
   u8 Hour;
   u8 Minute;
   u8 Second;
   u8 LeapYear:4;
   u8 Weekday:4;
} TIME;

void GetTime(TIME * pTime);
extern const char daynames[7][10];

typedef struct tagALARM
{
   u8 Day;
   u8 Hour;
   u8 Min;
   u8 Code;
} ALARM;

void SetWake(ALARM * pAlarm);
void SetAlarm(ALARM * pAlarm);

void CpuSpeed(u8 spd);
void ResumeOff(void);

// bitmap mode support//Function declarations
void CreateBitmap(u16 * BitmapAddress, u8 Width, u8 Height);
void SetPixel(u16 * BitmapAddress, s16 xPosition, s16 yPosition, u16 Colour);
//u8 GetPixel(u16 xPosition, u16 yPosition);
//void CopySingleTile(u8 TileGraphic, u16 TileNumber);
void PutBitmapTile(u16 * BitmapAddress, u16 InputTile, u16 OutputTile);
void DrawLine(u16 * BitmapAddress, u8 x1, u8 y1, u8 x2, u8 y2, u8 Colour);
void DrawLineVertical(u16 * BitmapAddress, u16 x, u16 y1, u16 y2, u16 Colour);
void DrawLineHorizontal(u16 * BitmapAddress, u16 x1, u16 x2, u16 y, u16 Colour);

// Helper function to streamline copying animation frames between a buffer and the tile memory
// copies a new animation frame into tile memory.
void CopyAnimationFrame(const u16 AnimTiles[][8], u16 TileNumber, u16 TileCount, u16 AnimNumber);

/*
 * Defines used by the library
 */
#define SCR_1_PLANE	(1)
#define SCR_2_PLANE	(2)
#define SPRITE_PLANE (3)

/*
 * Public variables
 */
extern volatile u8 VBCounter;

#define true 1
#define false 0

#endif

