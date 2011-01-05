#ifdef __cplusplus
extern "C" {
#endif


#ifndef _H_FM_FM_
#define _H_FM_FM_
#define ENV_BITS		10
#define ENV_LEN			(1<<ENV_BITS)
#define ENV_STEP		(128.0/ENV_LEN)

#define MAX_ATT_INDEX	(ENV_LEN-1) /* 1023 */
#define MIN_ATT_INDEX	(0)			/* 0 */

#define FREQ_SH			16  /* 16.16 fixed point (frequency calculations) */
#define EG_SH			16  /* 16.16 fixed point (envelope generator timing) */
#define LFO_SH			24  /*  8.24 fixed point (LFO calculations)       */
#define TIMER_SH		16  /* 16.16 fixed point (timers calculations)    */

#define FREQ_MASK		((1<<FREQ_SH)-1)

#define EG_ATT			4
#define EG_DEC			3
#define EG_SUS			2
#define EG_REL			1
#define EG_OFF			0

#define SIN_BITS		10
#define SIN_LEN			(1<<SIN_BITS)
#define SIN_MASK		(SIN_LEN-1)

#define TL_RES_LEN		(256) /* 8 bits addressing (real chip) */


#define MAXOUT		(+32767)
#define MINOUT		(-32768)


/*	TL_TAB_LEN is calculated as:
*	13 - sinus amplitude bits     (Y axis)
*	2  - sinus sign bit           (Y axis)
*	TL_RES_LEN - sinus resolution (X axis)
*/
#define TL_TAB_LEN (13*2*TL_RES_LEN)
#define ENV_QUIET		(TL_TAB_LEN>>3)  // 0x340

#define RATE_STEPS (8)

extern const UINT32 sl_table[];
extern const UINT8 dt_tab[];
extern const UINT8 opn_fktable[16];
extern const UINT8 eg_rate_select[];
extern const UINT8 eg_rate_shift[];
extern const UINT8 eg_inc[19*RATE_STEPS];
extern const UINT8 lfo_pm_output[7*8][8];
extern const UINT32 lfo_samples_per_step[8];
extern const UINT8 lfo_ams_depth_shift[4];
extern const signed char lfo_pm_table[128*8*32];
extern signed int YMOPN_ST_dt_tab[8][32];
extern unsigned int OPN_fn_table[4096];	/* fnumber->increment counter */
extern unsigned int OPN_lfo_freq[8];	/* LFO FREQ table */
extern const signed short tl_tab[];
extern const unsigned short sin_tab[];

extern void update_tables();
extern void fm_update_timers(void);
extern void fm_vdp_line_update(void);
extern void fm_update_dac(void);
extern int YM2612Init();
void YM2612ResetChip();
void YM2612UpdateOne(short *buffer,unsigned int length);
void fm_write(int a, UINT8 v);
int fm_read(int address);
void RefreshFm (void);
void update_timers(void);
/* For 22kHz sound */
//#define SND_22
/* For 16kHz sound */
#define SND_16
/* For 11kHz sound */
//#define SND_11



// samples size will change for 50fps
// 16Khz sound = 166 (165.6) samples
//#ifdef SND_16
//#define OUT_BUFFER_SIZE (138)
//#endif
//#ifdef SND_22
//#define OUT_BUFFER_SIZE (184)
//#endif
//#ifdef SND_11
//#define OUT_BUFFER_SIZE (92)
//#endif

#define MAX_OUTPUT  0x7fff
#define STEP        0x10000
#define FB_WNOISE   0x12000
#define FB_PNOISE   0x08000
#define NG_PRESET   0x0F35

#define INLINE static inline
#define FMSAMPLE signed short

//#ifndef PI
//#define PI 3.14159265358979323846
//#endif

/* register number to channel number , slot offset */
#define OPN_SLOT(N) ((N>>2)&3)

/* slot number */
#define SLOT1 0
#define SLOT2 2
#define SLOT3 1
#define SLOT4 3

/* struct describing a single operator (SLOT) */
typedef struct
{
	UINT32 eg_sh_active_mask;
	UINT32	sl;			/* sustain level:sl_table[SL] */
	UINT32 eg_sh_d1r_mask;
	UINT32 eg_sh_d2r_mask;
	UINT32 eg_sh_rr_mask;
	UINT32 eg_sh_ar_mask;
	UINT32	tl;			/* total level: TL << 3	*/
	UINT32	vol_out;	/* current output from EG circuit (without AM from LFO) */
	/* LFO */
	UINT32	AMmask;		/* AM enable flag */

	/* Phase Generator */
	UINT32	phase;		/* phase counter */
	UINT32	Incr;		/* phase step */
	UINT32	mul;		/* multiple        :ML_TABLE[ML] */
	UINT32	key;		/* 0=last key was KEY OFF, 1=KEY ON	*/

	UINT32	ar;			/* attack rate  */
	UINT32	d1r;		/* decay rate   */
	UINT32	d2r;		/* sustain rate */
	UINT32	rr;			/* release rate */
	
	INT32	volume;		/* envelope counter	*/
	INT32	*DT;		/* detune          :dt_tab[DT] */
	// 19 * 4
	// 13
	// 0x59 = align 0x5C
	UINT8	state;		/* phase type */
	UINT8	eg_sel_ar;	/*  (attack state) */
	UINT8	eg_sh_ar;	/*  (attack state) */
	UINT8	eg_sel_d1r;	/*  (decay state) */
	
	UINT8	eg_sh_d1r;	/*  (decay state) */
	UINT8	eg_sel_d2r;	/*  (sustain state) */
	UINT8	eg_sh_d2r;	/*  (sustain state) */
	UINT8	eg_sel_rr;	/*  (release state) */
	
	UINT8	eg_sh_rr;	/*  (release state) */
	UINT8	ssg;		/* SSG-EG waveform */
	UINT8	ssgn;		/* SSG-EG negated output */

	UINT8	KSR;		/* key scale rate  :3-KSR */
	UINT8	ksr;		/* key scale rate  :kcode>>(3-KSR) */

	
} FM_SLOT;

typedef struct
{
        UINT32	fc;			/* fnum,blk:adjusted to sample rate	*/
	UINT32	block_fnum;	/* current blk/fnum value for this slot (can be different betweeen slots of one channel in 3slot mode) */
	INT32	pms;		/* channel PMS */
	INT32	op1_out[2];	/* op1 output for feedback */
	INT32	mem_value;	/* delayed sample (MEM) value */
	UINT8	ALGO;		/* algorithm */
	UINT8	FB;			/* feedback shift */
	UINT8	ams;		/* channel AMS */
	UINT8	kcode;		/* key code: */

	FM_SLOT	SLOT[4];	/* four SLOTs (operators) */
} FM_CH;


typedef struct
{
        UINT32	mode;		// mode  CSM / 3SLOT
	int		TA;			// timer a				
	int		TA_Count;		// timer a counter		
	int 		TA_Base;
	int		TB_Count;		//timer b counter		
	int 		TB_Base;
	unsigned char   TB;			// timer b
	UINT8	address;	// address register		
	UINT8	irq;		// interrupt level		
	UINT8	irqmask;	// irq mask				
	UINT8	status;		// status flag			
	UINT8	prescaler_sel;// prescaler selector	
	UINT8	fn_h;		// freq latch			
					

} FM_ST;

/***********************************************************/
/* OPN unit                                                */
/***********************************************************/

/* OPN 3slot struct */
typedef struct
{
	UINT32  fc[3];	/* fnum3,blk3: calculated */
	UINT32	block_fnum[3];	/* current fnum value for this slot (can be different betweeen slots of one channel in 3slot mode) */
	UINT8	fn_h;			/* freq3 latch */
	UINT8	kcode[3];		/* key code */

} FM_3SLOT;

/* OPN/A/B common state */
typedef struct
{
	/* LFO */
	UINT32	lfo_inc;
	UINT32	lfo_cnt;

	UINT32	eg_cnt;			/* global envelope generator counter */
	UINT32	eg_timer;		/* global envelope generator counter works at frequency = chipclock/64/3 */
	UINT32	eg_timer_add;	/* step of eg_timer */
	UINT32	eg_timer_overflow;/* envelope generator timer overlfows every 3 samples (on real chip) */
} FM_OPN;

extern char OPN_pan[];	/* fm channels output masks */

extern FM_3SLOT SL3;			/* 3 slot mode state */
extern FM_ST	ST;				/* general state */
extern FM_OPN OPN;				/* OPN state			*/
extern FM_CH CH[6];				/* channel state		*/
extern int dacout;
extern int dacen;
extern unsigned short dac_buffer[];
extern int dac_sample;
extern unsigned int last_sample;
extern unsigned int current_sample;
extern unsigned int timer_base;

typedef int (*algo)(FM_CH *CH);

extern const algo algofuncs[8];


#endif /* _H_FM_FM_ */

#ifdef __cplusplus
} // End of extern "C"
#endif

