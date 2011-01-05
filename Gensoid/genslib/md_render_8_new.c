/*
Convert MD graphics into GP2X tiles

ConvertedTiles holds the 16bit pixel data for the MD's 2048 tiles.
It holds 4 versions of each tile, 1 for each MD palette.
*/
#include "app.h"

#define PAL_COUNT		4
char DirtyPal[PAL_COUNT];
char DirtyVram;
char DirtyTile[2048];
char ConvertedTiles[0x40000*4]; // 16bit Tile cache for all 4 pallets

char ClippingBuffer[16];
static
void RenderSprites(int pri)
{


}

static
void RenderBackground(int layer, int pri)
{
	unsigned char *fb,*vr;
	unsigned short *tiledata;
	unsigned short *pixeldata;
	unsigned short tile_flags;
	unsigned int tile,tile_height,shift,width,height,colmask,rowmask,line;
	unsigned char ShiftTable[4]={6,7,0,8};
	unsigned short hscroll,vscroll;
	unsigned char ColMaskTable[4]={0x1F,0x3F,0x1F,0x7F};
	unsigned short RowMaskTable[4]={0xFF,0x1FF,0x2FF,0x3FF};
	
	width=drmd.vdp_reg16;
	shift=ShiftTable[width];
	height=((width>>4)&3);
	width&=0x3;
	
	colmask=ColMaskTable[width];
	rowmask=RowMaskTable[height];
	
	//TODO: move calculation of hscroll to when vdp_reg11 is written
	switch(drmd.vdp_reg11&0x3)
	{
		case 0:  //hscroll fullscreen
			hscroll=vram[(((drmd.vdp_reg13<<10)+(layer<<1))&0xFFFF)];
			break;
		case 1:  //hscroll first 8
			hscroll=vram[((((drmd.vdp_reg13<<10)+(layer<<1))+((drmd.vdp_line&7)<<2))&0xFFFF)];
			break;
		case 2:  //hscroll row
			hscroll=vram[((((drmd.vdp_reg13<<10)+(layer<<1))+((drmd.vdp_line&0xFFF8)<<2))&0xFFFF)];
			break;
		case 3:  //hscroll line
			hscroll=vram[((((drmd.vdp_reg13<<10)+(layer<<1))+(drmd.vdp_line<<2))&0xFFFF)];
			break;
	}		
	
	hscroll&=0x3FF;
	vscroll=vsram[layer];
	
	line=(drmd.vdp_line+vscroll)&rowmask;
	tiledata=(unsigned short*)drmd.vram+((line>>3)<<shift)+(layer&1?((drmd.vdp_reg4&0x7)<<13):((drmd.vdp_reg2&0x38)<<10));
	fb=(unsigned char*)drmd.frame_buffer+((drmd.vdp_reg1&0x8)?0:0xA00)+(drmd.vdp_line*320)+((drmd.vdp_reg12&1)?0:32);
	tile=0-hscroll
	fb-=tile&0x7;
	tile=(tile>>3)&colmask;
	colmask++;
	tile_height=(line&0x7)<<2;
	
	tile_count=(drmd.vdp_reg12&1)?41:33;
	
	
	tile_flags=tiledata[tile];
	pixeldata=(unsigned short*)&convertedTiles[((tile_flags&0x7FF)+((tile_flags&0x1000)?(28-tile_height):tile_height))<<7];
	// Need to calc data for transparency
	// need array with has a bitmask showing which pixels
	// are not transparent, this will need to be done
	// when the tile is cached
}


static
void UpdateTile(int tile, int pal)
{
	int g=0;
	unsigned int *md=(unsigned int*)&vram[tile<<5];
	unsigned short *tiles=(unsigned short*)&ConvertedTiles[tile<<7];
	unsigned short *p=(unsigned short*)&pal_lookup[pal<<4];
	unsigned int data=0;
	
	for(g=0;g<8;g++)
	{
		data=*md++;
		*tiles++=p[(data&0x0000000F)>>0];
		*tiles++=p[(data&0x000000F0)>>4];
		*tiles++=p[(data&0x00000F00)>>8];
		*tiles++=p[(data&0x0000F000)>>12];
		*tiles++=p[(data&0x000F0000)>>16];
		*tiles++=p[(data&0x00F00000)>>20];
		*tiles++=p[(data&0x0F000000)>>24];
		*tiles++=p[(data&0xF0000000)>>28];
	}
}

static
void ClearScreen(void)
{

}

static
void ScreenOff(void)
{

}

void RenderLine(void)
{
	int pal=0,tile=0;

	if (drmd.vdp_reg1&0x40)
	{
		//screen is off
		ScreenOff();
		return;
	}
	
	if (drmd.vdp_line==0)
	{
		//First line so clear while frame buffer
		//TODO: probably should move this to DoFrame
		ClearScreen();
	}
	
	//Update tile cache
	for(pal=0;pal<PAL_COUNT;pal++)
	{
		if(DirtyPal[pal])
		{
			//If pal has been changed then all tile data will need to be updated
			for(tile=0;tile<2048;tile++)
			{
				UpdateTile(tile,pal);
				DirtyTile[tile]&=~(1<<pal);
			}
		}
		else if(DirtyVram)
		{
		    //If pal has not been touched but vram has been updated then any tiles
			//that are flagged as dirty will need to be updated
			for(tile=0;tile<2048;tile++)
			{
				if(DirtyTile[tile]&(1<<pal))
				{
					UpdateTile(tile,pal);
					DirtyTile[tile]&=~(1<<pal);
				}
			}
		}
	}
	
	//clear other dirty flags
	memset(DirtyPal,0,PAL_COUNT);
	DirtyVram=0;
	
	// Render graphics to current frame buffer
	RenderBackground(1,0);

}





