
#include "app.h"
#ifdef __GIZ__
#include <sys/wcefile.h>
#endif

char RomDir[MAX_PATH+1];
char MD_RomDir[MAX_PATH+1];
char SMS_RomDir[MAX_PATH+1];
char GG_RomDir[MAX_PATH+1];
#ifdef __GP32__
static DIR dir;
#endif
#if defined(__GP2X__) || defined(__GIZ__)
DIRDATA dir;
#endif


int rom_list_loaded;
#ifdef __GP32__
unsigned short cpu_speed_lookup[40]={ 
					22, 33, 40, 50, 66,
					100,133,136,140,144,
					146,150,154,156,160,
					164,166,168,172,176,
					180,184,188,192,196,
					200,204,208,212,216,
					220,224,228,232,236,
					240,244,248,252,256};
#endif
#if defined(__GP2X__) || defined(__GIZ__)
unsigned short cpu_speed_lookup[40]={ 
					10,20, 30, 40, 50,
					60,70, 80, 90,100,
					110,120,130,144,150,
					160,170,180,190,200,
					210,220,230,240,250,
					260,270,280,290,300,
					310,320,330,340,350,
					360,370,380,390,400};
#endif

extern volatile int Timer;
static int menutile_xscroll=0;
static int menutile_yscroll=0;
static int HeaderDone[4]; // variable that records if header graphics have been rendered or not
int quick_save_present=0;

struct RomList_Item romlist[MAX_ROMS];
struct MD_Menu_Options md_menu_options;
struct SMS_Menu_Options sms_menu_options;
struct GG_Menu_Options gg_menu_options;

static int romcount;
int currentrom=2;
char currentrom_shortname[MAX_PATH+1]="";

char menutext[256][50];

struct SaveState savestate[10];  // holds the filenames for the savestate and "inuse" flags
char savestate_name[MAX_PATH+MAX_PATH+2];       // holds the last filename to be scanned for save states

#if defined (__GP32__) || defined (__GIZ__)
void sync(void)
{
}
#endif

static void WaitForButtonsUp(void)
{
	int i=0,j=0,z=0;
	
	for(i=0;i<100;i++)
	{
		while(1)
		{     
			InputUpdate(0);
			z=0;
			for (j=0;j<32;j++)
			{
				if (Inp.held[j]) z=1;
			}
			if (z==0) break;
		}
	}
}

void MenuPause()
{
	int i=0,j=0,z=0;
	// wait for keys to be released
	for(i=0;i<100;i++)  // deal with keybounce by checking a few times
	{
		while(1)
		{     
			InputUpdate(0);
			z=0;
			for (j=0;j<32;j++)
			{
				if (Inp.held[j]) z=1;
			}
			if (z==0) break;
		}
	}
	
	for(i=0;i<100;i++)  // deal with keybounce by checking a few times
	{
		while(1)
		{     
			InputUpdate(0);
			z=0;
			for (j=0;j<32;j++)
			{
				if (Inp.held[j]) z=1;
			}
			if (z==1) break;
		}
	}
}

#ifdef __GP32__	
void MenuFlip()
{
	prevFlip=Flip;
	gp_setFramebuffer(framebuffer16[Flip],1);
    Flip++;
	Flip&=1;
}
#endif
#if defined (__GP2X__)	
void MenuFlip()
{
	prevFlip=Flip;
	gp_setFramebuffer(Flip,1);
    Flip++;
	Flip&=3;
}
#endif
#if defined (__GIZ__)	
void MenuFlip()
{
	prevFlip=Flip=0;
#ifdef __SDL__
	SDL_Flip(sdlScreen);
#else
	gp_setFramebuffer(Flip,0);
#endif

}
#endif
void SplitFilename(char *wholefilename, char *filename, char *ext)
{
	int len=strlen(wholefilename);
	int i=0,y=-1;

	ext[0]=0;
	filename[0]=0;
	//Check given string is not null
	if (len<=0)
	{
		return;
	}
	y=-1;
	for(i=len-2;i>0;i--)
	{
		if (wholefilename[i]=='.')
		{
			y=i;
			break;
		}
	}
    
	if (y>=0)
	{
		memcpy(filename,wholefilename,y);
		filename[y]=0;
		memcpy(ext,wholefilename+y+1,len-(y+1));
		ext[len-(y+1)+1]=0;
	}
	else
	{
		strcpy(filename,wholefilename);
	}
}


