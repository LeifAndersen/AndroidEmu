extern volatile int Timer;

static void TIMER4IRQ(void) __attribute__ ((interrupt ("IRQ")));
static void TIMER4IRQ(void) {
     Timer++;
}

static void start_timer4(int freq) {
   int pclk;
   pclk = gp_getPCLK();
   pclk/= 16;
   pclk/= 256;
   pclk/= freq;  

   rTCFG0 |= (0xFF<<8);   // Presacler for timer 2,3,4 = 256
   rTCFG1 |= (0x03<<16);  // timer4  1/16
   rTCNTB4 = (long)pclk;
   rTCON  = (0x1<<22) | (0x1<<20); // start timer4, auto reload
   IsrInstall(14,(void*)TIMER4IRQ);
}

static void stop_timer4(void) {

   rTCON = BITCLEAR(rTCON,20); // timer4 off
   IsrUninstall(14,(void*)TIMER4IRQ);

}

static int RunEmulation()
{
  int quit=0,ticks=0,now=0,done=0,i=0;
  int tick=0,fps=0;
  
  Timer=0;
  Frames=0;

  start_timer4(frame_limit); // frame limit is the number of frames per second
                             // NTSC = 60
			     // PAL = 50
  
  if(menu_options.frameskip==0) // auto frame skip
  {
	  while (quit==0)
	  {
	    if(Timer-tick>frame_limit) // fps monitor
	    {
	       fps=Frames;
	       Frames=0;
	       tick=Timer;
	       sprintf(fps_display,"Fps: %d",fps);
	    }  
	    now=Timer;
	    ticks=now-done;
	    if(ticks<1) continue;
	    if(ticks>10) ticks=10;
	    for (i=0; i<ticks-1; i++)
	    {
		quit|=DoFrame(0); // these frames are skipped
	    } 
	    if(ticks>=1)
	    {
		quit|=DoFrame(1); // this frame is rendered
	    }
	    done=now;
	  }
  }
  else  // fixed frame skip 
  {
     while (quit==0)
	  {
	    if(Timer-tick>frame_limit) // fps monitor
	    {
	       fps=Frames;
	       Frames=0;
	       tick=Timer;
	       sprintf(fps_display,"Fps: %d",fps);
	    }
	    for (i=0; i<menu_options.frameskip-1; i++)
	    {
		quit|=DoFrame(0); // these frames are skipped
	    } 
     	    quit|=DoFrame(1); // this frame is rendered
	  }
  }
  stop_timer4();
  
  return 0;
}