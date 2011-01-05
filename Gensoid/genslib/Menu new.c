
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


MenuText drmd_menu[MAIN_MENU_ITEM_COUNT];
static int menuMax=128,menuCount=MAIN_MENU_ITEM_COUNT;

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
  
  HeaderDone[Flip] = 1;
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
  
  gp_setCpuspeed(MENU_FAST_CPU_SPEED);
  
  PrintTile(Flip);
  PrintTitle(Flip);
  gp_drawString(8,120,25,"Getting Directory Info...",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  gp_setFramebuffer(framebuffer16[Flip],1);
  Flip^=1;
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
      PrintTile(Flip);
	  PrintTitle(Flip);
      sprintf(text,"Scanning Rom: %d of %d",z+1,filecount);
      gp_drawString(8,120,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
	  gp_setFramebuffer(framebuffer16[Flip],1);
	  Flip^=1;

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
  PrintTile(Flip);
  PrintTitle(Flip);
  gp_drawString(8,120,17,"All roms scanned.",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  gp_drawString(8,130,24,"Saving romlist to smc...",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  gp_setFramebuffer(framebuffer16[Flip],1);
  Flip^=1;
  smc_write("dev0:\\GPMM\\DRMD\\romlist.bin",(char*)&romlist,romcount*sizeof(dummy_romlist)); 
  gp_setCpuspeed(MENU_CPU_SPEED);
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
  
  if(Focus<2) Focus=2; // default menu to non menu item
                       // just to stop directory scan being started 
  smooth=Focus<<8;

	 
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
    unsigned char* writeaddress;
    unsigned int current_dt_tab;
    unsigned int reg;
    memset(saveaddress,0,savestatesize);
    writeaddress=saveaddress;
    menu_options.menu_ver=CURRENT_MENU_VER; //always default this to current version
                             // incase it gets overwritten
    memcpy(writeaddress,&menu_options,sizeof(menu_options));
    writeaddress+=sizeof(menu_options);
    
    memcpy(writeaddress,&drmd,sizeof(drmd));
    writeaddress+=sizeof(drmd);
    
    memcpy(writeaddress,&drz80,sizeof(drz80));
    writeaddress+=sizeof(drz80);
#ifdef EMU_C68K    
    memcpy(writeaddress,&cyclone,sizeof(cyclone));
    writeaddress+=sizeof(cyclone);
#endif   

#ifdef EMU_M68K
    reg=m68k_get_reg(NULL,M68K_REG_D0);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_D1);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_D2);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_D3);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_D4);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_D5);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_D6);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_D7);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_A0);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_A1);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_A2);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_A3);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_A4);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_A5);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_A6);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_A7);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_PC);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
    reg=m68k_get_reg(NULL,M68K_REG_SR);
    memcpy(writeaddress,&reg,4);
    writeaddress+=4;
