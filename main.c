#include "ngpc.h"
#include "carthdr.h" // TODO: edit game name in carthdr.h
#include "library.h"
#include "Tiles\Alphabet.c"
#include "vgmplayer.h"

void __interrupt myVBL()
{
    WATCHDOG = WATCHDOG_CLEAR;
    if (USR_SHUTDOWN) { SysShutdown(); while (1); }
    VGM_SendData();
}

void main()
{
	InitNGPC();
	SysSetSystemFont();

	VGM_InstallSoundDriver();

	__asm("di");
	VBL_INT = myVBL;
	__asm("ei");

	InstallTileSetAt(Alphabet,sizeof(Alphabet),0);

	SetBackgroundColour(RGB(0,0,4));

	InitialiseQRandom();

	SetPalette(SCR_1_PLANE, 0, 0, RGB(15,15,15), RGB(0,0,15), RGB(15,0,0));

	while(1)
	{
		u8 iGameMode;
		iGameMode=DefenderoidsLogo();
		switch(iGameMode) {
			case 1:
				DefenderoidsMain();
				break;
			case 2:
				DefenderoidsTest();
				break;
			default:
				break;
		}
	}
}


