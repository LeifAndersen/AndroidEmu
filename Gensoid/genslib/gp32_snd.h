#ifdef __cplusplus
extern "C" {
#endif

#ifndef GP32_SND_H
#define GP32_SND_H

extern short *pOutput[];
extern volatile unsigned char nFlip;
extern volatile unsigned char curseg;
extern volatile int Timer;
extern short snd_enabled;
unsigned int frame_buffer_size;
void Gp32_AudioStop (void);
int Gp32_AudioStart (int nReverse);
void Gp32_AudioPause (char doPause);

#endif /* GP32_SND_H */

#ifdef __cplusplus
} // End of extern "C"
#endif
