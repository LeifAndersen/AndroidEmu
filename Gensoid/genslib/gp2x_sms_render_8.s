		.DATA
       .global sms_render_8
       .extern drsms
       .include "DrSMS_core_vars.s"
drsms_local1:   .word drsms       
sms_render_8:
     stmfd sp!,{r3-r12,lr}
	 ldr drsms,drsms_local1
     ldr r0,[drsms,#frame_buffer]
     ;@ add r0,r0,#36
     ldrh r1,[drsms,#vdp_line]
	 ;@ add r1,r1,#24
     mov r2,#320
     mul r2,r1,r2	 
     add r0,r0,r2   ;@ add y offset
     str r0,frame_buffer_start
     ldrb r0,[drsms,#vdp_reg1]
     tst r0,#1<<6
     beq draw_screenoff

     bl draw_bg_low
     bl draw_sprites
     bl draw_bg_high
	 bl draw_borders
     
     ldmfd sp!,{r3-r12,pc}
frame_buffer_start: .word 0   


draw_borders:
     ldr r0,frame_buffer_start 
     eor r1,r1,r1
     sub r0,r0,#8
     str r1,[r0],#4 
	 str r1,[r0],#244 
	 str r1,[r0],#4 
	 str r1,[r0]
     mov pc,lr
     
draw_screenoff:
     ldr r0,frame_buffer_start
	 sub r0,r0,#8
     mov r1,#0
     mov r2,#8
1:
     str r1,[r0],#4
	 str r1,[r0],#4
	 str r1,[r0],#4
	 str r1,[r0],#4
	 str r1,[r0],#4
	 str r1,[r0],#4
	 str r1,[r0],#4
	 str r1,[r0],#4
	 str r1,[r0],#4
	 str r1,[r0],#4
     
     subs r2,r2,#1
     bne 1b
	 
     ldmfd sp!,{r3-r12,pc} 
  
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
     sub r0,r0,r9
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
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
 
     subs r9,r9,#1
     bne 1b
     b 3f
vertical_lock: .word 0	
2:
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
     ;@ pixel 1
     ands r12,r6,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r0],#1
     streqb r11,[r0],#1
 
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
     mov r9,#8
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
1:
     ldr r11,[r0,#-4]!
     
     ldrsh r1,[r0,#-4]! 
     cmp r1,#-7
	 blt 2f
	 
	 cmp r1,#240
	 bge 2f
	 
     add r4,r2,r1
     
     ;@ pixel 1
     ands r12,r11,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r4]
     ;@ pixel 2
     ands r12,r11,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r4,#1]
     ;@ pixel 3
     ands r12,r11,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r4,#2]
     ;@ pixel 4
     ands r12,r11,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r4,#3]
     ;@ pixel 5
     ands r12,r11,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r4,#4]
     ;@ pixel 6
     ands r12,r11,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r4,#5]
     ;@ pixel 7
     ands r12,r11,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r4,#6]
     ;@ pixel 8
     ands r12,r11,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r4,#7]
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
     sub r0,r0,r9
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
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r0,#1]
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r0,#2]
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r0,#3]
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r0,#4]
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r0,#5]
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r0,#6]
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r0,#7]
     b 3f

	
2:
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     ldrneb lr,[r8,r12, lsr #28]
     strneb lr,[r0]
     ;@ pixel 7
     ands r12,r6,#0xF000000
     ldrneb lr,[r8,r12, lsr #24]
     strneb lr,[r0,#1]
     ;@ pixel 6
     ands r12,r6,#0xF00000
     ldrneb lr,[r8,r12, lsr #20]
     strneb lr,[r0,#2]
     ;@ pixel 5
     ands r12,r6,#0xF0000
     ldrneb lr,[r8,r12, lsr #16]
     strneb lr,[r0,#3]
     ;@ pixel 4
     ands r12,r6,#0xF000
     ldrneb lr,[r8,r12, lsr #12]
     strneb lr,[r0,#4]
     ;@ pixel 3
     ands r12,r6,#0xF00
     ldrneb lr,[r8,r12, lsr #8]
     strneb lr,[r0,#5]
     ;@ pixel 2
     ands r12,r6,#0xF0
     ldrneb lr,[r8,r12, lsr #4]
     strneb lr,[r0,#6]
     ;@ pixel 1
     ands r12,r6,#0xF
     ldrneb lr,[r8,r12]
     strneb lr,[r0,#7]
3: 
     add r0,r0,#0x8
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
     mov r9,#8
     b 1b



