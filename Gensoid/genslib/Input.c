
#include "app.h"

struct Input Inp;
static int repeatCounter = 0;
int InputInit()
{
  memset(&Inp,0,sizeof(Inp));
  return 0;
}

int InputUpdate(int EnableDiagnals)
{
  int i=0;
  unsigned int key=0;
  // Get input
#ifdef __GP32__
  key=gp_getButton();
#endif
#if defined(__GP2X__)
  key=gp_getButton(EnableDiagnals);
  key&=	(1<<INP_BUTTON_UP)|
			(1<<INP_BUTTON_LEFT)|
			(1<<INP_BUTTON_DOWN)|
			(1<<INP_BUTTON_RIGHT)|
			(1<<INP_BUTTON_START)|
			(1<<INP_BUTTON_SELECT)|
			(1<<INP_BUTTON_L)|
			(1<<INP_BUTTON_R)|
			(1<<INP_BUTTON_A)|
			(1<<INP_BUTTON_B)|
			(1<<INP_BUTTON_X)|
			(1<<INP_BUTTON_Y)|
			(1<<INP_BUTTON_VOL_UP)|
			(1<<INP_BUTTON_VOL_DOWN)|
			(1<<INP_BUTTON_STICK_PUSH);
#endif
#if defined(__GIZ__)
  key=gp_getButton(EnableDiagnals);
  key&=	(1<<INP_BUTTON_UP)|
			(1<<INP_BUTTON_LEFT)|
			(1<<INP_BUTTON_DOWN)|
			(1<<INP_BUTTON_RIGHT)|
			(1<<INP_BUTTON_HOME)|
			(1<<INP_BUTTON_VOL)|
			(1<<INP_BUTTON_L)|
			(1<<INP_BUTTON_R)|
			(1<<INP_BUTTON_REWIND)|
			(1<<INP_BUTTON_FORWARD)|
			(1<<INP_BUTTON_PLAY)|
			(1<<INP_BUTTON_STOP)|
			(1<<INP_BUTTON_BRIGHT);
#endif
  // Find out how long key was pressed for
  for (i=0;i<32;i++)
  {
    int held=Inp.held[i];

    if (key&(1<<i)) held++; else held=0;

    //if (held>=0x80) held&=0xbf; // Keep looping around

    Inp.held[i]=held;
  }

  // Work out some key repeat values:
  for (i=0;i<32;i++)
  {
    char rep=0;
    int held=Inp.held[i];

    if (held==1) 
	{
		// Key has just been pressed again, so set repeat by default
		rep=1;
	}
	else
	{
		// Now make sure key has been held for a period of time
		// before auto toggling the repeat flag
		if (held>=0x20)
		{
			repeatCounter++;
			if(repeatCounter>15)
			{
				rep=1;
				repeatCounter=0;
			}
		}
	}

    Inp.repeat[i]=rep;
  }

  return 0;
}