#endif
    memcpy(writeaddress,&work_ram,0x10000);
    writeaddress+=0x10000;
    
    memcpy(writeaddress,&vram,0x10000);
    writeaddress+=0x10000;
    
    memcpy(writeaddress,&zram,0x4000);
    writeaddress+=0x4000;
    
    memcpy(writeaddress,&cram,0x80);
    writeaddress+=0x80;
    
    memcpy(writeaddress,&vsram,0x80);
    writeaddress+=0x80;
    
    memcpy(writeaddress,&SL3,sizeof(SL3));
    writeaddress+=sizeof(SL3);
    
    memcpy(writeaddress,&ST,sizeof(ST));
    writeaddress+=sizeof(ST);
    
    memcpy(writeaddress,&OPN,sizeof(OPN));
    writeaddress+=sizeof(OPN);
    
    memcpy(writeaddress,&CH,sizeof(CH));
    writeaddress+=sizeof(CH);
    
    memcpy(writeaddress,&dacout,4);
    writeaddress+=4;
    
    memcpy(writeaddress,&dacen,4);
    writeaddress+=4;
    
    memcpy(writeaddress,&OPN_pan,(6*2));
    writeaddress+=(6*2);
    
    memcpy(writeaddress,&PSG,sizeof(PSG));
    writeaddress+=sizeof(PSG);
    
    current_dt_tab = (unsigned int)&YMOPN_ST_dt_tab[0];
    memcpy(writeaddress,&current_dt_tab,4);
    writeaddress+=4;
	
	memcpy(writeaddress,&sram,0x10000);
    writeaddress+=0x10000;
    
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
   int x=0,y=0;
   loadaddress+=0x14; //skip menu options
   loadaddress+=(4*21);
   
   //memcpy(&drmd.m68k_aim,loadaddress,4);
   //loadaddress+=4;
   //memcpy(&drmd.m68k_total,loadaddress,4);
   //loadaddress+=(4*17); // skip to zbank
   //memcpy(&drmd.zbank,loadaddress,4);
   loadaddress+=(4*2)+(2*2); // skip to vdp status
   memcpy(&drmd.vdp_status,loadaddress,2);
   loadaddress+=2; // skip to vdp addr
   memcpy(&drmd.vdp_addr,loadaddress,2);
   loadaddress+=2; // skip to vdp addr letch
   memcpy(&drmd.vdp_addr_latch,loadaddress,2);
   loadaddress+=8; // skip to pad
   memcpy(&drmd.pad,loadaddress,61); // copy all as they should be the same

   loadaddress+=64; // skip to drz80
   
   memcpy(&drz80,loadaddress,sizeof(drz80));
   loadaddress+=sizeof(drz80);
#ifdef EMU_C68K
    memcpy(&cyclone,loadaddress,sizeof(cyclone));
    loadaddress+=sizeof(cyclone);
    //0x1BC
