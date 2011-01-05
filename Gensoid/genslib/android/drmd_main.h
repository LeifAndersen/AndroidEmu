#ifndef DRMD_MAIN_H
#define DRMD_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

int drmdInitialize();
void drmdCleanup();
void drmdEnableSRAM(int enable);
void drmdSetPadType(int type);
void drmdSetSound(int level);
void drmdPower();
void drmdReset();
int drmdLoadRom(const char *filename);
void drmdUnloadRom();
void drmdRunFrame(int render_video);

int drmdSaveState(const char *filename);
int drmdLoadState(const char *filename);

#ifdef __cplusplus
}
#endif
#endif