int MessageBox(char *message1,char *message2,char *message3,int mode)
{
  int select=0;
  int subaction=-1;
  int len=0;
  while(subaction==-1)
  {
     InputUpdate(0);
     if (Inp.repeat[INP_BUTTON_UP]) 
     {
       select^=1; // Up
     }
     if (Inp.repeat[INP_BUTTON_DOWN]) 
     {
       select^=1; // Down
     }
     if ((Inp.held[INP_BUTTON_MENU_SELECT]==1) || (Inp.held[INP_BUTTON_MENU_CANCEL]==1))
     {
        subaction=select;
     }
     PrintTile(Flip);
     PrintTitle(Flip);
	 len=strlen(message1);
	 if(len>39)len=39;
     gp_drawString(8,50,len,message1,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
     len=strlen(message2);
	 if(len>39)len=39;
	 gp_drawString(8,60,len,message2,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
     len=strlen(message3);
	 if(len>39)len=39;
	 gp_drawString(8,70,len,message3,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
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
     MenuFlip();
  }
  return(subaction);
}

static
int deleterom(int romindex, char *system)
{
	char text[MAX_PATH+1];
	char fullfilename[MAX_PATH+MAX_PATH+1];
	int x;
	struct RomList_Item dummy_romlist;
	FILE *stream=NULL;
	
    PrintTile(Flip);
    PrintTitle(Flip);
    MenuFlip();
	
    sprintf(text,"Deleting Rom..");
    gp_drawString(8,50,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]);
	
    sprintf(text,"%s",romlist[romindex].shortname);
	x=strlen(text);
	if(x>40) x=40;
	gp_drawString(0,60,x,text,(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]);
	
	sprintf(fullfilename,"%s%s%s",RomDir,DIR_SEPERATOR,romlist[romindex].shortname);
    remove(fullfilename);
	sync();
	
    sprintf(text,"Updating Rom List..");
    gp_drawString(8,70,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]);
    for(x=romindex;x<romcount;x++)
    {
		memcpy(&romlist[x],&romlist[x+1],sizeof(dummy_romlist));
    }
    romcount--;
    currentrom--;
	
	sprintf(fullfilename,"%s%s%s",system,DIR_SEPERATOR,ROM_LIST_FILENAME);
	stream=(FILE*)fopen(fullfilename,"wb");
	if (stream)
	{
		fwrite((char*)&romlist,romcount*sizeof(dummy_romlist),1,stream);
		fclose(stream);
		sync();
	}
     return(1);
}

#ifdef __GP32__
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

void PrintTitle(int flip)
{
  unsigned int *framebuffer = (unsigned int*)framebuffer16[flip]+((240-48)>>1);
  unsigned int *graphics = (unsigned int*)menu_header;
  unsigned int x,y;
  char text[256];
  //If header already drawn for this layer exit
  if (HeaderDone[flip]) return;
  
  for (x=0; x<320; x++)
  {
	  for (y=0; y<24; y++)
	  {
		  *framebuffer++ = *graphics++;
	  }
	  framebuffer+=((240-48)>>1);
  }
  
  sprintf(text,"%s",DRMD_VERSION);
  gp_drawString(100,12,strlen(text),text,(unsigned short)RGB(0,0,31),framebuffer16[flip]);
  
  HeaderDone[flip] = 1;
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
#endif

#if defined(__GP2X__) || defined(__GIZ__)
static int tile_counter=0;
void PrintTile(int flip)
{
  short x=0,x2=0;
  short y=0,y2=0;
  unsigned short *framebuffer1 = framebuffer16[flip]+(48*320);
  unsigned short *graphics1 = NULL;

	  x2=menutile_xscroll;
	  y2=(menutile_yscroll*menutile_width);
	  graphics1 = menutile+y2;
	  for (y=0; y<(240-48); y++)
	  {
		  for (x=0; x<320; x++)
		  {
			  *framebuffer1++ = graphics1[x2];
			  x2++;
			  x2&=(menutile_width-1);
		  }
		  y2+=menutile_width;
		  y2&=((menutile_height*menutile_width)-1);
		  graphics1=menutile+y2;
	  }

  tile_counter++;
  if (tile_counter > 5)
  {
		tile_counter=0;
	  menutile_xscroll++;
	  if(menutile_xscroll>=menutile_width) menutile_xscroll=0;
	  
	  menutile_yscroll++;
	  if(menutile_yscroll>=menutile_height) menutile_yscroll=0;
  }  
  return; 
}

void PrintTitle(int flip)
{
  unsigned short *framebuffer = (unsigned short*)framebuffer16[flip];
  unsigned short *graphics = (unsigned short*)menu_header;
  unsigned int x,y;
  char text[256];
  //If header already drawn for this layer exit
  if (HeaderDone[flip]) return;
  
  for (y=0; y<48; y++)
  {
	  for (x=0; x<320; x++)
	  {
		  *framebuffer++ = *graphics++;
	  }
  }
  
  sprintf(text,"%s",DRMD_VERSION);
  gp_drawString(100,12,strlen(text),text,(unsigned short)RGB(0,0,31),framebuffer16[flip]);

  HeaderDone[Flip] = 1;
}

void PrintBar(int flip, unsigned int given_y)
{
  unsigned int *framebuffer1 = NULL;
  unsigned int *graphics1 = (unsigned int *)highlightbar;
  unsigned int x,y;

	framebuffer1 = framebuffer16[flip]+(given_y*320);
	for (y=0; y<16; y++)
	{
		for (x=0; x<160; x++)
		{
			*framebuffer1++ = *graphics1++;
		}
	}

}
#endif

static int StringCompare(char *string1, char *string2)
{
	int i=0;
	char c1=0,c2=0;
	while(1)
	{
		c1=string1[i];
		c2=string2[i];
		// check for string end
		
		if ((c1 == 0) && (c2 == 0)) return 0;
		if (c1 == 0) return 1;
		if (c2 == 0) return -1;
		
		if ((c1 >= 0x61)&&(c1<=0x7A)) c1-=0x20;
		if ((c2 >= 0x61)&&(c2<=0x7A)) c2-=0x20;
		if (c1>c2)
			return 1;
		else if (c1<c2)
			return -1;
		i++;
	}

}

#if defined(__GP2X__) || defined(__GIZ__)
static
int get_dir(char *path, DIRDATA *dirdata)
{
	char filename[MAX_PATH+MAX_PATH+1];
	FILE *stream;
	DIR *d;
	struct dirent  *de;
	int x=0;
	d = opendir(RomDir);
	if (d)
	{
		while ((de = readdir(d)))
		{
			strcpy(dirdata->name[x],de->d_name);
			sprintf(filename,"%s%s%s", RomDir, DIR_SEPERATOR,de->d_name);
			stream=fopen(filename,"rb");
			if (stream)
			{
				fseek(stream,0,SEEK_END);
				dir.size[x] = ftell(stream);
				fclose(stream);
			}			
			x++;
		}
		closedir(d);
	}
	return(x);
}
#endif

#ifdef __GP32__
int FileScan(char *system,int rom_type)
{
	int i=0,x=0,y=0,z=0,size=0;
	char *text1;
	char *text2;
	char fullfilename[MAX_PATH+MAX_PATH+1];
	char text[256];
	char used[MAX_ROMS];
	int filecount;
	int filesdone;
	unsigned char romname_len=0;
	struct RomList_Item dummy_romlist;
	unsigned int *pix;
	FILE *stream;
  
	gp_setCpuspeed(MENU_FAST_CPU_SPEED);
  
	PrintTile(Flip);
	PrintTitle(Flip);
	gp_drawString(8,120,25,"Getting Directory Info...",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	MenuFlip();
	memset(&romlist,0,sizeof(romlist));
	memset(&used,0,sizeof(used));
  
	sprintf(fullfilename,"%s%s",RomDir,DIR_SEPERATOR);
		
#ifdef __GP32__
	filecount=smc_dir(fullfilename,&dir);  // get all files in directory
#endif
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
		MenuFlip();
		if((dir.name[y][0]!='.')&&(dir.size[y]!=0) )// check for actual files
		{
			sprintf(romlist[romcount].shortname,"%s",dir.name[y]);
			romlist[romcount].filesize = dir.size[y];
			sprintf(fullfilename,"%s%s%s",RomDir,DIR_SEPERATOR,romlist[romcount].shortname);
			
			if(check_zip(fullfilename))
			{
				// get CRC from zip info and lookup on crc list
				romlist[romcount].crc=get_archive_crc(fullfilename);
				if(romlist[romcount].crc)
				{
					if (rom_type==EMU_MODE_MD)
					{
#if defined (__EMU_MD__)
						x=MD_DAT_LookFor(romlist[romcount].crc);
						if(x!=-1) 
						{
								sprintf(romlist[romcount].longname,"%s",MD_DAT_getname(x));
						}
						else 
						{ 
							// get filename from inside zip file
							get_archive_filename(fullfilename,romlist[romcount].longname);
#ifdef __GP2X__
							if (strlen(romlist[romcount].shortname) > strlen(romlist[romcount].longname))
							{
								memset(romlist[romcount].longname,0,sizeof(romlist[romcount].longname));
							}
#endif
						}
#endif
					}
					else
					{
#if defined (__EMU_SMS__)
						x=SMS_DAT_LookFor(romlist[romcount].crc);
						if(x!=-1) 
						{
								sprintf(romlist[romcount].longname,"%s",SMS_DAT_getname(x));
						}
						else 
						{ 
							// get filename from inside zip file
							get_archive_filename(fullfilename,romlist[romcount].longname);
#ifdef __GP2X__
							if (strlen(romlist[romcount].shortname) > strlen(romlist[romcount].longname))
							{
								memset(romlist[romcount].longname,0,sizeof(romlist[romcount].longname));
							}
#endif
						}
#endif
					}
				}
				else
				{
					get_archive_filename(fullfilename,romlist[romcount].longname);
#ifdef __GP2X__
					if (strlen(romlist[romcount].shortname) > strlen(romlist[romcount].longname))
					{
						memset(romlist[romcount].longname,0,sizeof(romlist[romcount].longname));
					}
#endif
				}
				romlist[romcount].type=1;
			}
			else
			{
				// Get name of file from crc lookup list
				stream=(FILE*)fopen(fullfilename,"rb");
				if(stream)
				{
					fseek(stream,0,SEEK_END);
					size=ftell(stream);
					fseek(stream,0,SEEK_SET);
					fread(RomData, 1, size, stream);
					fclose(stream);
				}
				for(CRC=x=0;x<romlist[romcount].filesize;x++) GetCRC(RomData[x]);
				romlist[romcount].crc = CRC;
				if(rom_type==EMU_MODE_MD)
				{
#if defined (__EMU_MD__)
					x=MD_DAT_LookFor(CRC);
					if(x!=-1) 
					{
						sprintf(romlist[romcount].longname,"%s",MD_DAT_getname(x));
					}
					else 
					{ 
						memset(romlist[romcount].longname,0,sizeof(romlist[romcount].longname));
					}
#endif
				}
				else
				{
#if defined (__EMU_SMS__)
					x=SMS_DAT_LookFor(CRC);
					if(x!=-1) 
					{
						sprintf(romlist[romcount].longname,"%s",SMS_DAT_getname(x));
					}
					else 
					{ 
						memset(romlist[romcount].longname,0,sizeof(romlist[romcount].longname));
					}
#endif
				}
			}
			romcount++;
		}
	}
	PrintTile(Flip);
	PrintTitle(Flip);
	sprintf(text,"%d roms scanned.", romcount-3);
	gp_drawString(8,120,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	gp_drawString(8,130,24,"Saving romlist to smc...",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	MenuFlip();
	sprintf(fullfilename,"%s%s%s",system,DIR_SEPERATOR,ROM_LIST_FILENAME);
	stream=(FILE*)fopen(fullfilename,"wb");
	if (stream)
	{
		fwrite((char*)&romlist,romcount*sizeof(dummy_romlist),1,stream);
		fclose(stream);
		sync();
	}

	gp_setCpuspeed(MENU_CPU_SPEED);
	return romcount;
}
#endif

#if defined(__GIZ__)
static BOOL CharToWChar(wchar_t *wc, char *c)
{
	int len=strlen(c);
	int x=0;
	for (x=0;x<len;x++)
	{
		wc[x] = btowc(c[x]);
	}
	wc[len]=0;
	return TRUE;
}
#endif
#if defined(__GP2X__) || defined(__GIZ__)
int FileScan(char *system,int rom_type)
{
	int i=0,j=0;
	char text[256];
	DIR *d;
	char dirCheck[MAX_PATH+1];
	struct dirent  *de;
	int dircount=0;
	
#ifdef __GIZ__
	wchar_t  wc[MAX_PATH+1];
	HANDLE hTest;
    WIN32_FIND_DATAW fileInfo;
#endif
	
	gp_setCpuspeed(MENU_FAST_CPU_SPEED);
  
	PrintTile(Flip);
	PrintTitle(Flip);
	gp_drawString(8,120,25,"Getting Directory Info...",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
	MenuFlip();
	memset(&romlist,0,sizeof(romlist));

	//Get rom directory details
	romcount=0;
	
	// Now sort the directory details
	sprintf(romlist[0].longname,"Back To Main Menu");
	sprintf(romlist[1].longname,"..");
	romlist[2].shortname[0] = 0;
	romlist[2].longname[0] = 0;
	romcount=3;
	
	d = opendir(RomDir);

	if (d)
	{
		while ((de = readdir(d)))
		{
			
			if (de->d_name[0] != '.')
			{
#ifdef __GP2X__
				if (de->d_type == 4) // Directory
				{
#endif
#ifdef __GIZ__
				// Because windows GNU library does not return the file type
				// property I will have to try and open each file as a directory instead
				sprintf(dirCheck,"%s%s%s",RomDir,DIR_SEPERATOR,de->d_name);
				CharToWChar(wc, dirCheck);
				hTest=FindFirstFileW(wc, &fileInfo);
				if (fileInfo.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)
				{
#endif
				
					for (i=3;i<=(romcount+1);i++)
					{
						if (romlist[i].shortname[0] == 0) // string is empty so shove new value in
						{
							strcpy(romlist[i].shortname,de->d_name);
							romlist[i].type=FILE_TYPE_DIRECTORY;//de->d_type;
							break;
						}
						else
						{
							if ((StringCompare(romlist[i].shortname,de->d_name) > 0) ||
								  (romlist[i].type != FILE_TYPE_DIRECTORY))
							{
								// new entry is lower than current string so move all entries up one and insert
								// new value in
								for (j=romcount;j>=i;j--)
								{
									strcpy(romlist[j+1].shortname,romlist[j].shortname);
									romlist[j+1].type=romlist[j].type;
								}
								strcpy(romlist[i].shortname,de->d_name);
								romlist[i].type=FILE_TYPE_DIRECTORY;//de->d_type;
								break;
							}
						}
					}
					dircount++;
				}
				else // File*/
				{
					for (i=3+dircount;i<=(romcount+1);i++)
					{
						if (romlist[i].shortname[0] == 0) // string is empty so shove new value in
						{
							strcpy(romlist[i].shortname,de->d_name);
							romlist[i].type=FILE_TYPE_FILE;//de->d_type;
							break;
						}
						else
						{
							if (StringCompare(romlist[i].shortname,de->d_name) > 0)
							{
								// new entry is lower than current string so move all entries up one and insert
								// new value in
								for (j=romcount;j>=i;j--)
								{
									strcpy(romlist[j+1].shortname,romlist[j].shortname);
									romlist[j+1].type=romlist[j].type;
								}
								strcpy(romlist[i].shortname,de->d_name);
								romlist[i].type=FILE_TYPE_FILE;//de->d_type;
								break;
							}
						}
					}
				
				}

				romcount++;
				if (romcount > MAX_ROMS)
				{
					PrintTile(Flip);
					PrintTitle(Flip);
					sprintf(text,"Max rom limit exceeded! %d max",MAX_ROMS);
					gp_drawString(8,120,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
					sprintf(text,"Please reduce number of roms");
					gp_drawString(8,130,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
					MenuFlip();
					MenuPause();
					return MAX_ROMS;
				}
			}
		}
		closedir(d);
	}
	gp_setCpuspeed(MENU_CPU_SPEED);
	return romcount;
}
#endif

int FileSelect(int mode, char *system, int rom_type)
{
	char text[256];
	int romname_length;
	int action=0;
	int smooth=0;
	unsigned short color=0;
	int i=0;
	int Focus=2;
	int menuExit=0;
	int scanstart=0,scanend=0;
	char directorySeparator[2] = DIR_SEPERATOR; // used for char comparison;
  
#ifdef __GP32__
	if(!load_rom_list(system, rom_type)) FileScan(system, rom_type);
#endif
#if defined(__GP2X__) || defined(__GIZ__)
	FileScan(system, rom_type);
#endif  
	if(Focus<2) Focus=2; 	// default menu to non menu item
									// just to stop directory scan being started 
	smooth=Focus<<8;

	while (menuExit==0)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP])
		{
			Focus--; // Up
		}
		if (Inp.repeat[INP_BUTTON_DOWN])
		{
			Focus++; // Down
		}

		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) {action=0; menuExit=1;}
   
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
#ifdef __GP32__
		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{

			if(Focus==0)
			{
				FileScan(system,rom_type);
			}
			else if(Focus==1)
			{
				action=0;
				menuExit=1;
			}
			else if(Focus==2)
			{
				// nothing blank entry
			}
			else
			{
				// A or Start, load rom
				currentrom=Focus;
				quick_save_present=0;  // reset any quick saves
				action=1;
				menuExit=1;
			}
		}
#endif
#if defined(__GP2X__) || defined(__GIZ__)
	if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
    {
		if(Focus==0)
		{
			action=0;
			menuExit=1;
		}
		else if(Focus==1)
		{
			// up a directory
			//Remove a directory from RomPath and rescan
			for(i=strlen(RomDir)-1;i>0;i--) // don't want to change first char in screen
			{
				if(RomDir[i] == directorySeparator[0])
				{
					RomDir[i] = 0;
					break;
				}
			}
			FileScan(system, rom_type);
			Focus=2; // default menu to non menu item
											// just to stop directory scan being started 
			smooth=Focus<<8;
		}	
		else if(Focus==2)
		{
			// nothing blank entry
		}		
		else
		{
			// normal file or dir selected
			if (romlist[Focus].type == FILE_TYPE_DIRECTORY)
			{
				sprintf(RomDir,"%s%s%s",RomDir,DIR_SEPERATOR,romlist[Focus].shortname);
				FileScan(system, rom_type);
				Focus=2; // default menu to non menu item
												// just to stop directory scan being started 
				smooth=Focus<<8;
			}
			else
			{
				currentrom=Focus;
				quick_save_present=0;  // reset any quick saves
				action=1;
				menuExit=1;
			}
		}
    }
#endif

		if (Inp.held[INP_BUTTON_MENU_DELETE]==1)
		{
			if(Focus>2)
			{
				//delete current rom
				if (romlist[Focus].type != FILE_TYPE_DIRECTORY)
				{
					if(romlist[Focus].longname[0]==0)
						sprintf(text,"%s",romlist[Focus].shortname);
					else
						sprintf(text,"%.39s",romlist[Focus].longname);
						
					if(MessageBox("Are you sure you want to delete",text,"",0)==0)
					{
						deleterom(Focus,system);
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

		scanstart=Focus-15;
		if (scanstart<0) scanstart=0;
		scanend = Focus+15;
		if (scanend>romcount) scanend=romcount;
		
		for (i=scanstart;i<scanend;i++)
		{
			int x=0,y=0;
      
			y=(i<<4)-(smooth>>4);
#ifdef __GP32__
			x=0;
#endif
#if defined(__GP2X__) || defined(__GIZ__)
			x=8;
#endif
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

#if defined(__GP2X__) || defined(__GIZ__)
			// Draw Directory icon if current entry is a directory
			if(romlist[i].type == FILE_TYPE_DIRECTORY)
			{
				gp_drawString(x-8,y,1,"+",color,framebuffer16[Flip]); 
			}
#endif
			
			if(romlist[i].longname[0]!=0)
			{ 	// long name -  check for wrap.  if wrap rotate text
				romname_length=strlen(romlist[i].longname);
#ifdef __GP32__
				if(romname_length>40) romname_length=40;
#endif
#if defined(__GP2X__) || defined(__GIZ__)
				if(romname_length>39) romname_length=39;
#endif
				//if(sizeof(romlist[i].longname)>35)
				gp_drawString(x,y,romname_length,romlist[i].longname,color,framebuffer16[Flip]); 
			}
			else 
			{
				romname_length=strlen(romlist[i].shortname);
#ifdef __GP32__
				if(romname_length>40) romname_length=40;
#endif
#if defined(__GP2X__) || defined(__GIZ__)
				if(romname_length>39) romname_length=39;
#endif
				gp_drawString(x,y,romname_length,romlist[i].shortname,color,framebuffer16[Flip]); 
			}
		}

		MenuFlip();
	}

	return action;
}

int load_rom_list(char *system, int rom_type)
{
	char fullfilename[MAX_PATH+MAX_PATH+1];
	struct RomList_Item dummy_romlist;
	int err=0;
	int size=0;
	FILE *stream;
	if(rom_list_loaded==rom_type) return(1);
  
	sprintf(fullfilename,"%s%s%s",system,DIR_SEPERATOR,ROM_LIST_FILENAME);
  
	stream=(FILE*)fopen(fullfilename,"rb");
	if(stream)
	{
		fseek(stream,0,SEEK_END);
		size=ftell(stream);
		fseek(stream,0,SEEK_SET);
		if(fread((char*)&romlist, 1, size, stream)!=size)
		{
			fclose(stream);
			return(0);
		}
		else
		{
			fclose(stream);
			romcount=size/sizeof(dummy_romlist);
			rom_list_loaded=rom_type;
			return(1);
		}     
	}
	else
	{
		return(0);
	}
}

static void scan_savestates(char *romname)
{
	FILE *stream;
	int i=0;
	char savename[MAX_PATH+1];
	char filename[MAX_PATH+1];
	char ext[MAX_PATH+1];

	if(!strcmp(romname,savestate_name)) return; // is current save state rom so exit
	
	SplitFilename(romname,filename,ext);

	sprintf(savename,"%s.%s",filename,SAVESTATE_EXT);
  
	for(i=0;i<10;i++)
	{
      /*
       need to build a save state filename
       all saves are held in current working directory (system_dir)
       save filename has following format
          shortname(minus file ext) + SV + saveno ( 0 to 9 )
         */
		sprintf(savestate[i].filename,"%s%d",savename,i);
		sprintf(savestate[i].fullfilename,"%s%s%s",system_dir,DIR_SEPERATOR,savestate[i].filename);
		stream=(FILE*)fopen(savestate[i].fullfilename,"rb");
		if(stream)
		{
			// we have a savestate
			savestate[i].inuse = 1;
			fclose(stream);	
		}
		else
		{
			// no save state
			savestate[i].inuse = 0;
		}
	}
	strcpy(savestate_name,romname);  // save the last scanned romname
}

void loadstate_file(char *filename)
{
	int size=0;
	int ret=0;

	ret=load_archive((char*)filename, temp_state, &size, prevFlip);
	if(!ret)
	{
		gp_drawString(50,130,4,"fail",(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]); // write to current
		while(1==1)
		{
		}
	}
  
	loadstate_mem(temp_state);

}

static void  savestate_file(char *filename)
{
	int ret=0;

	gp_setCpuspeed(MENU_FAST_CPU_SPEED);

	ret=save_archive(filename,current_state,savestatesize, prevFlip);
	sync();
	gp_setCpuspeed(MENU_CPU_SPEED);
		
	if(!ret)
	{
		gp_drawString(50,130,10,"fail write",(unsigned short)RGB(31,31,31),framebuffer16[prevFlip]); // write to current
		while(1==1)
		{
		}
	}
	
}

static int savestate_menu(int mode)
{
	char text[128];
	int action=11;
	int saveno=0;
	memset(&HeaderDone,0,sizeof(HeaderDone));
	savestate_mem(current_state);  // save the current state of emulator
	if(currentrom<=2)
	{
		// no rom loaded
		// display error message and exit
		return(0);
	}
	scan_savestates(currentrom_shortname);

	while (action!=0&&action!=100)
	{
		InputUpdate(0);
		if(Inp.held[INP_BUTTON_UP]==1) {saveno--; action=1;}
		if(Inp.held[INP_BUTTON_DOWN]==1) {saveno++; action=1;}
		if(saveno<-1) saveno=9;
		if(saveno>9) saveno=-1;
	      
		if(Inp.held[INP_BUTTON_MENU_CANCEL]==1) action=0; // exit
		else if((Inp.held[INP_BUTTON_MENU_SELECT]==1)&&(saveno==-1)) action=0; // exit
		else if((Inp.held[INP_BUTTON_MENU_SELECT]==1)&&(mode==0)&&((action==2)||(action==5))) action=6;  // pre-save mode
		else if((Inp.held[INP_BUTTON_MENU_SELECT]==1)&&(mode==1)&&(action==5)) action=8;  // pre-load mode
		else if((Inp.held[INP_BUTTON_MENU_SELECT]==1)&&(mode==2)&&(action==5))
		{
			if(MessageBox("Are you sure you want to delete","this save?","",0)==0) action=13;  //delete slot with no preview
		}
		else if((Inp.held[INP_BUTTON_R]==1)&&(action==12)) action=3;  // preview slot mode
		else if((Inp.held[INP_BUTTON_MENU_SELECT]==1)&&(mode==1)&&(action==12)) action=8;  //load slot with no preview
		else if((Inp.held[INP_BUTTON_MENU_SELECT]==1)&&(mode==0)&&(action==12)) action=6;  //save slot with no preview
		else if((Inp.held[INP_BUTTON_MENU_SELECT]==1)&&(mode==2)&&(action==12))
		{
			if(MessageBox("Are you sure you want to delete","this save?","",0)==0) action=13;  //delete slot with no preview
		}

		PrintTile(Flip);
		PrintTitle(Flip);
		if(mode==SAVESTATE_MODE_SAVE) gp_drawString(6,35,10,"Save State",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
		if(mode==SAVESTATE_MODE_LOAD) gp_drawString(6,35,10,"Load State",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
		if(mode==SAVESTATE_MODE_DELETE) gp_drawString(6,35,12,"Delete State",(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
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
#if defined(__GP2X__) ||	defined(__GIZ__)		
				gp_drawString(112,145,12, "Coming Soon!",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
#endif
#ifdef __GP32__
				if (CurrentEmuMode==EMU_MODE_MD)
				{
					drmd.render_line = md_render_16_small;
					drmd.frame_buffer = (unsigned int)framebuffer16[Flip]+38400+62;
					current_sample=0;  // stops dac buffer from overflowing
					last_sample=0;
					DrMDRun(1);  // if in preview mode - render frame
				}

				if(mode==1) gp_drawString(100,210,15, "Press A to load",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==0) gp_drawString(80,210,20, "Press A to overwrite",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==2) gp_drawString(92,210,17, "Press A to delete",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
#endif
#if defined(__GP2X__) 
				if(mode==1) gp_drawString(100,210,15, "Press B to load",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==0) gp_drawString(80,210,20, "Press B to overwrite",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==2) gp_drawString(92,210,17, "Press B to delete",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
#endif
#if defined(__GIZ__)
				if(mode==1) gp_drawString(88,210,18, "Press PLAY to load",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==0) gp_drawString(68,210,23, "Press PLAY to overwrite",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==2) gp_drawString(80,210,20, "Press PLAY to delete",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
#endif
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
#ifdef __GP32__
				gp_drawString(124,145,9,"Slot used",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				gp_drawString(88,165,18,"Press R to preview",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				if(mode==1) gp_drawString(100,175,15, "Press A to load",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==0) gp_drawString(80,175,20, "Press A to overwrite",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==2) gp_drawString(92,175,17, "Press A to delete",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
#endif
#ifdef __GP2X__
				gp_drawString(124,145,9,"Slot used",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				gp_drawString(88,165,18,"Press R to preview",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				if(mode==1) gp_drawString(100,175,15, "Press B to load",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==0) gp_drawString(80,175,20, "Press B to overwrite",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==2) gp_drawString(92,175,17, "Press B to delete",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
#endif
#ifdef __GIZ__
				gp_drawString(124,145,9,"Slot used",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				gp_drawString(64,165,24,"Press FORWARD to preview",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				if(mode==1) gp_drawString(88,175,18, "Press PLAY to load",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==0) gp_drawString(68,175,23, "Press PLAY to overwrite",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				else if(mode==2) gp_drawString(80,175,20, "Press PLAY to delete",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
#endif
				break;
			case 13:
				gp_drawString(116,145,11,"Deleting....",(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
				break;
		}
      
		MenuFlip();
      
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
				loadstate_file(savestate[saveno].fullfilename);
				action=5;
				break;
			case 6:
				savestate_file(savestate[saveno].fullfilename);
				savestate[saveno].inuse=1;
				action=1;
				break;
			case 7:
				action=1;
				break;
			case 8:
				loadstate_file(savestate[saveno].fullfilename);
				action=100;  // loaded ok so exit
				break;
			case 9:
				action=1;
				break;
			case 11:
				action=1;
				break;
			case 13:
				remove(savestate[saveno].fullfilename);
				savestate[saveno].inuse = 0;
				action=1;
				break;
		}
	}
	memset(&HeaderDone,0,sizeof(HeaderDone));
	if(action==0) loadstate_mem(current_state);
	return(action);
}

#if defined(__GP32__)
int MDConfigPad()
{
  char text[128];
  char md_conv[8]={
							0,
							1,
							2,
							3,
							6,
							4,
							5,
							7};
  char questions[8][30] ={ "Press Button for UP    : ",
                        "Press Button for DOWN  : ",
			"Press Button for LEFT  : ",
			"Press Button for RIGHT : ",
			"Press Button for A     : ",
			"Press Button for B     : ",
			"Press Button for C     : ",
			"Press Button for START : "
			};
#ifdef __GP32__
  char but_name[32][10] = { "LEFT",
                        "DOWN",
			"RIGHT",
			"UP",
			"L",
			"B",
			"A",
			"R",
			"START",
			"SELECT"};
#endif

  int i=0;
  int question=0;
  int x=0,y=0,z=0;
  int but=0;
  // Draw screen:
  
  WaitForButtonsUp();

  for(z=0;z<32;z++)
  {
    md_menu_options.pad_config[z]=0xFF;
  }
	
	//Clear menu text
	for (z=0;z<256;z++)
		menutext[z][0] = 0;

  x = 10;
  y = 50;
  
  for(question=0;question<8;question++)
  {
		PrintTile(Flip);
		PrintTitle(Flip);
		
		for(z=0;z<12;z++)
		{
			gp_drawString(10,50 + (z*10),strlen(menutext[z]),menutext[z],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
		}
		
	  gp_drawString(x,y,strlen(questions[question]),questions[question],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
	  
	  MenuFlip();
	  WaitForButtonsUp();
	  
	  but=-1;
		while(but==-1)
	  {     
		InputUpdate(0);
		
		for(z=0;z<32;z++)
		{
		  if(Inp.held[z])
		  {
		    if(md_menu_options.pad_config[z] != 0xFF)
			{
				PrintTile(Flip);
				PrintTitle(Flip);
				sprintf(text,"Button has already been assigned");
				gp_drawString(8,50,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
				sprintf(text,"Please try again");
				gp_drawString(8,60,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
				MenuFlip();
				MenuPause();
			}
			else
			{
				but=z;
			}
		    break;
		  }
		}
	  }
	  md_menu_options.pad_config[but]=md_conv[question];
	  sprintf(menutext[question],"%s %s",questions[question],but_name[but]);
	  y+=10;
  }
  
  PrintTile(Flip);
  PrintTitle(Flip);
  for(z=0;z<12;z++)
  {
		gp_drawString(10,50 + (z*10),strlen(menutext[z]),menutext[z],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
  }
  sprintf(text,"Controls Configured!"); 
  gp_drawString(x,y+10,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  sprintf(text,"Press any button to return to menu"); 
  gp_drawString(x,y+20,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  MenuFlip();
  WaitForButtonsUp();
  MenuPause();
  return 0;
}
#endif

#if defined (__GP2X__) || defined (__GIZ__)
int MDConfigPad()
{
  char text[128];
  char md_conv[12]={
							0,
							1,
							2,
							3,
							6,
							4,
							5,
							10,
							9,
							8,
							7,
							11};
  char questions[12][30] ={ "Press Button for UP    : ",
                        "Press Button for DOWN  : ",
			"Press Button for LEFT  : ",
			"Press Button for RIGHT : ",
			"Press Button for A     : ",
			"Press Button for B     : ",
			"Press Button for C     : ",
			"Press Button for X     : ",
			"Press Button for Y     : ",
			"Press Button for Z     : ",
			"Press Button for START : ",
			"Press Button for MODE  : "
			};
#ifdef __GP32__
  char but_name[32][10] = { "LEFT",
                        "DOWN",
			"RIGHT",
			"UP",
			"L",
			"B",
			"A",
			"R",
			"START",
			"SELECT"};
#endif
#if defined(__GP2X__)
  char but_name[32][10] = { "UP",
							"",
							"LEFT",
							"",
							"DOWN",
							"",
							"RIGHT",
							"",
							"START",
							"SELECT",
							"L",
							"R",
							"A",
							"B",
							"X",
							"Y",
							"",
							"",
							"",
							"",
							"",
							"",
							"VOL UP",
							"VOL DOWN",
							"",
							"",
							"",
							"STICK PUSH",
							"",
							"",
							"",
							""};
#endif
#if defined(__GIZ__)
  char but_name[32][10] = { 
							"PLAY",
							"STOP",
							"REWIND",
							"FORWARD",
							"L",
							"R",
							"HOME",
							"VOL",
							"BRIGHT",
							"",
							"",
							"UP",
							"LEFT",
							"DOWN",
							"RIGHT",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							""};
#endif
  int i=0;
  int question=0;
  int x=0,y=0,z=0;
  int but=0;
  // Draw screen:
  
  WaitForButtonsUp();

  for(z=0;z<32;z++)
  {
    md_menu_options.pad_config[z]=0x00;
  }
	
	//Clear menu text
	for (z=0;z<256;z++)
		menutext[z][0] = 0;

  x = 10;
  y = 50;
  
  for(question=0;question<12;question++)
  {
		PrintTile(Flip);
		PrintTitle(Flip);
		
		for(z=0;z<12;z++)
		{
			gp_drawString(10,50 + (z*10),strlen(menutext[z]),menutext[z],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
		}
		
	  gp_drawString(x,y,strlen(questions[question]),questions[question],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
	  
	  MenuFlip();
	  WaitForButtonsUp();
	  
	  but=-1;
		while(but==-1)
	  {     
		InputUpdate(0);
		
		for(z=0;z<32;z++)
		{
		  if(Inp.held[z])
		  {
		    if(md_menu_options.pad_config[z])
			{
				PrintTile(Flip);
				PrintTitle(Flip);
				sprintf(text,"Button has already been assigned");
				gp_drawString(8,50,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
				sprintf(text,"Please try again");
				gp_drawString(8,60,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
				MenuFlip();
				MenuPause();
			}
			else
			{
				but=z;
			}
		    break;
		  }
		}
	  }
	  md_menu_options.pad_config[but]|=md_conv[question];
	  sprintf(menutext[question],"%s %s",questions[question],but_name[but]);
	  y+=10;
  }
  
  PrintTile(Flip);
  PrintTitle(Flip);
  for(z=0;z<12;z++)
  {
		gp_drawString(10,50 + (z*10),strlen(menutext[z]),menutext[z],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
  }
  sprintf(text,"Controls Configured!"); 
  gp_drawString(x,y+10,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  sprintf(text,"Press any button to return to menu"); 
  gp_drawString(x,y+20,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  MenuFlip();
  WaitForButtonsUp();
  MenuPause();
  return 0;
}
#endif
int SMSConfigPad()
{
  char text[128];
  char pad_conv[7]={0,
					1,
					2,
					3,
					4,
					5,
					7};
  char questions[7][30] ={ 
			"Press Button for UP    : ",
			"Press Button for DOWN  : ",
			"Press Button for LEFT  : ",
			"Press Button for RIGHT : ",
			"Press Button for 1     : ",
			"Press Button for 2     : ",
			"Press Button for PAUSE : "};
#ifdef __GP32__
  char but_name[32][10] = { "LEFT",
                        "DOWN",
			"RIGHT",
			"UP",
			"L",
			"B",
			"A",
			"R",
			"START",
			"SELECT"};
#endif
#if defined(__GP2X__)
  char but_name[32][10] = { "UP",
							"",
							"LEFT",
							"",
							"DOWN",
							"",
							"RIGHT",
							"",
							"START",
							"SELECT",
							"L",
							"R",
							"A",
							"B",
							"X",
							"Y",
							"",
							"",
							"",
							"",
							"",
							"",
							"VOL UP",
							"VOL DOWN",
							"",
							"",
							"",
							"STICK PUSH",
							"",
							"",
							"",
							""};
#endif
#if defined(__GIZ__)
  char but_name[32][10] = { 
							"PLAY",
							"STOP",
							"REWIND",
							"FORWARD",
							"L",
							"R",
							"HOME",
							"VOL",
							"BRIGHT",
							"",
							"",
							"UP",
							"LEFT",
							"DOWN",
							"RIGHT",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							""};
#endif

  int i=0;
  int question=0;
  int x=0,y=0,z=0;
  int but=0;
  
  // Draw screen:
  
  WaitForButtonsUp();
	 
  for(z=0;z<32;z++)
  {
    sms_menu_options.pad_config[z]=0xFF;
  }
  
	//Clear menu text
	for (z=0;z<256;z++)
		menutext[z][0] = 0;	
  
  x = 10;
  y = 50;
  
  for(question=0;question<7;question++)
  {
		PrintTile(Flip);
		PrintTitle(Flip);
		
		for(z=0;z<7;z++)
		{
			gp_drawString(10,50 + (z*10),strlen(menutext[z]),menutext[z],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
		}
	  
	  gp_drawString(x,y,strlen(questions[question]),questions[question],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 

	  MenuFlip();
      WaitForButtonsUp();

	  but=-1;
	  while(but==-1)
	  {     
		InputUpdate(0);
		
		for(z=0;z<32;z++)
		{
		  if(Inp.held[z])
		  {
		    but=z;
		    break;
		  }
		}
	  }
	  sms_menu_options.pad_config[but]=pad_conv[question];
	  sprintf(menutext[question],"%s %s",questions[question],but_name[but]);
	  y+=10;
  }
  PrintTile(Flip);
  PrintTitle(Flip);
  for(z=0;z<12;z++)
  {
		gp_drawString(10,50 + (z*10),strlen(menutext[z]),menutext[z],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
  }
  sprintf(text,"Controls Configured!"); 
  gp_drawString(x,y+10,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  sprintf(text,"Press any button to return to menu"); 
  gp_drawString(x,y+20,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  MenuFlip();
  WaitForButtonsUp();
  MenuPause();
  return 0;
}

int GGConfigPad()
{
  char text[128];
  char pad_conv[7]={0,
					1,
					2,
					3,
					4,
					5,
					7};
  char questions[7][30] ={ 
			"Press Button for UP    : ",
			"Press Button for DOWN  : ",
			"Press Button for LEFT  : ",
			"Press Button for RIGHT : ",
			"Press Button for 1     : ",
			"Press Button for 2     : ",
			"Press Button for PAUSE : "};
#ifdef __GP32__
  char but_name[32][10] = { "LEFT",
                        "DOWN",
			"RIGHT",
			"UP",
			"L",
			"B",
			"A",
			"R",
			"START",
			"SELECT"};
#endif
#if defined(__GP2X__)
  char but_name[32][10] = { "UP",
							"",
							"LEFT",
							"",
							"DOWN",
							"",
							"RIGHT",
							"",
							"START",
							"SELECT",
							"L",
							"R",
							"A",
							"B",
							"X",
							"Y",
							"",
							"",
							"",
							"",
							"",
							"",
							"VOL UP",
							"VOL DOWN",
							"",
							"",
							"",
							"STICK PUSH",
							"",
							"",
							"",
							""};
#endif
#if defined(__GIZ__)
  char but_name[32][10] = { 
							"PLAY",
							"STOP",
							"REWIND",
							"FORWARD",
							"L",
							"R",
							"HOME",
							"VOL",
							"BRIGHT",
							"",
							"",
							"UP",
							"LEFT",
							"DOWN",
							"RIGHT",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							"",
							""};
#endif

  int i=0;
  int question=0;
  int x=0,y=0,z=0;
  int but=0;
  
  // Draw screen:
  
  WaitForButtonsUp();
	 
  for(z=0;z<32;z++)
  {
    gg_menu_options.pad_config[z]=0xFF;
  }
  
	//Clear menu text
	for (z=0;z<256;z++)
		menutext[z][0] = 0;	
  
  x = 10;
  y = 50;
  
  for(question=0;question<7;question++)
  {
		PrintTile(Flip);
		PrintTitle(Flip);
		
		for(z=0;z<7;z++)
		{
			gp_drawString(10,50 + (z*10),strlen(menutext[z]),menutext[z],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
		}
	  
	  gp_drawString(x,y,strlen(questions[question]),questions[question],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 

	  MenuFlip();
      WaitForButtonsUp();

	  but=-1;
	  while(but==-1)
	  {     
		InputUpdate(0);
		
		for(z=0;z<32;z++)
		{
		  if(Inp.held[z])
		  {
		    but=z;
		    break;
		  }
		}
	  }
	  gg_menu_options.pad_config[but]=pad_conv[question];
	  sprintf(menutext[question],"%s %s",questions[question],but_name[but]);
	  y+=10;
  }
  PrintTile(Flip);
  PrintTitle(Flip);
  for(z=0;z<12;z++)
  {
		gp_drawString(10,50 + (z*10),strlen(menutext[z]),menutext[z],(unsigned short)RGB(31,31,31),framebuffer16[Flip]); 
  }
  sprintf(text,"Controls Configured!"); 
  gp_drawString(x,y+10,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  sprintf(text,"Press any button to return to menu"); 
  gp_drawString(x,y+20,strlen(text),text,(unsigned short)RGB(31,31,31),framebuffer16[Flip]);
  MenuFlip();
  WaitForButtonsUp();
  MenuPause();
  return 0;
}

static
void RenderMenu(char *menuName, int menuCount, int menuSmooth, int menuFocus)
{
	
	int i=0;
	char text[50];
	unsigned short color=0;
	PrintTile(Flip);
	PrintTitle(Flip);

	gp_drawString(6,35,strlen(menuName),menuName,(unsigned short)RGB(31,0,0),framebuffer16[Flip]); 
    
    // RRRRRGGGGGBBBBBI  gp32 color format
    for (i=0;i<menuCount;i++)
    {
      int x=0,y=0;

      y=(i<<4)-(menuSmooth>>4);
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

      sprintf(text,"%s",menutext[i]);
      gp_drawString(x,y,strlen(text),text,color,framebuffer16[Flip]);
    }

}

static
int LoadRomMenu(void)
{
	int menuExit=0,menuCount=LOAD_ROM_MENU_COUNT,menuFocus=0,menuSmooth=0;
	int action=0;
	int subaction=0;

	memset(&HeaderDone,0,sizeof(HeaderDone));
  
	//Update
#if defined (__EMU_MD__)
	sprintf(menutext[LOAD_ROM_MENU_MD],"Megadrive");
#endif
#if defined (__EMU_SMS__)
	sprintf(menutext[LOAD_ROM_MENU_SMS],"Master System");
	sprintf(menutext[LOAD_ROM_MENU_GG],"Game Gear");
#endif	
	sprintf(menutext[LOAD_ROM_MENU_RETURN],"Back");
	
	while (!menuExit)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP]) menuFocus--; // Up
		if (Inp.repeat[INP_BUTTON_DOWN]) menuFocus++; // Down
    
		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) menuExit=1;
    
		if (menuFocus>menuCount-1)
		{
			menuFocus=0;
			menuSmooth=(menuFocus<<8)-1;
		}   
		else if (menuFocus<0) 
		{
			menuFocus=menuCount-1;
			menuSmooth=(menuFocus<<8)-1;
		}

		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{
			switch(menuFocus)
			{
#if defined(__EMU_MD__)
				case LOAD_ROM_MENU_MD:

					memset(&HeaderDone,0,sizeof(HeaderDone));
					strcpy(RomDir,MD_RomDir);
					subaction=FileSelect(0,md_system_dir, EMU_MODE_MD);
					memset(&HeaderDone,0,sizeof(HeaderDone));
					if(subaction)
					{
						action=EVENT_LOAD_MD_ROM;
						menuExit=1;
					}
					break;
#endif
#if defined(__EMU_SMS__)
				case LOAD_ROM_MENU_SMS:

					memset(&HeaderDone,0,sizeof(HeaderDone));
					strcpy(RomDir,SMS_RomDir);
					subaction=FileSelect(0,sms_system_dir, EMU_MODE_SMS);
					memset(&HeaderDone,0,sizeof(HeaderDone));
					if(subaction)
					{
						action=EVENT_LOAD_SMS_ROM;
						menuExit=1;
					}
					break;
				
				case LOAD_ROM_MENU_GG:
					memset(&HeaderDone,0,sizeof(HeaderDone));
					strcpy(RomDir,GG_RomDir);
					subaction=FileSelect(0,gg_system_dir, EMU_MODE_GG);
					memset(&HeaderDone,0,sizeof(HeaderDone));
					if(subaction)
					{
						action=EVENT_LOAD_GG_ROM;
						menuExit=1;
					}
					break;
#endif
				case LOAD_ROM_MENU_RETURN:
					menuExit=1;
					break;
			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menuFocus<<8); menuSmooth>>=3;
		RenderMenu("Select Rom", menuCount,menuSmooth,menuFocus);
		MenuFlip();
       
	}
  
  return action;
}

static
int QuickStateMenu(void)
{
	int menuExit=0,menuCount=QUICKSTATE_MENU_COUNT,menuFocus=0,menuSmooth=0;
	int action=0;
	int subaction=0;

	memset(&HeaderDone,0,sizeof(HeaderDone));
  
	//Update
	sprintf(menutext[QUICKSTATE_MENU_LOAD],"Load Quick State");
	sprintf(menutext[QUICKSTATE_MENU_SAVE],"Save Quick State");
	sprintf(menutext[QUICKSTATE_MENU_RETURN],"Back");
	
	while (!menuExit)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP]) menuFocus--; // Up
		if (Inp.repeat[INP_BUTTON_DOWN]) menuFocus++; // Down
    
		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) menuExit=1;
    
		if (menuFocus>menuCount-1)
		{
			menuFocus=0;
			menuSmooth=(menuFocus<<8)-1;
		}   
		else if (menuFocus<0) 
		{
			menuFocus=menuCount-1;
			menuSmooth=(menuFocus<<8)-1;
		}

		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{
			switch(menuFocus)
			{
				case QUICKSTATE_MENU_LOAD:
					if(quick_save_present)
					{
						loadstate_mem(quick_state);
						menuExit=1;
						action=1;
					}
					break;
				case QUICKSTATE_MENU_SAVE:
					savestate_mem(quick_state);
					quick_save_present=1;
					menuExit=1;
					action=1;
					break;
				
				case QUICKSTATE_MENU_RETURN:
					menuExit=1;
					break;
			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menuFocus<<8); menuSmooth>>=3;
		RenderMenu("Quick States", menuCount,menuSmooth,menuFocus);
		MenuFlip();
       
	}
  
  return action;
}

static
int SaveStateMenu(void)
{
	int menuExit=0,menuCount=SAVESTATE_MENU_COUNT,menuFocus=0,menuSmooth=0;
	int action=0;
	int subaction=0;

	memset(&HeaderDone,0,sizeof(HeaderDone));
  
	//Update
	sprintf(menutext[SAVESTATE_MENU_LOAD],"Load State");
	sprintf(menutext[SAVESTATE_MENU_SAVE],"Save State");
	sprintf(menutext[SAVESTATE_MENU_DELETE],"Delete State");
	sprintf(menutext[SAVESTATE_MENU_RETURN],"Back");
	
	while (!menuExit)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP]) menuFocus--; // Up
		if (Inp.repeat[INP_BUTTON_DOWN]) menuFocus++; // Down
    
		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) menuExit=1;
    
		if (menuFocus>menuCount-1)
		{
			menuFocus=0;
			menuSmooth=(menuFocus<<8)-1;
		}   
		else if (menuFocus<0) 
		{
			menuFocus=menuCount-1;
			menuSmooth=(menuFocus<<8)-1;
		}

		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{
			switch(menuFocus)
			{
				case SAVESTATE_MENU_LOAD:
					subaction=savestate_menu(SAVESTATE_MODE_LOAD);
					if(subaction==100)
					{
						menuExit=1;
						action=100;
					}
					break;
				case SAVESTATE_MENU_SAVE:
					savestate_menu(SAVESTATE_MODE_SAVE);
					break;
				case SAVESTATE_MENU_DELETE:
					savestate_menu(SAVESTATE_MODE_DELETE);
					break;
				case SAVESTATE_MENU_RETURN:
					menuExit=1;
					break;
			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menuFocus<<8); menuSmooth>>=3;
		RenderMenu("Save States", menuCount,menuSmooth,menuFocus);
		MenuFlip();
       
	}
  
  return action;
}

static
int SramMenu(void)
{
	int menuExit=0,menuCount=SRAM_MENU_COUNT,menuFocus=0,menuSmooth=0;
	int action=0;
	int subaction=0;
	char *srammem=NULL;
	

	memset(&HeaderDone,0,sizeof(HeaderDone));
  
	//Update
	sprintf(menutext[SRAM_MENU_LOAD],"Load SRAM");
	sprintf(menutext[SRAM_MENU_SAVE],"Save SRAM");
	sprintf(menutext[SRAM_MENU_DELETE],"Delete SRAM");
	sprintf(menutext[SRAM_MENU_RETURN],"Back");
	
	while (!menuExit)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP]) menuFocus--; // Up
		if (Inp.repeat[INP_BUTTON_DOWN]) menuFocus++; // Down
    
		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) menuExit=1;
    
		if (menuFocus>menuCount-1)
		{
			menuFocus=0;
			menuSmooth=(menuFocus<<8)-1;
		}   
		else if (menuFocus<0) 
		{
			menuFocus=menuCount-1;
			menuSmooth=(menuFocus<<8)-1;
		}

		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{
			switch(menuFocus)
			{
				case SRAM_MENU_LOAD:
					LoadSram(system_dir,currentrom_shortname,SRAM_FILE_EXT,(char*)&sram);
					break;
				case SRAM_MENU_SAVE:
					SaveSram(system_dir,currentrom_shortname,SRAM_FILE_EXT,(char*)&sram);
					break;
				case SRAM_MENU_DELETE:
					DeleteSram(system_dir,currentrom_shortname,SRAM_FILE_EXT);
					break;
				case SRAM_MENU_RETURN:
					menuExit=1;
					break;
			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menuFocus<<8); menuSmooth>>=3;
		RenderMenu("SRAM", menuCount,menuSmooth,menuFocus);
		MenuFlip();
       
	}
  
  return action;
}

static 
void MDOptionsUpdateText(void)
{
	switch(md_menu_options.sound_on)
	{
		case 0:
			sprintf(menutext[MD_MENU_SOUND],"Sound: OFF");
			break;
		case 1:
			sprintf(menutext[MD_MENU_SOUND],"Sound: ON");
			break;
		case 2:
			sprintf(menutext[MD_MENU_SOUND],"Sound: ON - Accuracy 1");
			break;
		case 3:
			sprintf(menutext[MD_MENU_SOUND],"Sound: ON - Accuracy 2");
			break;
		case 4:
			sprintf(menutext[MD_MENU_SOUND],"Sound: ON - Accuracy 3");
			break;
	}
	
	sprintf(menutext[MD_MENU_SOUND_RATE],"Sound Rate: %d",(unsigned int)sound_rates[md_menu_options.sound_rate]);
#if defined(__GP2X__) || defined(__GIZ__)
	sprintf(menutext[MD_MENU_SOUND_VOL],"Volume: %d",md_menu_options.volume);
#endif	
	switch(md_menu_options.frameskip)
	{
		case 0:
			sprintf(menutext[MD_MENU_FRAMESKIP],"Frameskip: AUTO");
			break;
		default:
			sprintf(menutext[MD_MENU_FRAMESKIP],"Frameskip: %d",md_menu_options.frameskip-1);
			break;
	}
	
	sprintf(menutext[MD_MENU_CPU],"Cpu Speed: %dMhz",cpu_speed_lookup[md_menu_options.cpu_speed]);
	sprintf(menutext[MD_MENU_CONTROLS],"Configure Controls");
	
	switch(md_menu_options.force_region)
	{
		case 0:
			sprintf(menutext[MD_MENU_REGION],"Region: Auto");
			break;
		case 1:
			sprintf(menutext[MD_MENU_REGION],"Region: Usa 60 fps");
			break;  
		case 2:
			sprintf(menutext[MD_MENU_REGION],"Region: Europe 50 fps");
			break; 
		case 3:
			sprintf(menutext[MD_MENU_REGION],"Region: Japan 60 fps");
			break; 
		case 4:
			sprintf(menutext[MD_MENU_REGION],"Region: Japan 50 fps");
			break; 
	}

	switch(md_menu_options.show_fps)
	{
		case 0:
			sprintf(menutext[MD_MENU_FPS],"Show FPS: OFF");
			break;
		case 1:
			sprintf(menutext[MD_MENU_FPS],"Show FPS: ON");
			break;
	}

	sprintf(menutext[MD_MENU_GAMMA],"Brightness: %d",md_menu_options.gamma);
	
	switch(md_menu_options.auto_sram)
	{
		case 0:
			sprintf(menutext[MD_MENU_AUTO_SRAM],"Auto Sram: OFF");
			break;
		case 1:
			sprintf(menutext[MD_MENU_AUTO_SRAM],"Auto Sram: ON");
			break;
	}
	sprintf(menutext[MD_MENU_LOAD_GLOBAL],"Load Global Settings");
	sprintf(menutext[MD_MENU_SAVE_GLOBAL],"Save Global Settings");
	sprintf(menutext[MD_MENU_DELETE_GLOBAL],"Delete Global Settings");
	sprintf(menutext[MD_MENU_LOAD_CURRENT],"Load Settings For Current Game");
	sprintf(menutext[MD_MENU_SAVE_CURRENT],"Save Settings For Current Game");
	sprintf(menutext[MD_MENU_DELETE_CURRENT],"Delete Settings For Current Game");
	sprintf(menutext[MD_MENU_SET_ROMDIR],"Save Current Rom Directory");
	sprintf(menutext[MD_MENU_RETURN],"Back");
	
	switch(md_menu_options.pad_type)
	{
		case 0:
			sprintf(menutext[MD_MENU_PAD_TYPE],"Pad Type: 3 Button");
			break;
		case 1:
			sprintf(menutext[MD_MENU_PAD_TYPE],"Pad Type: 6 Button");
			break;
	}

}

char *DirectorySelector(char *path)
{
	


}
static
int MDOptionsMenu(void)
{
	int menuExit=0,menuCount=MD_MENU_COUNT,menuFocus=0,menuSmooth=0;
	int action=0;
	int subaction=0;

	memset(&HeaderDone,0,sizeof(HeaderDone));
  
	//Update
	MDOptionsUpdateText();
	
	while (!menuExit)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP]) menuFocus--; // Up
		if (Inp.repeat[INP_BUTTON_DOWN]) menuFocus++; // Down
    
		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) menuExit=1;
    
		if (menuFocus>menuCount-1)
		{
			menuFocus=0;
			menuSmooth=(menuFocus<<8)-1;
		}   
		else if (menuFocus<0) 
		{
			menuFocus=menuCount-1;
			menuSmooth=(menuFocus<<8)-1;
		}

		if (Inp.held[INP_BUTTON_LEFT]==1||
			  Inp.held[INP_BUTTON_RIGHT]==1||
			  Inp.repeat[INP_BUTTON_LEFT]||
			  Inp.repeat[INP_BUTTON_RIGHT])
		{
			switch(menuFocus)
			{
				case MD_MENU_SOUND:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						md_menu_options.sound_on++;
						if(md_menu_options.sound_on>4) md_menu_options.sound_on=0;
					}
					else
					{
						md_menu_options.sound_on--;
						if(md_menu_options.sound_on>4) md_menu_options.sound_on=4;
					}
					MDOptionsUpdateText();
					break;
				case MD_MENU_SOUND_RATE:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						md_menu_options.sound_rate++;
						if(md_menu_options.sound_rate>3) md_menu_options.sound_rate=0;
					}
					else
					{
						md_menu_options.sound_rate--;
						if(md_menu_options.sound_rate>3) md_menu_options.sound_rate=3;
					}
					MDOptionsUpdateText();
					break;
#if defined (__GP2X__) || defined (__GIZ__)
				case MD_MENU_SOUND_VOL:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						md_menu_options.volume+=5;
						if(md_menu_options.volume>100) md_menu_options.volume=0;
					}
					else
					{
						md_menu_options.volume-=5;
						if(md_menu_options.volume>100) md_menu_options.volume=100;
					}
					MDOptionsUpdateText();
					break;

#endif
				case MD_MENU_FRAMESKIP:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						md_menu_options.frameskip++;
						if(md_menu_options.frameskip>6) md_menu_options.frameskip=0;
					}
					else
					{
						md_menu_options.frameskip--;
						if(md_menu_options.frameskip>6) md_menu_options.frameskip=6;
					}
					MDOptionsUpdateText();
					break;
				case MD_MENU_CPU:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						md_menu_options.cpu_speed++;
						if(md_menu_options.cpu_speed>MAX_CPU) md_menu_options.cpu_speed=0;
					}
					else
					{
						md_menu_options.cpu_speed--;
						if(md_menu_options.cpu_speed>MAX_CPU) md_menu_options.cpu_speed=MAX_CPU;
					}
					MDOptionsUpdateText();
					break;
				
				case MD_MENU_REGION:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						md_menu_options.force_region++;
						if(md_menu_options.force_region>4) md_menu_options.force_region=0;
					}
					else
					{
						md_menu_options.force_region--;
						if(md_menu_options.force_region>4) md_menu_options.force_region=4;
					}
					MDOptionsUpdateText();
					break;
				case MD_MENU_FPS:
					md_menu_options.show_fps^=1;
					MDOptionsUpdateText();
					break;
				case MD_MENU_GAMMA:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						md_menu_options.gamma++;
						if(md_menu_options.gamma>4) md_menu_options.gamma=0;
					}
					else
					{
						md_menu_options.gamma--;
						if(md_menu_options.gamma>4) md_menu_options.gamma=4;
					}
					MDOptionsUpdateText();
					break;
				case MD_MENU_AUTO_SRAM:
					md_menu_options.auto_sram^=1;
					MDOptionsUpdateText();
					break;
				case MD_MENU_PAD_TYPE:
#if defined(__GP2X__) || defined(__GIZ__)
					md_menu_options.pad_type^=1;
#endif
#if defined(__GP32__)
					md_menu_options.pad_type=0;
#endif
					MDOptionsUpdateText();
					break;
			}
		}
		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{
			switch(menuFocus)
			{
				case MD_MENU_CONTROLS:
					MDConfigPad();
					MDOptionsUpdateText();
					break;
				case MD_MENU_LOAD_GLOBAL:
					LoadMenuOptions(md_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&md_menu_options, sizeof(md_menu_options));
					MDOptionsUpdateText();
					break;
				case MD_MENU_SAVE_GLOBAL:
					SaveMenuOptions(md_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&md_menu_options, sizeof(md_menu_options));
					break;
				case MD_MENU_DELETE_GLOBAL:
					DeleteMenuOptions(md_system_dir,MENU_OPTIONS_FILENAME,MENU_OPTIONS_EXT);
					break;
				case MD_MENU_LOAD_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						LoadMenuOptions(md_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&md_menu_options, sizeof(md_menu_options));
						MDOptionsUpdateText();
					}
					break;
				case MD_MENU_SAVE_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						SaveMenuOptions(md_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&md_menu_options, sizeof(md_menu_options));
					}
					break;
				case MD_MENU_DELETE_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						DeleteMenuOptions(md_system_dir, currentrom_shortname, MENU_OPTIONS_EXT);
					}
					break;
				case MD_MENU_SET_ROMDIR:
					SaveMenuOptions(md_system_dir, DEFAULT_ROM_DIR_FILENAME, DEFAULT_ROM_DIR_EXT, RomDir, strlen(RomDir));
					strcpy(MD_RomDir,RomDir);
					break;
				case MD_MENU_RETURN:
					menuExit=1;
					break;
			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menuFocus<<8); menuSmooth>>=3;
		RenderMenu("MD Options", menuCount,menuSmooth,menuFocus);
		MenuFlip();
       
	}
  
  return action;
}

static 
void SMSOptionsUpdateText(void)
{
	switch(sms_menu_options.sound_on)
	{
		case 0:
			sprintf(menutext[SMS_MENU_SOUND],"Sound: OFF");
			break;
		case 1:
			sprintf(menutext[SMS_MENU_SOUND],"Sound: ON");
			break;
	}
	
	sprintf(menutext[SMS_MENU_SOUND_RATE],"Sound Rate: %d",(unsigned int)sound_rates[sms_menu_options.sound_rate]);

#if defined(__GP2X__) || defined(__GIZ__)
	sprintf(menutext[SMS_MENU_SOUND_VOL],"Volume: %d",sms_menu_options.volume);
#endif
	switch(sms_menu_options.frameskip)
	{
		case 0:
			sprintf(menutext[SMS_MENU_FRAMESKIP],"Frameskip: AUTO");
			break;
		default:
			sprintf(menutext[SMS_MENU_FRAMESKIP],"Frameskip: %d",sms_menu_options.frameskip-1);
			break;
	}
	
	sprintf(menutext[SMS_MENU_CPU],"Cpu Speed: %dMhz",cpu_speed_lookup[sms_menu_options.cpu_speed]);
	sprintf(menutext[SMS_MENU_CONTROLS],"Configure Controls");
	
	switch(sms_menu_options.force_region)
	{
		case 0:
			sprintf(menutext[SMS_MENU_REGION],"Region: Auto");
			break;
		case 1:
			sprintf(menutext[SMS_MENU_REGION],"Region: Usa 60 fps");
			break;  
		case 2:
			sprintf(menutext[SMS_MENU_REGION],"Region: Europe 50 fps");
			break; 
		case 3:
			sprintf(menutext[SMS_MENU_REGION],"Region: Japan 60 fps");
			break; 
		case 4:
			sprintf(menutext[SMS_MENU_REGION],"Region: Japan 50 fps");
			break; 
	}

	switch(sms_menu_options.show_fps)
	{
		case 0:
			sprintf(menutext[SMS_MENU_FPS],"Show FPS: OFF");
			break;
		case 1:
			sprintf(menutext[SMS_MENU_FPS],"Show FPS: ON");
			break;
	}

	switch(sms_menu_options.render_mode)
	{
		case 0:
			sprintf(menutext[SMS_MENU_RENDER_MODE],"Render Mode: NORMAL");
			break;
		case 1:
			sprintf(menutext[SMS_MENU_RENDER_MODE],"Render Mode: FULL SCREEN");
			break;
	}
	
	
	sprintf(menutext[SMS_MENU_GAMMA],"Brightness: %d",sms_menu_options.gamma);
	
	switch(sms_menu_options.auto_sram)
	{
		case 0:
			sprintf(menutext[SMS_MENU_AUTO_SRAM],"Auto Sram: OFF");
			break;
		case 1:
			sprintf(menutext[SMS_MENU_AUTO_SRAM],"Auto Sram: ON");
			break;
	}
	sprintf(menutext[SMS_MENU_LOAD_GLOBAL],"Load Global Settings");
	sprintf(menutext[SMS_MENU_SAVE_GLOBAL],"Save Global Settings");
	sprintf(menutext[SMS_MENU_DELETE_GLOBAL],"Delete Global Settings");
	sprintf(menutext[SMS_MENU_LOAD_CURRENT],"Load Settings For Current Game");
	sprintf(menutext[SMS_MENU_SAVE_CURRENT],"Save Settings For Current Game");
	sprintf(menutext[SMS_MENU_DELETE_CURRENT],"Delete Settings For Current Game");
	sprintf(menutext[SMS_MENU_SET_ROMDIR],"Save Current Rom Directory");
	sprintf(menutext[SMS_MENU_RETURN],"Back");

}

static
int SMSOptionsMenu(void)
{
int menuExit=0,menuCount=SMS_MENU_COUNT,menuFocus=0,menuSmooth=0;
	int action=0;
	int subaction=0;

	memset(&HeaderDone,0,sizeof(HeaderDone));
  
	//Update
	SMSOptionsUpdateText();
	
	while (!menuExit)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP]) menuFocus--; // Up
		if (Inp.repeat[INP_BUTTON_DOWN]) menuFocus++; // Down
    
		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) menuExit=1;
    
		if (menuFocus>menuCount-1)
		{
			menuFocus=0;
			menuSmooth=(menuFocus<<8)-1;
		}   
		else if (menuFocus<0) 
		{
			menuFocus=menuCount-1;
			menuSmooth=(menuFocus<<8)-1;
		}

		if (Inp.held[INP_BUTTON_LEFT]==1||
			  Inp.held[INP_BUTTON_RIGHT]==1||
			  Inp.repeat[INP_BUTTON_LEFT]||
			  Inp.repeat[INP_BUTTON_RIGHT])
		{
			switch(menuFocus)
			{
				case SMS_MENU_SOUND:
					sms_menu_options.sound_on^=1;
					SMSOptionsUpdateText();
					break;
				case SMS_MENU_SOUND_RATE:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						sms_menu_options.sound_rate++;
						if(sms_menu_options.sound_rate>4) sms_menu_options.sound_rate=0;
					}
					else
					{
						sms_menu_options.sound_rate--;
						if(sms_menu_options.sound_rate>4) sms_menu_options.sound_rate=0;
					}
					SMSOptionsUpdateText();
					break;
#if defined (__GP2X__) || defined (__GIZ__)
				case SMS_MENU_SOUND_VOL:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						sms_menu_options.volume+=5;
						if(sms_menu_options.volume>100) sms_menu_options.volume=0;
					}
					else
					{
						sms_menu_options.volume-=5;
						if(sms_menu_options.volume>100) sms_menu_options.volume=100;
					}
					SMSOptionsUpdateText();
					break;
#endif
				case SMS_MENU_FRAMESKIP:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						sms_menu_options.frameskip++;
						if(sms_menu_options.frameskip>6) sms_menu_options.frameskip=0;
					}
					else
					{
						sms_menu_options.frameskip--;
						if(sms_menu_options.frameskip>6) sms_menu_options.frameskip=6;
					}
					SMSOptionsUpdateText();
					break;
				case SMS_MENU_RENDER_MODE:
					sms_menu_options.render_mode^=1;
					SMSOptionsUpdateText();
					break;
				case SMS_MENU_CPU:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						sms_menu_options.cpu_speed++;
						if(sms_menu_options.cpu_speed>MAX_CPU) sms_menu_options.cpu_speed=0;
					}
					else
					{
						sms_menu_options.cpu_speed--;
						if(sms_menu_options.cpu_speed>MAX_CPU) sms_menu_options.cpu_speed=MAX_CPU;
					}
					SMSOptionsUpdateText();
					break;
				
				case SMS_MENU_REGION:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						sms_menu_options.force_region++;
						if(sms_menu_options.force_region>4) sms_menu_options.force_region=0;
					}
					else
					{
						sms_menu_options.force_region--;
						if(sms_menu_options.force_region>4) sms_menu_options.force_region=4;
					}
					SMSOptionsUpdateText();
					break;
				case SMS_MENU_FPS:
					sms_menu_options.show_fps^=1;
					SMSOptionsUpdateText();
					break;
				case SMS_MENU_GAMMA:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						sms_menu_options.gamma++;
						if(sms_menu_options.gamma>4) sms_menu_options.gamma=0;
					}
					else
					{
						sms_menu_options.gamma--;
						if(sms_menu_options.gamma>4) sms_menu_options.gamma=4;
					}
					SMSOptionsUpdateText();
					break;
				case SMS_MENU_AUTO_SRAM:
					sms_menu_options.auto_sram^=1;
					SMSOptionsUpdateText();
					break;
			}
		}
		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{
			switch(menuFocus)
			{
				case SMS_MENU_CONTROLS:
					SMSConfigPad();
					SMSOptionsUpdateText();
					break;
				case SMS_MENU_LOAD_GLOBAL:
					LoadMenuOptions(sms_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&sms_menu_options, sizeof(sms_menu_options));
					SMSOptionsUpdateText();
					break;
				case SMS_MENU_SAVE_GLOBAL:
					SaveMenuOptions(sms_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&sms_menu_options, sizeof(sms_menu_options));
					break;
				case SMS_MENU_DELETE_GLOBAL:
					DeleteMenuOptions(sms_system_dir,MENU_OPTIONS_FILENAME,MENU_OPTIONS_EXT);
					break;
				case SMS_MENU_LOAD_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						LoadMenuOptions(sms_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&sms_menu_options, sizeof(sms_menu_options));
						SMSOptionsUpdateText();
					}
					break;
				case SMS_MENU_SAVE_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						SaveMenuOptions(sms_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&sms_menu_options, sizeof(sms_menu_options));
					}
					break;
				case SMS_MENU_DELETE_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						DeleteMenuOptions(sms_system_dir, currentrom_shortname, MENU_OPTIONS_EXT);
					}
					break;
				case SMS_MENU_SET_ROMDIR:
					SaveMenuOptions(sms_system_dir, DEFAULT_ROM_DIR_FILENAME, DEFAULT_ROM_DIR_EXT, RomDir, strlen(RomDir));
					strcpy(SMS_RomDir,RomDir);
					break;
					
				case SMS_MENU_RETURN:
					menuExit=1;
					break;
			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menuFocus<<8); menuSmooth>>=3;
		RenderMenu("SMS Options", menuCount,menuSmooth,menuFocus);
		MenuFlip();
       
	}
  
  return action;
}

static 
void GGOptionsUpdateText(void)
{
	switch(gg_menu_options.sound_on)
	{
		case 0:
			sprintf(menutext[GG_MENU_SOUND],"Sound: OFF");
			break;
		case 1:
			sprintf(menutext[GG_MENU_SOUND],"Sound: ON");
			break;
	}
	
	sprintf(menutext[GG_MENU_SOUND_RATE],"Sound Rate: %d",(unsigned int)sound_rates[gg_menu_options.sound_rate]);
#if defined(__GP2X__) || defined(__GIZ__)
	sprintf(menutext[GG_MENU_SOUND_VOL],"Volume: %d",gg_menu_options.volume);
#endif
	switch(gg_menu_options.frameskip)
	{
		case 0:
			sprintf(menutext[GG_MENU_FRAMESKIP],"Frameskip: AUTO");
			break;
		default:
			sprintf(menutext[GG_MENU_FRAMESKIP],"Frameskip: %d",gg_menu_options.frameskip-1);
			break;
	}
	
	sprintf(menutext[GG_MENU_CPU],"Cpu Speed: %dMhz",cpu_speed_lookup[gg_menu_options.cpu_speed]);
	sprintf(menutext[GG_MENU_CONTROLS],"Configure Controls");
	
	switch(gg_menu_options.force_region)
	{
		case 0:
			sprintf(menutext[GG_MENU_REGION],"Region: Auto");
			break;
		case 1:
			sprintf(menutext[GG_MENU_REGION],"Region: Usa 60 fps");
			break;  
		case 2:
			sprintf(menutext[GG_MENU_REGION],"Region: Europe 50 fps");
			break; 
		case 3:
			sprintf(menutext[GG_MENU_REGION],"Region: Japan 60 fps");
			break; 
		case 4:
			sprintf(menutext[GG_MENU_REGION],"Region: Japan 50 fps");
			break; 
	}

	switch(gg_menu_options.show_fps)
	{
		case 0:
			sprintf(menutext[GG_MENU_FPS],"Show FPS: OFF");
			break;
		case 1:
			sprintf(menutext[GG_MENU_FPS],"Show FPS: ON");
			break;
	}

	switch(gg_menu_options.render_mode)
	{
		case 0:
			sprintf(menutext[GG_MENU_RENDER_MODE],"Render Mode: NORMAL");
			break;
		case 1:
			sprintf(menutext[GG_MENU_RENDER_MODE],"Render Mode: FULL SCREEN");
			break;
	}
	
	
	sprintf(menutext[GG_MENU_GAMMA],"Brightness: %d",gg_menu_options.gamma);
	
	switch(gg_menu_options.auto_sram)
	{
		case 0:
			sprintf(menutext[GG_MENU_AUTO_SRAM],"Auto Sram: OFF");
			break;
		case 1:
			sprintf(menutext[GG_MENU_AUTO_SRAM],"Auto Sram: ON");
			break;
	}
	sprintf(menutext[GG_MENU_LOAD_GLOBAL],"Load Global Settings");
	sprintf(menutext[GG_MENU_SAVE_GLOBAL],"Save Global Settings");
	sprintf(menutext[GG_MENU_DELETE_GLOBAL],"Delete Global Settings");
	sprintf(menutext[GG_MENU_LOAD_CURRENT],"Load Settings For Current Game");
	sprintf(menutext[GG_MENU_SAVE_CURRENT],"Save Settings For Current Game");
	sprintf(menutext[GG_MENU_DELETE_CURRENT],"Delete Settings For Current Game");
	sprintf(menutext[GG_MENU_SET_ROMDIR],"Save Current Rom Directory");
	sprintf(menutext[GG_MENU_RETURN],"Back");

}

static
int GGOptionsMenu(void)
{
int menuExit=0,menuCount=GG_MENU_COUNT,menuFocus=0,menuSmooth=0;
	int action=0;
	int subaction=0;

	memset(&HeaderDone,0,sizeof(HeaderDone));
  
	//Update
	GGOptionsUpdateText();
	
	while (!menuExit)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP]) menuFocus--; // Up
		if (Inp.repeat[INP_BUTTON_DOWN]) menuFocus++; // Down
    
		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) menuExit=1;
    
		if (menuFocus>menuCount-1)
		{
			menuFocus=0;
			menuSmooth=(menuFocus<<8)-1;
		}   
		else if (menuFocus<0) 
		{
			menuFocus=menuCount-1;
			menuSmooth=(menuFocus<<8)-1;
		}

		if (Inp.held[INP_BUTTON_LEFT]==1||
			  Inp.held[INP_BUTTON_RIGHT]==1||
			  Inp.repeat[INP_BUTTON_LEFT]||
			  Inp.repeat[INP_BUTTON_RIGHT])
		{
			switch(menuFocus)
			{
				case GG_MENU_SOUND:
					gg_menu_options.sound_on^=1;
					GGOptionsUpdateText();
					break;
				case GG_MENU_SOUND_RATE:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						gg_menu_options.sound_rate++;
						if(gg_menu_options.sound_rate>4) gg_menu_options.sound_rate=0;
					}
					else
					{
						gg_menu_options.sound_rate--;
						if(gg_menu_options.sound_rate>4) gg_menu_options.sound_rate=0;
					}
					GGOptionsUpdateText();
					break;
#if defined (__GP2X__) || defined (__GIZ__)
				case GG_MENU_SOUND_VOL:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						gg_menu_options.volume+=5;
						if(gg_menu_options.volume>100) gg_menu_options.volume=0;
					}
					else
					{
						gg_menu_options.volume-=5;
						if(gg_menu_options.volume>100) gg_menu_options.volume=100;
					}
					GGOptionsUpdateText();
					break;
#endif
				case GG_MENU_FRAMESKIP:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						gg_menu_options.frameskip++;
						if(gg_menu_options.frameskip>6) gg_menu_options.frameskip=0;
					}
					else
					{
						gg_menu_options.frameskip--;
						if(gg_menu_options.frameskip>6) gg_menu_options.frameskip=6;
					}
					GGOptionsUpdateText();
					break;
				case GG_MENU_RENDER_MODE:
					gg_menu_options.render_mode^=1;
					GGOptionsUpdateText();
					break;
				case GG_MENU_CPU:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						gg_menu_options.cpu_speed++;
						if(gg_menu_options.cpu_speed>MAX_CPU) gg_menu_options.cpu_speed=0;
					}
					else
					{
						gg_menu_options.cpu_speed--;
						if(gg_menu_options.cpu_speed>MAX_CPU) gg_menu_options.cpu_speed=MAX_CPU;
					}
					GGOptionsUpdateText();
					break;
				
				case GG_MENU_REGION:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						gg_menu_options.force_region++;
						if(gg_menu_options.force_region>4) gg_menu_options.force_region=0;
					}
					else
					{
						gg_menu_options.force_region--;
						if(gg_menu_options.force_region>4) gg_menu_options.force_region=4;
					}
					GGOptionsUpdateText();
					break;
				case GG_MENU_FPS:
					gg_menu_options.show_fps^=1;
					GGOptionsUpdateText();
					break;
				case GG_MENU_GAMMA:
					if (Inp.held[INP_BUTTON_RIGHT]==1||Inp.repeat[INP_BUTTON_RIGHT])
					{
						gg_menu_options.gamma++;
						if(gg_menu_options.gamma>4) gg_menu_options.gamma=0;
					}
					else
					{
						gg_menu_options.gamma--;
						if(gg_menu_options.gamma>4) gg_menu_options.gamma=4;
					}
					GGOptionsUpdateText();
					break;
				case GG_MENU_AUTO_SRAM:
					gg_menu_options.auto_sram^=1;
					GGOptionsUpdateText();
					break;
			}
		}
		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{
			switch(menuFocus)
			{
				case GG_MENU_CONTROLS:
					GGConfigPad();
					GGOptionsUpdateText();
					break;
				case GG_MENU_LOAD_GLOBAL:
					LoadMenuOptions(gg_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&gg_menu_options, sizeof(gg_menu_options));
					GGOptionsUpdateText();
					break;
				case GG_MENU_SAVE_GLOBAL:
					SaveMenuOptions(gg_system_dir, MENU_OPTIONS_FILENAME, MENU_OPTIONS_EXT, (char*)&gg_menu_options, sizeof(gg_menu_options));
					break;
				case GG_MENU_DELETE_GLOBAL:
					DeleteMenuOptions(gg_system_dir,MENU_OPTIONS_FILENAME,MENU_OPTIONS_EXT);
					break;
				case GG_MENU_LOAD_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						LoadMenuOptions(gg_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&gg_menu_options, sizeof(gg_menu_options));
						GGOptionsUpdateText();
					}
					break;
				case GG_MENU_SAVE_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						SaveMenuOptions(gg_system_dir, currentrom_shortname, MENU_OPTIONS_EXT, (char*)&gg_menu_options, sizeof(gg_menu_options));
					}
					break;
				case GG_MENU_DELETE_CURRENT:
					if(currentrom_shortname[0]!=0)
					{
						DeleteMenuOptions(gg_system_dir, currentrom_shortname, MENU_OPTIONS_EXT);
					}
					break;
				case GG_MENU_SET_ROMDIR:
					SaveMenuOptions(gg_system_dir, DEFAULT_ROM_DIR_FILENAME, DEFAULT_ROM_DIR_EXT, RomDir, strlen(RomDir));
					strcpy(GG_RomDir,RomDir);
					break;
				case GG_MENU_RETURN:
					menuExit=1;
					break;
			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menuFocus<<8); menuSmooth>>=3;
		RenderMenu("GG Options", menuCount,menuSmooth,menuFocus);
		MenuFlip();
       
	}
  
  return action;
}

static
void MainMenuUpdateText(void)
{
	sprintf(menutext[MAIN_MENU_ROM_SELECT],"Select Rom");
	sprintf(menutext[MAIN_MENU_MANAGE_QUICK_STATE],"Manage Quick States");
	sprintf(menutext[MAIN_MENU_MANAGE_SAVE_STATE],"Manage Save States");
	sprintf(menutext[MAIN_MENU_MANAGE_SRAM],"Manage SRAM");
#if defined(__EMU_MD__)
	sprintf(menutext[MAIN_MENU_MD_OPTIONS],"MD Options");
#endif
#if defined(__EMU_SMS__)
	sprintf(menutext[MAIN_MENU_SMS_OPTIONS],"SMS Options");
	sprintf(menutext[MAIN_MENU_GG_OPTIONS],"GG Options");
#endif
	sprintf(menutext[MAIN_MENU_RESET_GAME	],"Reset Game");
	
#ifdef __GP32__	
	switch(gp32_lcdver)
	{
		case 0:
			sprintf(menutext[MAIN_MENU_LCD_TYPE],"LCD: Samsung");
			break;
		case 1:
			sprintf(menutext[MAIN_MENU_LCD_TYPE],"LCD: Taiwanese");
			break;
	}
#endif
	sprintf(menutext[MAIN_MENU_EXIT_APP],"Exit Application");
	sprintf(menutext[MAIN_MENU_RETURN],"Return To Game");


}


int MainMenu(int prevaction)
{
	int menuExit=0,menuCount=MAIN_MENU_COUNT,menuFocus=0,menuSmooth=0;
	int action=prevaction;
	int subaction=0;
	
	gp_setCpuspeed(MENU_CPU_SPEED);
	
	if  (Flip>1) Flip=0;

#ifdef __GP32__
	gp_initFramebuffer(framebuffer16[Flip],16,60,gp32_lcdver); 
#endif
#if defined (__GP2X__) || defined (__GIZ__)
	gp_initGraphics(16,Flip);
#endif
	
	// SAVE SRAM
	switch(CurrentEmuMode)
	{
		case EMU_MODE_MD:
			if (md_menu_options.auto_sram)
			{
				if(CheckSram()>10) SaveSram(md_system_dir,currentrom_shortname,SRAM_FILE_EXT,(char*)&sram);
			}
			break;
		
		case EMU_MODE_SMS:
			if (sms_menu_options.auto_sram)
			{
				if(CheckSram()>10) SaveSram(sms_system_dir,currentrom_shortname,SRAM_FILE_EXT,(char*)&sram);
			}
			break;
		
		case EMU_MODE_GG:
			if (gg_menu_options.auto_sram)
			{
				if(CheckSram()>10) SaveSram(gg_system_dir,currentrom_shortname,SRAM_FILE_EXT,(char*)&sram);
			}
			break;
	}

	memset(&HeaderDone,0,sizeof(HeaderDone));
	MainMenuUpdateText();
	
	while (!menuExit)
	{
		InputUpdate(0);

		// Change which rom is focused on:
		if (Inp.repeat[INP_BUTTON_UP]) menuFocus--; // Up
		if (Inp.repeat[INP_BUTTON_DOWN]) menuFocus++; // Down
    
		if (Inp.held[INP_BUTTON_MENU_CANCEL]==1 ) 
		{
			if(currentrom_shortname[0]!=0)
			{
				menuExit=1;
			}
		}
    
		if (menuFocus>menuCount-1)
		{
			menuFocus=0;
			menuSmooth=(menuFocus<<8)-1;
		}   
		else if (menuFocus<0) 
		{
			menuFocus=menuCount-1;
			menuSmooth=(menuFocus<<8)-1;
		}

		if (Inp.held[INP_BUTTON_MENU_SELECT]==1)
		{
			switch(menuFocus)
			{
				case MAIN_MENU_ROM_SELECT:
#if (defined(__EMU_MD__) && defined(__EMU_SMS__)) || defined(__EMU_SMS__)
					memset(&HeaderDone,0,sizeof(HeaderDone));
					subaction=LoadRomMenu();
					memset(&HeaderDone,0,sizeof(HeaderDone));
					if(subaction)
					{
						action=subaction;
						menuExit=1;
					}
					MainMenuUpdateText();
					break;
#endif
#if defined(__EMU_MD__) && !defined(__EMU_SMS__)
					memset(&HeaderDone,0,sizeof(HeaderDone));
					strcpy(RomDir,MD_RomDir);
					subaction=FileSelect(0,md_system_dir, EMU_MODE_MD);
					memset(&HeaderDone,0,sizeof(HeaderDone));
					if(subaction)
					{
						action=EVENT_LOAD_MD_ROM;
						menuExit=1;
					}
					break;
#endif
				case MAIN_MENU_MANAGE_QUICK_STATE:
					if(currentrom_shortname[0]!=0)
					{
						memset(&HeaderDone,0,sizeof(HeaderDone));
						subaction=QuickStateMenu();
						if(subaction) menuExit=1;
						memset(&HeaderDone,0,sizeof(HeaderDone));
						MainMenuUpdateText();
					}
					break;
				case MAIN_MENU_MANAGE_SAVE_STATE:
					if(currentrom_shortname[0]!=0)
					{
						memset(&HeaderDone,0,sizeof(HeaderDone));
						subaction=SaveStateMenu();
						if (subaction==100)
						{
							menuExit=1;
						}
						memset(&HeaderDone,0,sizeof(HeaderDone));
					}
					MainMenuUpdateText();
					break;
				case MAIN_MENU_MANAGE_SRAM:
					if(currentrom_shortname[0]!=0)
					{
						memset(&HeaderDone,0,sizeof(HeaderDone));
						subaction=SramMenu();
						memset(&HeaderDone,0,sizeof(HeaderDone));
						MainMenuUpdateText();
					}
					break;
#if defined(__EMU_MD__)
				case MAIN_MENU_MD_OPTIONS:

					memset(&HeaderDone,0,sizeof(HeaderDone));
					subaction=MDOptionsMenu();
					memset(&HeaderDone,0,sizeof(HeaderDone));
					MainMenuUpdateText();
					break;
#endif
#if defined(__EMU_SMS__)
				case MAIN_MENU_SMS_OPTIONS:

					memset(&HeaderDone,0,sizeof(HeaderDone));
					subaction=SMSOptionsMenu();
					memset(&HeaderDone,0,sizeof(HeaderDone));
					MainMenuUpdateText();
					break;
				case MAIN_MENU_GG_OPTIONS:
					memset(&HeaderDone,0,sizeof(HeaderDone));
					subaction=GGOptionsMenu();
					memset(&HeaderDone,0,sizeof(HeaderDone));
					MainMenuUpdateText();
					break;
#endif
				case MAIN_MENU_RESET_GAME	:
					if(currentrom_shortname[0]!=0)
					{
						switch(CurrentEmuMode)
						{
							case EMU_MODE_MD:
#if defined(__EMU_MD__)
								reset_drmd();
								menuExit=1;
#endif
								break;
							case EMU_MODE_SMS:
							case EMU_MODE_GG:
#if defined(__EMU_SMS__)
								DrSMS_Init();
								DrSMS_Reset();
								menuExit=1;
#endif
								break;
						}
						menuExit=1;
					}
					break;
				case MAIN_MENU_RETURN:
					if(currentrom_shortname[0]!=0)
					{
						menuExit=1;
					}
					break;
#ifdef __GP32__
				case MAIN_MENU_LCD_TYPE:
					gp32_lcdver^=1;
					gp_initFramebuffer(framebuffer16[prevFlip],16,60,gp32_lcdver); // 16bit screen mode, refresh 50
					MainMenuUpdateText();
					//TODO save config to disk
					break;
#endif
				case MAIN_MENU_EXIT_APP:
					action=EVENT_EXIT_APP;
					menuExit=1;
					break;

			}	
		}
		// Draw screen:
		menuSmooth=menuSmooth*7+(menuFocus<<8); menuSmooth>>=3;
		RenderMenu("Main Menu", menuCount,menuSmooth,menuFocus);
		MenuFlip();
       
	}
	
  WaitForButtonsUp();
  
  return action;
}

