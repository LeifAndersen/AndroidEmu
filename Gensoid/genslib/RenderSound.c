#include "app.h"

void RenderSound(short *buffer, unsigned int length)
{
	unsigned int psg_out=0;
	int i=0;
	
	/* If the volume is 0, increase the counter */
	for (i = 0;i < 4;i++)
	{
		if (PSG.sn_Volume[i] == 0)
		{
			/* note that I do count += length, NOT count = length + 1. You might think */
			/* it's the same since the volume is 0, but doing the latter could cause */
			/* interferencies when the program is rapidly modulating the volume. */
			if (PSG.sn_Count[i] <= (sound_buffer_size<<16))
				PSG.sn_Count[i] += (sound_buffer_size<<16);
		}
	}
	
	for(i=0; i < length ; i++)
	{
		psg_out=0;
		//if((menu_options.sound_on==1)||(menu_options.sound_on==3))
		{
			int vol;
	
			vol=0;
			if (PSG.sn_Output[0])
				vol = PSG.sn_Count[0];
			PSG.sn_Count[0] -= STEP;
	
			while (PSG.sn_Count[0] <= 0)
			{
				PSG.sn_Count[0] += PSG.sn_Period[0];
				if (PSG.sn_Count[0] > 0)
				{
					PSG.sn_Output[0] ^= 1;
					if (PSG.sn_Output[0])
						vol += PSG.sn_Period[0];
					break;
				}
				PSG.sn_Count[0] += PSG.sn_Period[0];
				vol += PSG.sn_Period[0];
			}
			if (PSG.sn_Output[0])
				vol -= PSG.sn_Count[0];
	
			psg_out = vol * PSG.sn_Volume[0];
	
			vol=0;
			if (PSG.sn_Output[1])
				vol = PSG.sn_Count[1];
			PSG.sn_Count[1] -= STEP;
	
			while (PSG.sn_Count[1] <= 0)
			{
				PSG.sn_Count[1] += PSG.sn_Period[1];
				if (PSG.sn_Count[1] > 0)
				{
					PSG.sn_Output[1] ^= 1;
					if (PSG.sn_Output[1])
						vol += PSG.sn_Period[1];
					break;
				}
				PSG.sn_Count[1] += PSG.sn_Period[1];
				vol += PSG.sn_Period[1];
			}
			if (PSG.sn_Output[1])
				vol -= PSG.sn_Count[1];
	
			psg_out += vol * PSG.sn_Volume[1];
	
			vol=0;
			if (PSG.sn_Output[2])
				vol = PSG.sn_Count[2];
			PSG.sn_Count[2] -= STEP;
			
			while (PSG.sn_Count[2] <= 0)
			{
				PSG.sn_Count[2] += PSG.sn_Period[2];
				if (PSG.sn_Count[2] > 0)
				{
					PSG.sn_Output[2] ^= 1;
					if (PSG.sn_Output[2])
						vol += PSG.sn_Period[2];
					break;
				}
				PSG.sn_Count[2] += PSG.sn_Period[2];
				vol += PSG.sn_Period[2];
			}
			if (PSG.sn_Output[2])
				vol -= PSG.sn_Count[2];
	
			psg_out += vol * PSG.sn_Volume[2];
			
			{
				int left = STEP;
	
				vol=0;
				do
				{
					int nextevent;
	
					if (PSG.sn_Count[3] < left)
						nextevent = PSG.sn_Count[3];
					else
						nextevent = left;
	
					if (PSG.sn_Output[3])
						vol += PSG.sn_Count[3];
	
					PSG.sn_Count[3] -= nextevent;
	
					if (PSG.sn_Count[3] <= 0)
					{
						if (PSG.sn_RNG & 1)
							PSG.sn_RNG ^= PSG.sn_NoiseFB;
						PSG.sn_RNG >>= 1;
						PSG.sn_Output[3] = PSG.sn_RNG & 1;
						PSG.sn_Count[3] += PSG.sn_Period[3];
						if (PSG.sn_Output[3])
							vol += PSG.sn_Period[3];
					}
					if (PSG.sn_Output[3])
						vol -= PSG.sn_Count[3];
	
					left -= nextevent;
				}
				while (left > 0);
	
				psg_out += vol * PSG.sn_Volume[3];
			}
	
	
		}
		psg_out>>=16;
				
		*buffer++ = (short)psg_out;
		*buffer++ = (short)psg_out;
	}
}			


