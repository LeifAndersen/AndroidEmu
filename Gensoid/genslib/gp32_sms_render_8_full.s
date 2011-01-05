       .global sms_render_8_full
       
       .include "DrSMS_core_vars.s"

scale_check: .word scale_check_table	   
scale_check_table: 	
				.byte 0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 
				.byte 0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0 
				.byte 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0 
				.byte 0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0 
				.byte 0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0 
				.byte 0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 
				.byte 0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0 
				.byte 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0 
				.byte 0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0 
				.byte 0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1,0 
				
sms_render_8_full:
     stmfd sp!,{r5-r12,lr}

	 ldr r0,[drsms,#frame_buffer]
	 add r0,r0,#0x780
	 ldrh r1,[drsms,#vdp_line]
	 
	 rsb r2,r1,#191	
     add r2,r2,r2,lsr#2 
	 add r0,r0,r2   ;@ add y offset
     str r0,frame_buffer_start
	 
	 ldr r5,scale_check
	 ldrb r5,[r5,r2]
	 tst r5,r5
	 bne 1f
     ;@ b 2f
     ldrb r0,[drsms,#vdp_reg1]
     tst r0,#1<<6
     beq draw_screenoff
     bl draw_bg_low
     bl draw_sprites
     bl draw_bg_high
	 bl draw_borders
     
     ldmfd sp!,{r5-r12,pc}
	 
1:
     ldrb r0,[drsms,#vdp_reg1]
     tst r0,#1<<6
     beq draw_screenoff2
     bl draw_bg_low2
     bl draw_sprites2
     bl draw_bg_high2
	 bl draw_borders2
2:   
     ldmfd sp!,{r5-r12,pc}
	 
frame_buffer_start: .word 0  

draw_borders:
     ldr r0,frame_buffer_start 
     eor r1,r1,r1
     sub r0,r0,#0x960
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240
	 strb r1,[r0],#240
	 strb r1,[r0],#240
     add r0,r0,#0x11000
	 add r0,r0,#0x940
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240
     strb r1,[r0],#240 
     strb r1,[r0],#240 	 
     strb r1,[r0]
     mov pc,lr
	 
draw_screenoff:
     ldr r0,frame_buffer_start
     mov r1,#0
     mov r2,#10
1:
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240   
     
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240  
     
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240 
     strb r1,[r0],#240  
     
     subs r2,r2,#1
     bne 1b
     ldmfd sp!,{r5-r12,pc} 
  
draw_bg_low:
     stmfd sp!,{lr}
     ldrh r1,[drsms,#vdp_line]
     ldr r0,frame_buffer_start
     ldrh r5,[drsms,#map_addr]
     ldr r10,[drsms,#vdp_memory]
     add r2,r10,r5
     ldrb r4,[drsms,#vdp_reg9]
     add r4,r4,r1
     ;@ the following wraps the screen on the y-axis
1:
     cmp r4,#224
     blt 2f
     sub r4,r4,#224
     b 1b
2:
     mov r5,r4,lsr #3  
     add r2,r2,r5, lsl #6
     ldrb r5,[drsms,#vdp_reg8]
     cmp r1,#0xF
     bgt 1f
     ldrb r6,[drsms,#vdp_reg0]
     tst r6,#1<<6
     eorne r5,r5,r5
1:
     rsb r5,r5,#0
     and r5,r5,#0xFF
     ;@ r0 = pointer to gp32 vid ram
     ;@ r1 = current line
     ;@ r2 = pointer sms map data
     ;@ r5 = backx
     ;@ r9 = x
     ;@ r4 = y val
     ldr r7,[drsms,#vdp_cram]
     and r9,r5,#7
     mov r8,#240
	 add r9,r9,r9,lsr#2
     mul r11,r8,r9
     sub r0,r0,r11
     mov r5,r5,lsr #2
     
     ldrb r9,[drsms,#vdp_reg0]
     tst r9,#1<<7  ;@ check for vertical scroll lock
     moveq r9,#0
     movne r9,#1
     str r9,vertical_lock
     moveq r9,#31
     movne r9,#24  ;@ number of tiles to draw ( 32 tiles is width of map )
     ldr r10,[drsms,#tile_cache]
	 add r5,r5,#2 ;@ skip first tile
1:  
     and r5,r5,#0x3E   ;@ wraps map data for horizontal scroll
     ldrh r12,[r2,r5]
     ;@ r12 = map data
     add r5,r5,#2
     and r6,r12,#0x800
     ;@ 1000 0000 0000
     ;@ 0000 0001 0000   0x10
     ldrb r11,[r7,r6,lsr #7]  ;@ get transparent color
     bic r6,r12,#0xFE00
     add r6,r10,r6,lsl #5 ;@ get pointer to tile cache
     and r8,r4,#7
     tst r12,#0x400        ;@ check vertical flip flap
     rsbne r8,r8,#7
     ldr r6,[r6,r8,lsl #2]
     ;@ r6 = 8 pixels of display data
     tst r12,#0x800        ;@ use bg pal or sprite pal
     addne r8,r7,#0x10
     moveq r8,r7
     ;@ r8 = pal base
     ;@ r11 = transparent color
     tst r12,#0x200        ;@ check horizontal flip flag
     bne 2f  
     ;@ pixel 1
     ands r12,r6,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
 
     subs r9,r9,#1
     bne 1b
     b 3f
vertical_lock: .word 0	
2:
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
     ;@ pixel 1
     ands r12,r6,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r0],#240
     streqb r11,[r0],#240
 
     subs r9,r9,#1
     bne 1b 
3:
     ldr r9,vertical_lock
     tst r9,r9
     ldmeqfd sp!,{pc}
     mov r9,#0
     str r9,vertical_lock
     ldr r2,[drsms,#vdp_memory]
     ldrh r9,[drsms,#map_addr]
     add r2,r2,r9
     mov r9,r1,lsr#3
     add r2,r2,r9,lsl#6
     mov r4,r1
     mov r9,#7
     b 1b

     
draw_sprites:
     stmfd sp!,{lr}
     ldrh r1,[drsms,#vdp_line]
     ldr r5,[drsms,#vdp_memory]
     ldrh r2,[drsms,#sprite_addr]
     add r2,r5,r2
     add r8,r2,#0x80
     ldr r4,[drsms,#tile_cache]
     ldrh r5,[drsms,#sprite_tiles]
     add r5,r4,r5
     ldrb r9,[drsms,#vdp_reg0]
     tst r9,#1<<3
     moveq r9,#8
     movne r9,#16
     ldrb r7,[drsms,#vdp_reg1]
     tst r7,#1<<1
     moveq r7,#8
     movne r7,#16
     ldr r0,sprite_buffer
     add r10,r0,#8*16
     sub r1,r1,#1   ;@ saves having to inc sprite line each sprite
     mov r4,#64     ;@ max sprite counter
1:   
     ldrb r6,[r2],#1
     cmp r6,#0xD0
     beq 3f
     subge r6,r6,#0x100  ;@ switch sign of number if greater than 0xF0
     cmp r1,r6
     blt 2f
     add r11,r6,r7
     cmp r1,r11
     bge 2f

     ;@ get x position and save in sprite buffer
     ldrb r11,[r8]
     sub r11,r11,r9
     strh r11,[r0],#4
     
     ;@ get tile data and save in sprite buffer
     ldrb r11,[r8,#1]
     bic r11,r11,r7, lsr #4
     mov r11,r11,lsl #5
     sub r12,r1,r6
     add r11,r11,r12,lsl #2
     ldr r11,[r5,r11]
     str r11,[r0],#4
     
     ;@ check for sprite overflow - ie 8+ sprites
     cmp r0,r10
     bge 3f
2:
     add r8,r8,#2     
     subs r4,r4,#1
     bne 1b
3: 
     ldr r10,sprite_buffer
     cmp r10,r0
     ldmeqfd sp!,{pc}  ;@ no sprites to render so exit 
     
     ldr r8,[drsms,#vdp_cram]
     add r8,r8,#0x10  ;@ get pointer to pal 
     ldr r2,frame_buffer_start
     mov r5,#240
1:
     ldr r11,[r0,#-4]!
     
     ldrsh r1,[r0,#-4]!

	 cmp r1,#-7
	 blt 2f
	 
	 cmp r1,#240
	 bge 2f
	 
     add r1,r1,r1,lsr#2      
     mul r12,r5,r1
     add r4,r2,r12
     
     ;@ pixel 1
     ands r12,r11,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r4]
     strneb lr,[r4,#240]
     ;@ pixel 2
     ands r12,r11,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r4,#480]
     ;@ pixel 3
     ands r12,r11,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r4,#720]
     ;@ pixel 4
     ands r12,r11,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r4,#960]
     ;@ pixel 5
     ands r12,r11,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r4,#1200]
     strneb lr,[r4,#1440]
     ;@ pixel 6
     ands r12,r11,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r4,#1680]
     ;@ pixel 7
     ands r12,r11,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r4,#1920]
     ;@ pixel 8
     ands r12,r11,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r4,#2160]
2:     
     cmp r0,r10
     bgt 1b
     ldmfd sp!,{pc}

sprite_buffer: .word sprite_buffer_data
sprite_buffer_data: .space 32*8

     
draw_bg_high:
     stmfd sp!,{lr}
     ldrh r1,[drsms,#vdp_line]
     ldr r0,frame_buffer_start
     ldrh r5,[drsms,#map_addr]
     ldr r10,[drsms,#vdp_memory]
     add r2,r10,r5
     ldrb r4,[drsms,#vdp_reg9]
     add r4,r4,r1
     ;@ the following wraps the screen on the y-axis
1:
     cmp r4,#224
     blt 2f
     sub r4,r4,#224
     b 1b
2:
     mov r5,r4,lsr #3  
     add r2,r2,r5, lsl #6
     ldrb r5,[drsms,#vdp_reg8]
     cmp r1,#0xF
     bgt 1f
     ldrb r6,[drsms,#vdp_reg0]
     tst r6,#1<<6
     eorne r5,r5,r5
1:  ;@ locktop 16 rows
     rsb r5,r5,#0
     and r5,r5,#0xFF
     ;@ r0 = pointer to gp32 vid ram
     ;@ r1 = current line
     ;@ r2 = pointer sms map data
     ;@ r5 = backx
     ;@ r9 = x
     ;@ r4 = y val
     ldr r7,[drsms,#vdp_cram]
     and r9,r5,#7
	 add r9,r9,r9,lsr#2
     mov r8,#240
     mul r11,r8,r9
     sub r0,r0,r11
     mov r5,r5,lsr #2
     
     ldrb r9,[drsms,#vdp_reg0]
     tst r9,#1<<7  ;@ check for vertical scroll lock
     moveq r9,#0
     movne r9,#1
     str r9,vertical_lock
     moveq r9,#31
     movne r9,#24  ;@ number of tiles to draw ( 32 tiles is width of map )
     ldr r10,[drsms,#tile_cache]
	 add r5,r5,#2 ;@ skip first tile
1:  
     and r5,r5,#0x3E   ;@ wraps map data for horizontal scroll
     ldrh r12,[r2,r5]
     add r5,r5,#2
     ;@ r12 = map data
     tst r12,#1<<0xC
     beq 3f

     bic r6,r12,#0xFE00
     add r6,r10,r6,lsl #5 ;@ get pointer to tile cache
     and r8,r4,#7
     tst r12,#0x400        ;@ check vertical flip flap
     rsbne r8,r8,#7
     ldr r6,[r6,r8,lsl #2]
     ;@ r6 = 8 pixels of display data
     tst r12,#0x800        ;@ use bg pal or sprite pal
     addne r8,r7,#0x10
     moveq r8,r7
     ;@ r8 = pal base
     ;@ r11 = transparent color
     tst r12,#0x200        ;@ check horizontal flip flag
     bne 2f  
     ;@ pixel 1
     ands r12,r6,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r0]
     strneb lr,[r0,#240]
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r0,#480]
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r0,#720]
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r0,#960]
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r0,#1200]
     strneb lr,[r0,#1440]
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r0,#1680]
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r0,#1920]
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r0,#2160]
     b 3f

	
2:
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r0]
     strneb lr,[r0,#240]
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r0,#480]
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r0,#720]
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r0,#960]
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r0,#1200]
     strneb lr,[r0,#1440]
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r0,#1680]
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r0,#1920]
     ;@ pixel 1
     ands r12,r6,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r0,#2160]
3: 
     add r0,r0,#2400
     subs r9,r9,#1
     bne 1b
4:
     ldr r9,vertical_lock
     tst r9,r9
     ldmeqfd sp!,{pc}
     mov r9,#0
     str r9,vertical_lock
     ldr r2,[drsms,#vdp_memory]
     ldrh r9,[drsms,#map_addr]
     add r2,r2,r9
     mov r9,r1,lsr#3
     add r2,r2,r9,lsl#6
     mov r4,r1
     mov r9,#7
     b 1b

;@##########################################################################
;@# Scaled line
;@##########################################################################	 
draw_borders2:
     ldr r0,frame_buffer_start 
     eor r1,r1,r1
     sub r0,r0,#0x960
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
     
     add r0,r0,#0x11000
	 add r0,r0,#0x940
     strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0]
     mov pc,lr
	 
draw_screenoff2:
     ldr r0,frame_buffer_start
     mov r1,#0
     mov r2,#10
1:
     strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240
     
     strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240   
     
     strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
     
     strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
	 strb r1,[r0,#1]
     strb r1,[r0],#240 
     
     subs r2,r2,#1
     bne 1b
     ldmfd sp!,{r5-r12,pc} 
  
draw_bg_low2:
     stmfd sp!,{lr}
     ldrh r1,[drsms,#vdp_line]
     ldr r0,frame_buffer_start
     ldrh r5,[drsms,#map_addr]
     ldr r10,[drsms,#vdp_memory]
     add r2,r10,r5
     ldrb r4,[drsms,#vdp_reg9]
     add r4,r4,r1
     ;@ the following wraps the screen on the y-axis
1:
     cmp r4,#224
     blt 2f
     sub r4,r4,#224
     b 1b
2:
     mov r5,r4,lsr #3  
     add r2,r2,r5, lsl #6
     ldrb r5,[drsms,#vdp_reg8]
     cmp r1,#0xF
     bgt 1f
     ldrb r6,[drsms,#vdp_reg0]
     tst r6,#1<<6
     eorne r5,r5,r5
1:
     rsb r5,r5,#0
     and r5,r5,#0xFF
     ;@ r0 = pointer to gp32 vid ram
     ;@ r1 = current line
     ;@ r2 = pointer sms map data
     ;@ r5 = backx
     ;@ r9 = x
     ;@ r4 = y val
     ldr r7,[drsms,#vdp_cram]
     and r9,r5,#7
     mov r8,#240
	 add r9,r9,r9,lsr#2
     mul r11,r8,r9
     sub r0,r0,r11
     mov r5,r5,lsr #2
     
     ldrb r9,[drsms,#vdp_reg0]
     tst r9,#1<<7  ;@ check for vertical scroll lock
     moveq r9,#0
     movne r9,#1
     str r9,vertical_lock
     moveq r9,#31
     movne r9,#24  ;@ number of tiles to draw ( 32 tiles is width of map )
     ldr r10,[drsms,#tile_cache]
	 add r5,r5,#2 ;@ skip first tile
1:  
     and r5,r5,#0x3E   ;@ wraps map data for horizontal scroll
     ldrh r12,[r2,r5]
     ;@ r12 = map data
     add r5,r5,#2
     and r6,r12,#0x800
     ;@ 1000 0000 0000
     ;@ 0000 0001 0000   0x10
     ldrb r11,[r7,r6,lsr #7]  ;@ get transparent color
	 orr r11,r11,r11,lsl#8
     bic r6,r12,#0xFE00
     add r6,r10,r6,lsl #5 ;@ get pointer to tile cache
     and r8,r4,#7
     tst r12,#0x400        ;@ check vertical flip flap
     rsbne r8,r8,#7
     ldr r6,[r6,r8,lsl #2]
     ;@ r6 = 8 pixels of display data
     tst r12,#0x800        ;@ use bg pal or sprite pal
     addne r8,r7,#0x10
     moveq r8,r7
     ;@ r8 = pal base
     ;@ r11 = transparent color
     tst r12,#0x200        ;@ check horizontal flip flag
     bne 2f  
     ;@ pixel 1
     ands r12,r6,#0xF
	 moveq lr,r11
     ldrneb lr,[r8,r12]
     strb lr,[r0,#1]
	 strb lr,[r0],#240
     strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 2
     ands r12,r6,#0xF0
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #4]
     strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 3
     ands r12,r6,#0xF00
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #8]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 4
     ands r12,r6,#0xF000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #12]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 5
     ands r12,r6,#0xF0000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #16]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 6
     ands r12,r6,#0xF00000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #20]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 7
     ands r12,r6,#0xF000000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #24]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 8
     ands r12,r6,#0xF0000000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #28]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
 
     subs r9,r9,#1
     bne 1b
     b 3f
2:
     ;@ pixel 8
     ands r12,r6,#0xF0000000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #28]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 7
     ands r12,r6,#0xF000000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #24]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 6
     ands r12,r6,#0xF00000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #20]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 5
     ands r12,r6,#0xF0000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #16]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 4
     ands r12,r6,#0xF000
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #12]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 3
     ands r12,r6,#0xF00
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #8]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 2
     ands r12,r6,#0xF0
	 moveq lr,r11
     ldrneb lr,[r8,r12, lsr #4]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
     ;@ pixel 1
     ands r12,r6,#0xF
	 moveq lr,r11
     ldrneb lr,[r8,r12]
	 strb lr,[r0,#1]
	 strb lr,[r0],#240
 
     subs r9,r9,#1
     bne 1b 
3:
     ldr r9,vertical_lock
     tst r9,r9
     ldmeqfd sp!,{pc}
     mov r9,#0
     str r9,vertical_lock
     ldr r2,[drsms,#vdp_memory]
     ldrh r9,[drsms,#map_addr]
     add r2,r2,r9
     mov r9,r1,lsr#3
     add r2,r2,r9,lsl#6
     mov r4,r1
     mov r9,#7
     b 1b

     
draw_sprites2:
     stmfd sp!,{lr}
     ldrh r1,[drsms,#vdp_line]
     ldr r5,[drsms,#vdp_memory]
     ldrh r2,[drsms,#sprite_addr]
     add r2,r5,r2
     add r8,r2,#0x80
     ldr r4,[drsms,#tile_cache]
     ldrh r5,[drsms,#sprite_tiles]
     add r5,r4,r5
     ldrb r9,[drsms,#vdp_reg0]
     tst r9,#1<<3
     moveq r9,#8
     movne r9,#16
     ldrb r7,[drsms,#vdp_reg1]
     tst r7,#1<<1
     moveq r7,#8
     movne r7,#16
     ldr r0,sprite_buffer
     add r10,r0,#8*16
     sub r1,r1,#1   ;@ saves having to inc sprite line each sprite
     mov r4,#64     ;@ max sprite counter
1:   
     ldrb r6,[r2],#1
     cmp r6,#0xD0
     beq 3f
     subge r6,r6,#0x100  ;@ switch sign of number if greater than 0xF0
     cmp r1,r6
     blt 2f
     add r11,r6,r7
     cmp r1,r11
     bge 2f

     ;@ get x position and save in sprite buffer
     ldrb r11,[r8]
     sub r11,r11,r9
     strh r11,[r0],#4
     
     ;@ get tile data and save in sprite buffer
     ldrb r11,[r8,#1]
     bic r11,r11,r7, lsr #4
     mov r11,r11,lsl #5
     sub r12,r1,r6
     add r11,r11,r12,lsl #2
     ldr r11,[r5,r11]
     str r11,[r0],#4
     
     ;@ check for sprite overflow - ie 8+ sprites
     cmp r0,r10
     bge 3f
2:
     add r8,r8,#2     
     subs r4,r4,#1
     bne 1b
3: 
     ldr r10,sprite_buffer
     cmp r10,r0
     ldmeqfd sp!,{pc}  ;@ no sprites to render so exit 
     
     ldr r8,[drsms,#vdp_cram]
     add r8,r8,#0x10  ;@ get pointer to pal 
     ldr r2,frame_buffer_start
     mov r5,#240
1:
     ldr r11,[r0,#-4]!
     
     ldrsh r1,[r0,#-4]!

	 cmp r1,#-7
	 blt 2f
	 
	 cmp r1,#240
	 bge 2f
	 
     add r1,r1,r1,lsr#2      
     mul r12,r5,r1
     add r4,r2,r12
     add r6,r4,#1
     ;@ pixel 1
     ands r12,r11,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r6]
     strneb lr,[r4]
	 strneb lr,[r6,#240]
     strneb lr,[r4,#240]
     ;@ pixel 2
     ands r12,r11,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
	 strneb lr,[r6,#480]
     strneb lr,[r4,#480]
     ;@ pixel 3
     ands r12,r11,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
	 strneb lr,[r6,#720]
     strneb lr,[r4,#720]
     ;@ pixel 4
     ands r12,r11,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
	 strneb lr,[r6,#960]
     strneb lr,[r4,#960]
     ;@ pixel 5
     ands r12,r11,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
	 strneb lr,[r6,#1200]
     strneb lr,[r4,#1200]
	 strneb lr,[r6,#1440]
     strneb lr,[r4,#1440]
     ;@ pixel 6
     ands r12,r11,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
	 strneb lr,[r6,#1680]
     strneb lr,[r4,#1680]
     ;@ pixel 7
     ands r12,r11,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
	 strneb lr,[r6,#1920]
     strneb lr,[r4,#1920]
     ;@ pixel 8
     ands r12,r11,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
	 strneb lr,[r6,#2160]
     strneb lr,[r4,#2160]
2:     
     cmp r0,r10
     bgt 1b
     ldmfd sp!,{pc}
    
draw_bg_high2:
     stmfd sp!,{lr}
     ldrh r1,[drsms,#vdp_line]
     ldr r0,frame_buffer_start
     ldrh r5,[drsms,#map_addr]
     ldr r10,[drsms,#vdp_memory]
     add r2,r10,r5
     ldrb r4,[drsms,#vdp_reg9]
     add r4,r4,r1
     ;@ the following wraps the screen on the y-axis
1:
     cmp r4,#224
     blt 2f
     sub r4,r4,#224
     b 1b
2:
     mov r5,r4,lsr #3  
     add r2,r2,r5, lsl #6
     ldrb r5,[drsms,#vdp_reg8]
     cmp r1,#0xF
     bgt 1f
     ldrb r6,[drsms,#vdp_reg0]
     tst r6,#1<<6
     eorne r5,r5,r5
1:  ;@ locktop 16 rows
     rsb r5,r5,#0
     and r5,r5,#0xFF
     ;@ r0 = pointer to gp32 vid ram
     ;@ r1 = current line
     ;@ r2 = pointer sms map data
     ;@ r5 = backx
     ;@ r9 = x
     ;@ r4 = y val
     ldr r7,[drsms,#vdp_cram]
     and r9,r5,#7
	 add r9,r9,r9,lsr#2
     mov r8,#240
     mul r11,r8,r9
     sub r0,r0,r11
	 add r11,r0,#1
     mov r5,r5,lsr #2
     
     ldrb r9,[drsms,#vdp_reg0]
     tst r9,#1<<7  ;@ check for vertical scroll lock
     moveq r9,#0
     movne r9,#1
     str r9,vertical_lock
     moveq r9,#31
     movne r9,#24  ;@ number of tiles to draw ( 32 tiles is width of map )
     ldr r10,[drsms,#tile_cache]
	 add r5,r5,#2 ;@ skip first tile
1:  
     and r5,r5,#0x3E   ;@ wraps map data for horizontal scroll
     ldrh r12,[r2,r5]
     add r5,r5,#2
     ;@ r12 = map data
     tst r12,#1<<0xC
     beq 3f

     bic r6,r12,#0xFE00
     add r6,r10,r6,lsl #5 ;@ get pointer to tile cache
     and r8,r4,#7
     tst r12,#0x400        ;@ check vertical flip flap
     rsbne r8,r8,#7
     ldr r6,[r6,r8,lsl #2]
     ;@ r6 = 8 pixels of display data
     tst r12,#0x800        ;@ use bg pal or sprite pal
     addne r8,r7,#0x10
     moveq r8,r7
     ;@ r8 = pal base
     tst r12,#0x200        ;@ check horizontal flip flag
     bne 2f  
     ;@ pixel 1
     ands r12,r6,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r0]
     strneb lr,[r11]
	 strneb lr,[r0,#240]
     strneb lr,[r11,#240]
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
	 strneb lr,[r0,#480]
     strneb lr,[r11,#480]
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
	 strneb lr,[r0,#720]
     strneb lr,[r11,#720]
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
	 strneb lr,[r0,#960]
     strneb lr,[r11,#960]
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
	 strneb lr,[r0,#1200]
     strneb lr,[r11,#1200]
	 strneb lr,[r0,#1440]
     strneb lr,[r11,#1440]
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
	 strneb lr,[r0,#1680]
     strneb lr,[r11,#1680]
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
	 strneb lr,[r0,#1920]
     strneb lr,[r11,#1920]
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
	 strneb lr,[r0,#2160]
     strneb lr,[r11,#2160]

     b 3f

	
2:
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
	 strneb lr,[r0]
     strneb lr,[r11]
	 strneb lr,[r0,#240]
     strneb lr,[r11,#240]
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
	 strneb lr,[r0,#480]
     strneb lr,[r11,#480]
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
	 strneb lr,[r0,#720]
     strneb lr,[r11,#720]
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
	 strneb lr,[r0,#960]
     strneb lr,[r11,#960]
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
	 strneb lr,[r0,#1200]
     strneb lr,[r11,#1200]
	 strneb lr,[r0,#1440]
     strneb lr,[r11,#1440]
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
	 strneb lr,[r0,#1680]
     strneb lr,[r11,#1680]
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
	 strneb lr,[r0,#1920]
     strneb lr,[r11,#1920]
     ;@ pixel 1
     ands r12,r6,#0xF
     ldrneb lr,[r8,r12]
	 strneb lr,[r0,#2160]
     strneb lr,[r11,#2160]
3: 
     add r0,r0,#2400
	 add r11,r11,#2400
     subs r9,r9,#1
     bne 1b
4:
     ldr r9,vertical_lock
     tst r9,r9
     ldmeqfd sp!,{pc}
     mov r9,#0
     str r9,vertical_lock
     ldr r2,[drsms,#vdp_memory]
     ldrh r9,[drsms,#map_addr]
     add r2,r2,r9
     mov r9,r1,lsr#3
     add r2,r2,r9,lsl#6
     mov r4,r1
     mov r9,#7
     b 1b



