    .DATA
/*
DrMD To Do's
------------
Rebase code,
   if zbank is changed need to rebasePC and SP

 
*/
      .equiv EMU_C68K,    1
      .equiv EMU_M68K,    0
	  .equiv __GP32__,     0
	  .equiv __GP2X__,     1
	  .equiv __GIZ__,     0

      .global DrMDRun
      .global m68k_read_memory_8  ;@ M68k mem functions
      .global m68k_read_memory_16 ;@ exposed for Cyclone init
      .global m68k_read_memory_32
      .global m68k_write_memory_8
      .global m68k_write_memory_16
      .global m68k_write_memory_32
      .global m68k_read_immediate_8
      .global m68k_read_immediate_16
      .global m68k_read_immediate_32
      .global m68k_read_pcrelative_8
      .global m68k_read_pcrelative_16
      .global m68k_read_pcrelative_32
      .global DrMD_Z80_write_8  ;@ Z80 mem functions
      .global DrMD_Z80_write_16 ;@ exposed for DrZ80 init
      .global DrMD_Z80_read_8
      .global DrMD_Z80_read_16
      .global DrMD_Z80_In
      .global DrMD_Z80_Out
      .global DrMD_Z80_Rebase_PC
      .global DrMD_Z80_Rebase_SP
      .global update_md_pal
      .global m68k_irq_callback
	  .global genesis_rom_banks_reset
	  .global genesis_rom_bank_set
	  

	  .extern drmd
	  .extern sram_modified
      .extern pal_lookup
      .extern fm_vdp_line_update
      .extern illegal_memory_io
	  .extern Cyclone_ClearIRQ
	  
.if __GIZ__
	  .extern DrMDBlit8To16
.endif

	  /*
	  .extern DirtyPal
	  .extern DirtyTile
	  .extern DirtyVram
	  */

;@ --------------------------- Defines ----------------------------
     .include "Gensoid/genslib/DrMD_core_vars.s"
;@ --------------------------- Framework --------------------------



	
md_pal_lookup_local: .word pal_lookup
	
