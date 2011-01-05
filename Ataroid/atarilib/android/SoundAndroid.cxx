//============================================================================
//
//   SSSS    tt          lll  lll       
//  SS  SS   tt           ll   ll        
//  SS     tttttt  eeee   ll   ll   aaaa 
//   SSSS    tt   ee  ee  ll   ll      aa
//      SS   tt   eeeeee  ll   ll   aaaaa  --  "An Atari 2600 VCS Emulator"
//  SS  SS   tt   ee      ll   ll  aa  aa
//   SSSS     ttt  eeeee llll llll  aaaaa
//
// Copyright (c) 1995-2010 by Bradford W. Mott, Stephen Anthony
// and the Stella Team
//
// See the file "License.txt" for information on usage and redistribution of
// this file, and for a DISCLAIMER OF ALL WARRANTIES.
//
// $Id: SoundAndroid.cxx 2039 2010-06-03 12:41:49Z stephena $
//============================================================================

#ifdef SOUND_SUPPORT

#define LOG_TAG "libatari"
#include <utils/Log.h>

#include <sstream>
#include <cassert>
#include <cmath>

#include "TIASnd.hxx"
#include "FrameBuffer.hxx"
#include "Settings.hxx"
#include "System.hxx"
#include "OSystem.hxx"
#include "Console.hxx"
#include "SoundAndroid.hxx"

