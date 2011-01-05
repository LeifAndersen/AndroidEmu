#ifndef DRMD_MAIN_H
#define DRMD_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

int drsmsInitialize();
void drsmsCleanup();
void drsmsSetPadType(int type);
void drsmsSetSound(int level);
void drsmsReset();
int drsmsLoadRom(const char *filename);
void drsmsUnloadRom();
void drsmsRunFrame(int render_video, unsigned int pad);

int drsmsSaveState(const char *filename);
int drsmsLoadState(const char *filename);

#ifdef __cplusplus
}
#endif
#endif
