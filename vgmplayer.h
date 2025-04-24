//VGMlib for NGPC
//NGPC C library for z80 driver that supports 
//playback of VGM files for background music and sound effects
//coded by winteriscoming
//special thanks to sodthor, mic_, and Ivan Mackintosh

#ifndef _VGMPLAYER
#define _VGMPLAYER

#define VGM_PULSE                     (*(u8*)0x00BC)
#define VGM_DATA_BUFFER               (u8*)0x74D0
#define VGM_DATA_BUFFER_1ST_BYTE      (*(u8*)0x74D0)
#define VGM_DATA_BUFFER_SFX           (u8*)0x7550
#define VGM_DATA_BUFFER_SFX_1ST_BYTE  (*(u8*)0x7550)

void waitTime(u16 inputTimer);

void VGM_SendData();

void VGM_PlaySFX(u8*SFX_Data, u8 givePriority);

void VGM_PlayBGM();

void VGM_PlayBGM_X_Times(u8*BGM_Data, u32 loop_point, u8 num);

void VGM_PlayBGM_Loop(u8*BGM_Data, u32 loop_point);

void VGM_StopBGM();

void VGM_ResumeBGM();

void VGM_InstallSoundDriver(void);

void VGM_RestoreSoundDriver(void);

#endif