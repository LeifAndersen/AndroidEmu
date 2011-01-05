

#ifndef __RTC_H__
#define __RTC_H__


struct rtc
{
	int batt;
	int sel;
	int latch;
	int d, h, m, s, t;
	time_t last;
	int stop, carry;
	byte regs[8];
};

extern struct rtc rtc;




#endif