#endif
    memcpy(&work_ram,loadaddress,0x10000);
    loadaddress+=0x10000;
    
    memcpy(&vram,loadaddress,0x10000);
    loadaddress+=0x10000;
    
    memcpy(&zram,loadaddress,0x4000);
    loadaddress+=0x4000;
    //0x241BC
    memcpy(&cram,loadaddress,0x80);
    loadaddress+=0x80;
    
    memcpy(&vsram,loadaddress,0x80);
    loadaddress+=0x80;
    
    //0x242BC
    memcpy(&SL3.fc[0],loadaddress,12);
    loadaddress+=12;
    memcpy(&SL3.fn_h,loadaddress,1);
    loadaddress+=1;
    memcpy(&SL3.kcode[0],loadaddress,3);
    loadaddress+=3;
    memcpy(&SL3.block_fnum[0],loadaddress,12);
    loadaddress+=12;
    
    //0x242D8
    memcpy(&ST.address,loadaddress,1);
    loadaddress+=1;
    memcpy(&ST.irq,loadaddress,1);
    loadaddress+=1;
    memcpy(&ST.irqmask,loadaddress,1);
    loadaddress+=1;
    memcpy(&ST.status,loadaddress,1);
    loadaddress+=1;
    memcpy(&ST.mode,loadaddress,4);
    loadaddress+=4;
    memcpy(&ST.prescaler_sel,loadaddress,1);
    loadaddress+=1;
    memcpy(&ST.fn_h,loadaddress,1);
    loadaddress+=1;
    memcpy(&ST.TB,loadaddress,1);
    loadaddress+=2; // align for next word
    memcpy(&ST.TA,loadaddress,4);
    loadaddress+=4;
    ST.TA_Count =0;
    loadaddress+=4;
    ST.TA_Base = (1024 - ST.TA) << (12);
    loadaddress+=4;
    ST.TB_Count =0;
    loadaddress+=4;
    ST.TB_Base = (256 - ST.TB) << (4 + 12);
    loadaddress+=4;
    //0x242F8
    memcpy(&OPN,loadaddress,sizeof(OPN));
    loadaddress+=sizeof(OPN);
    //0x24310
    for(x=0;x<6;x++)
    {    
    memcpy(&CH[x].ALGO,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].FB,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].ams,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].kcode,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].op1_out[0],loadaddress,8);
    loadaddress+=8;
    memcpy(&CH[x].mem_value,loadaddress,4);
    loadaddress+=4;
    for(y=0;y<4;y++)
    {
    memcpy(&CH[x].SLOT[y].eg_sh_active_mask,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].volume,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].state,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].eg_sel_ar,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].eg_sh_ar,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].eg_sel_d1r,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].eg_sh_d1r,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].eg_sel_d2r,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].eg_sh_d2r,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].eg_sel_rr,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].eg_sh_rr,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].ssg,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].ssgn,loadaddress,1);
    loadaddress+=2;
    memcpy(&CH[x].SLOT[y].sl,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].eg_sh_d1r_mask,loadaddress,4);
    loadaddress+=4;//28
    memcpy(&CH[x].SLOT[y].eg_sh_d2r_mask,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].eg_sh_rr_mask,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].eg_sh_ar_mask,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].tl,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].vol_out,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].AMmask,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].phase,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].Incr,loadaddress,4);
    loadaddress+=4;
    
    memcpy(&CH[x].SLOT[y].DT,loadaddress,4); // need to work outoffset
    CH[x].SLOT[y].DT=(int*)(((unsigned int)&YMOPN_ST_dt_tab[0])+((unsigned int)(CH[x].SLOT[y].DT)-0xC0D9E68));
      
    //0xC0D9E68 = YMOPN_ST_dt_tb in V1.0
    
    

    
    
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].mul,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].KSR,loadaddress,1);
    loadaddress+=1;
    memcpy(&CH[x].SLOT[y].ksr,loadaddress,1);
    loadaddress+=3;
    memcpy(&CH[x].SLOT[y].key,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].ar,loadaddress,4);
    loadaddress+=4;//80
    memcpy(&CH[x].SLOT[y].d1r,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].d2r,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].SLOT[y].rr,loadaddress,4);
    loadaddress+=4;//92
    }
    memcpy(&CH[x].pms,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].fc,loadaddress,4);
    loadaddress+=4;
    memcpy(&CH[x].block_fnum,loadaddress,4);
    loadaddress+=4; // 28
    }
    memcpy(&dacout,loadaddress,sizeof(dacout));
    loadaddress+=sizeof(dacout);
    
    memcpy(&dacen,loadaddress,sizeof(dacen));
    loadaddress+=sizeof(dacen);
    
    memcpy(&OPN_pan,loadaddress,(6*2));
    loadaddress+=(6*2);
    
    memcpy(&PSG.sn_Register,loadaddress,32);
    loadaddress+=32;
    
    memcpy(&PSG.sn_LastRegister,loadaddress,4);
    loadaddress+=4;
    
    memcpy(&PSG.sn_Volume,loadaddress,16);
    loadaddress+=16;
    
    memcpy(&PSG.sn_RNG,loadaddress,4);
    loadaddress+=4;
    
    memcpy(&PSG.sn_NoiseFB,loadaddress,4);
    loadaddress+=4;
    
    memcpy(&PSG.sn_Period,loadaddress,16);
    loadaddress+=16;
    
    memcpy(&PSG.sn_Count,loadaddress,16);
    loadaddress+=16;
    
    memcpy(&PSG.sn_Output,loadaddress,16);
    loadaddress+=16;
    
    
    
    DrMD_Init();
#ifdef EMU_C68K
    Cyclone_Init();
#endif
    DrZ80_Init();
#ifdef EMU_C68K 
    // update cyclone pointers
    cyclone.pc=DrMDCheckPc(cyclone.pc);  // rebase pc
#endif
    // update drz80 pointers
    drz80.Z80PC=drz80.Z80PC-drz80.Z80PC_BASE;
    drz80.Z80PC=Z80_Rebase_PC(drz80.Z80PC);
    
    drz80.Z80SP=drz80.Z80SP-drz80.Z80SP_BASE;
    drz80.Z80SP=Z80_Rebase_SP(drz80.Z80SP);
    
    // now need to update pointers to memory locations because these may be
    // different from when the save state was created.


    
    // re-sync gp32 pal and md pal;
    update_md_pal();
    // reset all menu graphics to the current gamma
    update_menu_graphics_gamma();
}

