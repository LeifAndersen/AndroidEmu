#include "app.h"
//#include "gpstdlib.h"
#include "gp32_snd.h"
//#include "fm.h"
#include "gp32.h"
//#include "fmint.h"

#define nINT_DMA2			(19)

#define L3C                             0x200   /* bit 9  */
#define L3M                             0x400   /* bit 10 */
#define L3D                             0x800   /* bit 11 */
#define L3_MASK                         0xe00

#define L3DELAY                             8   /* delay for bus-transfer */

#define PRESCALER_11_ENTRIES (32)

static const long prescaler11_256[PRESCALER_11_ENTRIES] = {
	2822400,		5644800,		8467200,		11289600,
	14112000,	16934400,	19756800,	22579200,
	25401600,	28224000,	31046400,	33868800,
	36691200,	39513600,	42336000,	45158400,
	47980800,	50803200,	53625600,	56448000,
	59270400,	62092800,	64915200,	67737600,
	70560000,	73382400,	76204800,	79027200,
	81849600,	84672000,	87494400,	90316800
};

static const long prescaler11_384[PRESCALER_11_ENTRIES] = {
	4233600,		8467200,		12700800,	16934400,
	21168000,	25401600,	29635200,	33868800,
	38102400,	42336000,	46569600,	50803200,
	55036800,	59270400,	63504000,	67737600,
	71971200,	76204800,	80438400,	84672000,
	88905600,	93139200,	97372800,	101606400,
	105840000,	110073600,	114307200,	118540800,
	122774400,	127008000,	131241600,	135475200
};

short *pOutput[5] __attribute__ ((aligned (4)));
short snd_enabled __attribute__ ((aligned (4)));
volatile int Timer __attribute__ ((aligned (4)));
volatile unsigned char nFlip __attribute__ ((aligned (4)));
volatile unsigned char curseg __attribute__ ((aligned (4)));
char snd_clean __attribute__ ((aligned (4)));
short snd_enable_backup __attribute__ ((aligned (4)));
unsigned int sound_buffer_size=(16500/60)/2;

/*
((sound rate/frame_rate)/2) =  buffer size
(sound rate/frame_rate) =  buffer size
60 fps
11Khz = 92
16Khz = 138
22Khz = 184

50 fps
11Khz = 111
16Khz = 165
22Khz = 221

*/
/*
#ifdef SND_16
#define OUT_BUFFER_SIZE (138)
#endif
#ifdef SND_22
#define OUT_BUFFER_SIZE (184)
#endif
#ifdef SND_11
#define OUT_BUFFER_SIZE (92)
#endif
*/
void playnextchunk(void);

void playnextchunk(void) {
	short *p;
	//short t3;
    
	//t3 = rTCNTO3;
	curseg=nFlip;
	p = pOutput[nFlip];
        
	
   /* start playing */
   rDIDST2=(1<<30)+     /* destination on peripheral bus */
         (1<<29)+       /* fixed adress */
         ((int)IISFIF); /* IIS FIFO txd */
   rDISRC2=(0<<30)+     /* source from system bus */
         (0<<29)+       /* auto-increment */
         (int)p;    		/* buffer adress */
   rDCON2 =(1<<30)+     /* handshake mode */
         (0<<29)+       /* DREQ and DACK are synchronized to PCLK (APB clock) */
         (1<<28)+       /* generate irq when transfer is done */
         (0<<27)+       /* unit transfer */
         (0<<26)+       /* single service */
         (0<<24)+       /* dma-req.source=I2SSDO */
         (1<<23)+       /* (H/W request mode) */
         (1<<22)+     	/* auto reload on/off */
         (1<<20)+       /* data size, byte,hword,word */
         (sound_buffer_size<<1);      /* transfer size (words) */

   rDMASKTRIG2=(0<<2)+(1<<1)+0;  /* no-stop, DMA2 channel on, no-sw trigger */

	/* Generate a new buffer full of sound */
	nFlip++;
	if(nFlip>=5) nFlip=0;
        //nFlip^=1;
	
	/*if (snd_enabled)
	{
		RefreshFm();
		//YM2612UpdateOne((short*)pOutput[nFlip]+(drmd.current_sample<<1),sound_buffer_size-drmd.current_sample);
		YM2612UpdateOne((short*)pOutput[nFlip],sound_buffer_size);
		//drmd.current_sample=0;
		//drmd.sample_count=0;
		snd_clean=0;
	}
	else
	{
		// Make sure the buffer is all silence, and play it endlessly
		if (!snd_clean)
		{
			for (p=pOutput[0]; p<(short *)(pOutput[1]+sound_buffer_size); p++)
				*p=0;

			snd_clean = 1;
		}
	}*/
	Timer++;

	//t3 -= rTCNTO3;
	//profileSND += (int)t3;

}

