#include "app.h"
//#include "fmint.h"
//#include "gp32_snd.h"

struct PSG_CONTEXT PSG;
unsigned int PSG_sn_UPDATESTEP;
/*
static int PSG.sn_VolTable[16] ;
int PSG.sn_Register[8] ;
int PSG.sn_LastRegister ;
int PSG.sn_Volume[4] ;
unsigned int PSG.sn_RNG ;
int PSG.sn_NoiseFB ;
int PSG.sn_Period[4] ;
int PSG.sn_Count[4] ;
int PSG.sn_Output[4] ;*/


INLINE int parity(int val)
{
	val^=val>>8;
	val^=val>>4;
	val^=val>>2;
	val^=val>>1;
	return val;
}

void psg_write(int data)
{
#if defined(__EMU_SMS__)
	if ((CurrentEmuMode != EMU_MODE_MD) && (sound_on != 0))
	{
	    //update sound before updating psg registers
	    int i;
	    int currstage;
	    int length;
	    currstage= sample_count_lookup[drsms.vdp_line];
	    length=currstage-laststage; 
	    if(length>0)
	    {
	       if(laststage<sound_buffer_size) 
	       {
	         RenderSound(soundbuffer+(laststage<<1),length);
		 laststage=currstage;
	       }
   
	    }
	}
#endif
	//if (snd_enabled)
	{
		if (data & 0x80)
		{
			int r = (data & 0x70) >> 4;
			int c = r/2;

			PSG.sn_LastRegister = r;
			PSG.sn_Register[r] = (PSG.sn_Register[r] & 0x3f0) | (data & 0x0f);
			switch (r)
			{
			case 0:	/* tone 0 : frequency */
			case 2:	/* tone 1 : frequency */
			case 4:	/* tone 2 : frequency */
				PSG.sn_Period[c] = PSG_sn_UPDATESTEP * PSG.sn_Register[r];
				if (PSG.sn_Period[c] == 0) PSG.sn_Period[c] = PSG_sn_UPDATESTEP;
				if (r == 4)
				{
					/* update noise shift frequency */
					if ((PSG.sn_Register[6] & 0x03) == 0x03)
						PSG.sn_Period[3] = 2 * PSG.sn_Period[2];
				}
				break;
			case 1:	/* tone 0 : volume */
			case 3:	/* tone 1 : volume */
			case 5:	/* tone 2 : volume */
			case 7:	/* noise  : volume */
				PSG.sn_Volume[c] = PSG.sn_VolTable[data & 0x0f];
				break;
			case 6:	/* noise  : frequency, mode */
				{
					int n = PSG.sn_Register[6];
					// orig
					PSG.sn_NoiseFB = (n & 4) ? FB_WNOISE : FB_PNOISE;
					

					//PSG.sn_NoiseFB = ( (n & 4) ? parity((PSG.sn_RNG) & 0x000f) : PSG.sn_RNG & 1 ) << 15;

					n &= 3;
					/* N/512,N/1024,N/2048,Tone #3 output */
					PSG.sn_Period[3] = (n == 3) ? 2 * PSG.sn_Period[2] : (PSG_sn_UPDATESTEP << (5+n));

					/* reset noise shifter */
					PSG.sn_RNG = NG_PRESET;
					PSG.sn_Output[3] = PSG.sn_RNG & 1;
				}
				break;
			}
		}
		else
		{
			int r = PSG.sn_LastRegister;
			int c = r/2;

			switch (r)
			{
			case 0:	/* tone 0 : frequency */
			case 2:	/* tone 1 : frequency */
			case 4:	/* tone 2 : frequency */
				PSG.sn_Register[r] = (PSG.sn_Register[r] & 0x0f) | ((data & 0x3f) << 4);
				PSG.sn_Period[c] = PSG_sn_UPDATESTEP * PSG.sn_Register[r];
				if (PSG.sn_Period[c] == 0) PSG.sn_Period[c] = PSG_sn_UPDATESTEP;
				if (r == 4)
				{
					/* update noise shift frequency */
					if ((PSG.sn_Register[6] & 0x03) == 0x03)
						PSG.sn_Period[3] = 2 * PSG.sn_Period[2];
				}
				break;
			}
		}
	}
}

int SN76496_sh_start(void)
{
	int i;
	double out;

	for (i = 0;i < 4;i++) PSG.sn_Volume[i] = 0;

	PSG.sn_LastRegister = 0;
	for (i = 0;i < 8;i+=2)
	{
		PSG.sn_Register[i] = 0;
		PSG.sn_Register[i + 1] = 0x0f;	/* volume = 0 */
	}

	for (i = 0;i < 4;i++)
	{
		PSG.sn_Output[i] = 0;
		PSG.sn_Period[i] = PSG.sn_Count[i] = PSG_sn_UPDATESTEP;
	}
	PSG.sn_RNG = NG_PRESET;
	PSG.sn_Output[3] = PSG.sn_RNG & 1;

	/* increase max output basing on gain (0.2 dB per step) */
	out = MAX_OUTPUT / 3;

	/* build volume table (2dB per step) */
	for (i = 0;i < 15;i++)
	{
		/* limit volume to avoid clipping */
		PSG.sn_VolTable[i] = out;

		out /= 1.258925412;	/* = 10 ^ (2/20) = 2dB */
	}
	PSG.sn_VolTable[15] = 0;

	return 0;
}