int loadstate_mem_v1(unsigned char *loadaddress)
{
   unsigned char* writeaddress;
   int x,y;
    writeaddress=loadaddress;
    
    memcpy(&menu_options,writeaddress,sizeof(menu_options));
    writeaddress+=sizeof(menu_options);
    
    memcpy(&drmd,writeaddress,sizeof(drmd));
    writeaddress+=sizeof(drmd);
    
    memcpy(&drz80,writeaddress,sizeof(drz80));
    writeaddress+=sizeof(drz80);
#ifdef EMU_C68K
    memcpy(&cyclone,writeaddress,sizeof(cyclone));
    writeaddress+=sizeof(cyclone);
#endif
    memcpy(&work_ram,writeaddress,0x10000);
    writeaddress+=0x10000;
    
    memcpy(&vram,writeaddress,0x10000);
    writeaddress+=0x10000;
    
    memcpy(&zram,writeaddress,0x4000);
    writeaddress+=0x4000;
    
    memcpy(&cram,writeaddress,0x80);
    writeaddress+=0x80;
    
    memcpy(&vsram,writeaddress,0x80);
    writeaddress+=0x80;
    
    memcpy(&SL3,writeaddress,sizeof(SL3));
    writeaddress+=sizeof(SL3);
    
    memcpy(&ST,writeaddress,sizeof(ST));
    writeaddress+=sizeof(ST);
    
    memcpy(&OPN,writeaddress,sizeof(OPN));
    writeaddress+=sizeof(OPN);
    
    memcpy(&CH,writeaddress,sizeof(CH));
    writeaddress+=sizeof(CH);
    
    memcpy(&dacout,writeaddress,sizeof(dacout));
    writeaddress+=sizeof(dacout);
    
    memcpy(&dacen,writeaddress,sizeof(dacen));
    writeaddress+=sizeof(dacen);
    
    memcpy(&OPN_pan,writeaddress,(6*2));
    writeaddress+=(6*2);
    
    memcpy(&PSG,writeaddress,sizeof(PSG));
    writeaddress+=sizeof(PSG);

    

    //0xC1052F0 location of YMOPN_ST_dt_tb in beta 7
    for(x=0;x<6;x++)
    {
    for(y=0;y<4;y++)
    {
    CH[x].SLOT[y].DT=(int*)(((unsigned int)&YMOPN_ST_dt_tab[0])+((unsigned int)(CH[x].SLOT[y].DT)-0xC1052F0));
    }
    }
    //update direct memory pointers, this is because
    // for example if DrMD is compiled with slight changes
    // the location of RomData or main md memory is
    // moved but the Z80PC will still be pointing at a memory
    // address based on the old location of rom data
    
    DrMD_Init();
#ifdef EMU_C68K
    Cyclone_Init();
#endif
    DrZ80_Init();
#ifdef EMU_C68K 
    // update cyclone pointers
    cyclone.pc=DrMDCheckPc(cyclone.pc);  // rebase pc
#endif
    // update drz80 pointers
    drz80.Z80PC=drz80.Z80PC-drz80.Z80PC_BASE;
    drz80.Z80PC=Z80_Rebase_PC(drz80.Z80PC);
    
    drz80.Z80SP=drz80.Z80SP-drz80.Z80SP_BASE;
    drz80.Z80SP=Z80_Rebase_SP(drz80.Z80SP);
    
    // now need to update pointers to memory locations because these may be
    // different from when the save state was created.


    
    // re-sync gp32 pal and md pal;
    update_md_pal();
    // reset all menu graphics to the current gamma
    update_menu_graphics_gamma();
}

