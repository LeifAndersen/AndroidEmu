		.DATA
       .global gg_render_8
       .extern drsms
       .include "DrSMS_core_vars.s"
drsms_local1:   .word drsms       
gg_render_8:
     stmfd sp!,{r3-r12,lr}
	 
	 
	 ldr drsms,drsms_local1
     ldr r0,[drsms,#frame_buffer]
     ;@ add r0,r0,#36
     ldrh r1,[drsms,#vdp_line]
	 cmp r1,#24
	 blt 1f
	 cmp r1,#176
	 bge 1f
	 
	 sub r1,r1,#24
     mov r2,#320
     mul r2,r1,r2	 
     add r0,r0,r2   ;@ add y offset
     str r0,frame_buffer_start

     ldrb r0,[drsms,#vdp_reg1]
     tst r0,#1<<6
     beq draw_screenoff

	 bl clear_screen
	 mov r0,#0
     bl draw_bg   ;@ draw low tiles
     bl draw_sprites
	 mov r0,#1
     bl draw_bg   ;@ draw high tiles
1:    
     ldmfd sp!,{r3-r12,pc}
frame_buffer_start: .word 0   

clear_screen:
      stmfd sp!,{r0-r12}
      ldr r12,frame_buffer_start
      eor r0,r0,r0
      mov r1,r0
	  mov r2,r0
	  mov r3,r0
	  mov r4,r0
	  mov r5,r0
	  mov r6,r0
	  mov r7,r0
	  mov r8,r0
	  mov r9,r0

      stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
      stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
      stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
      stmia r12!,{r0,r1,r2,r3,r4,r5,r6,r7,r8,r9}
	  
      ldmfd sp!,{r0-r12}
	  mov pc,lr
    
draw_screenoff:
     ldr r0,frame_buffer_start
	 sub r0,r0,#8
     mov r1,#0
     mov r2,#4
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
  
draw_bg:
     stmfd sp!,{lr}
     ldrh r1,[drsms,#vdp_line]
     ldr r11,frame_buffer_start
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
	 ;@ r0 = high or low flag
     ;@ r11 = pointer to gp32 vid ram
     ;@ r1 = current line
     ;@ r2 = pointer sms map data
     ;@ r5 = backx
     ;@ r9 = x
     ;@ r4 = y val
     and r9,r5,#7
     sub r11,r11,r9
     mov r5,r5,lsr #2
     
     mov r9,#0
     str r9,vertical_lock
     mov r9,#21
     ldr r10,[drsms,#tile_cache]
	 add r5,r5,#12 ;@ skip first tile
1:  
     and r5,r5,#0x3E   ;@ wraps map data for horizontal scroll
     ldrh r12,[r2,r5]
     ;@ r12 = map data
     add r5,r5,#2
	 and r6,r12,#1<<0xC
	 cmp r6,r0,lsl#0xC
     bne 3f
     ;@ 1000 0000 0000
     ;@ 0000 0001 0000   0x10
     bic r6,r12,#0xFE00
     add r6,r10,r6,lsl #5 ;@ get pointer to tile cache
     and r8,r4,#7
     tst r12,#0x400        ;@ check vertical flip flap
     rsbne r8,r8,#7
     ldr r6,[r6,r8,lsl #2]
     ;@ r6 = 8 pixels of display data
     tst r12,#0x800        ;@ use bg pal or sprite pal
     movne r7,#0x10
     moveq r7,#0
     tst r12,#0x200        ;@ check horizontal flip flag
     bne 2f  
	 
     ;@ pixel 1
     ands r12,r6,#0xF
	 orrne r12,r7,r12
	 strneb r12,[r11]
     ;@ pixel 2
     ands r12,r6,#0xF0
	 orrne r12,r7,r12,lsr#4
	 strneb r12,[r11,#1]
     ;@ pixel 3
     ands r12,r6,#0xF00
	 orrne r12,r7,r12,lsr#8
	 strneb r12,[r11,#2]
     ;@ pixel 4
     ands r12,r6,#0xF000
	 orrne r12,r7,r12,lsr#12
	 strneb r12,[r11,#3]
     ;@ pixel 5
     ands r12,r6,#0xF0000
	 orrne r12,r7,r12,lsr#16
	 strneb r12,[r11,#4]
     ;@ pixel 6
     ands r12,r6,#0xF00000
	 orrne r12,r7,r12,lsr#20
	 strneb r12,[r11,#5]
     ;@ pixel 7
     ands r12,r6,#0xF000000
	 orrne r12,r7,r12,lsr#24
	 strneb r12,[r11,#6]
     ;@ pixel 8
     ands r12,r6,#0xF0000000
	 orrne r12,r7,r12,lsr#28
	 strneb r12,[r11,#7]

     b 3f
	 
vertical_lock: .word 0	
2:
     ;@ pixel 8
     ands r12,r6,#0xF0000000
     orrne r12,r7,r12,lsr#28
	 strneb r12,[r11]
     ;@ pixel 7
     ands r12,r6,#0xF000000
     orrne r12,r7,r12,lsr#24
	 strneb r12,[r11,#1]
     ;@ pixel 6
     ands r12,r6,#0xF00000
     orrne r12,r7,r12,lsr#20
	 strneb r12,[r11,#2]
     ;@ pixel 5
     ands r12,r6,#0xF0000
     orrne r12,r7,r12,lsr#16
	 strneb r12,[r11,#3]
     ;@ pixel 4
     ands r12,r6,#0xF000
     orrne r12,r7,r12,lsr#12
	 strneb r12,[r11,#4]
     ;@ pixel 3
     ands r12,r6,#0xF00
     orrne r12,r7,r12,lsr#8
	 strneb r12,[r11,#5]
     ;@ pixel 2
     ands r12,r6,#0xF0
     orrne r12,r7,r12,lsr#4
	 strneb r12,[r11,#6]
     ;@ pixel 1
     ands r12,r6,#0xF
     orrne r12,r7,r12
	 strneb r12,[r11,#7]
3: 
     add r11,r11,#0x8
     subs r9,r9,#1
     bne 1b

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
     
     mov r8,#0x10
     ldr r2,frame_buffer_start
1:
     ldr r11,[r0,#-4]!
     
     ldrsh r1,[r0,#-4]! 
	 sub r1,r1,#40
     cmp r1,#-7
	 blt 2f
	 
	 cmp r1,#160
	 bge 2f
	 
     add r4,r2,r1
     
     ;@ pixel 1
     ands r12,r11,#0xF
     orrne r12,r8,r12
     strneb r12,[r4]
     ;@ pixel 2
     ands r12,r11,#0xF0
     orrne r12,r8,r12,lsr#4
     strneb r12,[r4,#1]
     ;@ pixel 3
     ands r12,r11,#0xF00
     orrne r12,r8,r12,lsr#8
     strneb r12,[r4,#2]
     ;@ pixel 4
     ands r12,r11,#0xF000
     orrne r12,r8,r12,lsr#12
     strneb r12,[r4,#3]
     ;@ pixel 5
     ands r12,r11,#0xF0000
     orrne r12,r8,r12,lsr#16
     strneb r12,[r4,#4]
     ;@ pixel 6
     ands r12,r11,#0xF00000
     orrne r12,r8,r12,lsr#20
     strneb r12,[r4,#5]
     ;@ pixel 7
     ands r12,r11,#0xF000000
     orrne r12,r8,r12,lsr#24
     strneb r12,[r4,#6]
     ;@ pixel 8
     ands r12,r11,#0xF0000000
     orrne r12,r8,r12,lsr#28
     strneb r12,[r4,#7]
2:     
     cmp r0,r10
     bgt 1b
     ldmfd sp!,{pc}

sprite_buffer: .word sprite_buffer_data
sprite_buffer_data: .space 32*8

     




