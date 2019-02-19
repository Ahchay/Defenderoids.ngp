#include "ngpc.h"
#include "carthdr.h" // TODO: edit game name in carthdr.h
#include "library.h"
#include "Defenderoids.h"

void main()
{
   InitNGPC();
   SysSetSystemFont();

   ClearScreen(SCR_1_PLANE);
   ClearScreen(SCR_2_PLANE);
   SetBackgroundColour(RGB(0, 0, 0));

   SetPalette(SCR_1_PLANE, 0, 0, 0, 0, RGB(15,15,15));
   PrintString(SCR_1_PLANE, 0, 4, 9, "DEFENDROIDS");
   PrintString(SCR_1_PLANE, 0, 1, 12, "Neither Defender");
   PrintString(SCR_1_PLANE, 0, 2, 12, "nor Asteroids");

   DefenderoidsMain();
}


