#ifndef FRAMEBUFFER_ANDROID_HXX
#define FRAMEBUFFER_ANDROID_HXX

#include "FrameBuffer.hxx"
#include "emuengine.h"

class FrameBufferAndroid : public FrameBuffer {
public:
	FrameBufferAndroid(OSystem* osystem);
    virtual ~FrameBufferAndroid();

	virtual void enablePhosphor(bool enable, int blend);
	virtual Uint32 mapRGB(Uint8 r, Uint8 g, Uint8 b) const;
    virtual BufferType type() const;

    virtual bool initSubsystem(VideoMode& mode);
    virtual void invalidate();

    virtual void drawTIA(bool full);
    virtual void postFrameUpdate();

	void renderFrame(const EmuEngine::Surface &surface);
};

#endif