update_md_pal:
    stmfd sp!,{r4-r6}
    ldr drmd,drmd_context0
    mov r0,#0x40
    ldr r1,[drmd,#cram]
    ldr r2,[drmd,#local_pal]
    ldr r4,md_pal_lookup_local
update_md_pal_loop:
    ldrh r3,[r1],#2
    bic r3,r3,#0xF000  ;@ make sure pal is 0 to FFF
    ldr r6,[r4,r3,lsl#2]
    str r6,[r2],#4
    subs r0,r0,#1
    bne update_md_pal_loop

    ldrb r0,[drmd,#vdp_reg7]  ;@ current border color
    ldr r1,[drmd,#local_pal]
    ldr r0,[r1,r0,lsl#2]
    str r0,[r1]
    str r0,[r1,#0x40]
    str r0,[r1,#0x80]
    str r0,[r1,#0xC0]
    
    ldmfd sp!,{r4-r6}
    mov pc,lr

.if __GP2X__
sync_local_pal:
     mov pc,lr
.endif

.if __GIZ__
sync_local_pal:
     mov pc,lr
.endif

.if __GP32__	 
sync_local_pal:
     ldr r0,pal_updated
     tst r0,r0
     moveq pc,lr  ;@ pal already synced so exit
     
	 mov r0,#0x14000000
     orr r0,r0,#0xA00000
     
     ;@ now in vsync
     ;@ now make sure not in hactive
     ;@ and then flip pal offset
sync_local_pal_wait_hactive:
     ldr r1,[r0,#0x10]
     mov r1,r1,lsr#17
     and r1,r1,#3
     cmp r1,#2
     bne sync_local_pal_wait_hactive
sync_local_pal_wait_hactive_end:
     ldr r1,[r0,#0x10]
     mov r1,r1,lsr#17
     and r1,r1,#3
     cmp r1,#2
     beq sync_local_pal_wait_hactive_end  
        
     ldr r1,[r0,#0xC]
     tst r1,#0x00600000
     orr r1,r1,#0x01000000
     orreq r1,r1,#0x00600000
     bicne r1,r1,#0x00600000
     str r1,[r0,#0xC]
     ldr r1,[drmd,#local_pal]
     orrne r1,r1,#0x180
     biceq r1,r1,#0x180
     str r1,[drmd,#local_pal]
     eor r0,r0,r0
     str r0,pal_updated
     mov pc,lr   
.endif

DrMD_Fix_Controllers:
      ldr r2,drmd_context0
	  ldrb r0,[r2,#pad_1_delay]
	  cmp r0,#25
	  eorgt r0,r0,r0
	  addlt r0,r0,#1
	  strb r0,[r2,#pad_1_delay]
	  ldrb r0,[r2,#pad_2_delay]
	  cmp r0,#25
	  eorgt r0,r0,r0
	  addlt r0,r0,#1
	  strb r0,[r2,#pad_2_delay]
	  mov pc,lr
	  
;@################################################
;@################################################

;@  System

;@################################################
;@################################################
drmd_context0:   .word drmd   ;@  saves the location of DrMD context
     ;@ r0 = skip frame  0 yes 1 = no
skip_frame: .word 0
vblank_start: .word 0
pal_updated: .word 0

.if __GIZ__
Blit8To16_local: .word DrMDBlit8To16
.endif

DrMDRun:
     stmfd sp!,{r4-r12,lr}  ;@ save registers on stack
     str r0,skip_frame
     ldr drmd,drmd_context0
md_framestart:
     ;@ check input for exit game
     ldrh r0,[drmd,#vdp_status]
     bic r0,r0,#0x08   ;@ clear vblank
     and r0,r0,#0xFF
     orr r0,r0,#0x3600
     ldrb r1,[drmd,#vdp_reg12]
     tst r1,#2
     eorne r0,r0,#0x10  ;@ toggle even/odd flag (IM2 only)
     strh r0,[drmd,#vdp_status]
     ldrb r0,[drmd,#vdp_reg10]
     strb r0,[drmd,#vdp_counter]
     ldrb r0,[drmd,#region]
     tst r0,#1<<6  ;@ check pal flag
     moveq r0,#0xE0
     beq md_framestart_change_vblank
     ldrb r0,[drmd,#vdp_reg1]
     tst r0,#0x8
     moveq r0,#0xE0
     movne r0,#0xF0
md_framestart_change_vblank:
     str r0,vblank_start
     eor r6,r6,r6
     strh r6,[drmd,#vdp_line]  ;@ reset vdp_line counter
     str r6,[drmd,#m68k_aim]
     str r6,[drmd,#m68k_total]
        

md_vdp_line:
	bl DrMD_Fix_Controllers
;@ ####################################################
                              ;@  set Hblank flag
;@ ####################################################
     ldrh r0,[drmd,#vdp_status]
     orr r0,r0,#0x0004       ;@ set hblank flag
     strh r0,[drmd,#vdp_status]
;@ ####################################################
                              ;@ M68K Hblank
;@ ####################################################     
     ;@ run m68k for hblank
	 ldrh r0,[drmd,#cpl_m68k]
	 sub r0,r0,#404
     bl run_m68k

;@ ####################################################
                              ;@ Clear HBlank flag
;@ #################################################### 
     ldrh r0,[drmd,#vdp_status]
     bic r0,r0,#0x0004       ;@ clear hblank flag
     strh r0,[drmd,#vdp_status]

	 
 
     cmp r6,#120 ;@ about halfway
     bne skip_update_pal
     bl update_md_pal
     mov r0,#1
     str r0,pal_updated
skip_update_pal:

;@ ####################################################
                              ;@ Check Hint
;@ #################################################### 
     ;@ check h-int
     ldrb r0,[drmd,#vdp_counter]
     subs r0,r0,#1
     strplb r0,[drmd,#vdp_counter]
     bpl md_vdp_line_skip_hint
     ldrb r0,[drmd,#vdp_reg10]
     strb r0,[drmd,#vdp_counter]
     mov r0,#1
     strb r0,[drmd,#hint_pending]
     bl update_irq_line
md_vdp_line_skip_hint:

;@ ####################################################
                              ;@ Render line
;@ #################################################### 
	 ;@ render line
     ldr r0,skip_frame
     tst r0,r0
     beq md_vdp_line_skipframe
     mov lr,pc
     ldr pc,[drmd,#render_line]
	 
.if __GIZ__	 
	 ;@ Test code to convert 8bit to 16bit line by line
	 bl update_md_pal
	 ldrh r0,[drmd,#vdp_line]
	 mov lr,pc
	 ldr pc,Blit8To16_local
.endif	  

md_vdp_line_skipframe:

    

;@ ####################################################
                              ;@ M68k Active scan
;@ #################################################### 
     ldrh r0,[drmd,#cpl_m68k]
     bl run_m68k
     
;@ ####################################################
                              ;@ Run Z80 for line
;@ ####################################################
     ldrb r0,[drmd,#zreset]
     tst r0,r0
     beq md_vdp_line_hblank_skip_z80
     ldrb r0,[drmd,#zbusreq]
     tst r0,r0
     bne md_vdp_line_hblank_skip_z80
     ldr r0,[drmd,#z80_context]
     ldrh r1,[drmd,#cpl_z80]
     mov lr,pc
     ldr pc,[drmd,#z80_run] ;@ run z80
md_vdp_line_hblank_skip_z80:
     
     
     
     stmfd sp!,{r0-r12,lr}
     mov lr,pc
     ldr pc,fm_vdp_line_update_local
     ldmfd sp!,{r0-r12,lr}
     bl sync_local_pal

;@ ####################################################
                              ;@ Setup for next line
;@ ####################################################
     add r6,r6,#1   ;@ increase vdp line
     strh r6,[drmd,#vdp_line]
     ldr r7,vblank_start
     cmp r6,r7
     blt md_vdp_line

     

      

;@ ####################################################
                              ;@ Check End of Frame Hint
;@ #################################################### 
     ;@ check h-int
     ldrb r0,[drmd,#vdp_counter]
     subs r0,r0,#1
     strplb r0,[drmd,#vdp_counter]
     bpl md_vdp_line_skip_hintlast
     ldrb r0,[drmd,#vdp_reg10]
     strb r0,[drmd,#vdp_counter]
     mov r0,#1
     strb r0,[drmd,#hint_pending]
     bl update_irq_line
md_vdp_line_skip_hintlast:
     
;@ ####################################################
                              ;@ Start of VBlank
;@ ####################################################
     ldrh r0,[drmd,#vdp_status]
     orr r0,r0,#0x0C
     strh r0,[drmd,#vdp_status]
     
;@ ####################################################
                              ;@ M68K End of Frame Hblank(last)
;@ ####################################################     
     ldrh r0,[drmd,#cpl_m68k]
	 sub r0,r0,#360
     bl run_m68k
;@ ####################################################
                              ;@ Run End of Frame Z80 
;@ ####################################################
     ldrb r0,[drmd,#zreset]
     tst r0,r0
     beq md_vdp_line_endofframe_z801
     ldrb r0,[drmd,#zbusreq]
     tst r0,r0
     bne md_vdp_line_endofframe_z801
     ldr r0,[drmd,#z80_context]
     ldrh r1,[drmd,#cpl_z80]
     sub r1,r1,#168
     mov lr,pc
     ldr pc,[drmd,#z80_run] ;@ run z80
md_vdp_line_endofframe_z801:

     ldrh r0,[drmd,#vdp_status]
     bic r0,r0,#0x4
     orr r0,r0,#0x80
     strh r0,[drmd,#vdp_status]

     mov r0,#1
     strb r0,[drmd,#vint_pending]
     bl update_irq_line

;@ ####################################################
                              ;@ Z80 Vblank INT
;@ ####################################################     
     ldrb r0,[drmd,#zreset]
     tst r0,r0
     beq md_vdp_line_vblank_skip_z80_int 
     ldrb r0,[drmd,#zbusreq]
     tst r0,r0
     bne md_vdp_line_vblank_skip_z80_int
     mov r0,#1
     mov lr,pc
     ldr pc,[drmd,#z80_set_irq]  ;@ raise irq
md_vdp_line_vblank_skip_z80_int:

;@ ####################################################
                              ;@ M68K start of vblank
;@ ####################################################     
     ldrh r0,[drmd,#cpl_m68k]
     bl run_m68k
;@ ####################################################
                              ;@ Run End of Frame Z80 
;@ ####################################################
     ldrb r0,[drmd,#zreset]
     tst r0,r0
     beq md_vdp_line_endofframe_z802
     ldrb r0,[drmd,#zbusreq]
     tst r0,r0
     bne md_vdp_line_endofframe_z802
     ldr r0,[drmd,#z80_context]
     ldrh r1,[drmd,#cpl_z80]
     mov lr,pc
     ldr pc,[drmd,#z80_run] ;@ run z80
md_vdp_line_endofframe_z802:
     
     stmfd sp!,{r0-r12,lr}
     mov lr,pc
     ldr pc,fm_vdp_line_update_local
     ldmfd sp!,{r0-r12,lr}

;@ ####################################################
                              ;@ VBlank Loop
;@ ####################################################

md_vdp_line_vblank_loop:

     ldrh r0,[drmd,#vdp_status]
     orr r0,r0,#0x0004       ;@ set hblank flag
     strh r0,[drmd,#vdp_status]
;@ ####################################################
                              ;@ M68K VBlank Hblank
;@ ####################################################     
     ldrh r0,[drmd,#cpl_m68k]
	 sub r0,r0,#404
     bl run_m68k

     ldrh r0,[drmd,#vdp_status]
     bic r0,r0,#0x0004       ;@ set hblank flag
     strh r0,[drmd,#vdp_status]
     
     ldrh r0,[drmd,#cpl_m68k]
     bl run_m68k

     ;@ run z80 for line
     ldrb r0,[drmd,#zreset]
     tst r0,r0
     beq md_vdp_line_vblank_skip_z80
     ldrb r0,[drmd,#zbusreq]
     tst r0,r0
     bne md_vdp_line_vblank_skip_z80
     ldr r0,[drmd,#z80_context]
     ldrh r1,[drmd,#cpl_z80]
     mov lr,pc
     ldr pc,[drmd,#z80_run] ;@ run z80
md_vdp_line_vblank_skip_z80:

     stmfd sp!,{r0-r12,lr}
     mov lr,pc
     ldr pc,fm_vdp_line_update_local
     ldmfd sp!,{r0-r12,lr}

     bl sync_local_pal

     add r6,r6,#1
     strh r6,[drmd,#vdp_line]
     ldrh r7,[drmd,#lines_per_frame]
     cmp r6,r7
     blt md_vdp_line_vblank_loop
         
     ldmfd sp!,{r4-r12,lr}
     mov r0,#0
     mov pc,lr

fm_vdp_line_update_local: .word fm_vdp_line_update

run_m68k:
.if EMU_C68K
     ;@ run m68k for hblank
     ldr r7,[drmd,#m68k_aim]
     ldr r8,[drmd,#m68k_total]
	 add r7,r7,r0  ;@ add cpu cycles to run
	 str r7,[drmd,#m68k_aim]
     ldr r0,[drmd,#m68k_context]
     subs r9,r7,r8 
	 movmi pc,lr  ;@ exit if cycles already used
     str r9,[r0,#0x5C]
	 stmfd sp!,{lr}
     mov lr,pc
     ldr pc,[drmd,#m68k_run]  ;@ run M68K
     ldr r0,[drmd,#m68k_context]
     ldr r0,[r0,#0x5C]
     subs r0,r0,#0
     subs r9,r9,r0
     add r8,r8,r9
     str r8,[drmd,#m68k_total]
	 ldmfd sp!,{pc} ;@ exit
.endif
.if EMU_M68K
    ldr r7,[drmd,#m68k_aim]
    ldr r8,[drmd,#m68k_total]
    add r7,r7,r0
    str r7,[drmd,#m68k_aim]
    subs r0,r7,r8
	movmi pc,lr  ;@ exit if cycles already used
	stmfd sp!,{lr}
    mov lr,pc
    ldr pc,[drmd,#m68k_run]  ;@ run M68K
    add r8,r8,r0
    str r8,[drmd,#m68k_total]
	ldmfd sp!,{pc} ;@ exit
.endif 
     
update_irq_line:
     ldrb r0,[drmd,#vdp_reg1]
     tst r0,#1<<5
     beq 1f
     ldrb r0,[drmd,#vint_pending]
     tst r0,r0
     beq 1f
     mov r0,#6
     ldr pc,[drmd,#m68k_set_irq]
	 
1: 	 ;@ check hint
     ldrb r0,[drmd,#vdp_reg0]
     tst r0,#1<<4   ;@ are they enabled
	 moveq pc,lr
     ldrb r0,[drmd,#hint_pending]
     tst r0,r0
     moveq pc,lr
     mov r0,#4
     ldr pc,[drmd,#m68k_set_irq]

 
.if EMU_C68K	 
m68k_irq_callback:
	 
     ldr r2,drmd_context0
     ldrb r0,[r2,#vdp_reg1]
     tst r0,#0x20                  ;@ make sure vint
     beq m68k_irq_callback_H_Ack
     ldrb r0,[r2,#vint_pending]  ;@ and vint pending
     tst r0,r0     
     beq m68k_irq_callback_H_Ack
m68k_irq_callback_V_Ack:
     mov r0,#0
     strb r0,[r2,#vint_pending]  ;@ clear pending vint
     ;@ now return 4 if a hint interrupt is pending
     ;@ as well
     ldrb r0,[r2,#hint_pending]
     ldrb r1,[r2,#vdp_reg0]
     and r1,r1,#0x10    ;@ get hint enabled flag
     and r0,r0,r1,lsr#4  ;@ and it with the hint pending flag
     mov r0,r0,lsl#2  ;@  turn 1 into 4 or 0 into 0
	 b Cyclone_ClearIRQ

m68k_irq_callback_H_Ack:     
     mov r0,#0
     strb r0,[r2,#hint_pending] 
	 strb r0,[r2,#vint_pending]
     b Cyclone_ClearIRQ
.endif	 
     
;@################################################
;@################################################

;@  Memory

;@################################################
;@################################################





;@##############################################
;@##############################################

;@   M68K read8

;@##############################################
;@##############################################

m68k_read_memory_8:
     ;@ 1111 1000 0000 0000 0000 0000
     ;@ F80000
     and r1,r0,#0xF80000
     ldr pc,[pc,r1, lsr #17]
     .word 0
m68k_read_memory_8_data:
     .word M68K_read_8_rom0   ;@ 0x000000 - 0x07FFFF
     .word M68K_read_8_rom1   ;@ 0x080000 - 0x0FFFFF
	 .word M68K_read_8_rom2   ;@ 0x100000 - 0x17FFFF
	 .word M68K_read_8_rom3   ;@ 0x180000 - 0x1FFFFF
	 .word M68K_read_8_rom4   ;@ 0x200000 - 0x27FFFF
	 .word M68K_read_8_rom5   ;@ 0x280000 - 0x2FFFFF
	 .word M68K_read_8_rom6   ;@ 0x300000 - 0x37FFFF
	 .word M68K_read_8_rom7   ;@ 0x380000 - 0x3FFFFF
	 .word M68K_read_8_rom8   ;@ 0x400000 - 0x47FFFF
	 .word M68K_read_8_rom9   ;@ 0x480000 - 0x4FFFFF
	 .word M68K_read_8_romA   ;@ 0x500000 - 0x57FFFF
	 .word M68K_read_8_romB   ;@ 0x580000 - 0x5FFFFF
	 .word M68K_read_8_romC   ;@ 0x600000 - 0x67FFFF
	 .word M68K_read_8_romD   ;@ 0x680000 - 0x6FFFFF
	 .word M68K_read_8_romE   ;@ 0x700000 - 0x77FFFF
	 .word M68K_read_8_romF   ;@ 0x780000 - 0x7FFFFF
	 .word M68K_read_8_bad    ;@ 0x800000 - 0x87FFFF
	 .word M68K_read_8_bad    ;@ 0x880000 - 0x8FFFFF
	 .word M68K_read_8_bad    ;@ 0x900000 - 0x97FFFF
	 .word M68K_read_8_bad    ;@ 0x980000 - 0x9FFFFF
	 .word M68K_read_8_misc   ;@ 0xA00000 - 0xA7FFFF
	 .word M68K_read_8_bad    ;@ 0xA80000 - 0xAFFFFF
	 .word M68K_read_8_bad    ;@ 0xB00000 - 0xB7FFFF
	 .word M68K_read_8_bad    ;@ 0xB80000 - 0xBFFFFF
     .word M68K_read_8_vdp    ;@ 0xC00000 - 0xC7FFFF
	 .word M68K_read_8_bad    ;@ 0xC80000 - 0xCFFFFF
	 .word M68K_read_8_bad    ;@ 0xD00000 - 0xD7FFFF
	 .word M68K_read_8_bad    ;@ 0xD80000 - 0xDFFFFF
     .word M68K_read_8_ram    ;@ 0xE00000 - 0xE7FFFF
	 .word M68K_read_8_ram    ;@ 0xE80000 - 0xEFFFFF
	 .word M68K_read_8_ram    ;@ 0xF00000 - 0xF7FFFF
	 .word M68K_read_8_ram    ;@ 0xF80000 - 0xFFFFFF
	 
Genesis_read_memory_8_data:
	 .word M68K_read_8_rom0   ;@ 0x000000 - 0x07FFFF
     .word M68K_read_8_rom1   ;@ 0x080000 - 0x0FFFFF
	 .word M68K_read_8_rom2   ;@ 0x100000 - 0x17FFFF
	 .word M68K_read_8_rom3   ;@ 0x180000 - 0x1FFFFF
	 .word M68K_read_8_rom4   ;@ 0x200000 - 0x27FFFF
	 .word M68K_read_8_rom5   ;@ 0x280000 - 0x2FFFFF
	 .word M68K_read_8_rom6   ;@ 0x300000 - 0x37FFFF
	 .word M68K_read_8_rom7   ;@ 0x380000 - 0x3FFFFF
	 .word M68K_read_8_rom8   ;@ 0x400000 - 0x47FFFF
	 .word M68K_read_8_rom9   ;@ 0x480000 - 0x4FFFFF
	 .word M68K_read_8_romA   ;@ 0x500000 - 0x57FFFF
	 .word M68K_read_8_romB   ;@ 0x580000 - 0x5FFFFF
	 .word M68K_read_8_romC   ;@ 0x600000 - 0x67FFFF
	 .word M68K_read_8_romD   ;@ 0x680000 - 0x6FFFFF
	 .word M68K_read_8_romE   ;@ 0x700000 - 0x77FFFF
	 .word M68K_read_8_romF   ;@ 0x780000 - 0x7FFFFF
	 .word M68K_read_8_bad    ;@ 0x800000 - 0x87FFFF
	 .word M68K_read_8_bad    ;@ 0x880000 - 0x8FFFFF
	 .word M68K_read_8_bad    ;@ 0x900000 - 0x97FFFF
	 .word M68K_read_8_bad    ;@ 0x980000 - 0x9FFFFF
	 .word M68K_read_8_misc   ;@ 0xA00000 - 0xA7FFFF
	 .word M68K_read_8_bad    ;@ 0xA80000 - 0xAFFFFF
	 .word M68K_read_8_bad    ;@ 0xB00000 - 0xB7FFFF
	 .word M68K_read_8_bad    ;@ 0xB80000 - 0xBFFFFF
     .word M68K_read_8_vdp    ;@ 0xC00000 - 0xC7FFFF
	 .word M68K_read_8_bad    ;@ 0xC80000 - 0xCFFFFF
	 .word M68K_read_8_bad    ;@ 0xD00000 - 0xD7FFFF
	 .word M68K_read_8_bad    ;@ 0xD80000 - 0xDFFFFF
     .word M68K_read_8_ram    ;@ 0xE00000 - 0xE7FFFF
	 .word M68K_read_8_ram    ;@ 0xE80000 - 0xEFFFFF
	 .word M68K_read_8_ram    ;@ 0xF00000 - 0xF7FFFF
	 .word M68K_read_8_ram    ;@ 0xF80000 - 0xFFFFFF
M68K_read_8_bad:
     eor r0,r0,r0
     mov pc,lr

M68K_read_8_rom0:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
	 
M68K_read_8_rom1:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x80000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_rom2:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x100000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_rom3:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x180000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
	 
M68K_read_8_rom4:
     ldr r1,drmd_context0
	 ldrb r2,[r1,#sram_flags]
	 tst r2,#1<<0
     beq 1f
	 ldr r2,[r1,#sram_end]
	 cmp r0,r2
	 bgt 1f
	 ldr r2,[r1,#sram_start]
	 cmp r0,r2
	 blt 1f
	 sub r0,r0,r2
	 eor r0,r0,#1
	 ldr r2,[r1,#sram]
	 ldrb r0,[r2,r0]
     mov pc,lr
1:	 
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x200000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_rom5:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x280000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_rom6:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x300000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_rom7:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x380000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_rom8:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x400000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_rom9:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x480000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_romA:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x500000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
M68K_read_8_romB:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x580000
     eor r0,r0,#1
     ;@ 0x 07 FF FF max address
     mov r0,r0,lsl#13
     ldrb r0,[r1,r0,lsr#13]
     mov pc,lr
 
M68K_read_8_romC:
M68K_read_8_romD:
M68K_read_8_romE:
M68K_read_8_romF:
	eor r0,r0,r0
	mov pc,lr
	
M68K_read_8_ram:
     ldr r1,drmd_context0
     ldr r1,[r1,#work_ram]
     eor r0,r0,#1
     mov r0,r0,lsl#16
     ldrb r0,[r1,r0,lsr#16]
     mov pc,lr   
   
M68K_read_8_misc:
     cmp r0,#0xA10000
     bge M68K_read_8_io
     ldr r2,drmd_context0
     ldrb r1,[r2,#zbusack]
     tst r1,r1
     bne M68K_read_8_bad
     ;@ 0110 0000 0000 0000
     and r1,r0,#0x6000
     ldr pc,[pc,r1,lsr#11]
     .word 0
     .word M68K_read_8_z80_ram
     .word M68K_read_8_z80_ram
     .word M68K_read_8_YM
     .word M68K_read_8_bad
M68K_read_8_z80_ram:
     ldr r1,[r2,#zram]
     ;@  max addr 0x1FFF 0000 0000 0000 0000 0001 1111 1111 1111
     mov r0,r0,lsl#19
     ldrb r0,[r1,r0,lsr#19]
     mov pc,lr
  
M68K_read_8_YM:
     and r0,r0,#0x3
     ldr pc,[r2,#fm_read]
              
M68K_read_8_io:
     mov r1,#0xA10000
	 orr r2,r1,#0x1100
	 cmp r0,r2
	 beq M68K_read_8_io_busack
	 orr r2,r1,#0xD
	 cmp r0,r2
	 bgt M68K_read_8_bad 
     b gen_io_r

M68K_read_8_io_busack:
     ldr r1,drmd_context0
     ldrb r0,[r1,#zbusack]
     and r0,r0,#1
     orr r0,r0,#0x80
     mov pc,lr     

M68K_read_8_vdp:
     mov r1,#0xC00000
	 orr r1,r1,#0x9
	 cmp r0,r1
	 bgt M68K_read_8_bad
     and r1,r0,#0xF
     ldr pc,[pc,r1,lsl#2]
     .word 0
     .word M68K_read_8_bad
     .word M68K_read_8_bad
     .word M68K_read_8_bad
     .word M68K_read_8_bad
     .word M68K_read_8_vdp_ctrl_low
     .word M68K_read_8_vdp_ctrl_high
     .word M68K_read_8_vdp_ctrl_low
     .word M68K_read_8_vdp_ctrl_high
     .word M68K_read_8_vdp_hvc_low
     .word M68K_read_8_vdp_hvc_high
     .word M68K_read_8_vdp_hvc_low
     .word M68K_read_8_vdp_hvc_high
     .word M68K_read_8_vdp_hvc_low
     .word M68K_read_8_vdp_hvc_high
     .word M68K_read_8_vdp_hvc_low
     .word M68K_read_8_vdp_hvc_high
     
M68K_read_8_vdp_ctrl_low:
     stmfd sp!,{lr}
     bl vdp_ctrl_r
     mov r0,r0,lsr#8
     ldmfd sp!,{pc}
     
M68K_read_8_vdp_ctrl_high:
     stmfd sp!,{lr}
     bl vdp_ctrl_r
     and r0,r0,#0xFF
     ldmfd sp!,{pc}
     
M68K_read_8_vdp_hvc_low:
     stmfd sp!,{lr}
     bl vdp_hvc_r
     mov r0,r0,lsr#8
     ldmfd sp!,{pc}
	 
M68K_read_8_vdp_hvc_high:
     stmfd sp!,{lr}
     bl vdp_hvc_r
     and r0,r0,#0xFF
     ldmfd sp!,{pc}
     
;@##############################################
;@##############################################

;@   M68K read16

;@##############################################
;@##############################################

m68k_read_memory_16:
     ;@ 1111 1000 0000 0000 0000 0000
     ;@ F80000
     and r1,r0,#0xF80000
     ldr pc,[pc,r1, lsr #17]
     .word 0
m68k_read_memory_16_data:
     .word M68K_read_16_rom0   ;@ 0x000000 - 0x07FFFF
     .word M68K_read_16_rom1   ;@ 0x080000 - 0x0FFFFF
	 .word M68K_read_16_rom2   ;@ 0x100000 - 0x17FFFF
	 .word M68K_read_16_rom3   ;@ 0x180000 - 0x1FFFFF
	 .word M68K_read_16_rom4   ;@ 0x200000 - 0x27FFFF
	 .word M68K_read_16_rom5   ;@ 0x280000 - 0x2FFFFF
	 .word M68K_read_16_rom6   ;@ 0x300000 - 0x37FFFF
	 .word M68K_read_16_rom7   ;@ 0x380000 - 0x3FFFFF
	 .word M68K_read_16_rom8   ;@ 0x400000 - 0x47FFFF
	 .word M68K_read_16_rom9   ;@ 0x480000 - 0x4FFFFF
	 .word M68K_read_16_romA   ;@ 0x500000 - 0x57FFFF
	 .word M68K_read_16_romB   ;@ 0x580000 - 0x5FFFFF
	 .word M68K_read_16_romC   ;@ 0x600000 - 0x67FFFF
	 .word M68K_read_16_romD   ;@ 0x680000 - 0x6FFFFF
	 .word M68K_read_16_romE   ;@ 0x700000 - 0x77FFFF
	 .word M68K_read_16_romF   ;@ 0x780000 - 0x7FFFFF
	 .word M68K_read_16_bad    ;@ 0x800000 - 0x87FFFF
	 .word M68K_read_16_bad    ;@ 0x880000 - 0x8FFFFF
	 .word M68K_read_16_bad    ;@ 0x900000 - 0x97FFFF
	 .word M68K_read_16_bad    ;@ 0x980000 - 0x9FFFFF
	 .word M68K_read_16_misc   ;@ 0xA00000 - 0xA7FFFF
	 .word M68K_read_16_bad    ;@ 0xA80000 - 0xAFFFFF
	 .word M68K_read_16_bad    ;@ 0xB00000 - 0xB7FFFF
	 .word M68K_read_16_bad    ;@ 0xB80000 - 0xBFFFFF
     .word M68K_read_16_vdp    ;@ 0xC00000 - 0xC7FFFF
	 .word M68K_read_16_bad    ;@ 0xC80000 - 0xCFFFFF
	 .word M68K_read_16_bad    ;@ 0xD00000 - 0xD7FFFF
	 .word M68K_read_16_bad    ;@ 0xD80000 - 0xDFFFFF
     .word M68K_read_16_ram    ;@ 0xE00000 - 0xE7FFFF
	 .word M68K_read_16_ram    ;@ 0xE80000 - 0xEFFFFF
	 .word M68K_read_16_ram    ;@ 0xF00000 - 0xF7FFFF
	 .word M68K_read_16_ram    ;@ 0xF80000 - 0xFFFFFF

Genesis_read_memory_16_data:
     .word M68K_read_16_rom0   ;@ 0x000000 - 0x07FFFF
     .word M68K_read_16_rom1   ;@ 0x080000 - 0x0FFFFF
	 .word M68K_read_16_rom2   ;@ 0x100000 - 0x17FFFF
	 .word M68K_read_16_rom3   ;@ 0x180000 - 0x1FFFFF
	 .word M68K_read_16_rom4   ;@ 0x200000 - 0x27FFFF
	 .word M68K_read_16_rom5   ;@ 0x280000 - 0x2FFFFF
	 .word M68K_read_16_rom6   ;@ 0x300000 - 0x37FFFF
	 .word M68K_read_16_rom7   ;@ 0x380000 - 0x3FFFFF
	 .word M68K_read_16_rom8   ;@ 0x400000 - 0x47FFFF
	 .word M68K_read_16_rom9   ;@ 0x480000 - 0x4FFFFF
	 .word M68K_read_16_romA   ;@ 0x500000 - 0x57FFFF
	 .word M68K_read_16_romB   ;@ 0x580000 - 0x5FFFFF
	 .word M68K_read_16_romC   ;@ 0x600000 - 0x67FFFF
	 .word M68K_read_16_romD   ;@ 0x680000 - 0x6FFFFF
	 .word M68K_read_16_romE   ;@ 0x700000 - 0x77FFFF
	 .word M68K_read_16_romF   ;@ 0x780000 - 0x7FFFFF
	 .word M68K_read_16_bad    ;@ 0x800000 - 0x87FFFF
	 .word M68K_read_16_bad    ;@ 0x880000 - 0x8FFFFF
	 .word M68K_read_16_bad    ;@ 0x900000 - 0x97FFFF
	 .word M68K_read_16_bad    ;@ 0x980000 - 0x9FFFFF
	 .word M68K_read_16_misc   ;@ 0xA00000 - 0xA7FFFF
	 .word M68K_read_16_bad    ;@ 0xA80000 - 0xAFFFFF
	 .word M68K_read_16_bad    ;@ 0xB00000 - 0xB7FFFF
	 .word M68K_read_16_bad    ;@ 0xB80000 - 0xBFFFFF
     .word M68K_read_16_vdp    ;@ 0xC00000 - 0xC7FFFF
	 .word M68K_read_16_bad    ;@ 0xC80000 - 0xCFFFFF
	 .word M68K_read_16_bad    ;@ 0xD00000 - 0xD7FFFF
	 .word M68K_read_16_bad    ;@ 0xD80000 - 0xDFFFFF
     .word M68K_read_16_ram    ;@ 0xE00000 - 0xE7FFFF
	 .word M68K_read_16_ram    ;@ 0xE80000 - 0xEFFFFF
	 .word M68K_read_16_ram    ;@ 0xF00000 - 0xF7FFFF
	 .word M68K_read_16_ram    ;@ 0xF80000 - 0xFFFFFF
	 
M68K_read_16_bad:
     eor r0,r0,r0
     mov pc,lr
	 
M68K_read_16_rom0:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr	
M68K_read_16_rom1:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x80000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
M68K_read_16_rom2:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x100000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
M68K_read_16_rom3:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x180000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
M68K_read_16_rom4:
     ldr r1,drmd_context0
	 ldrb r2,[r1,#sram_flags]
	 tst r2,#1<<0
     beq 1f
	 ldr r2,[r1,#sram_end]
	 cmp r0,r2
	 bgt 1f
	 ldr r2,[r1,#sram_start]
	 cmp r0,r2
	 blt 1f
	 sub r0,r0,r2
	 ldr r2,[r1,#sram]
	 add r2,r2,r0
	 ldrb r0,[r2],#1
	 ldrb r1,[r2]
	 orr r0,r0,r1,lsl#8
     mov pc,lr
1:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x200000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
	 
M68K_read_16_rom5:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x280000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
M68K_read_16_rom6:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x300000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
M68K_read_16_rom7:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x380000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
M68K_read_16_rom8:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x400000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
M68K_read_16_rom9:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x480000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr
M68K_read_16_romA:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x500000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr 
M68K_read_16_romB:
     ldr r1,drmd_context0
     ldr r1,[r1,#cart_rom]
	 add r1,r1,#0x580000
     mov r0,r0,lsl#13
     add r1,r1,r0,lsr#13
     ldrb r0,[r1],#1
	 ldrb r2,[r1]
	 orr r0,r0,r2,lsl#8
     mov pc,lr

M68K_read_16_romC:
M68K_read_16_romD:
M68K_read_16_romE:
M68K_read_16_romF:
     eor r0,r0,r0
     mov pc,lr 
	 
M68K_read_16_ram:
     ldr r1,drmd_context0
     ldr r1,[r1,#work_ram]
     mov r0,r0,lsl#16     
     add r1,r1,r0,lsr#16
     bic r1,r1,#1
     ldrh r0,[r1]
     mov pc,lr 
    
    
M68K_read_16_misc:
     cmp r0,#0xA10000
     bge M68K_read_16_io
     ldr r2,drmd_context0
     ldrb r1,[r2,#zbusack]
     tst r1,r1
     bne M68K_read_16_bad
     and r1,r0,#0x6000
     ldr pc,[pc,r1,lsr#11]
     .word 0
     .word M68K_read_16_z80_ram
     .word M68K_read_16_z80_ram
     .word M68K_read_16_YM
     .word M68K_read_16_bad
	 
M68K_read_16_z80_ram:
     ldr r1,[r2,#zram]
     mov r0,r0,lsl#19
     ldrb r0,[r1,r0,lsr#19]
     orr r0,r0,r0,lsl#8
     mov pc,lr
	 
M68K_read_16_YM:
     and r0,r0,#0x3
     stmfd sp!,{lr}
     mov lr,pc
     ldr pc,[r2,#fm_read]
     orr r0,r0,r0,lsl#8
     ldmfd sp!,{pc}  
	 
M68K_read_16_io:
     mov r1,#0xA10000
	 orr r2,r1,#0x1100
	 cmp r0,r2
	 beq M68K_read_16_io_busack
	 orr r2,r1,#0xD
	 cmp r0,r2
	 bgt M68K_read_16_bad
     stmfd sp!,{lr}	 
     bl gen_io_r
	 orr r0,r0,r0,lsl#8
	 ldmfd sp!,{pc}
     
M68K_read_16_io_busack:
     ldr r1,drmd_context0
     ldrb r1,[r1,#zbusack]
     and r1,r1,#1
	 orr r0,r1,#0x80
	 orr r0,r0,r0,lsl#8
     mov pc,lr     

M68K_read_16_vdp:
     mov r1,#0xC00000
	 orr r1,r1,#0x9
	 cmp r0,r1
	 bgt M68K_read_16_bad
     and r1,r0,#0xC
     ;@  0001 1110  1E
     ;@  0011 1100  3C
     ldr pc,[pc,r1]
     .word 0 
     .word vdp_data_r
     .word vdp_ctrl_r
     .word vdp_hvc_r
     .word vdp_hvc_r


     


;@##############################################
;@##############################################

;@   M68K read32

;@##############################################
;@##############################################
m68k_read_memory_32:
    stmfd sp!,{lr}
    stmfd sp!,{r0}
    bl m68k_read_memory_16
    ldmfd sp!,{r1}
    stmfd sp!,{r0}
    add r0,r1,#2
    bl m68k_read_memory_16
    ldmfd sp!,{r1}
    orr r0,r0,r1,lsl#16
    ldmfd sp!,{pc}
 
;@##############################################
;@##############################################

sram_modified_local: .word sram_modified

;@   M68K write8

;@##############################################
;@##############################################
;@ r0 addr, r1 data
m68k_write_memory_8:
     ;@ 1110 0000 0000 0000 0000 0000
     ;@ E00000
     and r2,r0,#0xE00000
     ldr pc,[pc,r2, lsr #19]
     .word 0
     .word M68K_write_8_sram
     .word M68K_write_8_sram
     .word M68K_write_8_sram
     .word M68K_write_8_sram
     .word M68K_lockup_w_8
     .word M68K_write_8_z80
     .word M68K_write_8_vdp
     .word M68K_write_8_ram
	 
M68K_write_8_sram:
     ldr r2,drmd_context0
	 ;@ check sram active
	 ;@ write enabled
	 ;@ and address in sram range
	 ldrb r3,[r2,#sram_flags]
	 tst r3,#1<<0
	 moveq pc,lr
	 tst r3,#1<<1
	 moveq pc,lr
	 ldr r3,[r2,#sram_end]
	 cmp r0,r3
	 movgt pc,lr
	 ldr r3,[r2,#sram_start]
	 cmp r0,r3
	 movlt pc,lr
	 ;@ sram write is valid
	 sub r0,r0,r3
	 eor r0,r0,#1
	 ldr r3,[r2,#sram]
	 strb r1,[r3,r0]
	 mov r0, #1
	 ldr r1, sram_modified_local
	 str r0, [r1]
     mov pc,lr

M68K_write_8_ram:
     ldr r2,drmd_context0
     ldr r2,[r2,#work_ram]
     eor r0,r0,#1
     mov r0,r0,lsl#16
     strb r1,[r2,r0,lsr#16]
     mov pc,lr    
   
M68K_write_8_z80:
     cmp r0,#0xA10000
     bge M68K_write_8_io
     ldr r2,drmd_context0
     ldrb r3,[r2,#zbusack]
     tst r3,r3
     movne pc,lr
     and r3,r0,#0x6000
     ldr pc,[pc,r3,lsr#11]
     .word 0
     .word M68K_write_8_z80_ram
     .word M68K_write_8_z80_ram
     .word M68K_write_8_YM
     .word M68K_write_8_z80_6000
M68K_write_8_z80_ram:
     ldr r3,[r2,#zram]
     mov r0,r0,lsl#19
     strb r1,[r3,r0,lsr#19]
     mov pc,lr
M68K_write_8_YM:
     and r0,r0,#0x3
     ldr pc,[r2,#fm_write]

M68K_write_8_z80_6000:
     and r3,r0,#0xFF00
     cmp r3,#0x7F00
     beq M68K_lockup_w_8
     cmp r3,#0x6000
     movne pc,lr
     and r0,r1,#1
     b gen_bank_w

          
M68K_write_8_io:
     mov r2,#0xA10000
	 orr r3,r2,#0x1100
	 cmp r0,r3
	 beq M68K_write_8_io_busack
	 orr r3,r2,#0x1200
	 cmp r0,r3
	 beq M68K_write_8_io_genreset
	 orr r3,r2,#0x9
	 ble M68K_write_8_io_iochip
	 orr r3,r2,#0x3000
	 orr r3,r3,#0xF1
	 cmp r0,r3
	 beq M68K_write_8_io_sram
	 movlt pc,lr
	 
	 orr r3,r3,#0xF
	 cmp r0,r3
	 movgt pc,lr
	 

	 ;@ page Genesis rom - 0xA130F2 - 0xA130FF
	 and r0,r0,#0xF
	 mov r0,r0,lsr#1
genesis_rom_bank_set:
	 and r1,r1,#0x1F
	 
	 ldr r3,Genesis_read_memory_8_table
	 ldr r2,[r3,r1,lsl#2]
	 ldr r3,m68k_read_memory_8_table
	 str r2,[r3,r0,lsl#2]
	 
	 ldr r3,Genesis_read_memory_16_table
	 ldr r2,[r3,r1,lsl#2]
	 ldr r3,m68k_read_memory_16_table
	 str r2,[r3,r0,lsl#2]
	 
	 ;@ save the bank for save states etc
	 ldr r2,drmd_context0
	 add r2,r2,#genesis_rom_banks
	 strb r1,[r2,r0]
	 
     mov pc,lr
	 
m68k_read_memory_8_table: 		.word m68k_read_memory_8_data
Genesis_read_memory_8_table: 	.word Genesis_read_memory_8_data
m68k_read_memory_16_table: 		.word m68k_read_memory_16_data
Genesis_read_memory_16_table: 	.word Genesis_read_memory_16_data

genesis_rom_banks_reset:
     ;@ reset 8bit banks
     ldr r0,m68k_read_memory_8_table
	 ldr r1,Genesis_read_memory_8_table
	 mov r3,#0x10
1:
	 ldr r2,[r1],#4
	 str r2,[r0],#4
	 subs r3,r3,#1
	 bne 1b
	 ;@ reset 16bit banks
	 ldr r0,m68k_read_memory_16_table
	 ldr r1,Genesis_read_memory_16_table
	 mov r3,#0x10
1:
	 ldr r2,[r1],#4
	 str r2,[r0],#4
	 subs r3,r3,#1
	 bne 1b
	 
	 ldr r2,drmd_context0
	 add r2,r2,#genesis_rom_banks
	 eor r1,r1,r1
1:
	 strb r1,[r2,r1]
	 add r1,r1,#1
	 cmp r1,#0x7
	 blt 1b
	 
	 mov pc,lr
	 
M68K_write_8_io_sram:
     ldr r2,drmd_context0
     strb r1,[r2,#sram_flags]
     mov pc,lr
	 
M68K_write_8_io_iochip:
     mov r2,#0xA10000
     orr r2,r2,#0xD
     cmp r0,r2
     movgt pc,lr
     b gen_io_w
	 
M68K_write_8_io_busack:
     tst r0,#1
     movne pc,lr
     and r0,r1,#1
     b gen_busreq_w

M68K_write_8_io_genreset:
     and r0,r1,#1
     b gen_reset_w
     
M68K_write_8_vdp:
     mov r2,#0xE70000
     orr r2,r2,#0xE0
     and r2,r2,r0
     cmp r2,#0xC00000
     bne M68K_lockup_w_8
     and r2,r0,#0x1F
     ldr pc,[pc,r2,lsl#2]
     .word 0
     .word M68K_write_8_vdp_data
     .word M68K_write_8_vdp_data
     .word M68K_write_8_vdp_data
     .word M68K_write_8_vdp_data
     .word M68K_write_8_vdp_ctrl
     .word M68K_write_8_vdp_ctrl
     .word M68K_write_8_vdp_ctrl
     .word M68K_write_8_vdp_ctrl
     .word M68K_lockup_w_8
     .word M68K_lockup_w_8
     .word M68K_lockup_w_8
     .word M68K_lockup_w_8
     .word M68K_lockup_w_8
     .word M68K_lockup_w_8
     .word M68K_lockup_w_8
     .word M68K_lockup_w_8
     .word M68K_unused_8_w
     .word M68K_write_8_psg
     .word M68K_unused_8_w
     .word M68K_write_8_psg
     .word M68K_unused_8_w
     .word M68K_write_8_psg
     .word M68K_unused_8_w
     .word M68K_write_8_psg
     .word M68K_unused_8_w
     .word M68K_unused_8_w
     .word M68K_unused_8_w
     .word M68K_unused_8_w
     .word M68K_unused_8_w
     .word M68K_unused_8_w
     .word M68K_unused_8_w
     .word M68K_unused_8_w

M68K_unused_8_w:
     mov pc,lr
     
M68K_write_8_vdp_data: 
     and r1,r1,#0xFF
     orr r0,r1,r1,lsl#8  
     b vdp_data_w

M68K_write_8_vdp_ctrl:
     and r1,r1,#0xFF
     orr r0,r1,r1,lsl#8
     b vdp_ctrl_w

M68K_write_8_psg:
     mov r0,r1
     ldr r2,drmd_context0
     ldr pc,[r2,#psg_write]
     
M68K_lockup_w_8_error: .word M68K_lockup_w_8_error_text
M68K_lockup_w_8_error_text: .ascii "M68K w 8 lockup"
      .align 4         
M68K_lockup_w_8:
      stmfd sp!,{r0-r12,lr}
      ldr r1,M68K_lockup_w_8_error
      mov lr,pc
      ldr pc,illegal_memory_io_local2
      ldmfd sp!,{r0-r12,pc}

;@##############################################
;@##############################################

;@   M68K write16

;@##############################################
;@##############################################
;@ r0 addr, r1 data
m68k_write_memory_16:
     ;@ 1110 0000 0000 0000 0000 0000
     ;@ E00000
     and r2,r0,#0xE00000
     ldr pc,[pc,r2, lsr #19]
     .word 0
     .word M68K_write_16_sram
     .word M68K_write_16_sram
     .word M68K_write_16_sram
     .word M68K_write_16_sram
     .word M68K_lockup_w_16
     .word M68K_write_16_z80
     .word M68K_write_16_vdp
     .word M68K_write_16_ram
	 
M68K_write_16_sram:
     ldr r2,drmd_context0
	 ;@ check sram active
	 ;@ write enabled
	 ;@ and address in sram range
	 ldrb r3,[r2,#sram_flags]
	 tst r3,#1<<0
	 moveq pc,lr
	 tst r3,#1<<1
	 moveq pc,lr
	 ldr r3,[r2,#sram_end]
	 cmp r0,r3
	 movgt pc,lr
	 ldr r3,[r2,#sram_start]
	 cmp r0,r3
	 movlt pc,lr
	 ;@ sram write is valid
	 sub r0,r0,r3
	 bic r0,r0,#1
	 ldr r3,[r2,#sram]
	 add r3,r3,r0
	 strh r1,[r3]
	 mov r0, #1
	 ldr r1, sram_modified_local
	 str r0, [r1]
     mov pc,lr

M68K_write_16_ram:
     ldr r2,drmd_context0
     ldr r2,[r2,#work_ram]
     mov r0,r0,lsl#16
     add r2,r2,r0,lsr#16
     bic r2,r2,#1
     strh r1,[r2]
     mov pc,lr    
   
M68K_write_16_z80:
     cmp r0,#0xA10000
     bge M68K_write_16_io
     ldr r2,drmd_context0
     ldrb r3,[r2,#zbusack]
     tst r3,r3
     movne pc,lr
     and r3,r0,#0x6000
     ldr pc,[pc,r3,lsr#11]
     .word 0
     .word M68K_write_16_z80_ram
     .word M68K_write_16_z80_ram
     .word M68K_write_16_YM
     .word M68K_write_16_z80_6000
M68K_write_16_z80_ram:
     ldr r3,[r2,#zram]
     mov r0,r0,lsl#19
     mov r1,r1,lsr#8
     strb r1,[r3,r0,lsr#19]
     mov pc,lr
M68K_write_16_YM:
     and r0,r0,#0x3
     mov r1,r1,lsr#8
     ldr pc,[r2,#fm_write]

M68K_write_16_z80_6000:
     and r3,r0,#0x7F00
     cmp r3,#0x7F00
     beq M68K_lockup_w_16
     cmp r3,#0x6000
     movne pc,lr
     mov r1,r1,lsr#8
     and r0,r1,#1
     b gen_bank_w

          
M68K_write_16_io:
     mov r2,#0xA10000
	 orr r3,r2,#0x1100
	 cmp r0,r3
	 beq M68K_write_16_io_busack
	 orr r3,r2,#0x1200
	 cmp r0,r3
	 beq M68K_write_16_io_genreset
	 orr r3,r2,#0x9
	 ble M68K_write_16_io_iochip
	 orr r3,r2,#0x3000
	 orr r3,r3,#0xF0
	 cmp r0,r3
	 beq M68K_write_16_io_sram
 
	 orr r3,r3,#0xF
	 cmp r0,r3
	 movgt pc,lr
	 
	 b genesis_rom_bank_set

M68K_write_16_io_sram:
     ldr r2,drmd_context0
     strb r1,[r2,#sram_flags]
     mov pc,lr
	 
M68K_write_16_io_iochip:
     mov r2,#0xA10000
     orr r2,r2,#0xD
     cmp r0,r2
     movgt pc,lr
     b gen_io_w
	 
M68K_write_16_io_busack:
     mov r0,r1,lsr#8
     and r0,r0,#1
     b gen_busreq_w
     
M68K_write_16_io_genreset:
     mov r0,r1,lsr#8
     and r0,r0,#1
     b gen_reset_w
     
M68K_write_16_vdp:
     mov r2,#0xE70000
     orr r2,r2,#0xE0
     and r2,r2,r0
     cmp r2,#0xC00000
     bne M68K_lockup_w_16
     and r2,r0,#0x1C
     ldr pc,[pc,r2]
     .word 0
     .word M68K_write_16_vdp_data
     .word M68K_write_16_vdp_ctrl
     .word M68K_lockup_w_16
     .word M68K_lockup_w_16
     .word M68K_write_16_psg
     .word M68K_write_16_psg
     .word M68K_unused_16_w
     .word M68K_unused_16_w

M68K_unused_16_w:
     mov pc,lr
	 
M68K_write_16_vdp_data: 
     mov r0,r1  
     b vdp_data_w

M68K_write_16_vdp_ctrl:
     mov r0,r1
     b vdp_ctrl_w

M68K_write_16_psg:
     and r0,r1,#0xFF
     ldr r2,drmd_context1
     ldr pc,[pc,#psg_write]
     
illegal_memory_io_local2: .word illegal_memory_io
M68K_lockup_w_16_error: .word M68K_lockup_w_16_error_text
M68K_lockup_w_16_error_text: .ascii "M68K w 16 lockup"    
      .align 4     
M68K_lockup_w_16:
      stmfd sp!,{r0-r12,lr}
      ldr r1,M68K_lockup_w_16_error
      mov lr,pc
      ldr pc,illegal_memory_io_local2
      ldmfd sp!,{r0-r12,pc}

m68k_write_memory_32:
    stmfd sp!,{r0,r1,lr}
    mov r1,r1,lsr#16
    bl m68k_write_memory_16
    ldmfd sp!,{r0,r1,lr}
    add r0,r0,#2
    mov r1,r1,lsl#16
    mov r1,r1,lsr#16
    b m68k_write_memory_16
 
    
;@################################################
;@################################################

;@  vdp

;@################################################
;@################################################
    
vdp_data_r:
    ldr r2,drmd_context1
    eor r0,r0,r0  ;@ clear r0  - default return val
    strb r0,[r2,#vdp_pending]
    ldrb r1,[r2,#vdp_code]
    and r1,r1,#0xF
    ldr pc,[pc,r1,lsl#2]
    .word 0
    .word vdp_data_r_vram
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_vsram
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_cram
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_exit
    .word vdp_data_r_exit

vdp_data_r_vram:
    ldr r0,[r2,#vram]
    ldrh r1,[r2,#vdp_addr]
    bic r1,r1,#1
    add r3,r0,r1
    ldrh r0,[r3]
    b vdp_data_r_exit
vdp_data_r_cram:
    ldr r0,[r2,#cram]
    ldrh r1,[r2,#vdp_addr]
    and r3,r1,#0x7E
    add r3,r0,r3
    ldrh r0,[r3]
    b vdp_data_r_exit
vdp_data_r_vsram:
    ldr r0,[r2,#vsram]
    ldrh r1,[r2,#vdp_addr]
    and r3,r1,#0x7E
    add r3,r0,r3
    ldrh r0,[r3]
    b vdp_data_r_exit    
    
vdp_data_r_exit:
    ldrb r3,[r2,#vdp_reg15]
    ldrh r1,[r2,#vdp_addr]
    add r1,r1,r3
    strh r1,[r2,#vdp_addr]
    ;@ returns r0 = vdp data
    mov pc,lr

vdp_data_w:
    ldr r2,drmd_context1
    eor r1,r1,r1
    strb r1,[r2,#vdp_pending]
    ldrb r1,[r2,#vdp_code]
    and r1,r1,#0xF
    ldr pc,[pc,r1,lsl#2]
    .word 0 
    .word vdp_data_w_exit
    .word vdp_data_w_vram
    .word vdp_data_w_exit
    .word vdp_data_w_cram
    .word vdp_data_w_exit
    .word vdp_data_w_vsram
    .word vdp_data_w_exit
    .word vdp_data_w_exit
    .word vdp_data_w_exit
    .word vdp_data_w_exit
    .word vdp_data_w_exit
    .word vdp_data_w_exit
    .word vdp_data_w_exit
    .word vdp_data_w_exit
    .word vdp_data_w_exit
    .word vdp_data_w_exit

/*	
dirtypallocal: .word DirtyPal
dirtyvramlocal: .word DirtyVram
dirtytilelocal: .word DirtyTile
*/
vdp_data_w_vram:
    ldrh r3,[r2,#vdp_addr]
    tst r3,#1
    andne r1,r0,#0xFF
    movne r0,r0,lsr#8
    andne r0,r0,#0xFF
    orrne r0,r0,r1,lsl#8  ;@ byte swap
    ldr r1,[r2,#vram]
    add r1,r1,r3 
    bic r1,r1,#1
    strh r0,[r1]
	
	;@ set dirty flags
	/*
	mov r0,#1
	ldr r1,dirtyvramlocal
	strb r0,[r1] ;@ mark global vram dirty flag
	ldr r1,dirtytilelocal
	mov r0,#0xF
	strb r0,[r1,r3,lsr#5] ;@ divide address by 32 to get tile number
	*/
    b vdp_data_w_exit

vdp_data_w_cram:
    ldrh r3,[r2,#vdp_addr]
    tst r3,#1
    andne r1,r0,#0xFF
    movne r0,r0,lsr#8
    andne r0,r0,#0xFF
    orrne r0,r0,r1,lsl#8  ;@ byte swap
    ldr r1,[r2,#cram]
    and r3,r3,#0x7E
    add r1,r3,r1 
    strh r0,[r1]
    
	;@ set dirty flags
	/*
	mov r0,#1
	ldr r1,dirtypallocal
	strb r0,[r1,r3,lsr#5]
	*/
    b vdp_data_w_exit
    
    ldrb r1,[r2,#vdp_reg7]
    cmp r1,r3,lsr#1
    beq vdp_data_w_cram_doborder
    ands r1,r3,#0x1F
    beq vdp_data_w_exit  ;@ do not update color 0 of pals 0to3
    ;@ md  0000 bbb0 ggg0 rrr0
    ;@ gp  rrrr rggg ggbb bbbi
    ldr r1,[r2,#local_pal]
    add r1,r1,r3,lsl#1
    ldrh r3,[r2,#gamma]
    ands r2,r0,#0xE
    orrne r2,r2,r2,lsl#3
    andne r2,r2,#0x7C
    orrne r3,r3,r2,lsl#9
    ands r2,r0,#0xE0
    orrne r2,r2,r2,lsr#3
    andne r2,r2,#0xF8
    orrne r3,r3,r2,lsl#3
    ands r2,r0,#0xE00
    orrne r2,r2,r2,lsr#3
    orrne r3,r3,r2,lsr#6
    strh r3,[r1]
    ldr r2,drmd_context1
    
	
    b vdp_data_w_exit
vdp_data_w_cram_doborder:
    ldr r1,[r2,#local_pal]
    add r1,r1,r3,lsl#1
    ldrh r3,[r2,#gamma]
    ands r2,r0,#0xE
    orrne r2,r2,r2,lsl#3
    andne r2,r2,#0x7C
    orrne r3,r3,r2,lsl#9
    ands r2,r0,#0xE0
    orrne r2,r2,r2,lsr#3
    andne r2,r2,#0xF8
    orrne r3,r3,r2,lsl#3
    ands r2,r0,#0xE00
    orrne r2,r2,r2,lsr#3
    orrne r3,r3,r2,lsr#6
    strh r3,[r1]
    ldr r2,drmd_context1
vdp_data_w_cram_doborder_skiptrans:
    ldr r1,[r2,#local_pal]
    strh r3,[r1],#0x40
    strh r3,[r1],#0x40
    strh r3,[r1],#0x40
    strh r3,[r1]
    b vdp_data_w_exit
vdp_data_w_vsram:
    ldrh r3,[r2,#vdp_addr]
    tst r3,#1
    andne r1,r0,#0xFF
    movne r0,r0,lsr#8
    andne r0,r0,#0xFF
    orrne r0,r0,r1,lsl#8  ;@ byte swap
    ldr r1,[r2,#vsram]
    and r3,r3,#0x7E
    add r1,r3,r1 
    strh r0,[r1]
    ;@  convert to GP32 tile graphics
    b vdp_data_w_exit
    
vdp_data_w_exit:
    ldrb r1,[r2,#vdp_reg15]
    ldrh r3,[r2,#vdp_addr]
    add r3,r3,r1
    strh r3,[r2,#vdp_addr]
    ldrb r1,[r2,#vdp_dma_fill]
    tst r1,r1
    moveq pc,lr


	
	
vdp_data_w_dma_fill:
    stmfd sp!,{r4,r5}
    eor r1,r1,r1
    strb r1,[r2,#vdp_dma_fill]
    ldrb r1,[r2,#vdp_reg20]
    ldrb r3,[r2,#vdp_reg19]
    orr r1,r3,r1,lsl#8
	mov r1,r1,lsl#16
	ldr r3,[r2,#vram]
	ldrh r4,[r2,#vdp_addr]
	strb r0,[r3,r4] ;@ save low bit
	mov r4,r4,lsl#16
	ldrb r5,[r2,#vdp_reg15]
	mov r0,r0,lsr#8
vdp_data_w_dma_fill_loop: 
    strb r0,[r3,r4,lsr#16]
	add r4,r4,r5,lsl#16
    subs r1,r1,#1<<16
    bne vdp_data_w_dma_fill_loop
	mov r4,r4,lsr#16
	strh r4,[r2,#vdp_addr]
	;@ eor r1,r1,r1
	;@ strb r1,[r2,#vdp_reg20]
	;@ strb r1,[r2,#vdp_reg19]
    ldmfd sp!,{r4,r5}
    mov pc,lr
	
   
   
vdp_ctrl_r:
    ldr r2,drmd_context1
    eor r0,r0,r0
    strb r0,[r2,#vdp_pending]
    ldrh r0,[r2,#vdp_status]
    orr r0,r0,#0x20  ;@  dma busy hack
    eor r0,r0,#0x300 ;@ fifo hack
    ;@ eor r0,r0,#4     ;@ hblank hack 
    bic r1,r0,#0x40
    strh r1,[r2,#vdp_status]
    mov pc,lr


vdp_ctrl_w:
    ldr r2,drmd_context1
    ldrb r1,[r2,#vdp_pending]
    tst r1,r1
    bne vdp_ctrl_w_clearpend 
    and r1,r0,#0xC000
    cmp r1,#0x8000
    bne vdp_ctrl_w_setpend
    and r1,r0,#0x1F00
    add r3,r2,#vdp_reg0
    strb r0,[r3,r1,lsr#8]
    ldr pc,[pc,r1,lsr#6]   ;@ do code require by reg change
    .word 0
    .word vdp_ctrl_w_setpend_done ;@ 0
    .word vdp_ctrl_w_setpend_done ;@ 1
    .word vdp_ctrl_w_setpend_done ;@ 2
    .word vdp_ctrl_w_setpend_done ;@ 3
    .word vdp_ctrl_w_setpend_done ;@ 4
    .word vdp_ctrl_w_setpend_done ;@ 5
    .word vdp_ctrl_w_setpend_done ;@ 6
    .word vdp_ctrl_w_reg7 ;@ 7
    .word vdp_ctrl_w_setpend_done ;@ 8
    .word vdp_ctrl_w_setpend_done ;@ 9
    .word vdp_ctrl_w_setpend_done ;@ 10
    .word vdp_ctrl_w_setpend_done ;@ 11
    .word vdp_ctrl_w_setpend_done ;@ 12
    .word vdp_ctrl_w_setpend_done ;@ 13
    .word vdp_ctrl_w_setpend_done ;@ 14
    .word vdp_ctrl_w_setpend_done ;@ 15
    .word vdp_ctrl_w_setpend_done ;@ 16
    .word vdp_ctrl_w_setpend_done ;@ 17
    .word vdp_ctrl_w_setpend_done ;@ 18
    .word vdp_ctrl_w_setpend_done ;@ 19
    .word vdp_ctrl_w_setpend_done ;@ 20
    .word vdp_ctrl_w_setpend_done ;@ 21
    .word vdp_ctrl_w_setpend_done ;@ 22
    .word vdp_ctrl_w_setpend_done ;@ 23
    .word vdp_ctrl_w_setpend_done ;@ 24
    .word vdp_ctrl_w_setpend_done ;@ 25
    .word vdp_ctrl_w_setpend_done ;@ 26
    .word vdp_ctrl_w_setpend_done ;@ 27
    .word vdp_ctrl_w_setpend_done ;@ 28
    .word vdp_ctrl_w_setpend_done ;@ 29
    .word vdp_ctrl_w_setpend_done ;@ 30
    .word vdp_ctrl_w_setpend_done ;@ 31

vdp_ctrl_w_reg7:  ;@ update border color
    and r1,r0,#0x3F
    strb r1,[r2,#vdp_reg7]   ;@ mask border color
    
    b vdp_ctrl_w_setpend_done
    
    
    ldr r3,[r2,#cram]
    add r3,r3,r1,lsl#1
    ldrh r1,[r3]   ;@ get md pal data
    ldrh r3,[r2,#gamma]
    ands r2,r1,#0xE
    orrne r2,r2,r2,lsl#3
    andne r2,r2,#0x7C
    orrne r3,r3,r2,lsl#9
    ands r2,r1,#0xE0
    orrne r2,r2,r2,lsr#3
    andne r2,r2,#0xF8
    orrne r3,r3,r2,lsl#3
    ands r2,r1,#0xE00
    orrne r2,r2,r2,lsr#3
    orrne r3,r3,r2,lsr#6
    ldr r2,drmd_context1   ;@ reload pointer to drmd
    ldr r1,[r2,#local_pal]
    strh r3,[r1],#0x40
    strh r3,[r1],#0x40
    strh r3,[r1],#0x40
    strh r3,[r1]
    b vdp_ctrl_w_setpend_done
vdp_ctrl_w_setpend:
    mov r1,#1
    strb r1,[r2,#vdp_pending]
vdp_ctrl_w_setpend_done:
    ldrh r1,[r2,#vdp_addr_latch]
    bic r3,r0,#0xFC000
    and r1,r1,#0xC000
    orr r3,r3,r1
    strh r3,[r2,#vdp_addr]
    mov r0,r0,lsr#14
    and r0,r0,#0x3
    ldrb r1,[r2,#vdp_code]
    and r1,r1,#0x3C
    orr r0,r0,r1
    and r0,r0,#0x3F
    strb r0,[r2,#vdp_code]
    mov pc,lr    
vdp_ctrl_w_clearpend:
    mov r1,#0
    strb r1,[r2,#vdp_pending]
    ldrh r1,[r2,#vdp_addr]
    bic r1,r1,#0xC000
    and r3,r0,#0x3
    orr r1,r1,r3,lsl#14
    strh r1,[r2,#vdp_addr]
    and r1,r1,#0xC000
    strh r1,[r2,#vdp_addr_latch]
    ldrb r1,[r2,#vdp_code]
    and r1,r1,#0x3
    mov r0,r0,lsr#2
    and r0,r0,#0x3C
    orr r0,r0,r1
    strb r0,[r2,#vdp_code]
    ;@ DMA command
    tst r0,#0x20
    moveq pc,lr
    ldrb r0,[r2,#vdp_reg1]
    tst r0,#0x10
    moveq pc,lr
    ldrb r0,[r2,#vdp_reg23]
    and r1,r0,#0xC0
    ldr pc,[pc,r1,lsr#4]
    .word 0
    .word vdp_ctrl_w_dma_vbus
    .word vdp_ctrl_w_dma_vbus
    .word vdp_ctrl_w_dma_fill
    .word vdp_ctrl_w_dma_copy

vdp_ctrl_w_dma_vbus:
    ;@ r0 = vdpReg23
    and r0,r0,#0x7F
    mov r0,r0,lsl#17
    ldrb r1,[r2,#vdp_reg22]
    orr r0,r0,r1,lsl#9
    ldrb r1,[r2,#vdp_reg21]
    orr r0,r0,r1,lsl#1
    ldrb r1,[r2,#vdp_reg20]
    ldrb r3,[r2,#vdp_reg19]
    orrs r1,r3,r1,lsl#8
    moveq r1,#0x10000
    stmfd sp!,{lr}
    ;@ r0 = source
    ;@ r1 = length
    and r3,r0,#0xFE0000
    str r3,dma_source_base
vdp_ctrl_w_dma_vbus_loop:
    stmfd sp!,{r0,r1}
    and r3,r0,#0xE00000
    ldr pc,[pc,r3, lsr #19]
    .word 0
    .word vdp_ctrl_w_dma_vbus_rom
    .word vdp_ctrl_w_dma_vbus_rom
    .word vdp_ctrl_w_dma_vbus_unused
    .word vdp_ctrl_w_dma_vbus_unused
    .word vdp_ctrl_w_dma_vbus_ram
    .word vdp_ctrl_w_dma_vbus_z80
    .word vdp_ctrl_w_dma_vbus_ram
    .word vdp_ctrl_w_dma_vbus_ram
dma_source_base:  .word 0    
vdp_ctrl_w_dma_vbus_end:
    bl vdp_data_w
    ldmfd sp!,{r0,r1}
    add r0,r0,#2
    mov r0,r0,lsl#15
    ldr r3,dma_source_base
    orr r0,r3,r0,lsr#15
    subs r1,r1,#1
    bne vdp_ctrl_w_dma_vbus_loop
    strb r1,[r2,#vdp_reg19]
    strb r1,[r2,#vdp_reg20]
    mov r1,r0,lsr#1
    strb r1,[r2,#vdp_reg21]
    mov r1,r0,lsr#9
    strb r1,[r2,#vdp_reg22]
    ldrb r1,[r2,#vdp_reg23]
    and r1,r1,#0x80
    mov r0,r0,lsr#17
    and r0,r0,#0x7F
    orr r0,r0,r1
    strb r0,[r2,#vdp_reg23]
    ldmfd sp!,{lr}
    mov pc,lr

vdp_ctrl_w_dma_vbus_rom:
    ldr r1,[r2,#cart_rom]
    mov r0,r0,lsl#9
    add r1,r1,r0,lsr#9
    ldrb r0,[r1],#1
    ldrb r1,[r1]
    orr r0,r0,r1,lsl#8
    b vdp_ctrl_w_dma_vbus_end    

vdp_ctrl_w_dma_vbus_unused:
    mov r0,#0xFF00
    b vdp_ctrl_w_dma_vbus_end    

vdp_ctrl_w_dma_vbus_ram:
    ldr r1,[r2,#work_ram]
    mov r0,r0,lsl#16     
    add r1,r1,r0,lsr#16
    ldrb r0,[r1],#1
    ldrb r1,[r1]
    orr r0,r0,r1,lsl#8
    b vdp_ctrl_w_dma_vbus_end    

vdp_ctrl_w_dma_vbus_z80:
    cmp r0,#0xA10000
    bge vdp_ctrl_w_dma_vbus_z80_io
    ldrb r3,[r2,#zbusack]
    tst r3,r3
    bne vdp_ctrl_w_dma_vbus_ram
    mov r0,#0xFF00
    orr r0,r0,#0xFF
    b vdp_ctrl_w_dma_vbus_end 
    
vdp_ctrl_w_dma_vbus_z80_io:
    mov r3,#0xA10000
    orr r3,r3,#0xF
    cmp r0,r3
    bgt vdp_ctrl_w_dma_vbus_ram
    bl gen_io_r
    orr r0,r0,r0,lsl#8
    b vdp_ctrl_w_dma_vbus_end
    
vdp_ctrl_w_dma_fill:
    mov r0,#1
    strb r0,[r2,#vdp_dma_fill]    
    mov pc,lr
    
vdp_ctrl_w_dma_copy:
    ldrb r0,[r2,#vdp_reg22]
    ldrb r1,[r2,#vdp_reg21]
    orr r0,r1,r0,lsl#8
	mov r0,r0,lsl#16
    ldrb r1,[r2,#vdp_reg20]
    ldrb r3,[r2,#vdp_reg19]
    orr r1,r3,r1,lsl#8
    mov r1,r1,lsl#16
    ldr r3,[r2,#vram]
    stmfd sp!,{r4,r5}
    ldrh r4,[r2,#vdp_addr]
	mov r4,r4,lsl#16
    ldrb r5,[r2,#vdp_reg15]
	mov r5,r5,lsl#16
vdp_ctrl_w_dma_copy_loop:   
    ldrb r2,[r3,r0,lsr#16]
    strb r2,[r3,r4,lsr#16]
    add r0,r0,#1<<16
    add r4,r4,r5
    subs r1,r1,#1<<16
    bne vdp_ctrl_w_dma_copy_loop
    ldr r2,drmd_context1  ;@ reload pointer
	mov r4,r4,lsr#16
    strh r4,[r2,#vdp_addr]
	mov r0,r0,lsr#16
	strb r0,[r2,#vdp_reg21]
	mov r0,r0,lsr#8
	strb r0,[r2,#vdp_reg22]
    ;@ eor r0,r0,r0
    ;@ strb r0,[r2,#vdp_reg19]
    ;@ strb r0,[r2,#vdp_reg20]
    ldmfd sp!,{r4,r5}
    mov pc,lr
   
vdp_hvc_r:
    ldr r2,drmd_context1
    ldrh r0,[r2,#vdp_line]
    cmp r0,#0xFF
    movgt r0,#0xFF
    mov r0,r0,lsl#8
    ;@ in theory r5 will be holding the current cyclone cycle count
    ;@ but if this is called from a z80 mem handler all bets are off
    
    ldrh r3,[r2,#cpl_m68k]
    cmp r5,r3
    movgt pc,lr  ;@  r5 is invalid so exit - hopefully will never happen
  
    ldrh r1,[r2,#vdp_status]
    tst r1,#4  ;@ check to see if we are in hblank or not
    sub r3,r3,r5   ;@ get cycles executed
    addne r3,r3,#404
    cmp r3,#0xFF
    movgt r3,#0xFF
    orr r0,r0,r3
    mov pc,lr

;@######################################################
;@######################################################

;@ Genesis

;@######################################################
;@######################################################  
gen_io_r:
    and r1,r0,#0xE
    ldr pc,[pc,r1,lsl#1]
    .word 0
    .word gen_io_r_MD_spec
    .word gen_io_r_pad_1
    .word gen_io_r_pad_2
    .word gen_io_r_Ser
    .word gen_io_r_pad_1_CT
    .word gen_io_r_pad_2_CT
    .word gen_io_r_Ser_CT
    .word gen_io_r_bad

gen_io_r_MD_spec:
    ldr r2,drmd_context1
    ldrb r0,[r2,#region]
    mov pc,lr

gen_io_r_pad_1_CT:
    ldr r2,drmd_context1
	ldrb r0,[r2,#pad_1_com]
	mov pc,lr

gen_io_r_pad_1_3_buttons:
    ldr r2,drmd_context1
	ldrb r0,[r2,#pad_1_state]
	ands r0,r0,#0x40
	ldr r1,[r2,#pad]
    eor r1,r1,#0xFF
	andne r1,r1,#0x3F
    andeq r2,r1,#0xC0
    andeq r1,r1,#0x3
	orreq r1,r1,r2,lsr#2
	orr r0,r0,r1
	mov pc,lr

gen_io_r_pad_1:
    ldr r2,drmd_context1
	ldrb r0,[r2,#pad_1_state]
	ldrb r1,[r2,#pad_1_type]
	tst r1,r1
	ldrneb r1,[r2,#pad_1_counter]
	
	mov r0,r0,lsr#6
	and r0,r0,#1
	and r1,r1,#3
	orr r1,r0,r1,lsl#1
	ldr pc,[pc,r1,lsl#2]
	.word 0
    .word gen_io_r_pad_1_first_low	
    .word gen_io_r_pad_1_first_high	
    .word gen_io_r_pad_1_second_low
    .word gen_io_r_pad_1_second_high	
    .word gen_io_r_pad_1_third_low	
    .word gen_io_r_pad_1_third_high
    .word gen_io_r_pad_1_fourth_low	
    .word gen_io_r_pad_1_fourth_high
	

gen_io_r_pad_1_first_high:	
gen_io_r_pad_1_second_high:	
gen_io_r_pad_1_third_high:
    ;@ C,B,RIGHT,LEFT,UP,DOWN
	ldr r0,[r2,#pad]
	eor r0,r0,#0xFF
	and r0,r0,#0x3F
	orr r0,r0,#0x40
	mov pc,lr
	
gen_io_r_pad_1_first_low:	
gen_io_r_pad_1_second_low:	
    ;@ START,A, DOWN, UP
	ldr r0,[r2,#pad]
	eor r0,r0,#0xFF
    and r1,r0,#0xC0
    and r0,r0,#0x3
	orr r0,r0,r1,lsr#2
	mov pc,lr
	
gen_io_r_pad_1_third_low:	
    ;@ START,A
    ldr r0,[r2,#pad]
	eor r0,r0,#0xFF
    and r0,r0,#0xC0
	mov r0,r0,lsr#2
	mov pc,lr
	
gen_io_r_pad_1_fourth_high:	
    ;@ C,B,MODE,X,Y,Z
    ldr r0,[r2,#pad]
	eor r0,r0,#0xFF0
	and r0,r0,#0xF30
	/*
	MXYZ 00CB 0000
	MXYZ 00CB MXYZ
	
	0000 0000
	0000 000C
	0000 0C0M
	000C 0M0Y
	
	0000 0000
	0000 000B
	0000 0B0X
	000B 0X0Z
	
	00CBMXYZ
	*/			
	orr r0,r0,r0,lsr#8
    and r0,r0,#0x3F
	orr r0,r0,#0x40
	mov pc,lr
	
gen_io_r_pad_1_fourth_low:	
    ;@ START, A
    ldr r0,[r2,#pad]
	eor r0,r0,#0xFF
    and r0,r0,#0xC0
	mov r0,r0,lsr#2
	orr r0,r0,#0xF
	mov pc,lr


gen_io_r_pad_2_CT:
    ldr r2,drmd_context1
	ldrb r0,[r2,#pad_2_com]
	mov pc,lr
	
gen_io_r_pad_2:
    ldr r2,drmd_context1
	ldrb r0,[r2,#pad_2_state]
	ands r0,r0,#0x40
    mov r1,#0xFF
	andne r1,r1,#0x3F
    andeq r2,r1,#0xC0
    andeq r1,r1,#0x3
	orreq r1,r1,r2,lsr#2
	orr r0,r0,r1
	mov pc,lr
	
gen_io_r_Ser:
    mov r0,#0xFF
	mov pc,lr

gen_io_r_Ser_CT:
gen_io_r_bad:
    eor r0,r0,r0
	mov pc,lr

gen_io_w:
    and r3,r0,#0xE
    ldr pc,[pc,r3,lsl#1]
    .word 0
    .word gen_io_w_bad
    .word gen_io_w_pad_1
    .word gen_io_w_pad_2
    .word gen_io_w_bad
    .word gen_io_w_pad_1_CT
    .word gen_io_w_pad_2_CT
    .word gen_io_w_bad
    .word gen_io_w_bad
	
gen_io_w_pad_2_CT:
gen_io_w_pad_2:
gen_io_w_bad:
     mov pc,lr

gen_io_w_pad_1:
     ldr r2,drmd_context1
	 ldrb r0,[r2,#pad_1_state]
	 tst r0,#0x40
	 bne 1f
	 tst r1,#0x40
	 beq 1f
	 ldrb r0,[r2,#pad_1_counter]
	 add r0,r0,#1
	 strb r0,[r2,#pad_1_counter]
1:	 
	 strb r1,[r2,#pad_1_state]
	 eor r0,r0,r0
	 strb r0,[r2,#pad_1_delay]
	 mov pc,lr
	 
gen_io_w_pad_1_CT:
     ldr r2,drmd_context1
     strb r1,[r2,#pad_1_com]
	 mov pc,lr
	 
     
gen_busreq_w:
     ldr r2,drmd_context1
	 eor r1,r1,r1
	 ;@ clear pad_1_counter
	 str r1,[r2,#pad_1_counter]
	 ;@ clear pad_2_counter
	 str r1,[r2,#pad_2_counter]
     and r0,r0,#1
     strb r0,[r2,#zbusreq]
     ldrb r3,[r2,#zreset]
     and r0,r0,r3
     eor r0,r0,#1
     strb r0,[r2,#zbusack]
     tst r0,r0
     movne pc,lr
     tst r3,r3
     moveq pc,lr
     ldr r0,[r2,#z80_context]
     mov r1,#32
     ldr pc,[r2,#z80_run]

gen_reset_w:
     ldr r2,drmd_context1
     and r0,r0,#1
     strb r0,[r2,#zreset]
     ldrb r3,[r2,#zbusreq]
     and r3,r3,r0
     eor r3,r3,#1
     strb r3,[r2,#zbusack]
     tst r0,r0
     movne pc,lr     
     ldr pc,[r2,#z80_reset]
     
gen_bank_w:
     ldr r2,drmd_context1
     ldr r1,[r2,#zbank]
     mov r1,r1,lsr#1
     and r0,r0,#1
     orr r1,r1,r0,lsl#23
     bic r1,r1,#0x7F00
     str r1,[r2,#zbank]
     mov pc,lr   

 
   
;@#################################################
;@#################################################

;@ Z80 Memory functions

;@#################################################
;@#################################################
drmd_context1:   .word drmd
DrMD_Z80_read_8:
     ;@ 0000 0000 0000 0000
     and r1,r0,#0xE000
     ldr pc,[pc,r1,lsr#11]
     .word 0
     .word Z80_read_8_zram
     .word Z80_read_8_zram
     .word Z80_read_8_fm
     .word Z80_read_8_vdp
     .word Z80_read_8_bankedmem
     .word Z80_read_8_bankedmem
     .word Z80_read_8_bankedmem
     .word Z80_read_8_bankedmem

Z80_read_8_zram:     
      ldr r2,drmd_context1
      ldr r1,[r2,#zram]
      ;@ 0000 0000 0000 0000 0001 1111 1111 1111
      ;@ 19
      mov r0,r0,lsl#19
      ldrb r0,[r1,r0,lsr#19]
      mov pc,lr

Z80_read_8_fm:    
     ldr r2,drmd_context1
     and r0,r0,#0x3
     ldr pc,[r2,#fm_read]

Z80_read_8_vdp:
      and r1,r0,#0xFF00
      cmp r1,#0x7F00
      beq Z80_vdp_r
      mov r0,#0xFF
      mov pc,lr

Z80_read_8_bankedmem:
      ldr r2,drmd_context1
      ldr r1,[r2,#zbank]
      bic r0,r0,#0xF8000
      add r0,r0,r1
      mov r1,r0,lsr#21
      and r1,r1,#0x7
      ldr pc,[pc,r1,lsl#2]
      .word 0
      .word Z80_read_8_bankedmem_rom
      .word Z80_read_8_bankedmem_rom
      .word Z80_read_8_bankedmem_unused  
      .word Z80_read_8_bankedmem_unused
      .word Z80_read_8_bankedmem_lockup
      .word Z80_read_8_bankedmem_z80
      .word Z80_read_8_bankedmem_vdp
      .word Z80_read_8_bankedmem_ram

Z80_read_8_bankedmem_rom:
      ldr r1,[r2,#cart_rom]
      eor r0,r0,#1
      ldrb r0,[r1,r0]
      mov pc,lr

Z80_read_8_bankedmem_unused:
      tst r0,#1
      moveq r0,#0
      movne r0,#0xFF
      mov pc,lr      
  
Z80_read_8_bankedmem_lockup_error: .word Z80_read_8_bankedmem_lockup_error_text
Z80_read_8_bankedmem_lockup_error_text: .ascii "Z80 read 8 banked mem lockup" 
      .align 4
Z80_read_8_bankedmem_lockup:
      stmfd sp!,{r0-r12,lr}
      ldr r1,Z80_read_8_bankedmem_lockup_error
      mov lr,pc
      ldr pc,illegal_memory_io_local
      ldmfd sp!,{r0-r12,pc} 

Z80_read_8_bankedmem_z80:
      mov r1,r0,lsr#8
      and r1,r1,#0xFF
      cmp r1,#0x00
      beq Z80_read_8_bankedmem_z80_io
      cmp r1,#0x10
      beq Z80_read_8_bankedmem_unused 
      cmp r1,#0x11
      moveq r0,#0xFF
      moveq pc,lr
      cmp r1,#0x12
      beq Z80_read_8_bankedmem_unused
      cmp r1,#0x13
      beq Z80_read_8_bankedmem_unused
      cmp r1,#0x20
      beq Z80_read_8_bankedmem_unused
      cmp r1,#0x30
      beq Z80_read_8_bankedmem_unused
      b Z80_read_8_bankedmem_lockup
      
Z80_read_8_bankedmem_z80_io: 
      mov r1,#0xA10000
      orr r1,r1,#0xF
      cmp r0,r1
      bgt Z80_read_8_bankedmem_unused
      b gen_io_r 

Z80_read_8_bankedmem_vdp:
     mov r1,#0xE70000
     orr r1,r1,#0xE0
     and r1,r1,r0
     cmp r1,#0xC00000
     bne Z80_read_8_bankedmem_lockup
Z80_read_8_bankedmem_vdp_decode:     
     and r1,r0,#0x1F
     ldr pc,[pc,r1,lsl#2]
     .word 0
     .word Z80_read_8_bankedmem_vdp_data_low
     .word Z80_read_8_bankedmem_vdp_data_high
     .word Z80_read_8_bankedmem_vdp_data_low
     .word Z80_read_8_bankedmem_vdp_data_high
     .word Z80_read_8_bankedmem_vdp_ctrl_low
     .word Z80_read_8_bankedmem_vdp_ctrl_high
     .word Z80_read_8_bankedmem_vdp_ctrl_low
     .word Z80_read_8_bankedmem_vdp_ctrl_high
     .word Z80_read_8_bankedmem_vdp_hvc_low
     .word Z80_read_8_bankedmem_vdp_hvc_high
     .word Z80_read_8_bankedmem_vdp_hvc_low
     .word Z80_read_8_bankedmem_vdp_hvc_high
     .word Z80_read_8_bankedmem_vdp_hvc_low
     .word Z80_read_8_bankedmem_vdp_hvc_high
     .word Z80_read_8_bankedmem_vdp_hvc_low
     .word Z80_read_8_bankedmem_vdp_hvc_high
     .word Z80_read_8_bankedmem_lockup
     .word Z80_read_8_bankedmem_lockup
     .word Z80_read_8_bankedmem_lockup
     .word Z80_read_8_bankedmem_lockup
     .word Z80_read_8_bankedmem_lockup
     .word Z80_read_8_bankedmem_lockup
     .word Z80_read_8_bankedmem_lockup
     .word Z80_read_8_bankedmem_lockup
     .word Z80_read_8_bankedmem_unused
     .word Z80_read_8_bankedmem_unused
     .word Z80_read_8_bankedmem_unused
     .word Z80_read_8_bankedmem_unused
     .word Z80_read_8_bankedmem_unused
     .word Z80_read_8_bankedmem_unused
     .word Z80_read_8_bankedmem_unused
     .word Z80_read_8_bankedmem_unused
     
Z80_read_8_bankedmem_vdp_data_high:   
     stmfd sp!,{lr}
     bl vdp_data_r
     ldmfd sp!,{lr}
     and r0,r0,#0xFF
     mov pc,lr
Z80_read_8_bankedmem_vdp_data_low: 
     stmfd sp!,{lr}
     bl vdp_data_r
     ldmfd sp!,{lr}
     mov r0,r0,lsr #8
     mov pc,lr

Z80_read_8_bankedmem_vdp_ctrl_low:
     stmfd sp!,{lr}
     bl vdp_ctrl_r
     ldmfd sp!,{lr}
     mov r0,r0,lsr#8
     orr r0,r0,#0xFC
     mov pc,lr
     
Z80_read_8_bankedmem_vdp_ctrl_high:
     stmfd sp!,{lr}
     bl vdp_ctrl_r
     ldmfd sp!,{lr}
     and r0,r0,#0xFF
     mov pc,lr
     
Z80_read_8_bankedmem_vdp_hvc_low:
     stmfd sp!,{lr}
     bl vdp_hvc_r
     ldmfd sp!,{lr}
     mov r0,r0,lsr#8
     mov pc,lr
Z80_read_8_bankedmem_vdp_hvc_high:
     stmfd sp!,{lr}
     bl vdp_hvc_r
     ldmfd sp!,{lr}
     and r0,r0,#0xFF
     mov pc,lr

Z80_vdp_r:
     and r1,r0,#0xFF
     cmp r1,#0x1F
     ble Z80_read_8_bankedmem_vdp_decode
Z80_vdp_r_lockup: 
      ;@ invalid
      stmfd sp!,{r0-r12,lr}
      ldr r1,Z80_vdp_r_lockup_error
      mov lr,pc
      ldr pc,illegal_memory_io_local
      ldmfd sp!,{r0-r12,pc} 
Z80_vdp_r_lockup_error: .word Z80_vdp_r_lockup_error_text
Z80_vdp_r_lockup_error_text: .ascii "Z80 vdp r lockup" 
      .align 4
      
Z80_read_8_bankedmem_ram:
     mov r0,#0xFF
     mov pc,lr     
      
DrMD_Z80_write_8:
     mov r2,r0
     mov r0,r1
     mov r1,r2
     and r2,r0,#0xE000
     ldr pc,[pc,r2,lsr#11]
     .word 0
     .word Z80_write_8_zram
     .word Z80_write_8_zram
     .word Z80_write_8_ym   
     .word Z80_write_8_zbank     
     .word Z80_write_8_bankedmem
     .word Z80_write_8_bankedmem
     .word Z80_write_8_bankedmem
     .word Z80_write_8_bankedmem

Z80_write_8_zram:
     ldr r2,drmd_context1
     ldr r3,[r2,#zram]
     mov r0,r0,lsl#19
     strb r1,[r3,r0,lsr#19]
     mov pc,lr
     
Z80_write_8_ym:
     and r0,r0,#0x3
     ldr r2,drmd_context1
     ldr pc,[r2,#fm_write]
     
Z80_write_8_zbank:
     and r2,r0,#0xFF00
     cmp r2,#0x6000
     andeq r0,r1,#1
     beq gen_bank_w
     cmp r2,#0x7F00
     beq Z80_vdp_w
     mov pc,lr

Z80_write_8_bankedmem:
     ldr r2,drmd_context1   
     ldr r3,[r2,#zbank]
     bic r0,r0,#0xF8000
     add r0,r0,r3
     mov r3,r0,lsr#21
     and r3,r3,#0x7
     ldr pc,[pc,r3,lsl#2]
     .word 0
     .word Z80_write_8_bankedmem_unused
     .word Z80_write_8_bankedmem_unused
     .word Z80_write_8_bankedmem_unused
     .word Z80_write_8_bankedmem_unused
     .word Z80_write_8_bankedmem_lockup
     .word Z80_write_8_bankedmem_z80
     .word Z80_write_8_bankedmem_vdp
     .word Z80_write_8_bankedmem_ram

Z80_write_8_bankedmem_unused:
      mov pc,lr   

Z80_write_8_bankedmem_lockup_error: .word Z80_write_8_bankedmem_lockup_error_text
Z80_write_8_bankedmem_lockup_error_text: .ascii "Z80 write 8 banked mem lockup" 
      .align 4
Z80_write_8_bankedmem_lockup:
      stmfd sp!,{r0-r12,lr}
      ldr r1,Z80_write_8_bankedmem_lockup_error
      mov lr,pc
      ldr pc,illegal_memory_io_local
      ldmfd sp!,{r0-r12,pc} 

Z80_write_8_bankedmem_z80:
      mov r2,#0xA10000
	  orr r3,r2,#0xFF00
      cmp r3,r0
      beq Z80_write_8_bankedmem_z80_io
	  orr r3,r2,#0x1100
	  cmp r3,r0
      beq Z80_write_8_bankedmem_z80_busreq
	  orr r3,r2,#0x1200
	  cmp r3,r0
      beq Z80_write_8_bankedmem_z80_reset
	  mov pc,lr

Z80_write_8_bankedmem_z80_io:
      mov r3,#0xA10000
      orr r3,r3,#0xD
      cmp r0,r3
      bgt Z80_write_8_bankedmem_unused
      b gen_io_w

Z80_write_8_bankedmem_z80_busreq:
      tst r0,#1
      bne Z80_write_8_bankedmem_unused
      and r0,r1,#1
      b gen_busreq_w

Z80_write_8_bankedmem_z80_reset:
      tst r0,#1
      bne Z80_write_8_bankedmem_unused
      and r0,r1,#1
      b gen_reset_w

Z80_vdp_w:
      and r3,r0,#0xFF
      cmp r3,#0x1F
      ble Z80_write_8_bankedmem_vdp_decode
Z80_vdp_w_lockup:
      ;@ invalid
      stmfd sp!,{r0-r12,lr}
      ldr r1,Z80_vdp_w_lockup_error
      mov lr,pc
      ldr pc,illegal_memory_io_local
      ldmfd sp!,{r0-r12,pc} 
Z80_vdp_w_lockup_error: .word Z80_vdp_w_lockup_error_text
Z80_vdp_w_lockup_error_text: .ascii "Z80 vdp w lockup" 
      .align 4
     
Z80_write_8_bankedmem_vdp:
      mov r3,#0xE70000
      orr r3,r3,#0xE0
      and r3,r3,r0      
      cmp r3,#0xC00000
      bne Z80_write_8_bankedmem_lockup
Z80_write_8_bankedmem_vdp_decode:
      and r3,r0,#0x1F
      ldr pc,[pc,r3,lsl#2]
      .word 0
      .word Z80_write_8_bankedmem_vdp_data
      .word Z80_write_8_bankedmem_vdp_data
      .word Z80_write_8_bankedmem_vdp_data
      .word Z80_write_8_bankedmem_vdp_data
      .word Z80_write_8_bankedmem_vdp_ctrl
      .word Z80_write_8_bankedmem_vdp_ctrl
      .word Z80_write_8_bankedmem_vdp_ctrl
      .word Z80_write_8_bankedmem_vdp_ctrl
      .word Z80_write_8_bankedmem_lockup
      .word Z80_write_8_bankedmem_lockup
      .word Z80_write_8_bankedmem_lockup
      .word Z80_write_8_bankedmem_lockup
      .word Z80_write_8_bankedmem_lockup
      .word Z80_write_8_bankedmem_lockup
      .word Z80_write_8_bankedmem_lockup
      .word Z80_write_8_bankedmem_lockup
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_vdp_psg
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_vdp_psg
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_vdp_psg
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_vdp_psg
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_unused
      .word Z80_write_8_bankedmem_unused

Z80_write_8_bankedmem_vdp_data:
      orr r0,r1,r1,lsl#8
      b vdp_data_w
      
Z80_write_8_bankedmem_vdp_ctrl:
      orr r0,r1,r1,lsl#8
      b vdp_ctrl_w

Z80_write_8_bankedmem_vdp_psg:
      ldr r2,drmd_context1   ;@ reload pointer incase of entry from Z80_vdp_w
      mov r0,r1
      ldr pc,[r2,#psg_write]
      
Z80_write_8_bankedmem_ram:
      ldr r3,[r2,#work_ram]
      mov r0,r0,lsl#16
      strb r1,[r3,r0,lsr#16]
      mov pc,lr

DrMD_Z80_read_16:
      stmfd sp!,{lr}
      stmfd sp!,{r0}
      bl DrMD_Z80_read_8
      ldmfd sp!,{r1}
      stmfd sp!,{r0}
      add r0,r1,#1
      bl DrMD_Z80_read_8
      ldmfd sp!,{r1}
      orr r0,r1,r0,lsl#8
      ldmfd sp!,{lr}
      mov pc,lr

DrMD_Z80_write_16:
      stmfd sp!,{lr}
      stmfd sp!,{r0,r1}
      and r0,r0,#0xFF
      bl DrMD_Z80_write_8
      ldmfd sp!,{r0,r1}
      add r1,r1,#1
      mov r0,r0,lsr#8
      bl DrMD_Z80_write_8
      ldmfd sp!,{lr}
      mov pc,lr
      
DrMD_Z80_In:
      mov r0,#0xFF
      mov pc,lr

DrMD_Z80_Out:
      mov pc,lr     

 
DrMD_Z80_Rebase_PC:
      ;@ r0 = pc to rebase
      and r1,r0,#0xF000
      ldr pc,[pc,r1,lsr#10]
      .word 0
      .word Z80_Rebase_PC_ram
      .word Z80_Rebase_PC_ram
      .word Z80_Rebase_PC_lockup
      .word Z80_Rebase_PC_lockup
      .word Z80_Rebase_PC_lockup
      .word Z80_Rebase_PC_lockup
      .word Z80_Rebase_PC_lockup
      .word Z80_Rebase_PC_lockup
      .word Z80_Rebase_PC_banked
      .word Z80_Rebase_PC_banked
      .word Z80_Rebase_PC_banked
      .word Z80_Rebase_PC_banked
      .word Z80_Rebase_PC_banked
      .word Z80_Rebase_PC_banked
      .word Z80_Rebase_PC_banked
      .word Z80_Rebase_PC_banked

Z80_Rebase_PC_ram:
      ldr r2,drmd_context1
      ldr r3,[r2,#z80_context]
      ldr r1,[r2,#zram]
      str r1,[r3,#7*4]  ;@  z80pc_base
      add r0,r1,r0
      str r0,[r3,#0*4]  ;@  z80pc
      mov pc,lr
      
Z80_Rebase_PC_lockup_error: .word Z80_Rebase_PC_lockup_error_text
Z80_Rebase_PC_lockup_error_text: .ascii "Z80 rebase PC lockup"  
      .align 4
Z80_Rebase_PC_lockup:
      ;@ invalid
      stmfd sp!,{r0-r12,lr}
      ldr r1,Z80_Rebase_PC_lockup_error
      mov lr,pc
      ldr pc,illegal_memory_io_local
      ldmfd sp!,{r0-r12,pc} 
      
Z80_Rebase_PC_banked: 
      ldr r2,drmd_context1
      ldr r1,[r2,#zbank]
      bic r3,r0,#0xF8000
      add r3,r3,r1
      mov r3,r3,lsr#21
      and r3,r3,#0x7
      ldr pc,[pc,r3,lsl#2]
      .word 0
      .word Z80_Rebase_PC_banked_rom
      .word Z80_Rebase_PC_banked_rom
      .word Z80_Rebase_PC_banked_lockup
      .word Z80_Rebase_PC_banked_lockup
      .word Z80_Rebase_PC_banked_lockup
      .word Z80_Rebase_PC_banked_lockup
      .word Z80_Rebase_PC_banked_lockup
      .word Z80_Rebase_PC_banked_lockup

Z80_Rebase_PC_banked_rom:
      ldr r3,[r2,#cart_rom]
      add r1,r1,r3  ;@ add zbank
      sub r1,r1,#0x8000
      ldr r3,[r2,#z80_context]
      str r1,[r3,#7*4]  ;@  z80pc_base
      add r0,r1,r0
      str r0,[r3,#0*4]  ;@  z80pc
      mov pc,lr

Z80_Rebase_PC_banked_lockup_error: .word Z80_Rebase_PC_banked_lockup_error_text
Z80_Rebase_PC_banked_lockup_error_text: .ascii "Z80 rebase PC into banked ram" 
      .align 4  
Z80_Rebase_PC_banked_lockup:
      ;@ invalid
      stmfd sp!,{r0-r12,lr}
      ldr r1,Z80_Rebase_PC_banked_lockup_error
      mov lr,pc
      ldr pc,illegal_memory_io_local
      ldmfd sp!,{r0-r12,pc}

      
DrMD_Z80_Rebase_SP:
      ;@ r0 = sp to rebase
      and r1,r0,#0xF000
      ldr pc,[pc,r1,lsr#10]
      .word 0
      .word Z80_Rebase_SP_ram
      .word Z80_Rebase_SP_ram
      .word Z80_Rebase_SP_ram
      .word Z80_Rebase_SP_ram
      .word Z80_Rebase_SP_lockup
      .word Z80_Rebase_SP_lockup
      .word Z80_Rebase_SP_lockup
      .word Z80_Rebase_SP_lockup
      .word Z80_Rebase_SP_banked
      .word Z80_Rebase_SP_banked
      .word Z80_Rebase_SP_banked
      .word Z80_Rebase_SP_banked
      .word Z80_Rebase_SP_banked
      .word Z80_Rebase_SP_banked
      .word Z80_Rebase_SP_banked
      .word Z80_Rebase_SP_banked

Z80_Rebase_SP_ram:
      ldr r2,drmd_context1
      ldr r3,[r2,#z80_context]
      ldr r1,[r2,#zram]
      str r1,[r3,#8*4]  ;@  z80sp_base
      add r0,r1,r0
      str r0,[r3,#6*4]  ;@  z80sp
      mov pc,lr
      
Z80_Rebase_SP_lockup_error: .word Z80_Rebase_SP_lockup_error_text
Z80_Rebase_SP_lockup_error_text: .ascii "Z80 rebase SP lockup"  
      .align 4       
Z80_Rebase_SP_lockup:
      stmfd sp!,{r0-r12,lr}
      ldr r1,Z80_Rebase_SP_lockup_error
      mov lr,pc
      ldr pc,illegal_memory_io_local
      ldmfd sp!,{r0-r12,pc}

illegal_memory_io_local: .word illegal_memory_io
Z80_Rebase_SP_banked_error: .word Z80_Rebase_SP_banked_error_text
Z80_Rebase_SP_banked_error_text: .ascii "Z80 rebase SP into banked ram"  
      .align 4        
Z80_Rebase_SP_banked: 
      ;@ it is not really possible to have
      ;@ stack in banked ram, because it can
      ;@ only be written no read, a program
      ;@ may use the stack pointer to load
      ;@ data though
      stmfd sp!,{r0-r12,lr}
      ldr r1,Z80_Rebase_SP_banked_error
      mov lr,pc
      ldr pc,illegal_memory_io_local
      ldmfd sp!,{r0-r12,pc}


      