static void SoundIsr(void) __attribute__ ((interrupt ("IRQ")));
static void SoundIsr (void)
{
        //asm volatile ("stmdb    r13!,{r0-r12,lr}");
        playnextchunk();
	//asm volatile ("ldmia    r13!,{r0-r12,lr}");
        //asm volatile ("subs     pc,lr,#4");
}

void IsrInstall(unsigned long nr,void *ptr)
{
	unsigned int mask;
	gp_disableIRQ();
        //ARMDisableInterrupt();
	mask = 1<<nr;

	/* Clear any pending interrupts */
	rSRCPND = mask;
	rINTPND = mask;
	/* Set this interrupt to IRQ rather than FIQ. Shouldn't be
		necessary because INTMOD should be zero, but just in case...
	 */
	rINTMOD &= ~(mask);

	/* set vector for selected interrupt interrupt. This system call copies the
		vector to the table used by the interrupt handler in the bios. It also clears
		the mask bit for the interrupt
	*/

	
	gp_installSWIIRQ(nr,ptr);
	gp_enableIRQ();
	
	//swi_install_irq(nr,ptr);
	//ARMEnableInterrupt();
}

void IsrUninstall(unsigned long nr,void *ptr)
{
	unsigned int mask;
        gp_removeSWIIRQ(nr,ptr);
	//swi_uninstall_irq(nr);
	mask = 1<<nr;

	/* Clear any pending interrupts */
	rSRCPND = mask;
	rINTPND = mask;
}

/*
   This routine sends an address plus type of transfer to the L3 bus
*/
static void _WrL3Addr(unsigned char data)
{
   signed long i,j;

   rPEDAT = rPEDAT & ~L3_MASK;         //L3D=L/L3M=L(in address mode)/L3C=L
   rPEDAT |= L3C;                      //L3C=H
   for(j=0;j<L3DELAY;j++);             //tsu(L3) > 190ns

   //PD[8:6]=L3D:L3C:L3M
   for(i=0;i<8;i++)  //LSB first
   {
      if(data&0x1)   //if data's LSB is 'H'
      {
         rPEDAT &= ~L3C;           //L3C=L
         rPEDAT |= L3D;            //L3D=H
         for(j=0;j<L3DELAY;j++);   //tcy(L3) > 500ns
         rPEDAT |= L3C;            //L3C=H
         rPEDAT |= L3D;            //L3D=H
         for(j=0;j<L3DELAY;j++);   //tcy(L3) > 500ns
      }
      else     //if data's LSB is 'L'
      {
         rPEDAT &= ~L3C;           //L3C=L
         rPEDAT &= ~L3D;           //L3D=L
         for(j=0;j<L3DELAY;j++);   //tcy(L3) > 500ns
         rPEDAT |= L3C;            //L3C=H
         rPEDAT &= ~L3D;           //L3D=L
         for(j=0;j<L3DELAY;j++);   //tcy(L3) > 500ns
      }
      data >>=1;
   }
   //L3M=H,L3C=H
   rPEDAT = (rPEDAT & ~L3_MASK) | (L3C|L3M);
}

/*
   This routine sends a data word to the L3 bus
*/
static void _WrL3Data(unsigned char data,int halt)
{
   signed long i,j;

   if(halt)
   {
      rPEDAT |= L3C;                //L3C=H(while tstp, L3 interface halt condition)
      for(j=0;j<L3DELAY;j++);       //tstp(L3) > 190ns
      rPEDAT &= (~L3M);
      for (j=0;j<L3DELAY;j++);
      rPEDAT |= L3M;
   }

   rPEDAT = (rPEDAT & ~L3_MASK) | (L3C|L3M);        //L3M=H(in data transfer mode)
   for(j=0;j<L3DELAY;j++);                          //tsu(L3)D > 190ns

   //PD[8:6]=L3D:L3C:L3M
   for(i=0;i<8;i++)
   {
      if(data&0x1)   //if data's LSB is 'H'
      {
         rPEDAT &= ~L3C;           //L3C=L
         rPEDAT |= L3D;            //L3D=H
         for(j=0;j<L3DELAY;j++);   //tcy(L3) > 500ns
         rPEDAT |= (L3C|L3D);      //L3C=H,L3D=H
         for(j=0;j<L3DELAY;j++);   //tcy(L3) > 500ns
      }
      else     //if data's LSB is 'L'
      {
         rPEDAT &= ~L3C;           //L3C=L
         rPEDAT &= ~L3D;           //L3D=L
         for(j=0;j<L3DELAY;j++);   //tcy(L3) > 500ns
         rPEDAT |= L3C;            //L3C=H
         rPEDAT &= ~L3D;           //L3D=L
         for(j=0;j<L3DELAY;j++);   //tcy(L3) > 500ns
      }
      data>>=1;   //for check next bit
   }

   rPEDAT = (rPEDAT & ~L3_MASK) | L3C;
   for (j=0;j<L3DELAY;j++);
   rPEDAT |= L3M;
}

