#define LOG_TAG "libatari"
#include <utils/Log.h>
#include "emuengine.h"

#include "FrameBufferAndroid.hxx"
#include "SoundAndroid.hxx"
#include "OSystem.hxx"
#include "OSystemUNIX.hxx"
#include "SettingsUNIX.hxx"
#include "StateManager.hxx"
#include "Console.hxx"
#include "TIA.hxx"

static EmuEngine *engine;
EmuEngine::Callbacks *callbacks;

class AtariEngine : public EmuEngine {
public:
	AtariEngine();
	virtual ~AtariEngine();

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
	OSystem *theOSystem;
	Settings *theSettings;
	int switches;
	bool soundEnabled;
	int soundBufSize;
};


AtariEngine::AtariEngine()
		: switches(0),
		  soundEnabled(false),
		  soundBufSize(0)
{
	engine = this;

	theOSystem = new OSystemUNIX();
	theSettings = new SettingsUNIX(theOSystem);
}

AtariEngine::~AtariEngine()
{
	delete theOSystem;
	delete theSettings;

	engine = NULL;
}

bool AtariEngine::initialize(EmuEngine::Callbacks *cbs)
{
	callbacks = cbs;

	theOSystem->settings().validate();
	return theOSystem->create();
}

void AtariEngine::destroy()
{
	delete this;
}

void AtariEngine::reset()
{
}

void AtariEngine::power()
{
}

void AtariEngine::fireLightGun(int x, int y)
{
}

AtariEngine::Game *AtariEngine::loadRom(const char *file)
{
	if (!theOSystem->createConsole(file))
		return NULL;

	const TIA &tia = theOSystem->console().tia();

	static Game game;
	game.videoWidth = tia.width();
	game.videoHeight = tia.height();
	game.soundRate = 22050;
	game.soundBits = 8;
	game.soundChannels = 1;
	game.fps = (int) theOSystem->frameRate();

	soundBufSize = game.soundRate / game.fps;
	return &game;
}

void AtariEngine::unloadRom()
{
	theOSystem->deleteConsole();
}

void AtariEngine::renderFrame(const Surface &surface)
{
	FrameBufferAndroid &frameBuffer =
			(FrameBufferAndroid &) theOSystem->frameBuffer();
	frameBuffer.renderFrame(surface);
}

bool AtariEngine::saveState(const char *file)
{
	Serializer out(file);
	return theOSystem->state().saveState(out);
}

bool AtariEngine::loadState(const char *file)
{
	Serializer in(file, true);
	return theOSystem->state().loadState(in);
}

bool AtariEngine::addCheat(const char *code)
{
	return false;
}

void AtariEngine::removeCheat(const char *code)
{
}

void AtariEngine::runFrame(unsigned int keys, bool skip)
{
	theOSystem->runFrame(keys | switches, skip);
	switches = 0;

	if (soundEnabled) {
		SoundAndroid &sound = 
				(SoundAndroid &) theOSystem->sound();
		Uint8 buffer[4096];
		sound.processFragment(buffer, soundBufSize);

		callbacks->playAudio(buffer, soundBufSize);
	}
}

static int getColorSwitch(const char *value)
{
	if (strcmp(value, "color") == 0)
		return 0x0100;
	if (strcmp(value, "bw") == 0)
		return 0x0200;
	return 0;
}

static int getDifficultySwitch(const char *value)
{
	if (strcmp(value, "A") == 0)
		return 0x1000;
	if (strcmp(value, "B") == 0)
		return 0x2000;
	return 0;
}

void AtariEngine::setOption(const char *name, const char *value)
{
	if (strcmp(name, "soundEnabled") == 0) {
		soundEnabled = (strcmp(value, "true") == 0);

	} else if (strcmp(name, "colorSwitch") == 0) {
		switches |= getColorSwitch(value);
	} else if (strcmp(name, "leftDifficultySwitch") == 0) {
		switches |= getDifficultySwitch(value);
	} else if (strcmp(name, "rightDifficultySwitch") == 0) {
		switches |= (getDifficultySwitch(value) << 2);
	}
}

extern "C" __attribute__((visibility("default")))
void *createObject()
{
	return new AtariEngine;
}