int loadstate_mem_v2(unsigned char *loadaddress)
{
   unsigned char* writeaddress;
   int x=0,y=0;
   unsigned int old_DT_Table;
    writeaddress=loadaddress;
    
    memcpy(&menu_options,writeaddress,sizeof(menu_options));
    writeaddress+=sizeof(menu_options);
    
    memcpy(&drmd,writeaddress,sizeof(drmd));
    writeaddress+=sizeof(drmd);
    
    memcpy(&drz80,writeaddress,sizeof(drz80));
    writeaddress+=sizeof(drz80);
#ifdef EMU_C68K
    memcpy(&cyclone,writeaddress,sizeof(cyclone));
    writeaddress+=sizeof(cyclone);
#endif
    memcpy(&work_ram,writeaddress,0x10000);
    writeaddress+=0x10000;
    
    memcpy(&vram,writeaddress,0x10000);
    writeaddress+=0x10000;
    
    memcpy(&zram,writeaddress,0x4000);
    writeaddress+=0x4000;
    
    memcpy(&cram,writeaddress,0x80);
    writeaddress+=0x80;
    
    memcpy(&vsram,writeaddress,0x80);
    writeaddress+=0x80;
    
    memcpy(&SL3,writeaddress,sizeof(SL3));
    writeaddress+=sizeof(SL3);
    
    memcpy(&ST,writeaddress,sizeof(ST));
    writeaddress+=sizeof(ST);
    
    memcpy(&OPN,writeaddress,sizeof(OPN));
    writeaddress+=sizeof(OPN);
    
    memcpy(&CH,writeaddress,sizeof(CH));
    writeaddress+=sizeof(CH);
    
    memcpy(&dacout,writeaddress,sizeof(dacout));
    writeaddress+=sizeof(dacout);
    
    memcpy(&dacen,writeaddress,sizeof(dacen));
    writeaddress+=sizeof(dacen);
    
    memcpy(&OPN_pan,writeaddress,(6*2));
    writeaddress+=(6*2);
    
    memcpy(&PSG,writeaddress,sizeof(PSG));
    writeaddress+=sizeof(PSG);

    // update old DT table pointers to current location
    // of DT table
    
    memcpy(&old_DT_Table,writeaddress,4); // get base pointer for DT table
    
	//make sure sram is cleared if an old save state is loaded
    memset(&sram,0,0x10000);
    
    for(x=0;x<6;x++)
    {
    for(y=0;y<4;y++)
    {
    CH[x].SLOT[y].DT=(int*)(((unsigned int)&YMOPN_ST_dt_tab[0])+((unsigned int)(CH[x].SLOT[y].DT)-old_DT_Table));
    }
    }
    
    //update direct memory pointers, this is because
    // for example if DrMD is compiled with slight changes
    // the location of RomData or main md memory is
    // moved but the Z80PC will still be pointing at a memory
    // address based on the old location of rom data
    
    DrMD_Init();
#ifdef EMU_C68K
    Cyclone_Init();
#endif
    DrZ80_Init();
#ifdef EMU_C68K 
    // update cyclone pointers
    cyclone.pc=DrMDCheckPc(cyclone.pc);  // rebase pc
#endif
    // update drz80 pointers
    drz80.Z80PC=drz80.Z80PC-drz80.Z80PC_BASE;
    drz80.Z80PC=Z80_Rebase_PC(drz80.Z80PC);
    
    drz80.Z80SP=drz80.Z80SP-drz80.Z80SP_BASE;
    drz80.Z80SP=Z80_Rebase_SP(drz80.Z80SP);
    
    // now need to update pointers to memory locations because these may be
    // different from when the save state was created.


    
    // re-sync gp32 pal and md pal;
    update_md_pal();
    // reset all menu graphics to the current gamma
    update_menu_graphics_gamma();
}

