
#include "app.h"

char *RomDir="dev0:\\gpmm\\genesis\\";
static DIR dir;

int rom_list_loaded;
unsigned short cpu_speed_lookup[40]={ 22, 33, 40, 50, 66,
                              100,133,136,140,144,
			      146,150,154,156,160,
			      164,166,168,172,176,
			      180,184,188,192,196,
			      200,204,208,212,216,
			      220,224,228,232,236,
			      240,244,248,252,256};

static int Focus=0;
static int menuFocus=0;

static short menutile_xscroll=0;
static short menutile_yscroll=0;
static int HeaderDone[2];
static int menusmooth=0;
int quick_save_present=0;
struct RomList_Item romlist[512];
struct Menu_Options menu_options;

static int romcount;
int currentrom=2;

typedef struct 
{
  char text[128];
} MenuText;


MenuText drmd_menu[22];
static int menuMax=128,menuCount=22;

struct SaveState savestate[10];  // holds the filenames for the savestate and "inuse" flags
char savestate_name[256];       // holds the last filename to be scanned for save states


int MessageBox(char *message1,char *message2,char *message3,int mode)
{
  char text[256];
  int select=0;
  int subaction=-1;
  while(subaction==-1)
  {
     InputUpdate();
     if (Inp.repeat[INP_BUTTON_UP]) 
     {
       select^=1; // Up
     }
     if (Inp.repeat[INP_BUTTON_DOWN]) 
     {
       select^=1; // Down
     }
     if ((Inp.held[INP_BUTTON_A]==1) || (Inp.held[INP_BUTTON_B]==1))
     {
        subaction=select;
     }
     PrintTile(Flip);
     PrintTitle(Flip);
     gp_drawString(8,50,strlen(message1),message1,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
     gp_drawString(8,60,strlen(message2),message2,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
     gp_drawString(8,70,strlen(message3),message3,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
     switch(mode)
     {
        case 0: // yes no input
	       if(select==0)
	       {
	          PrintBar(Flip, 120-4);
	          gp_drawString(8,120,3,"YES",(unsigned short)RGB(0,0,0),framebuffer16[Flip]);
	          gp_drawString(8,140,2,"NO",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	       }
	       else
	       {
	          PrintBar(Flip, 140-4);
	          gp_drawString(8,120,3,"YES",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	          gp_drawString(8,140,2,"NO",(unsigned short)RGB(0,0,0),framebuffer16[Flip]);
	       
	       }
	       break;
     }
     gp_setFramebuffer(framebuffer16[Flip],1);
     Flip^=1;
  }
  return(subaction);
}

int deleterom(int romindex)
{
  struct RomList_Item dummy_romlist;
  char text[256];
  int x;

     PrintTile(Flip);
     PrintTitle(Flip);
     gp_setFramebuffer(framebuffer16[Flip],1);
     sprintf(text,"Deleting Rom..");
     gp_drawString(8,50,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
     sprintf(text,"%.120s%.120s",RomDir,romlist[romindex].shortname);
     gp_drawString(8,60,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
     smc_delete(text);
     sprintf(text,"Updating Rom List..");
     gp_drawString(8,70,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
     for(x=romindex;x<romcount;x++)
     {
        memcpy(&romlist[x],&romlist[x+1],sizeof(dummy_romlist));
     }
     romcount--;
     currentrom--;
     smc_write("dev0:\\GPMM\\DRMD\\romlist.bin",(char*)&romlist,romcount*sizeof(dummy_romlist)); 
     return(1);
}

void PrintTile(int Flip)
{
  short x=0,x2=0;
  short y=0,y2=0;
  unsigned short *framebuffer1 = framebuffer16[Flip];
  unsigned int *framebuffer2 = (unsigned int*)framebuffer16[Flip];
  unsigned short *graphics1 = NULL;
  unsigned int *graphics2 = NULL;

  // Just clear screen - scrolling background looked crap any how.
  if (1)//(menutile_yscroll&1)
  {
	  x2=(menutile_xscroll*menutile_height);
	  y2=menutile_yscroll;
	  graphics1 = menutile+x2;
	  for (x=0; x<(320); x++)
	  {
		  for (y=0; y<(240-48); y++)
		  {
			  *framebuffer1++ = graphics1[y2];
			  y2++;
			  y2&=(menutile_height-1);
		  }
		  x2+=menutile_height;
		  x2&=((menutile_height*menutile_width)-1);
		  graphics1=menutile+x2;
		  framebuffer1+=48;
	  }
  }
  else
  {
	  x2=(menutile_xscroll*(menutile_width>>1));
	  y2=menutile_yscroll>>1;
	  graphics2 = (unsigned int*)menutile+x2;
	  for (x=0; x<(320); x++)
	  {
		  for (y=0; y<(120-24); y++)
		  {
			  *framebuffer2++ = graphics2[y2];
			  y2++;
			  y2&=((menutile_height>>1)-1);
		  }
		  x2+=(menutile_height>>1);
		  x2&=(((menutile_height*menutile_width)>>1)-1);
		  graphics2=(unsigned int*)menutile+x2;
		  framebuffer2+=24;
	  }
  }
  menutile_xscroll++;
  if(menutile_xscroll>=menutile_width) menutile_xscroll=0;
  
  menutile_yscroll++;
  if(menutile_yscroll>=menutile_height) menutile_yscroll=0;
  
  return; 
}

void PrintTitle(int Flip)
{
  unsigned int *framebuffer = (unsigned int*)framebuffer16[Flip]+((240-48)>>1);
  unsigned int *graphics = (unsigned int*)DrMD_header;
  unsigned int x,y;
  //If header already drawn for this layer exit
  if (HeaderDone[Flip]) return;
  
  for (x=0; x<320; x++)
  {
	  for (y=0; y<24; y++)
	  {
		  *framebuffer++ = *graphics++;
	  }
	  framebuffer+=((240-48)>>1);
  }
  
  //HeaderDone[Flip] = 1;
}

void PrintBar(int Flip, unsigned int given_y)
{
  unsigned short *framebuffer1 = NULL;
  unsigned int *framebuffer2 = NULL;
  unsigned short *graphics1 = highlightbar;
  unsigned int *graphics2 = (unsigned int*)highlightbar;
  unsigned int x,y;

  if (1)
  {
	  framebuffer1 = framebuffer16[Flip]+(240-given_y-16);
	  for (x=0; x<320; x++)
	  {
		  for (y=0; y<16; y++)
		  {
			  *framebuffer1++ = *graphics1++;
		  }
		  framebuffer1+=(240-16);
	  }
  }
  else
  {
	  framebuffer2 = (unsigned int*)framebuffer16[Flip]+((240-given_y-16)>>1);
	  for (x=0; x<320; x++)
	  {
		  for (y=0; y<8; y++)
		  {
			  *framebuffer2++ = *graphics2++;
		  }
		  framebuffer2+=((240-16)>>1);
	  }
  }
}

int FileScan()
{
  int i=0,x=0,y=0,z=0;
  char *text1;
  char *text2;
  char filename[256];
  char romname_long[256];
  char text[256];
  char used[512];
  int filecount;
  int filesdone;
  unsigned char romname_len=0;
  struct RomList_Item dummy_romlist;
  unsigned int *pix;
  FILE *fd = NULL;
  GPFILE *gpfile;
  
  gp_clearFramebuffer16(framebuffer16[Flip],(unsigned short)RGB(0,0,0));
  PrintTitle(Flip);
  gp_setFramebuffer(framebuffer16[Flip],1);
  gp_drawString(8,120,25,"Getting Directory Info...",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  
  memset(&romlist,0,sizeof(romlist));
  memset(&used,0,sizeof(used));
  
  filecount=smc_dir(RomDir,&dir);  // get all files in directory ---128 max
  
  sprintf(romlist[0].longname,"Re-Scan Rom Directory");
  sprintf(romlist[1].longname,"Back To Main Menu");
  
  sprintf(romlist[2].longname,"");
  sprintf(romlist[2].shortname,"");
  romcount=3;
  

  // now try to find long filename in MD Rom list using CRC
  for(z=0;z<filecount;z++)
  {
      //find first name to check
      for(i=0;i<filecount;i++)
      {
        if(!used[i]) {y=i; break;}
      }
      for(i=0;i<filecount;i++)
      {
        if((strcmp(dir.name[y],dir.name[i])>0)&&(!used[i])) y=i;
      }
      
      used[y]=1;
      pix=(unsigned int*)framebuffer16[Flip]+56;
      for(x=0;x<320;x++)
      {
	   pix[0] = 0;
	   pix[1] = 0;
	   pix[2] = 0;
	   pix[3] = 0;
	   pix+=120;
      }
      sprintf(text,"Scanning Rom: %d of %d",z+1,filecount);
      gp_drawString(8,120,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);  

      if((dir.name[y][0]!='.')&&dir.size[y]!=0) // check for actual files
      {
	      sprintf(romlist[romcount].shortname,"%.120s",dir.name[y]);
	      romlist[romcount].filesize = dir.size[y];
	      
	      
	      sprintf(filename,"%.120s%.120s",RomDir,romlist[romcount].shortname);
	      if(check_zip(filename))
	      {
	        // file is zip file
	        romlist[romcount].type=1;
		// get CRC from zip info and lookup on crc list
		romlist[romcount].crc=get_archive_crc(filename);
		if(romlist[romcount].crc)
		{
	            x=DAT_LookFor(romlist[romcount].crc);
	            if(x!=-1) 
	            {
		       sprintf(romlist[romcount].longname,"%.120s",(char*)DAT_getname(x));
	            }
	            else 
	            { 
			// get filename from inside zip file
			get_archive_filename(filename,romlist[romcount].longname);
	            }
		}
		else
		{
		  get_archive_filename(filename,romlist[romcount].longname);
		}
	      }
	      else
	      {
	        romlist[romcount].type=0;
		// Get name of file from crc lookup list
	        smc_read(filename,RomData,0,romlist[romcount].filesize);
	        for(CRC=x=0;x<romlist[romcount].filesize;x++) GetCRC(RomData[x]);
	        romlist[romcount].crc = CRC;
	        x=DAT_LookFor(CRC);
	        if(x!=-1) 
	        {
		   sprintf(romlist[romcount].longname,"%.120s",(char*)DAT_getname(x));
	        }
	        else 
	        { 
		  memset(romlist[romcount].longname,0,sizeof(romlist[romcount].longname));
	        }
	      }
	      romcount++;
	}
  }
  Flip^=1;
  gp_clearFramebuffer16(framebuffer16[Flip],(unsigned short)RGB(0,0,0));
  PrintTitle(Flip);
  gp_drawString(8,120,17,"All roms scanned.",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  gp_drawString(8,130,24,"Saving romlist to smc...",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  gp_setFramebuffer(framebuffer16[Flip],1);
  Flip^=1;
  //gp_setCpuspeed(40);
  smc_write("dev0:\\GPMM\\DRMD\\romlist.bin",(char*)&romlist,romcount*sizeof(dummy_romlist)); 
  //gp_setCpuspeed(133);
  return romcount;
}

int load_rom_list()
{
  struct RomList_Item dummy_romlist;
  int err=0;
  int size=0;
  GPFILE *gpfile;
  if(rom_list_loaded) return(1);
  gpfile=(GPFILE*)smc_fopen("dev0:\\GPMM\\DRMD\\romlist.bin","rb");
  if(gpfile)
  {
     size = smc_filesize(gpfile);
     if(smc_fread((char*)&romlist, size, 1, gpfile)!=size)
     {
        smc_fclose(gpfile);
        return(0);
     }
     else
     {
        smc_fclose(gpfile);
        romcount=size/sizeof(dummy_romlist);
	rom_list_loaded=1;
	return(1);
     }     
  }
  else
  {
     return(0);
  }
}

int FileSelect(int mode)
{
  char text[256];
  int romname_length;
  int action=0;
  int smooth=0;
  unsigned short color=0;
  int i=0;
  
  if(!load_rom_list()) FileScan();
  
  memset(&text,0,sizeof(text));
  if(Focus<2) Focus=2; // default menu to non menu item
                       // just to stop directory scan being started 
  smooth=Focus<<8;
  /*while(1==1)
         {     
            InputUpdate();
	    if (!(Inp.held[0])&&!(Inp.held[1])&&
	        !(Inp.held[2])&&!(Inp.held[3])&&
		!(Inp.held[4])&&!(Inp.held[5])&&
		!(Inp.held[6])&&!(Inp.held[7])&&
		!(Inp.held[8])&&!(Inp.held[9]))
	       break;
         }*/
	 
  while (action==0)
  {
    InputUpdate();

    // Change which rom is focused on:
    if (Inp.repeat[INP_BUTTON_UP]) 
    {
       Focus--; // Up
    }
    if (Inp.repeat[INP_BUTTON_DOWN]) 
    {
       Focus++; // Down
    }
    

    // L+R=Back to game:
   if (Inp.held[INP_BUTTON_L] && Inp.held[INP_BUTTON_R]==1   ) action=1;
   else if (Inp.held[INP_BUTTON_L]==1 && Inp.held[INP_BUTTON_R]   ) action=1;
   
   if(Inp.repeat[INP_BUTTON_LEFT] || Inp.repeat[INP_BUTTON_RIGHT]   )
    {
      if(Inp.repeat[INP_BUTTON_LEFT]) 
      {
         Focus-=12;
	 smooth=(Focus<<8)-1;
      }      
      else if(Inp.repeat[INP_BUTTON_RIGHT])
      {
         Focus+=12;
	 smooth=(Focus<<8)-1;
      }   
      if (Focus>romcount-1) 
      {
         Focus=romcount-1;
         smooth=(Focus<<8)-1;
      }
      else if (Focus<0)
      {
         Focus=0;
         smooth=(Focus<<8)-1;
      }
    }

    
    //if (Inp.held[4]    && Inp.held[7]) action=1;
    if (Focus>romcount-1) 
    {
      Focus=0;
      smooth=(Focus<<8)-1;
    }
    else if (Focus<0)
    {
      Focus=romcount-1;
      smooth=(Focus<<8)-1;
    }
    else
    {
      
    }
    if ((Inp.held[INP_BUTTON_A]==1) || (Inp.held[INP_BUTTON_B]==1))
    {
      if(Focus==0)
      {
         FileScan();
	 
      }
      else if(Focus==1)
      {
        action=1;
      }
      else if(Focus==2)
      {
        // nothing blank entry
      }
      else
      {
        if(mode==0)
	{
         // A or Start, load rom
         currentrom=Focus;
	 quick_save_present=0;  // reset any quick saves
         action=2;
	}
	else if(mode==1)
	{
	 //delete current rom
	 if(romlist[Focus].longname[0]==0)
	    sprintf(text,"%s",romlist[Focus].shortname);
	 else
	    sprintf(text,"%.39s",romlist[Focus].longname);
	 if(MessageBox("Are you sure you want to delete",text,"",0)==0)
	 {
	    deleterom(Focus);
	 }
	}
      }
    }

    // Draw screen:

    //gp_clearFramebuffer16(framebuffer16[Flip],(unsigned short)RGB(0,0,0));
    PrintTile(Flip);
    PrintTitle(Flip);
    if(mode==0) gp_drawString(6,35,10,"Select Rom",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
    if(mode==1) gp_drawString(6,35,10,"Delete Rom",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 

    smooth=smooth*7+(Focus<<8); smooth>>=3;
    

    for (i=0;i<romcount;i++)
    {
      int x=0,y=0;
      
      y=(i<<4)-(smooth>>4);
      x=0;
      y+=112;
      if (y<=48 || y>=232) continue;
      
      memset(&text,0,sizeof(text));
      
      if (i==Focus)
      {
        color=(unsigned short)RGB(0,0,0);
		PrintBar(Flip,y-4);
      }
      else
      {
        color=(unsigned short)RGB(31,31,31);
      }
       
      if(romlist[i].longname[0]!=0)
      { // long name -  check for wrap.  if wrap rotate text
         romname_length=strlen(romlist[i].longname);
	 if(romname_length>40) romname_length=40;
         //if(sizeof(romlist[i].longname)>35)
	 gp_drawString(x,y,romname_length,romlist[i].longname,color,framebuffer16[Flip]); 
      }
      else 
      {
         gp_drawString(x,y,strlen(romlist[i].shortname),romlist[i].shortname,color,framebuffer16[Flip]); 
      }
    }

    gp_setFramebuffer(framebuffer16[Flip],1);
    Flip^=1;
  }

  return action;
}

static int scan_savestates(char *romname)
{
   GPFILE *gpfile;
   int i=0,c=0,len=0;
   char savename[256];
   char text[256];
   if(!strcmp(romname,savestate_name)) return 0; // is current save state rom so exit
   
   memset(&text,0,sizeof(text));

   len=-1;
   for(c=strlen(romname);c>0;c--)
   {
        if(romname[c]=='.')
	{
	  len=c+1;
	  break;
	}
    }
    if(len!=-1)
    {
         // file has extension
	 memcpy(&text,&romname[0],len);
	 sprintf(savename,"%sSV",text);
    }
    else
    {
         // does not have extension
	 sprintf(savename,"%s.SV",romname);
    }
      
   for(i=0;i<10;i++)
   {
      /*
       need to build a save state filename
       all saves are held in "dev0:\\GPMM\DrMD\\"
       save filename has following format
          shortname(minus file ext) + SV + saveno ( 0 to 9 )
         */
      sprintf(savestate[i].filename,"%s%d",savename,i);
      sprintf(savestate[i].fullfilename,"dev0:\\GPMM\\DRMD\\%s",savestate[i].filename);
      gpfile=(GPFILE*)smc_fopen(savestate[i].fullfilename,"rb");
      if(gpfile)
      {
         // we have a savestate
	 savestate[i].inuse = 1;
         smc_fclose(gpfile);	
      }
      else
      {
         // no save state
	 savestate[i].inuse = 0;
      }
   }
   strcpy(savestate_name,romname);  // save the last scanned romname
}

int savestate_mem(unsigned char *saveaddress)
{
    
    
    return(1);
}

/*
version 1.0 format

offset                        detail
0x00                          menu_options
0x14                           drmd context
23*4=92
6*2=12
52*1=62=
total=166=0x9C

*/
static
int loadstate_mem_v0(unsigned char* loadaddress)
{
   
}

int loadstate_mem_v1(unsigned char *loadaddress)
{
  
}

int loadstate_mem(unsigned char *loadaddress)
{
  
}

int loadstate_file(unsigned char *filename)
{
  GPFILE *gpfile;
  int size=0;
  int read=0;
  int ret=0;
  char text[256];
  ret=load_archive(filename, temp_state, &size, Flip^1);
  if(!ret)
  {
    gp_drawString(50,130,4,"fail",(unsigned short)RGB(31,31,31),framebuffer16[Flip^1]); // write to current
    while(1==1)
    {
    }
  }
  /*gpfile=(GPFILE*)smc_fopen(filename,"r");
  if(!gpfile)
  {
     gp_drawString(50,130,1,"1",(unsigned short)RGB(31,31,31),framebuffer16[Flip^1]); // write to current
     while(1==1)
     {
     }
     return(0);
  }
  size=smc_filesize(gpfile);
  read=smc_fread(temp_state,size,1,gpfile);
  if(read!=size)
  {
    smc_fclose(gpfile);
    sprintf(text,"size: %d read: %d",size,read);
    gp_drawString(50,130,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip^1]); // write to current
     while(1==1)
     {
     }
    return(0);
  }
  smc_fclose(gpfile); */
  if(temp_state[0]==0) loadstate_mem_v0(temp_state);
  else if(temp_state[0]==1) loadstate_mem_v1(temp_state);
  else loadstate_mem(temp_state);
  
  
  

  return(1);
}

static int savestate_file(char *filename)
{
  GPFILE *gpfile;
  int size=0;
  int read=0;
  char text[128];
  int ret=0;
  //gp_setCpuspeed(40);
  ret=save_archive(filename,current_state,savestatesize);
  //gp_setCpuspeed(133);
  if(!ret)
  {
    gp_drawString(50,130,10,"fail write",(unsigned short)RGB(31,31,31),framebuffer16[Flip^1]); // write to current
     while(1==1)
     {
     }
  }
  
  
  /*
  smc_write(filename,current_state,savestatesize);
  gpfile=(GPFILE*)smc_fopen(filename,"r");
  size=smc_filesize(gpfile);
  read=smc_fread(current_state,size,1,gpfile);
  smc_fclose(gpfile);
  if(read!=size) 
  {
     gp_drawString(50,130,10,"fail write",(unsigned short)RGB(31,31,31),framebuffer16[Flip^1]); // write to current
     while(1==1)
     {
     }
  }*/

  return(1);
}

static int savestate_menu(int mode)
{
   GPFILE *gpfile;
   int i=0,c=0,len=0;
   int x=0,y=0;
   char text[128];
   int action=11;
   int saveno=0;
   unsigned int delay=0;
   savestate_mem(current_state);  // save the current state of emulator
   if(currentrom<=2)
   {
      // no rom loaded
      // display error message and exit
      return(0);
   }
   scan_savestates(romlist[currentrom].shortname);

	 
   while (action!=0&&action!=100)
   {
	      InputUpdate();
	      if(Inp.held[INP_BUTTON_UP]==1) {saveno--; action=1;}
	      if(Inp.held[INP_BUTTON_DOWN]==1) {saveno++; action=1;}
	      if(saveno<-1) saveno=9;
	      if(saveno>9) saveno=-1;
	      
	      if((Inp.held[INP_BUTTON_R])&&(Inp.held[INP_BUTTON_L])) action=0; // exit
	      else if(((Inp.held[INP_BUTTON_B]==1)||(Inp.held[INP_BUTTON_A]==1))&&(saveno==-1)) action=0; // exit
	      else if((Inp.held[INP_BUTTON_A]==1)&&(mode==0)&&((action==2)||(action==5))) action=6;  // pre-save mode
	      else if((Inp.held[INP_BUTTON_A]==1)&&(mode==1)&&(action==5)) action=8;  // pre-load mode
	      else if((Inp.held[INP_BUTTON_A]==1)&&(mode==2)&&(action==5))
	      {
	         if(MessageBox("Are you sure you want to delete","this save?","",0)==0) action=13;  //delete slot with no preview
	      }
	      else if((Inp.held[INP_BUTTON_B]==1)&&(action==12)) action=3;  // preview slot mode
	      else if((Inp.held[INP_BUTTON_A]==1)&&(mode==1)&&(action==12)) action=8;  //load slot with no preview
	      else if((Inp.held[INP_BUTTON_A]==1)&&(mode==0)&&(action==12)) action=6;  //save slot with no preview
	      else if((Inp.held[INP_BUTTON_A]==1)&&(mode==2)&&(action==12))
              {
	         if(MessageBox("Are you sure you want to delete","this save?","",0)==0) action=13;  //delete slot with no preview
	      }
      
      gp_clearFramebuffer16(framebuffer16[Flip],(unsigned short)RGB(0,0,0));
      PrintTitle(Flip);
      if(mode==0) gp_drawString(6,35,10,"Save State",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
      if(mode==1) gp_drawString(6,35,10,"Load State",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
      if(mode==2) gp_drawString(6,35,12,"Delete State",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
      sprintf(text,"Press UP and DOWN to change save slot");
      gp_drawString(12,230,strlen(text),text,(unsigned short)RGB(31,15,5),framebuffer16[Flip]);
      
      if(saveno==-1) 
      {
	 if(action!=10&&action!=0) 
	 {
	   action=10;
	 }
      }
      else
      {
         PrintBar(Flip,60-4);
         sprintf(text,"SLOT %d",saveno);
         gp_drawString(136,60,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
      }
      
      switch(action)
	{
	case 1:
		//gp_drawString(112,145,14,"Checking....",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 2:
		gp_drawString(144,145,4,"FREE",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 3:
		gp_drawString(104,145,14,"Previewing....",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 4:
		gp_drawString(88,145,18,"Previewing....fail",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 5: 
		drmd.render_line = render_line_16_small;
		drmd.frame_buffer = (unsigned int)framebuffer16[Flip]+38400+62;
		current_sample=0;  // stops dac buffer from overflowing
	        last_sample=0;
		DrMDRun(1);  // if in preview mode - render frame
		if(mode==1) gp_drawString(100,210,15, "Press A to load",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		else if(mode==0) gp_drawString(80,210,20, "Press A to overwrite",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		else if(mode==2) gp_drawString(92,210,17, "Press A to delete",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 6:
		gp_drawString(124,145,9,"Saving...",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 7:
		gp_drawString(124,145,14,"Saving...Fail!",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 8:
		gp_drawString(116,145,11,"loading....",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
        case 9:
		gp_drawString(116,145,15,"loading....Fail",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 10:	
		PrintBar(Flip,145-4);
		gp_drawString(104,145,14,"Return To Menu",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 12:
		gp_drawString(124,145,9,"Slot used",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		gp_drawString(88,165,18,"Press B to preview",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		if(mode==1) gp_drawString(100,175,15, "Press A to load",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		else if(mode==0) gp_drawString(80,175,20, "Press A to overwrite",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		else if(mode==2) gp_drawString(92,175,17, "Press A to delete",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	case 13:
		gp_drawString(116,145,11,"Deleting....",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
		break;
	}
      
      gp_setFramebuffer(framebuffer16[Flip],1);
      Flip^=1;
      
      switch(action)
	{
	case 1:
		if(savestate[saveno].inuse) 
		{
		  action=12;
		}
		else 
		{
		 action=2;
		}
		break;
        case 3:
		if(loadstate_file(savestate[saveno].fullfilename))
		{
		   action=5;
		   drmd.gp32_pal=0x14A00400;
		   update_md_pal();
		}
		else action=4;
		break;
	case 6:
		if(savestate_file(savestate[saveno].fullfilename))
		{
			savestate[saveno].inuse=1;
			action=1;
		}
		else 
		{
			savestate[saveno].inuse=0;
			action=7;
		}
		break;
	case 7:
		action=1;
		break;
	case 8:
		if(loadstate_file(savestate[saveno].fullfilename))
		{	 
			action=100;  // loaded ok so exit
		}
		else 
		{ 
			action=9;
		}
		break;
	case 9:
	        action=1;
		break;
	case 11:
	        action=1;
		break;
	case 13:
	        smc_delete(savestate[saveno].fullfilename);
		savestate[saveno].inuse = 0;
		action=1;
		break;
	}
   }
   if(action==0) loadstate_mem(current_state);
   return(action);
}


int ConfigPad()
{
  char text[128];
  char md_conv[8]={0x01,
                   0x02,
		   0x04,
		   0x08,
		   0x40,
		   0x10,
		   0x20,
		   0x80};
  MenuText questions[8] ={ "Press Button for UP    : ",
                        "Press Button for DOWN  : ",
			"Press Button for LEFT  : ",
			"Press Button for RIGHT : ",
			"Press Button for A     : ",
			"Press Button for B     : ",
			"Press Button for C     :",
			"Press Button for START : "};
  MenuText but_name[10] = { "LEFT",
                        "DOWN",
			"RIGHT",
			"UP",
			"L",
			"B",
			"A",
			"R",
			"START",
			"SELECT"};
  int i=0;
  int question=0;
  int x=0,y=0,z=0;
  int but=0;
  
  // Draw screen:
  
  gp_clearFramebuffer16(framebuffer16[Flip],(unsigned short)RGB(0,0,0));
  PrintTitle(Flip);
  gp_setFramebuffer(framebuffer16[Flip],1);
  
  while(1==1)
         {     
            InputUpdate();
	    if (!(Inp.held[0])&&!(Inp.held[1])&&
	        !(Inp.held[2])&&!(Inp.held[3])&&
		!(Inp.held[4])&&!(Inp.held[5])&&
		!(Inp.held[6])&&!(Inp.held[7])&&
		!(Inp.held[8])&&!(Inp.held[9]))
	       break;
         }
	 
  for(z=0;z<10;z++)
  {
    menu_options.pad_config[z]=0;
  }
		
  
  x = 10;
  y = 50;
  
  for(question=0;question<8;question++)
  {
	  gp_drawString(x,y,25,questions[question].text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
	  for(i=0;i<1000;i++)  // deal with keybounce by checking a few times
	  {
	      while(1==1)
	      { 	
		InputUpdate();
		if (!(Inp.held[0])&&!(Inp.held[1])&&!(Inp.held[2])&&
		     !(Inp.held[3])&&!(Inp.held[4])&&!(Inp.held[5])&&
		     !(Inp.held[6])&&!(Inp.held[7])&&!(Inp.held[8])&&
		     !(Inp.held[9]))
		   break;
	      }
	  }
	  but=-1;
	  while(but==-1)
	  {     
		InputUpdate();
		
		for(z=0;z<10;z++)
		{
		  if(Inp.held[z])
		  {
		    but=z;
		    break;
		  }
		}
	  }
	  menu_options.pad_config[but]=md_conv[question];
	  gp_drawString(x+(25*8),y,strlen(but_name[but].text),but_name[but].text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
	  y+=10;
  }
  
  
  for(i=0;i<1000;i++)  // deal with keybounce by checking a few times
  {
      while(1==1)
      {     
	InputUpdate();
	if (!(Inp.held[5])&&!(Inp.held[6]))
	   break;
      }
  }
  sprintf(text,"Controls Configured!"); 
  gp_drawString(x,y+10,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  sprintf(text,"Press any A or B to return to menu"); 
  gp_drawString(x,y+20,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  while(1==1)
  {     
	InputUpdate();
	if ((Inp.held[5])||(Inp.held[6]))
	  break;
  }
    
    
  return 0;
}

static update_menu_text(void)
{

	sprintf(drmd_menu[RETURN_MENU_ITEM_MENU_ITEM].text,"%s","Return to Game");
	sprintf(drmd_menu[QUICK_LOAD_MENU_ITEM].text,"%s","Quick Load");
	sprintf(drmd_menu[QUICK_SAVE_MENU_ITEM].text,"%s","Quick Save");
	sprintf(drmd_menu[ROM_SELECT_MENU_ITEM].text,"Select Rom");
	sprintf(drmd_menu[ROM_DELETE_MENU_ITEM].text,"Delete Rom");
	switch(menu_options.sound_on)
	{
		case 0:
			sprintf(drmd_menu[SOUND_MENU_ITEM].text,"Sound: OFF");
			break;
		case 1:
			sprintf(drmd_menu[SOUND_MENU_ITEM].text,"Sound: ON");
			break;  
		case 2:
			sprintf(drmd_menu[SOUND_MENU_ITEM].text,"Sound: FM");
			break; 
		case 3:
			sprintf(drmd_menu[SOUND_MENU_ITEM].text,"Sound: PSG and FM");
			break; 	     
	}
 
	sprintf(drmd_menu[CPU_MENU_ITEM].text,"Cpu Speed: %dMhz",cpu_speed_lookup[menu_options.cpu_speed]);
  
	switch(menu_options.force_region)
	{
		case 0:
			sprintf(drmd_menu[REGION_MENU_ITEM].text,"Region: Auto");
			break;
		case 1:
			sprintf(drmd_menu[REGION_MENU_ITEM].text,"Region: Usa 60 fps");
			break;  
		case 2:
			sprintf(drmd_menu[REGION_MENU_ITEM].text,"Region: Europe 50 fps");
			break; 
		case 3:
			sprintf(drmd_menu[REGION_MENU_ITEM].text,"Region: Japan 60 fps");
			break; 
		case 4:
			sprintf(drmd_menu[REGION_MENU_ITEM].text,"Region: Japan 50 fps");
			break; 
	}
	
	switch(menu_options.show_fps)
	{
		case 0:
			sprintf(drmd_menu[FPS_MENU_ITEM].text,"Show FPS: OFF");
			break;
		case 1:
			sprintf(drmd_menu[FPS_MENU_ITEM].text,"Show FPS: ON");
			break;  	     
	}
	
	sprintf(drmd_menu[GAMMA_MENU_ITEM].text,"Brightness: %d",menu_options.gamma);

	switch(menu_options.lcdver)
	{
		case 0:
			sprintf(drmd_menu[LCD_MENU_ITEM].text,"LCD: Samsung");
			break;
		case 1:
			sprintf(drmd_menu[LCD_MENU_ITEM].text,"LCD: Taiwanese");
			break;
	}	
 
	sprintf(drmd_menu[CONTROLS_MENU_ITEM].text,"%s","Configure Controls");
	sprintf(drmd_menu[LOAD_STATE_MENU_ITEM].text,"%s","Load State");
	sprintf(drmd_menu[SAVE_STATE_MENU_ITEM].text,"%s","Save State");
	sprintf(drmd_menu[DELETE_STATE_MENU_ITEM].text,"%s","Delete State");
	sprintf(drmd_menu[SAVE_SETTINGS_MENU_ITEM].text,"%s","Save Settings");
	sprintf(drmd_menu[RESET_GAME_MENU_ITEM].text,"%s","Reset Game");  
	sprintf(drmd_menu[QUIT_MENU_ITEM].text,"%s","Exit DrMD");
	
	if(menu_options.frameskip==0)
	{
		sprintf(drmd_menu[FRAMESKIP_MENU_ITEM].text,"Frameskip: AUTO");
	}
	else
	{
		sprintf(drmd_menu[FRAMESKIP_MENU_ITEM].text,"Frameskip: %d",menu_options.frameskip-1);
	}

	sprintf(drmd_menu[SAVE_SETTINGS_FOR_GAME_MENU_ITEM].text,"%s","Save Settings For This Game");
	sprintf(drmd_menu[DELETE_SETTINGS_FOR_GAME_MENU_ITEM].text,"%s","Delete Settings For This Game");
	switch(menu_options.sound_rate)
	{
		case 0:
			sprintf(drmd_menu[SOUND_RATE_MENU_ITEM].text,"Sound Rate: 8250");
			break;
		case 1:
			sprintf(drmd_menu[SOUND_RATE_MENU_ITEM].text,"Sound Rate: 11025");
			break;
		case 2:
			sprintf(drmd_menu[SOUND_RATE_MENU_ITEM].text,"Sound Rate: 16500");
			break;
		case 3:
			sprintf(drmd_menu[SOUND_RATE_MENU_ITEM].text,"Sound Rate: 22050");
			break;  	         	     
	}
	
	switch(menu_options.autosram)
	{
		case 0:
			sprintf(drmd_menu[AUTO_SRAM_MENU_ITEM].text,"%s","Auto Load Sram: OFF");
			break;
		case 1:
			sprintf(drmd_menu[AUTO_SRAM_MENU_ITEM].text,"%s","Auto Load Sram: ON");
			break;
	}
	
	sprintf(drmd_menu[LOAD_SRAM_MENU_ITEM].text,"%s","Load Sram");
	sprintf(drmd_menu[SAVE_SRAM_MENU_ITEM].text,"%s","Save Sram");
	sprintf(drmd_menu[DELETE_SRAM_MENU_ITEM].text,"%s","Delete Sram");
}

int menu()
{
  char text[256];
  int i=0;
  unsigned long read=0;
  int len=0,c=0;
  int action=0;
  int subaction=0;
  
  unsigned short color=0;
  
  //Make sure flip not 3
  if(Flip) Flip=1;
  
  memset(&text,0,sizeof(text));

  menusmooth=menuFocus<<8;
  gp_setCpuspeed(133);
  gp_initFramebuffer(framebuffer16[Flip],16,60,menu_options.lcdver); // 16bit screen mode, refresh 50

  update_menu_text();

  while (action==0)
  {
    InputUpdate();

    
    
    // Change which rom is focused on:
    if (Inp.repeat[3]) menuFocus--; // Up
    if (Inp.repeat[1]) menuFocus++; // Down
    
    // L+R=Back to game:
    if (Inp.held[INP_BUTTON_L]==1 && Inp.held[INP_BUTTON_R]   ) action=1;
    else if (Inp.held[INP_BUTTON_L]    && Inp.held[INP_BUTTON_R]==1) action=1;

    if(Inp.repeat[INP_BUTTON_LEFT] || Inp.repeat[INP_BUTTON_RIGHT]   )
    {
      if(Inp.repeat[INP_BUTTON_LEFT]) 
      {
         menuFocus-=12;
	 menusmooth=(menuFocus<<8)-1;
      }      
      else if(Inp.repeat[INP_BUTTON_RIGHT])
      {
         menuFocus+=12;
	 menusmooth=(menuFocus<<8)-1;
      }   
      if (menuFocus>menuCount-1) 
      {
         menuFocus=menuCount-1;
         menusmooth=(menuFocus<<8)-1;
      }
      else if (menuFocus<0)
      {
         menuFocus=0;
         menusmooth=(menuFocus<<8)-1;
      }
    }
    
    if (menuFocus>menuCount-1)
    {
       menuFocus=0;
       menusmooth=(menuFocus<<8)-1;
    }   
    else if (menuFocus<0) 
    {
       menuFocus=menuCount-1;
       menusmooth=(menuFocus<<8)-1;
    }

    if (Inp.repeat[INP_BUTTON_A] || Inp.repeat[INP_BUTTON_B])
    {
      switch(menuFocus)
      {
      case ROM_SELECT_MENU_ITEM:
         subaction=FileSelect(0);
	 if(subaction==2) action=2;
	 break;
      case ROM_DELETE_MENU_ITEM:
         subaction=FileSelect(1);
	 break;
#ifdef EMU_C68K
      case QUICK_LOAD_MENU_ITEM:
        if(quick_save_present)
	{
          loadstate_mem(quick_state);
	  action=1;
	}
	break;
      case QUICK_SAVE_MENU_ITEM:
        if(currentrom>2) // only do save if rom loaded
	{
          savestate_mem(quick_state);
	  quick_save_present=1;
	  action=1;
	}
	break;
      case LOAD_STATE_MENU_ITEM:
	subaction=savestate_menu(1);  // load mode
	if(subaction==100) action=1; // exit menu back to game
	break;
      case SAVE_STATE_MENU_ITEM:
	savestate_menu(0);  // save mode
	break;
      case DELETE_STATE_MENU_ITEM:
	savestate_menu(2);  // save mode
	break;
#endif
      case SOUND_MENU_ITEM:
         if (Inp.held[6]==1)
	 {
             menu_options.sound_on++;
	     if(menu_options.sound_on>1) menu_options.sound_on=0;
	 }
	 else
	 {
	     menu_options.sound_on--;
	     if(menu_options.sound_on>1) menu_options.sound_on=1;
	 }
	 update_menu_text();
	 break;
      case SOUND_RATE_MENU_ITEM:
         if (Inp.held[6]==1)
	 {
             menu_options.sound_rate++;
	     if(menu_options.sound_rate>3) menu_options.sound_rate=0;
	 }
	 else
	 {
	     menu_options.sound_rate--;
	     if(menu_options.sound_rate>3) menu_options.sound_rate=3;
	 }
	 update_menu_text();
	 break;
      case CPU_MENU_ITEM:
         if (Inp.held[6]==1)
	 {
           menu_options.cpu_speed++;
	   if (menu_options.cpu_speed>39) menu_options.cpu_speed=0;
	 }
	 else
	 {
	   menu_options.cpu_speed--;
	   if (menu_options.cpu_speed>39) menu_options.cpu_speed=39;
	 }
         
         update_menu_text();
	 break;
            
      case REGION_MENU_ITEM:
              menu_options.force_region++;
	      if (menu_options.force_region>4) menu_options.force_region=0;
	      update_menu_text();
	break;
      case FPS_MENU_ITEM:
              menu_options.show_fps^=1;
	      update_menu_text();
	break;
      case GAMMA_MENU_ITEM:
	      if (Inp.held[INP_BUTTON_A])
	      {
                 menu_options.gamma++;
	         if (menu_options.gamma>28) menu_options.gamma=28;
	      }
	      else
	      {
	         menu_options.gamma--;
	         if (menu_options.gamma>28) menu_options.gamma=0;
	      }
	      update_menu_text();
	      update_menu_graphics_gamma();
	break;
      case CONTROLS_MENU_ITEM:
        subaction=ConfigPad(); 
	break;
      case LCD_MENU_ITEM:
         menu_options.lcdver^=1;
	 gp_initFramebuffer(framebuffer16[Flip^1],16,60,menu_options.lcdver); // 16bit screen mode, refresh 50
         update_menu_text();
	 break;
      /*case STEREO_MENU_ITEM:
         menu_options.stereo^=1;
         update_menu_text();
	 break;*/
      case SAVE_SETTINGS_MENU_ITEM:
        gp_drawSprite ( (unsigned short*)highlightbar,0, 240-16, framebuffer16[Flip^1], 320, 16 );
        sprintf(text,"Saving menu options to smc...");
	gp_drawString(40,228,sizeof(text),text,(unsigned short)RGB(0,0,0),framebuffer16[Flip^1]);
	//gp_setCpuspeed(40);
	smc_write("dev0:\\GPMM\\DRMD\\menuopts.bin",(char*)&menu_options,sizeof(menu_options));
	//gp_setCpuspeed(133);
	break;
      case RETURN_MENU_ITEM_MENU_ITEM:
	action=1; // return to game
	break;
      case RESET_GAME_MENU_ITEM:
        reset_drmd();
	action=1; // return to game
	break;
      case QUIT_MENU_ITEM:
	action=3;  // exit DrMD
	break; 
      case FRAMESKIP_MENU_ITEM:
	      if (Inp.held[INP_BUTTON_A])
	      {
                 menu_options.frameskip++;
	         if (menu_options.frameskip>6) menu_options.frameskip=6;
	      }
	      else
	      {
	         menu_options.frameskip--;
	         if (menu_options.frameskip>6) menu_options.frameskip=0;
	      }
	      update_menu_text();
	break;
      case SAVE_SETTINGS_FOR_GAME_MENU_ITEM:
        if(currentrom>2) // only save settings if game loaded because we need current rom filename
	{
		char inifile[256];
		gp_drawSprite ( (unsigned short*)highlightbar,0, 240-16, framebuffer16[Flip^1], 320, 16 );
		sprintf(text,"Saving options for game to smc...");
		gp_drawString(40,228,sizeof(text),text,(unsigned short)RGB(0,0,0),framebuffer16[Flip^1]);
		memset(&text,0,sizeof(text));
		len=-1;
		for(c=strlen(romlist[currentrom].shortname);c>0;c--)
		{
			if(romlist[currentrom].shortname[c]=='.')
			{
			  len=c+1;
			  break;
			}
		}
		if(len!=-1)
		{
			// file has extension
			 memcpy(&text,&romlist[currentrom].shortname,len);
			 sprintf(inifile,"dev0:\\GPMM\\DRMD\\%sINI",text);
		}
		else
		{
			 // does not have extension
			 sprintf(inifile,"dev0:\\GPMM\\DRMD\\%s.INI",romlist[currentrom].shortname);
		}
    
		
		//gp_setCpuspeed(40);
		smc_write(inifile,(char*)&menu_options,sizeof(menu_options));
		//gp_setCpuspeed(133);
	}
	break;
	case DELETE_SETTINGS_FOR_GAME_MENU_ITEM:
        if(currentrom>2) // only save settings if game loaded because we need current rom filename
	{
		char inifile[256];
		gp_drawSprite ( (unsigned short*)highlightbar,0, 240-16, framebuffer16[Flip^1], 320, 16 );
		sprintf(text,"Deleting options for game..");
		gp_drawString(40,228,sizeof(text),text,(unsigned short)RGB(0,0,0),framebuffer16[Flip^1]);
		memset(&text,0,sizeof(text));
		len=-1;
		for(c=strlen(romlist[currentrom].shortname);c>0;c--)
		{
			if(romlist[currentrom].shortname[c]=='.')
			{
			  len=c+1;
			  break;
			}
		}
		if(len!=-1)
		{
			// file has extension
			 memcpy(&text,&romlist[currentrom].shortname,len);
			 sprintf(inifile,"dev0:\\GPMM\\DRMD\\%sINI",text);
		}
		else
		{
			 // does not have extension
			 sprintf(inifile,"dev0:\\GPMM\\DRMD\\%s.INI",romlist[currentrom].shortname);
		}
    
		
		//gp_setCpuspeed(40);
		smc_delete(inifile);
		//gp_setCpuspeed(133);
	}
	break;
       }	
    }

    // Draw screen:
    //gp_clearFramebuffer16(framebuffer16[Flip],(unsigned short)RGB(0,0,0));
    PrintTile(Flip);
    PrintTitle(Flip);
    gp_drawString(6,35,9,"Main Menu",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
    menusmooth=menusmooth*7+(menuFocus<<8); menusmooth>>=3;
    // RRRRRGGGGGBBBBBI  gp32 color format
    for (i=0;i<menuCount;i++)
    {
      int x=0,y=0;

      

      y=(i<<4)-(menusmooth>>4);
      x=8;
      y+=112;

      if (y<=48 || y>=232) continue;
      
      if (i==menuFocus)
      {
        color=(unsigned short)RGB(0,0,0);
	gp_drawSprite ( (unsigned short*)highlightbar,0, y-4, framebuffer16[Flip], 320, 16 );
      }
      else
      {
        color=(unsigned short)RGB(31,31,31);
      }
      
      memset(&text,0,sizeof(text));
      sprintf(text,"%s",drmd_menu[i].text);
      gp_drawString(x,y,sizeof(text),text,color,framebuffer16[Flip]);
    }

    gp_setFramebuffer(framebuffer16[Flip],1);  // WAIT FOR VSYNC
    Flip^=1;
       
  }
  
  return action;
}

