#ifndef GBC_MAIN_H
#define GBC_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

int gbcInitialize(int rate);
void gbcCleanup();
int gbcLoadRom(const char *file);
int gbcUnloadRom();
void gbcReset();
void gbcHandleInput(int keys);
void gbcRunFrame(int render_video);
int gbcSaveState(const char *file);
int gbcLoadState(const char *file);

void gbcEnableSRAM(int enable);

void sound_mix(short *sndbuf, int len);

#ifdef __cplusplus
}
#endif
#endif