int loadstate_mem(unsigned char *loadaddress)
{
   unsigned char* writeaddress;
   int x=0,y=0;
   unsigned int old_DT_Table;
    writeaddress=loadaddress;
    
    memcpy(&menu_options,writeaddress,sizeof(menu_options));
    writeaddress+=sizeof(menu_options);
    
    memcpy(&drmd,writeaddress,sizeof(drmd));
    writeaddress+=sizeof(drmd);
    
    memcpy(&drz80,writeaddress,sizeof(drz80));
    writeaddress+=sizeof(drz80);
#ifdef EMU_C68K
    memcpy(&cyclone,writeaddress,sizeof(cyclone));
    writeaddress+=sizeof(cyclone);
#endif
    memcpy(&work_ram,writeaddress,0x10000);
    writeaddress+=0x10000;
    
    memcpy(&vram,writeaddress,0x10000);
    writeaddress+=0x10000;
    
    memcpy(&zram,writeaddress,0x4000);
    writeaddress+=0x4000;
    
    memcpy(&cram,writeaddress,0x80);
    writeaddress+=0x80;
    
    memcpy(&vsram,writeaddress,0x80);
    writeaddress+=0x80;
    
    memcpy(&SL3,writeaddress,sizeof(SL3));
    writeaddress+=sizeof(SL3);
    
    memcpy(&ST,writeaddress,sizeof(ST));
    writeaddress+=sizeof(ST);
    
    memcpy(&OPN,writeaddress,sizeof(OPN));
    writeaddress+=sizeof(OPN);
    
    memcpy(&CH,writeaddress,sizeof(CH));
    writeaddress+=sizeof(CH);
    
    memcpy(&dacout,writeaddress,sizeof(dacout));
    writeaddress+=sizeof(dacout);
    
    memcpy(&dacen,writeaddress,sizeof(dacen));
    writeaddress+=sizeof(dacen);
    
    memcpy(&OPN_pan,writeaddress,(6*2));
    writeaddress+=(6*2);
    
    memcpy(&PSG,writeaddress,sizeof(PSG));
    writeaddress+=sizeof(PSG);

    // update old DT table pointers to current location
    // of DT table
    
    memcpy(&old_DT_Table,writeaddress,4); // get base pointer for DT table
    writeaddress+=4;
    
	memcpy(&sram,writeaddress,0x10000);
    writeaddress+=100000;
	
    for(x=0;x<6;x++)
    {
    for(y=0;y<4;y++)
    {
    CH[x].SLOT[y].DT=(int*)(((unsigned int)&YMOPN_ST_dt_tab[0])+((unsigned int)(CH[x].SLOT[y].DT)-old_DT_Table));
    }
    }
    
    //update direct memory pointers, this is because
    // for example if DrMD is compiled with slight changes
    // the location of RomData or main md memory is
    // moved but the Z80PC will still be pointing at a memory
    // address based on the old location of rom data
    
    DrMD_Init();
#ifdef EMU_C68K
    Cyclone_Init();
#endif
    DrZ80_Init();
#ifdef EMU_C68K 
    // update cyclone pointers
    cyclone.pc=DrMDCheckPc(cyclone.pc);  // rebase pc
#endif
    // update drz80 pointers
    drz80.Z80PC=drz80.Z80PC-drz80.Z80PC_BASE;
    drz80.Z80PC=Z80_Rebase_PC(drz80.Z80PC);
    
    drz80.Z80SP=drz80.Z80SP-drz80.Z80SP_BASE;
    drz80.Z80SP=Z80_Rebase_SP(drz80.Z80SP);
    
    // now need to update pointers to memory locations because these may be
    // different from when the save state was created.


    
    // re-sync gp32 pal and md pal;
    update_md_pal();
    // reset all menu graphics to the current gamma
    update_menu_graphics_gamma();
}

int load_sram(unsigned char *filename)
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
	switch(temp_state[0])
	{
		case 0:
			loadstate_mem_v0(temp_state);
			break;
		case 1:
			loadstate_mem_v1(temp_state);
			break;
		case 2:
			loadstate_mem_v2(temp_state);
			break;
		default:
			loadstate_mem(temp_state);
			break;
	}

  return(1);
}

