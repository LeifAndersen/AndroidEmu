      .global md_render_8

;@ --------------------------- Defines ----------------------------
     .include "DrMD_core_vars.s"
    
screen_off:
      ;@ r0 = vdp_reg1 which is read at start of render line
      ;@ get number of lines in a frame
      ldrb r0,[drmd,#vdp_reg1]
      tst r0,#0x8
      moveq r1,#0xE8
      movne r1,#0xF0
      
      ;@ use number of lines in frame to calc pos on gp32 screen
      rsb r0,r6,r1
      sub r0,r0,#1
      ldr r12,[drmd,#frame_buffer]
      add r12,r12,r0
      mov r1,#10
      mov r0,#0x50
screen_off_loop:
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      strb r0,[r12],#240
      
      subs r1,r1,#1
      bne screen_off_loop
      ldmfd sp!,{r0-r12,pc}

clear_screen:
      stmfd sp!,{r0-r12,lr}
      ldr r12,[drmd,#frame_buffer]
      mov lr,#80
      ldrb r0,[drmd,#vdp_reg1]
      tst r0,#0x8
      moveq r11,#0x50
      orreq r11,r11,r11,lsl#8
	  orreq r11,r11,r11,lsl#16
      eorne r11,r11,r11
	  mov r10,r11
	  
	  mov r2,#0
	  mov r3,r2
	  mov r4,r2
	  mov r5,r2
	  mov r6,r2
	  mov r7,r2
	  mov r8,r2
	  mov r9,r2

1:	  
	  mov r0,r11
	  mov r1,r11
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  mov r0,r2
	  mov r1,r2
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r2,r3,r4,r5,r6,r7,r8,r9,r10,r11}
	  
	  mov r0,r11
	  mov r1,r11
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  mov r0,r2
	  mov r1,r2
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r2,r3,r4,r5,r6,r7,r8,r9,r10,r11}
	  
	  mov r0,r11
	  mov r1,r11
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  mov r0,r2
	  mov r1,r2
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r2,r3,r4,r5,r6,r7,r8,r9,r10,r11}
	  
	  mov r0,r11
	  mov r1,r11
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  mov r0,r2
	  mov r1,r2
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  stmia r12!,{r2,r3,r4,r5,r6,r7,r8,r9,r10,r11}

	  subs lr,lr,#1
	  bne 1b

      ldmfd sp!,{r0-r12,pc}

window_check:
      /* Now see if a window needs to be drawn instead of layer A */
      ldrh r6,[drmd,#vdp_line]
      ldrb r0,[drmd,#vdp_reg18]
      and r1,r0,#0x1F
      mov r1,r1,lsl#3
      tst r0,#0x80
      beq window_bottom
window_top:
      cmp r6,r1
      movge r0,#1
      movlt r0,#0
      b window_check_done
window_bottom:      
      cmp r6,r1
      movlt r0,#1
      movge r0,#0
window_check_done:
      str r0, window_flag
      mov pc,lr
      
;@  gp32_vid_mem: .word 0x0C7B4000   
md_render_8:
      stmfd sp!,{r0-r12,lr}
      
      
      ;@ r6 = line
      ;@ r5 = Pointer to DrMD context
      
      ;@ r0 = vdp_reg1 which is read at start of render line
      tst r6,r6
      bne 1f
      bl clear_screen
      ;@ bl window_check
1:      
      
      ldrh r6,[drmd,#vdp_line]
      tst r6,r6
      bleq parse_sprites
      
      
      ldrb r0,[drmd,#vdp_reg1]
      tst r0,#0x40
      beq screen_off
      
render_line_8_start:      
      ;@ get number of lines in a frame
      ldrb r0,[drmd,#vdp_reg1]
      tst r0,#0x8
      moveq r1,#0xE8
      movne r1,#0xF0
      
      ;@ use number of lines in frame to calc pos on gp32 screen
      rsb r0,r6,r1
      sub r0,r0,#1
      ldr r12,[drmd,#frame_buffer]
      add r12,r12,r0
      ldrb r0,[drmd,#vdp_reg12]
      tst r0,#1
      addeq r12,r12,#0x1E00
      str r12,gp32_curr_line
      ;@ also set number of tiles on a screen
      moveq r0,#33
      movne r0,#41
      str r0,tile_count
      
      ;@ plane B low  
      ;@ r0 = plane
      ;@ r4 = high
      ;@ r12 = pointer to gp32 vid mem
draw_layerB_low:
      ;@ setup pointers for high tiles

      ldr r12,gp32_curr_line
      mov r0,#1  ;@ plane B
      ldr r4,planeb_data
      str r4,current_plane
      ldrb r1,[drmd,#vdp_reg11]
      tst r1,#4
      add lr,pc,#4
      beq draw_plane
      bne draw_plane_vs
      
      /*ldrh r6,[drmd,#vdp_line]
      tst r6,r6
      beq 1f
      ldrb r0,[drmd,#vdp_reg18]
      and r0,r0,#0x1F
      cmp r6,r0,lsl#3
      bne 1f*/
      bl window_check
      
1:

      ldr r0,window_flag
      tst r0,r0
      bne draw_window_low
draw_layerA_low:
      ldr r12,gp32_curr_line
      mov r0,#0  ;@ plane A
      ldr r4,planea_data
      str r4,current_plane
      ldrb r1,[drmd,#vdp_reg11]
      tst r1,#4
      add lr,pc,#4
      beq draw_plane
      bne draw_plane_vs
      b draw_layerA_low_done
draw_window_low:
      ldr r12,gp32_curr_line 
      ldr r4,planea_data
      str r4,current_plane
      bl draw_window 
draw_layerA_low_done:
     
      ldrh r6,[drmd,#vdp_line]
      bl parse_sprite_line
      
      ldr r0,sprite_low_buffer 
      str r0,current_sprite_buffer
      bl render_sprites
      
      
      /* now render the high tiles
        These are now created in the first parse of the layer ( ie low )
	The data is stored in 2 buffers, one for layer A and one for layer B
      */
      
      
      ldr r4,planeb_data
      str r4,current_plane
      bl draw_plane_high
      
      
      ;@ setup planeA or window high tile pointers
      ldr r4,planea_data
      str r4,current_plane
      bl draw_plane_high
      
      ldr r0,sprite_high_buffer 
      str r0,current_sprite_buffer
      bl render_sprites
      
      
      ;@ only mask the screen off once
      ;@ rather than on every line
      ;@ so if screen is in small mode at end of frame
      ;@ mask it
      ldrh r0,[drmd,#vdp_line]
      and r0,r0,#3
      cmp r0,#3
      bne mask_screen_skip
      ldrb r0,[drmd,#vdp_reg12]
      tst r0,#1
      bleq mask_screen
mask_screen_skip:
      ldmfd sp!,{r0-r12,lr}
      mov pc,lr

col_mask_table: .word col_mask_table_data
col_mask_table_data: .byte 0x1F, 0x3F, 0x1F, 0x7F
row_mask_table: .word row_mask_table_data
row_mask_table_data: .word 0x0FF, 0x1FF, 0x2FF, 0x3FF      
shift_table2: .word shift_table2_data
shift_table2_data: .byte 6, 7, 0, 8

draw_plane:
      ldrb r8,[drmd,#vdp_reg16]
      mov r9,r8,lsr#4
      and r9,r9,#0x3   ;@ get height of map
      and r8,r8,#0x3   ;@ get width of map

      ldr r10,shift_table2
      ldrb r11,[r10,r8]  
      ldr r10,col_mask_table
      ldrb r3,[r10,r8]
      ldr r10,row_mask_table
      ldr r9,[r10,r9,lsl#2]

      ldrb r2,[drmd,#vdp_reg13]
      mov r2,r2,lsl#10
      ldrb r10,[drmd,#vdp_reg11]
	  and r10,r10,#3
	  ldr pc,[pc,r10,lsl#2]
	  .word 0
	  .word draw_plane_hscroll_fullscreen
	  .word draw_plane_hscroll_first_8
	  .word draw_plane_hscroll_row
	  .word draw_plane_hscroll_line
	  
draw_plane_hscroll_fullscreen:
      ldr r10,[drmd,#vram]	
	  add r2,r2,r0,lsl#1
	  mov r2,r2,lsl#16
	  add r8,r10,r2,lsr#16
      ldrh r2,[r8] ;@ get hscroll BG A or B
	  b 1f
draw_plane_hscroll_first_8:
	  ldr r10,[drmd,#vram]	
	  add r2,r2,r0,lsl#1
	  and r8,r6,#7
	  add r2,r2,r8,lsl#2
	  mov r2,r2,lsl#16
	  add r8,r10,r2,lsr#16
      ldrh r2,[r8] ;@ get hscroll BG A or B
	  b 1f
draw_plane_hscroll_row:
      ldr r10,[drmd,#vram]	
	  add r2,r2,r0,lsl#1
	  bic r8,r6,#7
	  add r2,r2,r8,lsl#2
	  mov r2,r2,lsl#16
	  add r8,r10,r2,lsr#16
      ldrh r2,[r8] ;@ get hscroll BG A or B
	  b 1f
draw_plane_hscroll_line:
      ldr r10,[drmd,#vram]	
	  add r2,r2,r0,lsl#1
	  add r2,r2,r6,lsl#2
	  mov r2,r2,lsl#16
	  add r8,r10,r2,lsr#16
      ldrh r2,[r8] ;@ get hscroll BG A or B
1:
      mov r7,#0x300
	  orr r7,r7,#0xFF
	  and r2,r2,r7
	  
	  ldr r7,[drmd,#vsram]
      add r8,r7,r0,lsl#1
      ldrh r7,[r8] ;@ get vscroll BG A
           
      add r1,r6,r7
      and r1,r1,r9

      mov r7,r1,lsr#3
      tst r0,r0
      ldreqb r0,[drmd,#vdp_reg2]
      andeq r0,r0,#0x38
      moveq r0,r0,lsl#10
      ldrneb r0,[drmd,#vdp_reg4]
      andne r0,r0,#0x7
      movne r0,r0,lsl#13
      
      add r0,r0,r7,lsl r11
      add r0,r0,r10  ;@ base ntab in vram
      
      /*
       before drawing line need to know
       r0 - nametab
       r1 - line
       r2 - hscroll
       r3 - xmask
       r4 - plane 
       r12 = gp32 vid mem
      */
draw_tile:
      stmfd sp!,{lr}  ;@ save lr reg, as it will be used to high buffer pointer
      
      
      
      ;@ r6 = tilex
      ;@ r7 = ty
      ;@ r8 = dx
      
      rsb r6,r2,#0     ;@  get first tile
      and r8,r6,#7
      mov r7,#240
      mul r10,r7,r8
      sub r12,r12,r10
          
      
      
      mov r6,r6,lsr#3
      and r6,r6,r3
      add r3,r3,#1
      orr r3,r3,r6,lsl#16
      
      and r7,r1,#7
      mov r7,r7,lsl#2
      
      ;@ setup buffer pointers for high tiles
      ldr r1,[r4,#0x4]  ;@ plane_high     
      ldr r2,[drmd,#vram]
      ldr r8,tile_count
draw_tile_loop: 
      ;@ ####################
      ;@ tile 1
      ;@ ####################
      add r10,r0,r3,lsr#15
      ldrh r9,[r10]
      mov r10,r9,lsl#21  ;@ get tile number 7FF
      tst r9,#0x1000
      rsbne r11,r7,#28
      addne r10,r11,r10,lsr#16
      addeq r10,r7,r10,lsr#16
      ldr r10,[r2,r10]  ;@ get MD tile data
      tst r10,r10
      beq 2f ;@  whole tile blank no skip
      tst r9,#0x8000
      bne 3f   ;@ high tile so add to high buffer 
      tst r9,#0x800
      mov r9,r9,lsr#9
      and r9,r9,#0x30
      bne 1f
      ands r11,r10,#0x0000F000
      orrne r11,r9,r11,lsr#12
      strneb r11,[r12]
      ands r11,r10,#0x00000F00
      orrne r11,r9,r11,lsr#8
      strneb r11,[r12,#240]
      ands r11,r10,#0x000000F0
      orrne r11,r9,r11,lsr#4
      strneb r11,[r12,#480]
      ands r11,r10,#0x0000000F
      orrne r11,r9,r11
      strneb r11,[r12,#720]
      ands r11,r10,#0xF0000000
      orrne r11,r9,r11,lsr#28
      strneb r11,[r12,#960]
      ands r11,r10,#0x0F000000
      orrne r11,r9,r11,lsr#24
      strneb r11,[r12,#1200]
      ands r11,r10,#0x00F00000
      orrne r11,r9,r11,lsr#20
      strneb r11,[r12,#1440]
      ands r11,r10,#0x000F0000
      orrne r11,r9,r11,lsr#16
      strneb r11,[r12,#1680]
      b 2f 
3:
      str r12,[r1],#4  ;@ save pointer to gp32 mem
      strh r9,[r1],#4   ;@ save map data
      str r10,[r1],#8  ;@ save tile data
      b 2f     
1:
      ands r11,r10,#0x000F0000
      orrne r11,r9,r11,lsr#16
      strneb r11,[r12]
      ands r11,r10,#0x00F00000
      orrne r11,r9,r11,lsr#20
      strneb r11,[r12,#240]
      ands r11,r10,#0x0F000000
      orrne r11,r9,r11,lsr#24
      strneb r11,[r12,#480]
      ands r11,r10,#0xF0000000
      orrne r11,r9,r11,lsr#28
      strneb r11,[r12,#720]
      ands r11,r10,#0x0000000F
      orrne r11,r9,r11
      strneb r11,[r12,#960]
      ands r11,r10,#0x000000F0
      orrne r11,r9,r11,lsr#4
      strneb r11,[r12,#1200]
      ands r11,r10,#0x00000F00
      orrne r11,r9,r11,lsr#8
      strneb r11,[r12,#1440]
      ands r11,r10,#0x0000F000
      orrne r11,r9,r11,lsr#12
      strneb r11,[r12,#1680] 
2:
      add r12,r12,#0x780     
      add r3,r3,#1<<16
      bic r3,r3,r3,lsl#16
      
      subs r8,r8,#1
      bne draw_tile_loop
      ldr r0,current_plane
      ldr r2,[r0,#0x4]  ;@ plane_high
      sub r2,r1,r2
      mov r2,r2,lsr#4  ;@ divide diff by 16
      str r2,[r0]
      ldmfd sp!,{pc}

playfield_row_mask: .word 0
playfield_shift: .word 0
nametab: .word 0
layer: .word 0
draw_plane_vs:
      str r0,layer
      ldrb r8,[drmd,#vdp_reg16]
      mov r9,r8,lsr#4
      and r9,r9,#0x3   ;@ get height of map
      and r8,r8,#0x3   ;@ get width of map

      ldr r10,shift_table2
      ldrb r11,[r10,r8]  
      strb r11,playfield_shift  ;@ save shift value
      
      ldr r10,col_mask_table
      ldrb r3,[r10,r8]
      
      ldr r10,row_mask_table
      ldr r9,[r10,r9,lsl#2]
      str r9,playfield_row_mask ;@ save row mask
      
      ldrb r2,[drmd,#vdp_reg13]
      mov r2,r2,lsl#10
      ldrb r10,[drmd,#vdp_reg11]
      tst r10,#2
      addne r2,r2,r6,lsl#2
      tst r10,#1
      biceq r2,r2,#0x1F
      add r2,r2,r0,lsl#1
      ldr r10,[drmd,#vram]
      mov r2,r2,lsl#16
      add r8,r10,r2,lsr#16
      ldrh r2,[r8] ;@ get h scroll

      ;@ get pointer to current name table a or b
      tst r0,r0
      ldreqb r0,[drmd,#vdp_reg2]
      andeq r0,r0,#0x38
      moveq r0,r0,lsl#10
      ldrneb r0,[drmd,#vdp_reg4]
      andne r0,r0,#0x7
      movne r0,r0,lsl#13
      add r0,r0,r10  ;@ base ntab in vram
      str r0,nametab  ;@ save base of name table
      
draw_tile_vs:
      stmfd sp!,{lr}  ;@ save lr reg, as it will be used to high buffer pointer
     
      rsb r10,r2,#0     ;@  get first tile
      and r8,r10,#7
      mov r7,#240
      mul r11,r7,r8
      sub r12,r12,r11
      
      mov r10,r10,lsr#3
      and r10,r10,r3
      add r3,r3,#1
      orr r3,r3,r10,lsl#16
      
      ;@ setup buffer pointers for high tiles
      ldr r1,[r4,#0x4]  ;@ plane_high     
      ldr r2,[drmd,#vram]
      ldr r8,tile_count
draw_tile_vs_loop: 
      ;@ ####################
      ;@ tile 1
      ;@ ####################
      ;@ in VS mode, hscroll and tile line need to
      ;@ be re-calculated for each tile.
      
      ldr r7,[drmd,#vsram]
      ldr r10,tile_count
      sub r10,r10,r8
      mov r10,r10,lsr#1
      add r7,r7,r10,lsl#2
      
      ldr r10,layer
      add r7,r7,r10,lsl#1  ;@ first word or second ( A or B )
      ldrh r10,[r7] ;@ get vscroll for col index
      bic r10,r10,#0xFC00
      add r10,r10,r6 ;@ add line
      ldr r7,playfield_row_mask
      and r10,r10,r7
      and r7,r10,#7
      mov r7,r7,lsl#2
      
      mov r10,r10,lsr#3
      ldr r0,nametab
      ldrb r11,playfield_shift
      add r0,r0,r10,lsl r11
      
      add r10,r0,r3,lsr#15
      ldrh r9,[r10]
      mov r10,r9,lsl#21  ;@ get tile number 7FF
      tst r9,#0x1000
      rsbne r11,r7,#28
      addne r10,r11,r10,lsr#16
      addeq r10,r7,r10,lsr#16
      ldr r10,[r2,r10]  ;@ get MD tile data
      tst r10,r10
      beq 2f ;@  whole tile blank no skip
      tst r9,#0x8000
      bne 3f   ;@ high tile so add to high buffer 
      tst r9,#0x800
      mov r9,r9,lsr#9
      and r9,r9,#0x30
      bne 1f
      ands r11,r10,#0x0000F000
      orrne r11,r9,r11,lsr#12
      strneb r11,[r12]
      ands r11,r10,#0x00000F00
      orrne r11,r9,r11,lsr#8
      strneb r11,[r12,#240]
      ands r11,r10,#0x000000F0
      orrne r11,r9,r11,lsr#4
      strneb r11,[r12,#480]
      ands r11,r10,#0x0000000F
      orrne r11,r9,r11
      strneb r11,[r12,#720]
      ands r11,r10,#0xF0000000
      orrne r11,r9,r11,lsr#28
      strneb r11,[r12,#960]
      ands r11,r10,#0x0F000000
      orrne r11,r9,r11,lsr#24
      strneb r11,[r12,#1200]
      ands r11,r10,#0x00F00000
      orrne r11,r9,r11,lsr#20
      strneb r11,[r12,#1440]
      ands r11,r10,#0x000F0000
      orrne r11,r9,r11,lsr#16
      strneb r11,[r12,#1680]
      b 2f 
3:
      str r12,[r1],#4  ;@ save pointer to gp32 mem
      strh r9,[r1],#4   ;@ save map data
      str r10,[r1],#8  ;@ save tile data
      b 2f     
1:
      ands r11,r10,#0x000F0000
      orrne r11,r9,r11,lsr#16
      strneb r11,[r12]
      ands r11,r10,#0x00F00000
      orrne r11,r9,r11,lsr#20
      strneb r11,[r12,#240]
      ands r11,r10,#0x0F000000
      orrne r11,r9,r11,lsr#24
      strneb r11,[r12,#480]
      ands r11,r10,#0xF0000000
      orrne r11,r9,r11,lsr#28
      strneb r11,[r12,#720]
      ands r11,r10,#0x0000000F
      orrne r11,r9,r11
      strneb r11,[r12,#960]
      ands r11,r10,#0x000000F0
      orrne r11,r9,r11,lsr#4
      strneb r11,[r12,#1200]
      ands r11,r10,#0x00000F00
      orrne r11,r9,r11,lsr#8
      strneb r11,[r12,#1440]
      ands r11,r10,#0x0000F000
      orrne r11,r9,r11,lsr#12
      strneb r11,[r12,#1680] 
2:
      add r12,r12,#0x780     
      add r3,r3,#1<<16
      bic r3,r3,r3,lsl#16
      
      subs r8,r8,#1
      bne draw_tile_vs_loop
      ldr r0,current_plane
      ldr r2,[r0,#0x4]  ;@ plane_high
      sub r2,r1,r2
      mov r2,r2,lsr#4  ;@ divide diff by 16
      str r2,[r0]
      ldmfd sp!,{pc}
      

gp32_curr_line: .word 0
tile_count: .word 0 
window_flag: .word 0 
current_plane:   .word planeb_data

planeb_data: .word planeb_count
planea_data: .word planea_count


planeb_count:     .word 0
planeb_buffer: .word planeb_high_data


planea_count:      .word 0   ;@ number of tiles to render
planea_high: .word planea_high_data

draw_plane_high:
      ldr r0,[r4,#0x0] ;@ get tile count
      tst r0,r0
      moveq pc,lr
      
      ldr r1,[r4,#0x4] ;@ pointer to md tile data
draw_plane_high_loop: 
      ldr r12,[r1],#4
      ldrh r9,[r1],#4   
      ldr r10,[r1],#8      
      tst r9,#0x800
      mov r9,r9,lsr#9
      and r9,r9,#0x30
      bne draw_plane_high_tile_flip
draw_plane_high_tile_norm:
      ands r11,r10,#0x0000F000
      orrne r11,r9,r11,lsr#12
      strneb r11,[r12]
      ands r11,r10,#0x00000F00
      orrne r11,r9,r11,lsr#8
      strneb r11,[r12,#240]
      ands r11,r10,#0x000000F0
      orrne r11,r9,r11,lsr#4
      strneb r11,[r12,#480]
      ands r11,r10,#0x0000000F
      orrne r11,r9,r11
      strneb r11,[r12,#720]
      ands r11,r10,#0xF0000000
      orrne r11,r9,r11,lsr#28
      strneb r11,[r12,#960]
      ands r11,r10,#0x0F000000
      orrne r11,r9,r11,lsr#24
      strneb r11,[r12,#1200]
      ands r11,r10,#0x00F00000
      orrne r11,r9,r11,lsr#20
      strneb r11,[r12,#1440]
      ands r11,r10,#0x000F0000
      orrne r11,r9,r11,lsr#16
      strneb r11,[r12,#1680]      
      b draw_plane_high_tile_next

draw_plane_high_tile_flip:
      ands r11,r10,#0x000F0000
      orrne r11,r9,r11,lsr#16
      strneb r11,[r12]
      ands r11,r10,#0x00F00000
      orrne r11,r9,r11,lsr#20
      strneb r11,[r12,#240]
      ands r11,r10,#0x0F000000
      orrne r11,r9,r11,lsr#24
      strneb r11,[r12,#480]
      ands r11,r10,#0xF0000000
      orrne r11,r9,r11,lsr#28
      strneb r11,[r12,#720]
      ands r11,r10,#0x0000000F
      orrne r11,r9,r11
      strneb r11,[r12,#960]
      ands r11,r10,#0x000000F0
      orrne r11,r9,r11,lsr#4
      strneb r11,[r12,#1200]
      ands r11,r10,#0x00000F00
      orrne r11,r9,r11,lsr#8
      strneb r11,[r12,#1440]
      ands r11,r10,#0x0000F000
      orrne r11,r9,r11,lsr#12
      strneb r11,[r12,#1680] 
draw_plane_high_tile_next:      
      subs r0,r0,#1
      bne draw_plane_high_loop
      mov pc,lr
      
mask_screen:
      ldr r12,gp32_curr_line
      sub r12,r12,#0x1E00
      mov r0,#0x50
      orr r0,r0,r0,lsl#8
      orr r0,r0,r0,lsl#16
      mov r2,#2
mask_screen_loop: 
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      str r0,[r12],#240 
      str r0,[r12],#240
      
      add r12,r12,#0xF000
      subs r2,r2,#1
      bne mask_screen_loop
      mov pc,lr
      
draw_window: 
      ldrb r0,[drmd,#vdp_reg12]
      tst r0,#1
      beq draw_window_32cell
draw_window_40cell:
      ldrb r0,[drmd,#vdp_reg3]
      mov r0,r0,lsl#10
      and r0,r0,#0xF000
      mov r1,r6,lsr#3
      orr r0,r0,r1,lsl#7
      b draw_window_setnametab_done
draw_window_32cell:
      ldrb r0,[drmd,#vdp_reg3]
      mov r0,r0,lsl#10
      and r0,r0,#0xF800
      mov r1,r6,lsr#3
      orr r0,r0,r1,lsl#6
      ;@ add r12,r12,#0x1E00 ;@ centre window
draw_window_setnametab_done:      
      ldr r1,[drmd,#vram]
      add r0,r0,r1
      mov r2,#0
      mov r3,#0x3F  ;@ xmask
      mov r1,r6     ;@ set line
      stmfd sp!,{lr}
      bl draw_tile
      ldmfd sp!,{lr}
      mov pc,lr

parse_sprite_line:
     ;@ scan sprites to find any on current scan line
     ldr r0,sprite_lookup_pointer2
     add r0,r0,r6,lsl#5
     ldrb r1,[r0]
     tst r1,r1
     streq r1,sprite_low_count
     streq r1,sprite_high_count
     moveq pc,lr
     
     stmfd sp!,{r4-r12,lr}
     mov r9,r0
     mov r8,r1
     ldr r0,sprite_low_table
     str r0,sprite_low_count
     ldr r0,sprite_high_table
     str r0,sprite_high_count
     
     eor r0,r0,r0
     strb r0,[r9],#1
     str r0,curr_sprite_vmax
     
     
     
     add r6,r6,#0x80
     ldrb r0,[drmd,#vdp_reg12]
    
     tst r0,#1
     movne lr,#0x28  ;@ set max sprites pixels on vdp line
     moveq lr,#0x20
     str lr,sprite_limit

     ldr r10,[drmd,#vram]
     ldrb r0,[drmd,#vdp_reg5]
     andne r0,r0,#0x7E
     andeq r0,r0,#0x7F
     add r10,r10,r0,lsl#9   ;@ get sprite base address
     
     ;@ r5 = pointer to drmd context
     ;@ r6 = line
     ;@ r10 = sprite data in vram pointer
     ;@ r11 = link     
parse_sprite_line_main_loop:
     ldrb r11,[r9],#1 ;@ get next sprite
     add r0,r10,r11,lsl#3
     ldrh r1,[r0] ;@ get y
     bic r1,r1,#0xFE00 ;@ mask y 0x1FF   
     ldrb r2,[r0,#3]   ;@ get height and width info
     and r3,r2,#3
     add r3,r3,#1
     add r4,r1,r3,lsl#3
     str r4,curr_sprite_vmax
     mov r2,r2,lsr#2
     and r2,r2,#3
     ;@ sprite is in range so add it to sprites
     ;@ to be buffered 
     ;@ r0 = sprite raw data
     ;@ r1 = y
     ;@ r2 = width  0 1 2 3
     ;@ r3 = height 1 2 3 4
     ;@ r4 = free
     ;@ r7 = free
     ;@ r8 = total sprites
     ;@ r11 = free
     ldrh r4,[r0,#4]
     tst r4,#0x8000
     ldreq r12,sprite_low_count   ;@ where to write sprite data to
     ldrne r12,sprite_high_count   ;@ where to write sprite data to
     strh r4,[r12],#2  ;@ save code (pri,pal,xflip,yflip)
     
     sub r1,r6,r1  ;@ row
     tst r4,#0x1000
     movne r7,r3,lsl#3
     subne r7,r7,#1
     subne r1,r7,r1  ;@ row flip y
     
     tst r4,#0x800   ;@ delta flip
     bic r11,r4,#0xF800  ;@ get tile
     mov r7,r1,lsr#3
     add r11,r11,r7
     mulne r7,r3,r2
     addne r11,r11,r7
     rsbne r3,r3,#0
     
     
     mov r11,r11,lsl#5
     and r1,r1,#7
     add r11,r11,r1,lsl#2
         
     strh r11,[r12],#2  ;@ save tile
     mov r3,r3,lsl#5
     strh r3,[r12],#2  ;@ save delta
     strh r2,[r12],#2  ;@ save width
     add r2,r2,#1
     ldrh r1,[r0,#6]
     bics r1,r1,#0xFE00
     sub r1,r1,#0x78
     strh r1,[r12],#8 ;@ save x
     beq parse_sprites_checkmask  
parse_sprites_checkmask_return:
     ldr r1,curr_sprite_vmax
     str r1,prev_sprite_vmax
     
     tst r4,#0x8000
     streq r12,sprite_low_count   ;@ where to write sprite data to
     strne r12,sprite_high_count   ;@ where to write sprite data to
     
     cmp lr,r2
     subgt lr,lr,r2
     bgt parse_sprite_line_next
     sub lr,lr,#1
     sub r2,r12,#10
     strh lr,[r2]  ;@ update sprite width
     
     ldrb r0,[drmd,#vint_pending]
     tst r0,r0
     bne parse_sprite_line_end
     ldrh r0,[drmd,#vdp_status]
     orr r0,r0,#0x40
     strh r0,[drmd,#vdp_status]
     b parse_sprite_line_end
parse_sprite_line_next:  
     subs r8,r8,#1
     bne parse_sprite_line_main_loop
parse_sprite_line_end:
     ldr r0,sprite_low_count
     ldr r1,sprite_low_table
     sub r0,r0,r1
     mov r0,r0,lsr#4
     str r0,sprite_low_count
     ldr r0,sprite_high_count
     ldr r1,sprite_high_table
     sub r0,r0,r1
     mov r0,r0,lsr#4
     str r0,sprite_high_count
     ldmfd sp!,{r4-r12,pc}


parse_sprites_checkmask:
     ldr r0,sprite_limit
     cmp lr,r0  ;@ check to see if first sprite
     beq parse_sprites_checkmask_mask
     ldr r1,prev_sprite_vmax
     cmp r1,r6
     ble parse_sprites_checkmask_return
parse_sprites_checkmask_mask:
     cmp lr,r2
     movgt lr,r2
     b parse_sprites_checkmask_return

parse_sprites:
     ;@ scan sprites to find any on current scan line
     ;@ and are actually being displayed
     ;@ this needs to run even if frame is skipped
     stmfd sp!,{r4-r12,lr}
     
    
        
     ldrb r0,[drmd,#vdp_reg12]   
     tst r0,#1
     movne r7,#0x14  ;@ set max sprites on vdp line
     moveq r7,#0x10

     movne r8,#80 ;@ set total sprites to scan
     moveq r8,#64
     ldr r10,[drmd,#vram]
     ldrb r0,[drmd,#vdp_reg5]
     andne r0,r0,#0x7E
     andeq r0,r0,#0x7F
     add r10,r10,r0,lsl#9   ;@ get sprite base address
     mov r11,#0             ;@ reset sprite link to 0
     
     ldr r6,sprite_lookup_pointer
     ;@ r5 = pointer to drmd context
     ;@ r6 = pointer to sprite lookup
     ;@ r9 = current sprite count
     ;@ r10 = sprite data in vram pointer
     ;@ r11 = link  
     ;@ lr = ymask     
parse_sprites_main_loop:
     add r0,r10,r11,lsl#3
     ldrh r1,[r0] ;@ get y
     bic r1,r1,#0xFE00 ;@ mask y 0x1FF
     cmp r1,#96
     blt parse_sprites_next ;@ sprite never appears
     cmp r1,#400
     bge parse_sprites_next ;@ sprite never appears
     ldrb r2,[r0,#3]   ;@ get height and width info
     and r3,r2,#3
     add r3,r3,#1
     mov r3,r3,lsl#3
     add r4,r6,r1,lsl#5  ;@ get start point in buffer
parse_sprites_update_sprite_line:     
     ldrb r2,[r4]  ;@ get sprite count
     cmp r2,r7  ;@ check sprite limit
     addge r4,r4,#32 
     bge parse_sprites_update_sprite_line_skip
     
     add r2,r2,#1  ;@ increase sprite count
     strb r11,[r4,r2] ;@ add sprite no to buffer
     strb r2,[r4],#32  ;@ update sprite count
                       ;@ and move to next line
parse_sprites_update_sprite_line_skip:
     subs r3,r3,#1
     bne parse_sprites_update_sprite_line
     
parse_sprites_next:  
     ldrh r11,[r0,#2]   
     ands r11,r11,#0x7F  ;@  get next sprite link
     ldmeqfd sp!,{r4-r12,pc} ;@  exit if sprite 0
     subs r8,r8,#1
     bne parse_sprites_main_loop
     
     ldmfd sp!,{r4-r12,pc}
     

     
;@ 96 = 0
;@ 128 = 32    
sprite_lookup_pointer: .word sprite_lookup-(96*32)
sprite_lookup_pointer2: .word sprite_lookup+(32*32)

curr_sprite_vmax: .word 0
prev_sprite_vmax:  .word 0

sprite_limit: .word 0     
sprite_count: .word 0
current_sprite_buffer: .word sprite_low_count

sprite_low_buffer: .word sprite_low_count
sprite_high_buffer: .word sprite_high_count

sprite_low_count: .word 0
sprite_low_table: .word sprite_low_table_data  

sprite_high_count: .word 0
sprite_high_table: .word sprite_high_table_data 
   

    

render_sprites:
      ;@ r0 = temp pointer to current sprite in buffer
      ;@ r1 = sprite count - invalid
      ;@ r2 = pointer vram
      ;@ r3 = xpos
      ;@ r4 = delta
      ;@ r5 = drsms
      ;@ r6 = code
      ;@ r7 = width
      ;@ r8 = tile data offset
      ;@ r9 = free
      ;@ r10 = free
      ;@ r11 = free
      ;@ r12 = free
      ;@ lr = free
      ldr r0,current_sprite_buffer
      ldr r1,[r0] ;@ get sprite count
      tst r1,r1
      moveq pc,lr
      stmfd sp!,{lr}
      ldr lr,[r0,#4]
      sub r1,r1,#1
      ldr r2,[drmd,#vram]  ;@ create pointer to vram
      
render_sprite_loop:
      add r0,lr,r1,lsl#4   ;@ create pointer to sprite data
      ldr r12,gp32_curr_line ;@ reset pointer to start of vdp line
      ldrh r6,[r0]       ;@ get code
      ldrh r7,[r0,#6]   ;@ get width
      ldrh r8,[r0,#2] 
      mov r8,r8,lsl#16
      ldrsh r3,[r0,#8]  ;@ get signed x pos
      ldrsh r4,[r0,#4]  ;@ get delta
      mov r4,r4,lsl#16
render_sprite_dofirst:
      cmp r3,#0
      ble render_sprite_offscreen
      cmp r3,#7
      ble render_sprite_part ;@ do partial sprite
      mov r10,#240
      sub r3,r3,#8
      mul r11,r10,r3
      add r12,r12,r11  ;@ get start xpos
      mov r9,r6,lsr#9
      and r9,r9,#0x30   ;@ get pal
      tst r6,#0x800
      bne render_sprite_flip_loop  
      b render_sprite_norm_loop
render_sprite_part:
      and r10,r3,#7
      eor r10,r10,#7
      mov r11,#240
      mul r9,r11,r10
      sub r12,r12,r9
      mov r9,r6,lsr#9
      and r9,r9,#0x30   ;@ get pal
      sub r3,r3,#8
      tst r6,#0x800
      bne render_sprite_flip_loop  
render_sprite_norm_loop:      
      cmp r3,#0x140
      bge render_sprite_getnext      
      ldr r10,[r2,r8,lsr#16] ;@ get tile data
      tst r10,r10
      beq render_sprite_norm_next ;@  whole tile blank no skip
render_sprite_norm:
      ands r11,r10,#0x0000F000
      orrne r11,r9,r11,lsr#12
      strneb r11,[r12]
      ands r11,r10,#0x00000F00
      orrne r11,r9,r11,lsr#8
      strneb r11,[r12,#240]
      ands r11,r10,#0x000000F0
      orrne r11,r9,r11,lsr#4
      strneb r11,[r12,#480]
      ands r11,r10,#0x0000000F
      orrne r11,r9,r11
      strneb r11,[r12,#720]
      ands r11,r10,#0xF0000000
      orrne r11,r9,r11,lsr#28
      strneb r11,[r12,#960]
      ands r11,r10,#0x0F000000
      orrne r11,r9,r11,lsr#24
      strneb r11,[r12,#1200]
      ands r11,r10,#0x00F00000
      orrne r11,r9,r11,lsr#20
      strneb r11,[r12,#1440]
      ands r11,r10,#0x000F0000
      orrne r11,r9,r11,lsr#16
      strneb r11,[r12,#1680] 
render_sprite_norm_next:
      subs r7,r7,#1
      bmi render_sprite_getnext
      add r8,r8,r4  ;@ add 
      add r3,r3,#8
      add r12,r12,#0x780
      b render_sprite_norm_loop
      
render_sprite_getnext:
      subs r1,r1,#1
      bpl render_sprite_loop 
      ldmfd sp!,{pc}    

render_sprite_offscreen:
      subs r7,r7,#1
      bmi render_sprite_getnext
      add r8,r8,r4 ;@ add 
      add r3,r3,#8
      b render_sprite_dofirst
      
render_sprite_flip_loop:      
      cmp r3,#0x140
      bge render_sprite_getnext      
      ldr r10,[r2,r8,lsr#16] ;@ get tile data
      tst r10,r10
      beq render_sprite_flip_next ;@  whole tile blank no skip
render_sprite_flip:
      ands r11,r10,#0x000F0000
      orrne r11,r9,r11,lsr#16
      strneb r11,[r12]
      ands r11,r10,#0x00F00000
      orrne r11,r9,r11,lsr#20
      strneb r11,[r12,#240]
      ands r11,r10,#0x0F000000
      orrne r11,r9,r11,lsr#24
      strneb r11,[r12,#480]
      ands r11,r10,#0xF0000000
      orrne r11,r9,r11,lsr#28
      strneb r11,[r12,#720]
      ands r11,r10,#0x0000000F
      orrne r11,r9,r11
      strneb r11,[r12,#960]
      ands r11,r10,#0x000000F0
      orrne r11,r9,r11,lsr#4
      strneb r11,[r12,#1200]
      ands r11,r10,#0x00000F00
      orrne r11,r9,r11,lsr#8
      strneb r11,[r12,#1440]
      ands r11,r10,#0x0000F000
      orrne r11,r9,r11,lsr#12
      strneb r11,[r12,#1680] 
render_sprite_flip_next:
      subs r7,r7,#1
      bmi render_sprite_getnext
      add r8,r8,r4  ;@ add 
      add r3,r3,#8
      add r12,r12,#0x780
      b render_sprite_flip_loop
      
 
planeb_high_data: .space 700
planea_high_data: .space 700    ;@ MAX 40 tiles x 16 bytes			    



sprite_lookup: .space (0xF0+64)*32
sprite_high_table_data:
     ;@ this holds the index for each sprite found on
     ;@ the scan line.  The sprite rendering code
     ;@ uses this info
     
     ;@ format
     ;@ code info     = hword 0
     ;@ tile          = hword 2
     ;@ delta         = hword 4
     ;@ width         = hword 6
     ;@ xpos          = hword 8 
     ;@ spare         = hword 10
     ;@ spare         = hword 12
     ;@ spare         = hword 14
     ;@ max 20 sprites on line ( maybe 16 )
     .space 40*(8*2)

sprite_low_table_data:
     ;@ this holds the index for each sprite found on
     ;@ the scan line.  The sprite rendering code
     ;@ uses this info
     
     ;@ format
     ;@ code info     = hword 0
     ;@ tile          = hword 2
     ;@ delta         = hword 4
     ;@ width         = hword 6
     ;@ xpos          = hword 8 
     ;@ spare         = hword 10
     ;@ spare         = hword 12
     ;@ spare         = hword 14
     ;@ max 20 sprites on line ( maybe 16 )
     .space 40*(8*2)
     
     