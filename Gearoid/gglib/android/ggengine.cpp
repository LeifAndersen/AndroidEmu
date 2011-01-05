#define LOG_TAG "libgg"
#include <utils/Log.h>
#include "emuengine.h"
#include "drsms_main.h"
#include "app.h"

#define SMS_SCREEN_W		240
#define SMS_SCREEN_H		192
#define SMS_SCREEN_PITCH	320

#define GG_SCREEN_W			160
#define GG_SCREEN_H			144
#define GG_SCREEN_PITCH		320

extern "C" void Blit8To16Asm(void *src, void *dst, void *pal, int width);
extern "C" void Blit8To16RevAsm(void *src, void *dst, void *pal, int width);

extern unsigned char VBuf[];
extern unsigned int pal_lookup[];

static EmuEngine *engine;
static EmuEngine::Callbacks *callbacks;

class GGEngine : public EmuEngine {
public:
	GGEngine();
	virtual ~GGEngine();

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


GGEngine::GGEngine()
{
	engine = this;
}

GGEngine::~GGEngine()
{
	drsmsCleanup();
	engine = NULL;
}

bool GGEngine::initialize(EmuEngine::Callbacks *cbs)
{
	callbacks = cbs;

	return drsmsInitialize();
}

void GGEngine::destroy()
{
	delete this;
}

void GGEngine::reset()
{
	drsmsReset();
}

void GGEngine::power()
{
	reset();
}

void GGEngine::fireLightGun(int x, int y)
{
}

GGEngine::Game *GGEngine::loadRom(const char *file)
{
	if (!drsmsLoadRom(file))
		return NULL;

	static Game game;

	if (CurrentEmuMode == EMU_MODE_SMS) {
		game.videoWidth = SMS_SCREEN_W;
		game.videoHeight = SMS_SCREEN_H;
	} else {
		game.videoWidth = GG_SCREEN_W;
		game.videoHeight = GG_SCREEN_H;
	}
	game.soundRate = sound_rate;
	game.soundBits = 16;
	game.soundChannels = 2;
	game.fps = frame_limit;
	return &game;
}

void GGEngine::unloadRom()
{
	drsmsUnloadRom();
}

inline void smsRenderFrame(const EmuEngine::Surface &surface)
{
	void (*blt)(void*, void*, void*, int) = Blit8To16Asm;
	unsigned char *d = (unsigned char *) surface.bits;
	unsigned char *s = VBuf;
	int h = SMS_SCREEN_H;

	if (surface.bpr < 0) {
		blt = Blit8To16RevAsm;
		d += (h - 1) * -surface.bpr + SMS_SCREEN_W * 2;
	}
	while (--h >= 0) {
		blt(s, d, pal_lookup, SMS_SCREEN_W);
		d += surface.bpr;
		s += SMS_SCREEN_PITCH;
	}
}

inline void ggRenderFrame(const EmuEngine::Surface &surface)
{
	unsigned char *d = (unsigned char *) surface.bits;
	unsigned char *s = VBuf;
	int h = GG_SCREEN_H;

	if (surface.bpr > 0) {
		while (--h >= 0) {
			memcpy(d, s, GG_SCREEN_PITCH);
			d += surface.bpr;
			s += GG_SCREEN_PITCH;
		}
	} else {
		d += (h - 1) * -surface.bpr + GG_SCREEN_W * 2;
		while (--h >= 0) {
			uint32 *src = (uint32 *) s;
			uint32 *dst = (uint32 *) d;
			for (int w = GG_SCREEN_W / 2; --w >= 0; src++)
				*--dst = (*src << 16) | (*src >> 16);

			d += surface.bpr;
			s += GG_SCREEN_PITCH;
		}
	}
}

void GGEngine::renderFrame(const Surface &surface)
{
	if (CurrentEmuMode == EMU_MODE_SMS)
		smsRenderFrame(surface);
	else
		ggRenderFrame(surface);
}

bool GGEngine::saveState(const char *file)
{
	drsmsSaveState(file);
	return true;
}

bool GGEngine::loadState(const char *file)
{
	drsmsLoadState(file);
	return true;
}

inline void videoUpdate()
{
	EmuEngine::Surface surface;
	if (callbacks->lockSurface(&surface)) {
		engine->renderFrame(surface);
		callbacks->unlockSurface(&surface);
	}
}

void GGEngine::runFrame(unsigned int keys, bool skip)
{
	if (skip) {
		drsmsRunFrame(0, keys);
	} else {
		drsmsRunFrame(1, keys);
		videoUpdate();
	}

	extern short soundbuffer[];
	if (sound_on)
		callbacks->playAudio(soundbuffer, sound_buffer_size * 4);
}

void GGEngine::setOption(const char *name, const char *value)
{
	if (strcmp(name, "soundEnabled") == 0)
		drsmsSetSound(strcmp(value, "true") == 0);
}

extern "C" __attribute__((visibility("default")))
void *createObject()
{
	return new GGEngine;
}
