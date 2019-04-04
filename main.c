#include "ngpc.h"
#include "carthdr.h" // TODO: edit game name in carthdr.h
#include "library.h"

void main()
{
	InitNGPC();
	SysSetSystemFont();

	SetBackgroundColour(RGB(0,0,4));

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

	while(1)
	{

		while(DefenderoidsLogo())
		{
			DefenderoidsMain();
		}
	}
}


