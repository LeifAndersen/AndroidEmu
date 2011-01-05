#define LOG_TAG "libgens"
#include <utils/Log.h>
#include "emuengine.h"
#include "drmd_main.h"
#include "app.h"

#define SCREEN_W		320
#define SCREEN_H		240
#define SCREEN_PITCH	320

#define GAMEPAD_A		0x0040
#define GAMEPAD_B		0x0010
#define GAMEPAD_C		0x0020
#define GAMEPAD_X		0x0400
#define GAMEPAD_Y		0x0200
#define GAMEPAD_Z		0x0100
#define GAMEPAD_XYZ		(GAMEPAD_X | GAMEPAD_Y | GAMEPAD_Z)

extern "C" void Blit8To16Asm(void *src, void *dst, void *pal, int width);
extern "C" void Blit8To16RevAsm(void *src, void *dst, void *pal, int width);

static EmuEngine *engine;
static EmuEngine::Callbacks *callbacks;

class GensEngine : public EmuEngine {
public:
	GensEngine();
	virtual ~GensEngine();

	virtual bool initialize(Callbacks *cbs);
	virtual void destroy();
	virtual void reset();
	virtual void power();
	virtual void fireLightGun(int x, int y);
	virtual Game *loadRom(const char *file);
	virtual void unloadRom();
	virtual void renderFrame(const Surface &surface);
	virtual bool saveState(const char *file);
	virtual bool loadState(const char *file);
	virtual void runFrame(unsigned int keys, bool skip);
	virtual void setOption(const char *name, const char *value);
};


GensEngine::GensEngine()
{
	engine = this;
}

GensEngine::~GensEngine()
{
	drmdCleanup();
	engine = NULL;
}

bool GensEngine::initialize(EmuEngine::Callbacks *cbs)
{
	callbacks = cbs;

	return drmdInitialize();
}

void GensEngine::destroy()
{
	delete this;
}

void GensEngine::reset()
{
	drmdReset();
}

void GensEngine::power()
{
	drmdPower();
}

void GensEngine::fireLightGun(int x, int y)
{
}

GensEngine::Game *GensEngine::loadRom(const char *file)
{
	if (!drmdLoadRom(file))
		return NULL;

	extern int frame_limit;

	static Game game;
	game.videoWidth = SCREEN_W;
	game.videoHeight = SCREEN_H;
	if (!PAL)
		game.videoHeight -= 16;

	game.soundRate = 22050;
	game.soundBits = 16;
	game.soundChannels = 2;
	game.fps = frame_limit;
	return &game;
}

void GensEngine::unloadRom()
{
	drmdUnloadRom();
}

void GensEngine::renderFrame(const Surface &surface)
{
	extern unsigned char VBuf[];
	extern unsigned int VPalette[];

	void (*blt)(void*, void*, void*, int) = Blit8To16Asm;
	unsigned char *d = (unsigned char *) surface.bits;
	unsigned char *s = VBuf;
	int h = SCREEN_H;
	if (!PAL) {
		s += SCREEN_PITCH * 8;
		h -= 16;
	}
	if (surface.bpr < 0) {
		blt = Blit8To16RevAsm;
		d += (h - 1) * -surface.bpr + SCREEN_W * 2;
	}
	while (--h >= 0) {
		blt(s, d, VPalette, SCREEN_W);
		d += surface.bpr;
		s += SCREEN_PITCH;
	}
}

bool GensEngine::saveState(const char *file)
{
	drmdSaveState(file);
	return true;
}

bool GensEngine::loadState(const char *file)
{
	drmdLoadState(file);
	return true;
}

static void videoUpdate()
{
	EmuEngine::Surface surface;
	if (callbacks->lockSurface(&surface)) {
		engine->renderFrame(surface);
		callbacks->unlockSurface(&surface);
	}
}

inline unsigned int rapidFire(unsigned int keys)
{
	if (keys & GAMEPAD_XYZ) {
		if (keys & GAMEPAD_X)
			keys |= GAMEPAD_A;
		if (keys & GAMEPAD_Y)
			keys |= GAMEPAD_B;
		if (keys & GAMEPAD_Z)
			keys |= GAMEPAD_C;
		keys &= ~GAMEPAD_XYZ;
	}
	return keys;
}

void GensEngine::runFrame(unsigned int keys, bool skip)
{
	static int turbo = 0;
	turbo ^= 1;
	if (turbo) {
		if (!drmd.pad_1_type)
 			keys = rapidFire(keys);
		if (!drmd.pad_2_type)
			keys |= (rapidFire(keys >> 16) << 16);
	}
	drmd.pad = keys;

	if (skip) {
		drmdRunFrame(0);
	} else {
		drmdRunFrame(1);
		videoUpdate();
	}

	extern short soundbuffer[];
	extern unsigned int sound_buffer_size;
	if (sound_on)
		callbacks->playAudio(soundbuffer, sound_buffer_size * 4);
}

void GensEngine::setOption(const char *name, const char *value)
{
	if (strcmp(name, "sixButtonPad") == 0)
		drmdSetPadType(strcmp(value, "false") != 0);
	else if (strcmp(name, "soundEnabled") == 0)
		drmdSetSound(strcmp(value, "false") != 0);
	else if (strcmp(name, "enableSRAM") == 0)
		drmdEnableSRAM(strcmp(value, "true") == 0);
}

extern "C" __attribute__((visibility("default")))
void *createObject()
{
	return new GensEngine;
}
