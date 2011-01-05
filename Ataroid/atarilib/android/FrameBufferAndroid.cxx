#define LOG_TAG "libatari"
#include <utils/Log.h>
#include "FrameBufferAndroid.hxx"
#include "OSystem.hxx"
#include "Console.hxx"
#include "TIA.hxx"

FrameBufferAndroid::FrameBufferAndroid(OSystem* osystem)
		: FrameBuffer(osystem)
{
}

FrameBufferAndroid::~FrameBufferAndroid()
{
}

void FrameBufferAndroid::enablePhosphor(bool enable, int blend)
{
}

BufferType FrameBufferAndroid::type() const
{
	return kSoftBuffer;
}

Uint32 FrameBufferAndroid::mapRGB(Uint8 r, Uint8 g, Uint8 b) const
{
	return ((r & 0xf8) << 8) | ((g & 0xfc) << 3) | (b >> 3);
}

bool FrameBufferAndroid::initSubsystem(VideoMode& mode)
{
	return true;
}

void FrameBufferAndroid::invalidate()
{
}

extern "C" void Blit8To16Asm(void *src, void *dst, void *pal, int width);
extern "C" void Blit8To16RevAsm(void *src, void *dst, void *pal, int width);

void FrameBufferAndroid::renderFrame(const EmuEngine::Surface &surface)
{
	const TIA& tia = myOSystem->console().tia();
	int w = tia.width();
	int h = tia.height();

	uInt8 *VBuf = tia.currentFrameBuffer();
	uInt16 *dst = (uInt16 *) surface.bits;

	void (*blt)(void*, void*, void*, int) = Blit8To16Asm;
	unsigned char *d = (unsigned char *) surface.bits;
	unsigned char *s = VBuf;

	if (surface.bpr < 0) {
		blt = Blit8To16RevAsm;
		d += (h - 1) * -surface.bpr + w * 2;
	}
	while (--h >= 0) {
		blt(s, d, myDefPalette, w);
		d += surface.bpr;
		s += w;
	}
}

void FrameBufferAndroid::drawTIA(bool fullRedraw)
{
	extern EmuEngine::Callbacks *callbacks;

	EmuEngine::Surface surface;
	if (callbacks->lockSurface(&surface)) {
		renderFrame(surface);
		callbacks->unlockSurface(&surface);
	}
}

void FrameBufferAndroid::postFrameUpdate()
{
}