/*
        This routine initializes the i/o port used for L3-bus and
        then configures the UDA1330
*/
static void Init1330( int SC384 )
{
   /****** I/O Port E Initialize ******/
   /* port reconfiguration :
      PORTE 9,10,11 --> output
      pull-up disable
      L3_MODE, L3_CLK High
   */
   if (SC384)
   	SC384=1;

   rPEDAT = (rPEDAT & ~0xe00) | (L3M|L3C);
   rPEUP |= 0xe00;
   rPECON = (rPECON & (~(0x3f << 18))) | (0x15<<18);

   /****** send commands via L3 Interface
   Data bits 7 to 2 represent a 6-bit device address where bit 7 is the MSB.
   The address of the UDA1330ATS is 000101 (bit 7 to bit 2).
   ******/

   /* status type transfer , data value - clock=512fs, MSB format */
   _WrL3Addr(0x14+2); //STATUS (0) (000101xx+10)
   _WrL3Data(  ((2-SC384)<<4)+  //  00,  : 512,384,256fs         (SC : System Clock Freq)
               (0<<1)   // 000,  : iis,lsb16,lsb18,lsb20,msb
               ,0);

   /* data type transfer , data value - full volume */
   _WrL3Addr(0x14 + 0);       /* DATA0 (000101xx+00) */
   _WrL3Data(0x0              /* volume */
               ,0);

   /* data type transfer , data value - de-emphasis, no muting */
   _WrL3Addr(0x14 + 0);       /* DATA0 (000101xx+00) */
   _WrL3Data(0x80+            /* select de-emhasis/mute */
               (2<<3)         /* de-emp 44khz */
               ,1);
}

static void InitIIS(int nReverse, int nPrescaler, int SC384)
{
	int nRev;

	if (nReverse)
		nRev=0;
	else
		nRev=1;

   if (SC384)
   	SC384=1;
    /****** IIS Initialize ******/

    /* prescaler for 11khz */
    rIISPSR=(nPrescaler<<5)+nPrescaler;

    rIISCON=/* 8,7,6 readonly */
            (1<<5)+  /* dma transmit request enable */
            (0<<4)+  /* dma receive request disable */
            (0<<3)+  /* Transmit LR-Clock idle state */
            (1<<2)+  /* Receive LR-Clock idle state */
            (1<<1)+  /* prescaler enabled */
            (0<<0);  /* iis disabled */

    rIISMOD=
            (0<<8)+             /* master mode */
            (2<<6)+             /* transmit mode */
            (nRev<<5)+          /* lowbyte=left channel (1) or right channel (0) */
            (0<<4)+             /* iis compatible MSB samples */
            (1<<3)+             /* 16 bit per channel */
            (SC384<<2)+             /* CDCLK=256fs (0) or 384fs (1) */
            (1<<0);             /* serial bitclock 48fs */

    rIISFIFCON=
               (1<<11)+   /* transmit fifo mode = dma */
               (0<<10)+   /* recieve fifo mode = normal */
               (1<<9)+    /* transmit fifo enable */
               (0<<8);    /* receive fifo disable */
                /* 7-4 Transmit Fifo Data Count (readonly) */
                /* 3-0 Recieve Fifo Data Count (readonly) */

    /****** IIS Tx Start ******/
    rIISCON |=0x1;  /* iis enable */

}