#define SOUND_RATE		22050

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundAndroid::SoundAndroid(OSystem* osystem)
  : Sound(osystem),
    myIsEnabled(osystem->settings().getBool("sound")),
    myIsInitializedFlag(false),
    myLastRegisterSetCycle(0),
    myDisplayFrameRate(60.0),
    myNumChannels(1),
    myFragmentSizeLogBase2(0),
    myIsMuted(false),
    myVolume(100)
{
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundAndroid::~SoundAndroid()
{
  // Close the audio system if it's initialized
  close();
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::setEnabled(bool state)
{
  myIsEnabled = state;
  myOSystem->settings().setBool("sound", state);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::open()
{
  // Check whether to start the sound subsystem
  if(!myIsEnabled)
  {
    close();
    myOSystem->logMessage("Sound disabled.\n\n", 1);
    return;
  }

  // Make sure the sound queue is clear
  myRegWriteQueue.clear();
  myTIASound.reset();

  if(!myIsInitializedFlag)
  {
    myIsMuted = false;
    myLastRegisterSetCycle = 0;

    uInt32 fragsize = myOSystem->settings().getInt("fragsize");
    Int32 tiafreq   = myOSystem->settings().getInt("tiafreq");

    myIsInitializedFlag = true;
    myIsMuted = false;
    myFragmentSizeLogBase2 = log((double)fragsize) / log(2.0);

    // Now initialize the TIASound object which will actually generate sound
    myTIASound.outputFrequency(SOUND_RATE);
    myTIASound.tiaFrequency(tiafreq);
    myTIASound.channels(myNumChannels);

    bool clipvol = myOSystem->settings().getBool("clipvol");
    myTIASound.clipVolume(clipvol);

    // Adjust volume to that defined in settings
    myVolume = myOSystem->settings().getInt("volume");
    setVolume(myVolume);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::close()
{
    myIsInitializedFlag = false;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundAndroid::isSuccessfullyInitialized() const
{
  return myIsInitializedFlag;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::mute(bool state)
{
  if(myIsInitializedFlag)
  {
    myIsMuted = state;

    myRegWriteQueue.clear();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::reset()
{
  if(myIsInitializedFlag)
  {
    myIsMuted = false;
    myLastRegisterSetCycle = 0;
    myTIASound.reset();
    myRegWriteQueue.clear();
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::setVolume(Int32 percent)
{
  if(myIsInitializedFlag && (percent >= 0) && (percent <= 100))
  {
    myOSystem->settings().setInt("volume", percent);
    myVolume = percent;
    myTIASound.volume(percent);
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::adjustVolume(Int8 direction)
{
  ostringstream strval;
  string message;

  Int32 percent = myVolume;

  if(direction == -1)
    percent -= 2;
  else if(direction == 1)
    percent += 2;

  if((percent < 0) || (percent > 100))
    return;

  setVolume(percent);

  // Now show an onscreen message
  strval << percent;
  message = "Volume set to ";
  message += strval.str();

  myOSystem->frameBuffer().showMessage(message);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::adjustCycleCounter(Int32 amount)
{
  myLastRegisterSetCycle += amount;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::setChannels(uInt32 channels)
{
  if(channels == 1 || channels == 2)
    myNumChannels = channels;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::setFrameRate(float framerate)
{
  // FIXME - should we clear out the queue or adjust the values in it?
  myDisplayFrameRate = framerate;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::set(uInt16 addr, uInt8 value, Int32 cycle)
{
  // First, calulate how many seconds would have past since the last
  // register write on a real 2600
  double delta = (((double)(cycle - myLastRegisterSetCycle)) / 
      (1193191.66666667));

  // Now, adjust the time based on the frame rate the user has selected. For
  // the sound to "scale" correctly, we have to know the games real frame 
  // rate (e.g., 50 or 60) and the currently emulated frame rate. We use these
  // values to "scale" the time before the register change occurs.
  RegWrite info;
  info.addr = addr;
  info.value = value;
  info.delta = delta;
  myRegWriteQueue.enqueue(info);

  // Update last cycle counter to the current cycle
  myLastRegisterSetCycle = cycle;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::processFragment(uInt8* stream, Int32 length)
{
  if(!myIsInitializedFlag)
    return;

  uInt32 channels = myNumChannels;
  length = length / channels;

  // If there are excessive items on the queue then we'll remove some
  if(myRegWriteQueue.duration() > 
      (myFragmentSizeLogBase2 / myDisplayFrameRate))
  {
    double removed = 0.0;
    while(removed < ((myFragmentSizeLogBase2 - 1) / myDisplayFrameRate))
    {
      RegWrite& info = myRegWriteQueue.front();
      removed += info.delta;
      myTIASound.set(info.addr, info.value);
      myRegWriteQueue.dequeue();
    }
  }

  double position = 0.0;
  double remaining = length;

  while(remaining > 0.0)
  {
    if(myRegWriteQueue.size() == 0)
    {
      // There are no more pending TIA sound register updates so we'll
      // use the current settings to finish filling the sound fragment
//    myTIASound.process(stream + (uInt32)position, length - (uInt32)position);
      myTIASound.process(stream + ((uInt32)position * channels),
          length - (uInt32)position);

      // Since we had to fill the fragment we'll reset the cycle counter
      // to zero.  NOTE: This isn't 100% correct, however, it'll do for
      // now.  We should really remember the overrun and remove it from
      // the delta of the next write.
      myLastRegisterSetCycle = 0;
      break;
    }
    else
    {
      // There are pending TIA sound register updates so we need to
      // update the sound buffer to the point of the next register update
      RegWrite& info = myRegWriteQueue.front();

      // How long will the remaining samples in the fragment take to play
      double duration = remaining / (double) SOUND_RATE;

      // Does the register update occur before the end of the fragment?
      if(info.delta <= duration)
      {
        // If the register update time hasn't already passed then
        // process samples upto the point where it should occur
        if(info.delta > 0.0)
        {
          // Process the fragment upto the next TIA register write.  We
          // round the count passed to process up if needed.
          double samples = (SOUND_RATE * info.delta);
//        myTIASound.process(stream + (uInt32)position, (uInt32)samples +
//            (uInt32)(position + samples) - 
//            ((uInt32)position + (uInt32)samples));
          myTIASound.process(stream + ((uInt32)position * channels),
              (uInt32)samples + (uInt32)(position + samples) - 
              ((uInt32)position + (uInt32)samples));

          position += samples;
          remaining -= samples;
        }
        myTIASound.set(info.addr, info.value);
        myRegWriteQueue.dequeue();
      }
      else
      {
        // The next register update occurs in the next fragment so finish
        // this fragment with the current TIA settings and reduce the register
        // update delay by the corresponding amount of time
//      myTIASound.process(stream + (uInt32)position, length - (uInt32)position);
        myTIASound.process(stream + ((uInt32)position * channels),
            length - (uInt32)position);
        info.delta -= duration;
        break;
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundAndroid::save(Serializer& out) const
{
  try
  {
    out.putString(name());

    uInt8 reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0, reg5 = 0, reg6 = 0;

    // Only get the TIA sound registers if sound is enabled
    if(myIsInitializedFlag)
    {
      reg1 = myTIASound.get(0x15);
      reg2 = myTIASound.get(0x16);
      reg3 = myTIASound.get(0x17);
      reg4 = myTIASound.get(0x18);
      reg5 = myTIASound.get(0x19);
      reg6 = myTIASound.get(0x1a);
    }

    out.putByte((char)reg1);
    out.putByte((char)reg2);
    out.putByte((char)reg3);
    out.putByte((char)reg4);
    out.putByte((char)reg5);
    out.putByte((char)reg6);

    out.putInt(myLastRegisterSetCycle);
  }
  catch(const char* msg)
  {
    ostringstream buf;
    buf << "ERROR: SoundAndroid::save" << endl << "  " << msg << endl;
    myOSystem->logMessage(buf.str(), 0);
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool SoundAndroid::load(Serializer& in)
{
  try
  {
    if(in.getString() != name())
      return false;

    uInt8 reg1 = 0, reg2 = 0, reg3 = 0, reg4 = 0, reg5 = 0, reg6 = 0;
    reg1 = (uInt8) in.getByte();
    reg2 = (uInt8) in.getByte();
    reg3 = (uInt8) in.getByte();
    reg4 = (uInt8) in.getByte();
    reg5 = (uInt8) in.getByte();
    reg6 = (uInt8) in.getByte();

    myLastRegisterSetCycle = (Int32) in.getInt();

    // Only update the TIA sound registers if sound is enabled
    // Make sure to empty the queue of previous sound fragments
    if(myIsInitializedFlag)
    {
      myRegWriteQueue.clear();
      myTIASound.set(0x15, reg1);
      myTIASound.set(0x16, reg2);
      myTIASound.set(0x17, reg3);
      myTIASound.set(0x18, reg4);
      myTIASound.set(0x19, reg5);
      myTIASound.set(0x1a, reg6);
    }
  }
  catch(const char* msg)
  {
    ostringstream buf;
    buf << "ERROR: SoundAndroid::load" << endl << "  " << msg << endl;
    myOSystem->logMessage(buf.str(), 0);
    return false;
  }

  return true;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundAndroid::RegWriteQueue::RegWriteQueue(uInt32 capacity)
  : myCapacity(capacity),
    myBuffer(0),
    mySize(0),
    myHead(0),
    myTail(0)
{
  myBuffer = new RegWrite[myCapacity];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundAndroid::RegWriteQueue::~RegWriteQueue()
{
  delete[] myBuffer;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::RegWriteQueue::clear()
{
  myHead = myTail = mySize = 0;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::RegWriteQueue::dequeue()
{
  if(mySize > 0)
  {
    myHead = (myHead + 1) % myCapacity;
    --mySize;
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
double SoundAndroid::RegWriteQueue::duration()
{
  double duration = 0.0;
  for(uInt32 i = 0; i < mySize; ++i)
  {
    duration += myBuffer[(myHead + i) % myCapacity].delta;
  }
  return duration;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::RegWriteQueue::enqueue(const RegWrite& info)
{
  // If an attempt is made to enqueue more than the queue can hold then
  // we'll enlarge the queue's capacity.
  if(mySize == myCapacity)
    grow();

  myBuffer[myTail] = info;
  myTail = (myTail + 1) % myCapacity;
  ++mySize;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SoundAndroid::RegWrite& SoundAndroid::RegWriteQueue::front()
{
  assert(mySize != 0);
  return myBuffer[myHead];
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
uInt32 SoundAndroid::RegWriteQueue::size() const
{
  return mySize;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void SoundAndroid::RegWriteQueue::grow()
{
  RegWrite* buffer = new RegWrite[myCapacity * 2];
  for(uInt32 i = 0; i < mySize; ++i)
  {
    buffer[i] = myBuffer[(myHead + i) % myCapacity];
  }
  myHead = 0;
  myTail = mySize;
  myCapacity = myCapacity * 2;
  delete[] myBuffer;
  myBuffer = buffer;
}

#endif  // SOUND_SUPPORT
