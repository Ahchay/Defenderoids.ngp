#include "ngpc.h"
#include "carthdr.h" // TODO: edit game name in carthdr.h
#include "library.h"

void main()
{
   InitNGPC();
   SysSetSystemFont();

   ClearScreen(SCR_1_PLANE);
   ClearScreen(SCR_2_PLANE);
   SetBackgroundColour(RGB(0, 0, 0));
   SetPalette(SCR_1_PLANE, 0, 0, 0, 0, RGB(15,15,15));

   DefenderoidsMain();
}


