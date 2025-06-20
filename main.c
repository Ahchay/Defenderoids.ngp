#include "ngpc.h"
#include "carthdr.h"
#include "library.h"
#include "Tiles\Alphabet.c"
#include "vgmplayer.h"

void main()
{
	InitNGPC();
	SysSetSystemFont();

	VGM_InstallSoundDriver();

	InstallTileSetAt(Alphabet,sizeof(Alphabet),0);

	SetBackgroundColour(RGB(0,0,0));

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
			default:
				break;
		}
	}
}