int Gp32_AudioStart (int nReverse)
{
	int x;
	int nPrescaler=0;
	int nLeastDiff;
	int SC384=0;
	long pclk;
	int diff;
	short *p;
	unsigned int nTcon;
	unsigned int nT;

	pOutput[0]=(short *)(SAMPLEBUFFER0);
	pOutput[1]=pOutput[0]+(sound_buffer_size<<1);
	pOutput[2]=pOutput[1]+(sound_buffer_size<<1);
	pOutput[3]=pOutput[2]+(sound_buffer_size<<1);
	pOutput[4]=pOutput[3]+(sound_buffer_size<<1);
	for (p=pOutput[0]; p<(short *)(pOutput[4]+sound_buffer_size); p++)
		*p=0;

	snd_clean = 1;

	rINTMSK = 0xffffffff;

   /* stop dma2 */
   rDMASKTRIG2=(1<<2)+(0<<1)+0;  /* stop, DMA2 channel off, no-sw trigger */

	nLeastDiff = 1000000000;

        switch(sound_rate)
	{
	   case 0:
	      pclk = (gp_getPCLK()*4)/3;  // 8250
	      break;
	   case 1:
	      pclk = gp_getPCLK();        // 11025
	      break;
	   case 2:
	      pclk = (gp_getPCLK()*2)/3;  // 16500
	      break;
	   case 3:
	      pclk = gp_getPCLK()/2;      // 22050
	      break;
		case 4:
	      pclk = gp_getPCLK()/4;      // 44100
	      break;
        }

	for (x=0;x<PRESCALER_11_ENTRIES;x++)
	{
		diff = pclk - prescaler11_256[x];
		if (diff<0)
			diff=-diff;
		if (diff<nLeastDiff)
		{
			SC384=0;
			nPrescaler=x;
			nLeastDiff=diff;
		}
		diff = pclk - prescaler11_384[x];
		if (diff<0)
			diff=-diff;
		if (diff<nLeastDiff)
		{
			SC384=1;
			nPrescaler=x;
			nLeastDiff=diff;
		}
	}

	for (x=0;x<10000000;x++);

	nFlip=0;

   /* initialize uda1330a by sending appropriate commands over L3 bus */
   Init1330(SC384);
   /* setup IIS bus */
   InitIIS(nReverse,nPrescaler,SC384);
   /* install dma2-irq */
   IsrInstall(nINT_DMA2,(void*)SoundIsr);
   /* start playing */
   rDIDST2=(1<<30)+       /* destination on peripheral bus */
         (1<<29)+       /* fixed adress */
         ((int)IISFIF); /* IIS FIFO txd */
   rDISRC2=(0<<30)+       /* source from system bus */
         (0<<29)+       /* auto-increment */
         (int)pOutput[0];    /* buffer adress */
   rDCON2 =(1<<30)+       /* handshake mode */
         (0<<29)+       /* DREQ and DACK are synchronized to PCLK (APB clock) */
         (1<<28)+       /* generate irq when transfer is done */
         (0<<27)+       /* unit transfer */
         (0<<26)+       /* single service */
         (0<<24)+       /* dma-req.source=I2SSDO */
         (1<<23)+       /* (H/W request mode) */
         (1<<22)+     /* auto reload on/off */
         (1<<20)+       /* data size, byte,hword,word */
         (sound_buffer_size<<1);      /* transfer size (words) */

   rDMASKTRIG2=(0<<2)+(1<<1)+0;  /* no-stop, DMA2 channel on, no-sw trigger */

	/* Now set up timer 3 to run at 16000 counts/sec */
//	nTcon = rTCON;
//	nT = rTCFG0;
//	nT &= 0xffff00ff;
//	nT |= 0xff00;	/* Set prescaler to 256 */
//	rTCFG0 = nT;
//	nT = rTCFG1;
//	nT &= 0xffff0fff;
//	nT |= 0x00003000;	/* Set divider to 1/16 */
//	rTCFG1 = nT;
//	/* Set reload register to 0xffff */
//	rTCNTB3 = 0xffff;
//	rTCMPB3 = 0xffff;
///	/* Now enable timer 3 */
//	nT = rTCON;
//	nT &= 0xfff0ffff;
//	nT |= 0x00020000;
//	rTCON = nT;
//	nT &= 0xfff0ffff;
//	nT |= 0x00090000;
//	rTCON = nT;

   return 0;
}


void Gp32_AudioStop (void)
{
	/* stop dma2 */
   rDMASKTRIG2=(1<<2)+(0<<1)+0;  /* stop, DMA2 channel off, no-sw trigger */

   IsrUninstall(nINT_DMA2,(void*)SoundIsr);
}

void Gp32_AudioPause (char doPause)
{
	if (doPause)
	{
		snd_enable_backup = snd_enabled;
		snd_enabled = 0;
	}
	else
	{
		Gp32_AudioStop();
		snd_enabled = snd_enable_backup;
		Gp32_AudioStart(0);
	}
}

