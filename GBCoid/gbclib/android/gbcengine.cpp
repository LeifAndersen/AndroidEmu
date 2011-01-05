#define LOG_TAG "libgbc"
#include <utils/Log.h>
#include "emuengine.h"
#include "gbc_main.h"
#include "cheats.h"

#define SCREEN_W		160
#define SCREEN_H		144
#define SCREEN_PITCH	320

#define SOUND_RATE		22050

#define GAMEPAD_A			0x0010
#define GAMEPAD_B			0x0020
#define GAMEPAD_A_TURBO		(GAMEPAD_A << 16)
#define GAMEPAD_B_TURBO		(GAMEPAD_B << 16)

static EmuEngine *engine;
static EmuEngine::Callbacks *callbacks;

class GbcEngine : public EmuEngine {
public:
	GbcEngine();
	virtual ~GbcEngine();

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
	virtual bool addCheat(const char *code);
	virtual void removeCheat(const char *code);
	virtual void runFrame(unsigned int keys, bool skip);
	virtual void setOption(const char *name, const char *value);

private:
	bool soundEnabled;
};


GbcEngine::GbcEngine()
		: soundEnabled(false)
{
	engine = this;
}

GbcEngine::~GbcEngine()
{
	gbcCleanup();
	engine = NULL;
}

bool GbcEngine::initialize(EmuEngine::Callbacks *cbs)
{
	callbacks = cbs;

	return gbcInitialize(SOUND_RATE);
}

void GbcEngine::destroy()
{
	delete this;
}

void GbcEngine::reset()
{
	gbcReset();
}

void GbcEngine::power()
{
	reset();
}

void GbcEngine::fireLightGun(int x, int y)
{
}

GbcEngine::Game *GbcEngine::loadRom(const char *file)
{
	if (!gbcLoadRom(file))
		return NULL;

	static Game game;
	game.videoWidth = SCREEN_W;
	game.videoHeight = SCREEN_H;
	game.soundRate = SOUND_RATE;
	game.soundBits = 16;
	game.soundChannels = 2;
	game.fps = 60;
	return &game;
}

void GbcEngine::unloadRom()
{
	gbcUnloadRom();
}

void GbcEngine::renderFrame(const Surface &surface)
{
	extern char vidram[];
	unsigned char *d = (unsigned char *) surface.bits;
	unsigned char *s = (unsigned char *) vidram;
	int h = SCREEN_H;

	if (surface.bpr > 0) {
		while (--h >= 0) {
			memcpy(d, s, SCREEN_PITCH);
			d += surface.bpr;
			s += SCREEN_PITCH;
		}
	} else {
		d += (h - 1) * -surface.bpr + SCREEN_W * 2;
		while (--h >= 0) {
			unsigned int *src = (unsigned int *) s;
			unsigned int *dst = (unsigned int *) d;
			for (int w = SCREEN_W / 2; --w >= 0; src++)
				*--dst = (*src << 16) | (*src >> 16);

			d += surface.bpr;
			s += SCREEN_PITCH;
		}
	}
}

bool GbcEngine::saveState(const char *file)
{
	return gbcSaveState(file);
}

bool GbcEngine::loadState(const char *file)
{
	return gbcLoadState(file);
}

bool GbcEngine::addCheat(const char *code)
{
	return gbAddGsCheat(code, "") || gbAddGgCheat(code, "");
}

void GbcEngine::removeCheat(const char *code)
{
	int i = gbCheatFind(code);
	if (i >= 0)
		gbCheatRemove(i);
}

inline void videoUpdate()
{
	EmuEngine::Surface surface;
	if (callbacks->lockSurface(&surface)) {
		engine->renderFrame(surface);
		callbacks->unlockSurface(&surface);
	}
}

void GbcEngine::runFrame(unsigned int keys, bool skip)
{
	static int turbo = 0;
	if (turbo ^= 1) {
		if (keys & GAMEPAD_A_TURBO)
			keys |= GAMEPAD_A;
		if (keys & GAMEPAD_B_TURBO)
			keys |= GAMEPAD_B;
	}
	gbcHandleInput(keys);

	if (skip) {
		gbcRunFrame(0);
	} else {
		gbcRunFrame(1);
		videoUpdate();
	}

	static short data[(SOUND_RATE * 2 / 60) & ~1];
	sound_mix(data, SOUND_RATE / 60);
	if (soundEnabled)
		callbacks->playAudio(data, sizeof(data));
}

void GbcEngine::setOption(const char *name, const char *value)
{
	if (strcmp(name, "soundEnabled") == 0)
		soundEnabled = (strcmp(value, "true") == 0);
	else if (strcmp(name, "enableSRAM") == 0)
		gbcEnableSRAM(strcmp(value, "true") == 0);
}

extern "C" void die(char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	__android_log_vprint(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ap);
	va_end(ap);
}

extern "C" __attribute__((visibility("default")))
void *createObject()
{
	return new GbcEngine;
}