static int savestate_file(char *filename)
{
  int ret=0;
  gp_setCpuspeed(MENU_FAST_CPU_SPEED);
  ret=save_archive(filename,current_state,savestatesize);
  gp_setCpuspeed(MENU_CPU_SPEED);
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

	  PrintTile(Flip);
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
  
  PrintTile(Flip);
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
  /*switch(menu_options.stereo)
         {
          case 0:
             sprintf(drmd_menu[STEREO_MENU_ITEM].text,"Stereo: Normal");
	     break;
	  case 1:
             sprintf(drmd_menu[STEREO_MENU_ITEM].text,"Stereo: Reversed");
	     break;
         }*/	 
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
  int ret=0;
  int size=0;
  //Make sure flip not 3
  if(Flip) Flip=1;
  
  memset(&text,0,sizeof(text));

  menusmooth=menuFocus<<8;
  gp_setCpuspeed(MENU_CPU_SPEED);
  gp_initFramebuffer(framebuffer16[Flip],16,60,menu_options.lcdver); // 16bit screen mode, refresh 50

  memset(&HeaderDone,0,8);
  
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
				memset(&HeaderDone,0,8);
				subaction=FileSelect(0);
				memset(&HeaderDone,0,8);
				if(subaction==2) action=2;
				break;
			case ROM_DELETE_MENU_ITEM:
				memset(&HeaderDone,0,8);
				subaction=FileSelect(1);
				memset(&HeaderDone,0,8);
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
				memset(&HeaderDone,0,8);
				subaction=savestate_menu(1);  // load mode
				memset(&HeaderDone,0,8);
				if(subaction==100) action=1; // exit menu back to game
				break;
			case SAVE_STATE_MENU_ITEM:
				memset(&HeaderDone,0,8);
				savestate_menu(0);  // save mode
				memset(&HeaderDone,0,8);
				break;
			case DELETE_STATE_MENU_ITEM:
				memset(&HeaderDone,0,8);
				savestate_menu(2);  // delete mode
				memset(&HeaderDone,0,8);
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
				memset(&HeaderDone,0,8);
				subaction=ConfigPad(); 
				memset(&HeaderDone,0,8);
				break;
			case LCD_MENU_ITEM:
				menu_options.lcdver^=1;
				gp_initFramebuffer(framebuffer16[Flip^1],16,60,menu_options.lcdver); // 16bit screen mode, refresh 50
				update_menu_text();
				break;
		
			case SAVE_SETTINGS_MENU_ITEM:
				PrintBar(Flip^1,240-16);
				sprintf(text,"Saving menu options to smc...");
				gp_drawString(40,228,sizeof(text),text,(unsigned short)RGB(0,0,0),framebuffer16[Flip^1]);
				gp_setCpuspeed(MENU_FAST_CPU_SPEED);
				smc_write("dev0:\\GPMM\\DRMD\\menuopts.bin",(char*)&menu_options,sizeof(menu_options));
				gp_setCpuspeed(MENU_CPU_SPEED);
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
					PrintBar(Flip^1,240-16);
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
					gp_setCpuspeed(MENU_FAST_CPU_SPEED);
					smc_write(inifile,(char*)&menu_options,sizeof(menu_options));
					gp_setCpuspeed(MENU_CPU_SPEED);
				}
				break;
			case DELETE_SETTINGS_FOR_GAME_MENU_ITEM:
				if(currentrom>2) // only save settings if game loaded because we need current rom filename
				{
					char inifile[256];
					PrintBar(Flip^1,140-16);
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
					gp_setCpuspeed(MENU_FAST_CPU_SPEED);
					smc_delete(inifile);
					gp_setCpuspeed(MENU_CPU_SPEED);
				}
				break;
			case AUTO_SRAM_MENU_ITEM:
				menu_options.autosram^=1;
				update_menu_text();
				break;
			case LOAD_SRAM_MENU_ITEM:
				if(currentrom>2) // only save sram if game loaded because we need current rom filename
				{
					char sramfile[256];
					GPFILE *gpfile;
					
					memset(&text,0,sizeof(text));
					len=-1;
					for(c=strlen(romlist[currentrom].shortname);c>0;c--)
					{
						if(romlist[currentrom].shortname[c]=='.')
						{
						  len=c;
						  break;
						}
					}
					if(len!=-1)
					{
						// file has extension
						 memcpy(&text,&romlist[currentrom].shortname,len);
						 sprintf(sramfile,"dev0:\\GPMM\\DRMD\\%s.SRM",text);
					}
					else
					{
						 // does not have extension
						 sprintf(sramfile,"dev0:\\GPMM\\DRMD\\%s.SRM",romlist[currentrom].shortname);
					}
					gpfile=(GPFILE*)smc_fopen(sramfile,"rb");
					if(gpfile)
					{
						smc_fclose(gpfile);
						PrintBar(Flip^1,240-16);
						sprintf(text,"loading sram for game...");
						gp_drawString(40,228,sizeof(text),text,(unsigned short)RGB(0,0,0),framebuffer16[Flip^1]);
						size=0;
						gp_setCpuspeed(MENU_FAST_CPU_SPEED);
						ret=load_archive(sramfile, sram, &size, Flip^1);
						gp_setCpuspeed(MENU_CPU_SPEED);
						if(!ret)
						{
							gp_drawString(50,130,17,"Fail to load sram",(unsigned short)RGB(31,31,31),framebuffer16[Flip^1]); // write to current
							while(1==1)
							{
							}
						}
					}
				}
				break;
			case SAVE_SRAM_MENU_ITEM:
				if(currentrom>2) // only save sram if game loaded because we need current rom filename
				{
					char sramfile[256];
					PrintBar(Flip^1,240-16);
					sprintf(text,"Saving sram for game to smc...");
					gp_drawString(40,228,sizeof(text),text,(unsigned short)RGB(0,0,0),framebuffer16[Flip^1]);
					memset(&text,0,sizeof(text));
					len=-1;
					for(c=strlen(romlist[currentrom].shortname);c>0;c--)
					{
						if(romlist[currentrom].shortname[c]=='.')
						{
						  len=c;
						  break;
						}
					}
					if(len!=-1)
					{
						// file has extension
						 memcpy(&text,&romlist[currentrom].shortname,len);
						 sprintf(sramfile,"dev0:\\GPMM\\DRMD\\%s.SRM",text);
					}
					else
					{
						 // does not have extension
						 sprintf(sramfile,"dev0:\\GPMM\\DRMD\\%s.SRM",romlist[currentrom].shortname);
					}
					gp_setCpuspeed(MENU_FAST_CPU_SPEED);
					ret=save_archive(sramfile,(char*)&sram,0x10000);
					gp_setCpuspeed(MENU_CPU_SPEED);
					if(!ret)
					{
						gp_drawString(50,130,10,"fail write",(unsigned short)RGB(31,31,31),framebuffer16[Flip^1]); // write to current
						while(1==1)
						{
						}
					}
  
				}
				break;
			case DELETE_SRAM_MENU_ITEM:
				if(currentrom>2) // only save sram if game loaded because we need current rom filename
				{
					char sramfile[256];
					PrintBar(Flip^1,240-16);
					sprintf(text,"Deleting sram for game...");
					gp_drawString(40,228,sizeof(text),text,(unsigned short)RGB(0,0,0),framebuffer16[Flip^1]);
					memset(&text,0,sizeof(text));
					len=-1;
					for(c=strlen(romlist[currentrom].shortname);c>0;c--)
					{
						if(romlist[currentrom].shortname[c]=='.')
						{
						  len=c;
						  break;
						}
					}
					if(len!=-1)
					{
						// file has extension
						 memcpy(&text,&romlist[currentrom].shortname,len);
						 sprintf(sramfile,"dev0:\\GPMM\\DRMD\\%s.SRM",text);
					}
					else
					{
						 // does not have extension
						 sprintf(sramfile,"dev0:\\GPMM\\DRMD\\%s.SRM",romlist[currentrom].shortname);
					}
					gp_setCpuspeed(MENU_FAST_CPU_SPEED);
					smc_delete(sramfile);
					gp_setCpuspeed(MENU_CPU_SPEED);
				}
				break;
        }	
    }

    // Draw screen:
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
		PrintBar(Flip,y-4);
      }
      else
      {
        color=(unsigned short)RGB(31,31,31);
      }

      sprintf(text,"%s",drmd_menu[i].text);
      gp_drawString(x,y,strlen(text),text,color,framebuffer16[Flip]);
    }

    gp_setFramebuffer(framebuffer16[Flip],1);  // WAIT FOR VSYNC
    Flip^=1;
       
  }
  
  return action;
}



