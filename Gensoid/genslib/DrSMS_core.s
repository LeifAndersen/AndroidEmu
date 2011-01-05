	  .DATA  ;@ hack to get code running under linux
	  
	  .equiv __GP32__,     0
	  .equiv __GP2X__,     1
	  .equiv __GIZ__,       0
	  
      .global DrSMSRun
      .global DrSMS_Z80_Rebase_PC
      .global DrSMS_Z80_Rebase_SP
      .global DrSMS_Rebase_Banks
      .global drsms_context
      .global init_sms_pal
      .global update_tile_cache
	  .global DrSMS_Z80_write_8
	  .global DrSMS_Z80_write_16
	  .global DrSMS_Z80_read_8
	  .global DrSMS_Z80_read_16
	  .global DrSMS_Z80_In
	  .global DrSMS_Z80_Out
      
	  .extern drsms
	  .extern pal_lookup
.if __GIZ__
	  .extern DrSMSBlit8To16
.endif	  
;@ --------------------------- Defines ----------------------------
    
     .include "DrSMS_core_vars.s"

sms_pal_lookup_local: .word pal_lookup
init_sms_pal: ;@ r0 = address of pal to update
    stmfd sp!,{r0-r12}
    mov r3,#0
    ;@ 0011 1001 1100 1111 0000 0000 0000 0000
    ;@ rrrr rggg ggbb bbbi
    ;@ 0x39CF
    ;@ ldr r4,=0x39CF
	;@ sms = bbggrr
     ldr r4,pal_decode
load_pal_loop:
    and r1,r3,#0x30
    ldrb r1,[r4,r1,lsr#4]
	strb r1,[r0],#1

    and r1,r3,#0xC
    ldrb r1,[r4,r1,lsr#2]
	strb r1,[r0],#1
	
	and r1,r3,#0x3
    ldrb r1,[r4,r1]
	strb r1,[r0],#2
	
    add r3,r3,#1
    cmp r3,#64
    ble load_pal_loop
    ldmfd sp!,{r0-r12}
    mov pc,lr 
pal_decode: .word pal_decode_data
pal_decode_data:	.byte 0x00  ;@ 00000000
							.byte 0x55  ;@ 01010101
							.byte 0xAA  ;@ 10101010
							.byte 0xFF  ;@ 11111111
							
.if __GIZ__
Blit8To16_local: .word DrSMSBlit8To16
update_pal:
    stmfd sp!,{r3-r6}
    ldr drsms,drsms_local1
	ldrb r0,[drsms,#gg_mode]
	tst r0,r0
	bne update_gg_pal
    mov r0,#0x20
    ldr r1,[drsms,#vdp_cram]
    ldr r2,[drsms,#local_pal]
    ldr r4,sms_pal_lookup_local
update_sms_pal_loop:
    ldrb r3,[r1],#1
    ldr r6,[r4,r3,lsl#2]
    str r6,[r2],#4
    subs r0,r0,#1
    bne update_sms_pal_loop
   
    ldmfd sp!,{r3-r6}
    mov pc,lr
	
update_gg_pal:
    mov r0,#0x20
    ldr r1,[drsms,#vdp_cram]
    ldr r2,[drsms,#local_pal]
    ldr r4,sms_pal_lookup_local
update_gg_pal_loop:
    ldrh r3,[r1],#2
	bic r3,r3,#0xF000
    ldr r6,[r4,r3,lsl#2]
    str r6,[r2],#4
    subs r0,r0,#1
    bne update_gg_pal_loop
   
    ldmfd sp!,{r3-r6}
    mov pc,lr
.endif

	
drsms_local1:	.word drsms

		 
DrSMS_Z80_Rebase_PC:  ;@ r0 = new pc
     ldr r1,drsms_local1
     mov r2,r0, lsr #12
     ldr r2,[r1,r2, lsl #2]
	 ldr r3,[r1,#z80_context]
     str r2,[r3,#7*4]			;@ PCBASE
     add r0,r0,r2
     str r0,[r3,#0*4]			;@ Z80PC
     mov pc,lr

DrSMS_Z80_Rebase_SP:  ;@ r0 = new sp
     ldr r1,drsms_local1
     mov r2,r0, lsr #12
     ldr r2,[r1,r2, lsl #2]
	 ldr r3,[r1,#z80_context]
     str r2,[r3,#8*4]			;@ SPBASE
     add r0,r0,r2
     str r0,[r3,#6*4]			;@ Z80SP
     mov pc,lr

DrSMS_Rebase_Banks:
     
     stmfd sp!,{r0-r12,lr}
     ldr drsms,drsms_local1
     ldrb r1,[drsms,#memory_reg_D]
     ldrb r2,[drsms,#max_rom_pages]
     and r1,r1,r2
     ldr r2,[drsms,#cart_rom]
     add r2,r2,r1,lsl #14
     mov r1,drsms
     mov r0,r2
     stmia r1!, {r0,r2}
     stmia r1!, {r0,r2}
     
     ldrb r1,[drsms,#memory_reg_E]
     ldrb r2,[drsms,#max_rom_pages]
     and r1,r1,r2
     ldr r2,[drsms,#cart_rom]
     add r2,r2,r1,lsl #14
     sub r2,r2,#0x4000
     add r1,drsms,#16
     mov r0,r2
     stmia r1!, {r0,r2}
     stmia r1!, {r0,r2}
     
     ldrb r1,[drsms,#memory_reg_C]
     tst r1,#1<<3
     beq 1f
     ;@ page in SRAM
     tst r1,#1<<2
     ldreq r2,[drsms,#sram1]
     ldrne r2,[drsms,#sram2]
     str r2,[drsms,#sram]
	 sub r2,r2,#0x8000
     add r1,drsms,#32
     mov r0,r2
     stmia r1!, {r0, r2}		;@ Store RAM bank address:
     stmia r1!, {r0, r2}		;@ Store RAM bank address  :
     ldr r0,sram_write_pointer
     ldr r2,check_mem_pagein_lookup_pointer
     str r0,[r2,#0x8*4]
     str r0,[r2,#0x9*4]
     str r0,[r2,#0xA*4]
     str r0,[r2,#0xB*4]
     ldr r0,page_in_rom2_disabled_pointer
     ldr r2,ram_write_reg_lookup_pointer
     str r0,[r2,#0x3*4]      
     b 2f
1:   ;@ page in rom 2
     ldr r0,rom_write_pointer
     ldr r2,check_mem_pagein_lookup_pointer
     str r0,[r2,#0x8*4]
     str r0,[r2,#0x9*4]
     str r0,[r2,#0xA*4]
     str r0,[r2,#0xB*4]
     ldr r0,page_in_rom2_pointer
     ldr r2,ram_write_reg_lookup_pointer
     str r0,[r2,#0x3*4]
     ldrb r1,[drsms,#memory_reg_F]
     ldrb r2,[drsms,#max_rom_pages]
     and r1,r1,r2
     ldr r2,[drsms,#cart_rom]
     add r2,r2,r1,lsl #14
     sub r2,r2,#0x8000
     add r1,drsms,#32
     mov r0,r2
     stmia r1!, {r0,r2}
     stmia r1!, {r0,r2}
2:
     ldr r0,[drsms,#zram]
	 sub r0,r0,#0xC000
     str r0,[drsms,#bankc]
     str r0,[drsms,#bankd]
     sub r0,r0,#0x2000
     str r0,[drsms,#banke]
     str r0,[drsms,#bankf]
     ldmfd sp!,{r0-r12,pc}
     
DrSMSExit:
     ldmfd sp!,{r4-r11,pc}
	 
skip_frame: .word 0     
DrSMSRun:
     ;@ reset vcount
     stmfd sp!,{r4-r11,lr}
     ;@ load cpu variables
     ldr drsms,drsms_local1
     str r0,skip_frame
     
     mvn r1,#0
     strh r1,[drsms,#vdp_line]
     ldrb r2,[drsms,#vdp_regA]
     strb r2,[drsms,#vdp_left]  
sms_line:
     ldrsh r1,[drsms,#vdp_line]
     add r1,r1,#1
     strh r1,[drsms,#vdp_line]
     ldrh r2,[drsms,#lines_per_frame]
     cmp r1,r2
     bgt DrSMSExit
     cmp r1,#0xC0
     beq vdp_int_vblank
     bgt z80_execute_start

;@ active display draw line of sms data
     ldr r0,skip_frame
     tst r0,r0
     beq 1f
     mov lr,pc
     ldr pc,[drsms,#render_line]
	 
.if __GIZ__	 
	 ;@ Test code to convert 8bit to 16bit line by line
	 bl update_pal
	 ldrh r0,[drsms,#vdp_line]
	 mov lr,pc
	 ldr pc,Blit8To16_local
	 ldr drsms,drsms_local1
.endif

1:

vdp_int_lineint:
     ldrb r2,[drsms,#vdp_left]
     subs r2,r2,#1
     strplb r2,[drsms,#vdp_left]
     bpl z80_execute_start
     ldrb r0,[drsms,#vdp_regA]
     strb r0,[drsms,#vdp_left]
     ldrb r0,[drsms,#vdp_reg0]
     tst r0,#0x10
     bne z80_set_irq_line
z80_execute_start:
     mov r1,#0xE3
	 ldr r0,[drsms,#z80_context]
	 mov lr,pc
	 ldr pc,[drsms,#z80_run]
	 
	 ldr drsms,drsms_local1
     b sms_line

vdp_int_vblank:
     ldrb r2,[drsms,#vdp_status]
     orr r2,r2,#1<<7
     strb r2,[drsms,#vdp_status]
     ldrb r0,[drsms,#vdp_reg1]
     tst r0,#0x20
     bne z80_set_irq_line 
     b z80_execute_start

z80_set_irq_line:
     mov r0,#1
	 
	 mov lr,pc
	 ldr pc,[drsms,#z80_set_irq]
	 
	 ldr drsms,drsms_local1
     b z80_execute_start

DrSMS_Z80_write_8:
      and r2,r1,#0xF000
      ldr pc,[pc,r2, lsr #10]
      .word 0x00
check_mem_pagein_lookup: 
                        .word rom_write ;@ 0
                        .word rom_write ;@ 1
                        .word rom_write ;@ 2
                        .word rom_write ;@ 3
                        .word rom_write ;@ 4
                        .word rom_write ;@ 5
                        .word rom_write ;@ 6
                        .word rom_write ;@ 7
                        .word rom_write ;@ 8
                        .word rom_write ;@ 9
                        .word rom_write ;@ A
                        .word rom_write ;@ B
                        .word ram_write ;@ C
                        .word ram_write ;@ D
                        .word ram_write_2 ;@ E
                        .word ram_write_reg ;@ F

rom_write:
      mov pc,lr

sram_write:
      ldr drsms,drsms_local1
      ldr r2,[drsms,#sram]
	  sub r2,r2,#0x8000
      strb r0,[r2,r1]
      mov pc,lr

ram_write:
      ldr drsms,drsms_local1
	  ldr r2,[drsms,#zram]
	  sub r2,r2,#0xC000
      strb r0,[r2,r1]
      mov pc,lr
	  
ram_write_2:
      ldr drsms,drsms_local1
      ldr r2,[drsms,#zram]
	  sub r2,r2,#0xE000
      strb r0,[r2,r1]
      mov pc,lr
	  
ram_write_reg:
      ldr drsms,drsms_local1
      ldr r2,[drsms,#zram]
	  sub r2,r2,#0xE000
      strb r0,[r2,r1]
      mov r2,#0xFF00
      orr r2,r2,#0xFC
      subs r1,r1,r2   ;@ r0 becomes 0 to 3
      movcc pc,lr  ;@ exit if less
      ldr pc,[pc,r1, lsl #2]
      .word 0x00
ram_write_reg_lookup:  .word page_in_sram ;@ C
                      .word page_in_rom0 ;@ D
                      .word page_in_rom1 ;@ E
                      .word page_in_rom2 ;@ F

page_in_rom0:
    ldrb r2,[drsms,#max_rom_pages]
    and r0, r0, r2		      ;@ Mask off first 5 bits:
    strb r0,[drsms,#memory_reg_D]
    ldr r2, [drsms,#cart_rom]	;@ Offset to ROM banks:
    add r2, r2, r0, lsl #14		;@ Select bank (r1 * 0x4000):
    mov r1, drsms         	;@ Apply offset drsms (0, 16, 32):
    mov r0, r2			;@ Copy of ROM bank address:
    stmia r1!, {r0, r2}		;@ Store ROM bank address:
    stmia r1!, {r0, r2}		;@ Store ROM bank address:
    mov pc, lr			;@ Exit:
    
page_in_rom1:
    ldrb r2,[drsms,#max_rom_pages]
    and r0, r0, r2		      ;@ Mask off first 5 bits
    strb r0,[drsms,#memory_reg_E]
    ldr r2, [drsms,#cart_rom]	;@ Offset to ROM banks
    add r2, r2, r0, lsl #14		;@ Select bank (r1 * 0x4000)
    sub r2, r2, #0x4000
    add r1, drsms, #16	;@ Apply offset drsms (0, 16, 32):
    mov r0, r2			;@ Copy of ROM bank address:
    stmia r1!, {r0, r2}		;@ Store ROM bank address:
    stmia r1!, {r0, r2}		;@ Store ROM bank address:
    mov pc, lr			;@ Exit:

page_in_sram:
    strb r0, [drsms,#memory_reg_C]
    ands r0,r0,#1<<3
    beq page_out_sram
    tst r0,#1<<2
    ldreq r2,[drsms,#sram1]
    ldrne r2,[drsms,#sram2]
    str r2,[drsms,#sram]
    add r1,drsms,#32
    mov r0,r2
    stmia r1!, {r0, r2}		;@ Store RAM bank address:
    stmia r1!, {r0, r2}		;@ Store RAM bank address  :
    ldr r0,sram_write_pointer
    ldr r2,check_mem_pagein_lookup_pointer
    str r0,[r2,#0x8*4]
    str r0,[r2,#0x9*4]
    str r0,[r2,#0xA*4]
    str r0,[r2,#0xB*4]
    ldr r0,page_in_rom2_disabled_pointer
    ldr r2,ram_write_reg_lookup_pointer
    str r0,[r2,#0x3*4]      
    mov pc,lr
;@ ##############################
;@ # Sram pagein variables
;@ ##############################
sram_write_pointer:              .word sram_write
rom_write_pointer:               .word rom_write
check_mem_pagein_lookup_pointer: .word check_mem_pagein_lookup
page_in_rom2_pointer:            .word page_in_rom2
page_in_rom2_disabled_pointer:   .word page_in_rom2_disabled
ram_write_reg_lookup_pointer:    .word ram_write_reg_lookup
;@ ##############################
page_out_sram:
      ldr r0,rom_write_pointer
      ldr r2,check_mem_pagein_lookup_pointer
      str r0,[r2,#0x8*4]
      str r0,[r2,#0x9*4]
      str r0,[r2,#0xA*4]
      str r0,[r2,#0xB*4]
      ldr r0,page_in_rom2_pointer
      ldr r2,ram_write_reg_lookup_pointer
      str r0,[r2,#0x3*4]
      ldrb r0,[drsms,#memory_reg_F]
page_in_rom2:
      ldrb r2,[drsms,#max_rom_pages]
      and r0, r0, r2		      ;@ Mask off first 5 bits:
      strb r0,[drsms,#memory_reg_F]
      ldr r2, [drsms,#cart_rom]	;@ Offset to ROM banks:
      add r2, r2, r0, lsl #14		;@ Select bank (r1 * 0x4000):
      sub r2,r2,#0x8000
      add r1, drsms, #32	;@ Apply offset drsms (0, 16, 32):
      mov r0, r2			;@ Copy of ROM bank address
      stmia r1!, {r0, r2}		;@ Store ROM bank address:
      stmia r1!, {r0, r2}		;@ Store ROM bank address:
      mov pc, lr
page_in_rom2_disabled:
      strb r0,[drsms,#memory_reg_F]
      mov pc,lr 

DrSMS_Z80_read_8:
      ldr drsms,drsms_local1
	  mov r1,r0,lsr#12
	  ldr r2,[drsms,r1,lsl#2]
	  ldrb r0,[r2,r0]
	  mov pc,lr
	  
DrSMS_Z80_read_16:
      ldr drsms,drsms_local1
	  mov r1,r0,lsr#12
	  ldr r2,[drsms,r1,lsl#2]
	  ldrb r1,[r2,r0]
	  add r0,r0,#1
	  ldrb r0,[r2,r0]
	  orr r0,r1,r0,lsl#8
	  mov pc,lr

DrSMS_Z80_write_16:
      stmfd sp!,{r0,r1,lr}
	  and r0,r0,#0xFF
	  bl DrSMS_Z80_write_8
	  ldmfd sp!,{r0,r1,lr}
	  add r1,r1,#1
	  mov r0,r0,lsr#8
	  b DrSMS_Z80_write_8

;@ ##################################
;@ ##################################
;@ ###  Input and Output Functions   ############
;@ ##################################
;@ ##################################
DrSMS_Z80_In:   ;@ r0 = port to be read
     and r0,r0,#0xFF 
     ldr pc,[pc,r0, lsl #2]
z80_in_lookup: .word 0
	.word z80_in_0,   z80_in_1,     z80_in_00_05,z80_in_00_05,z80_in_00_05,z80_in_00_05,z80_in_00_3F,z80_in_00_3F ;@ 0
	.word z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F ;@ 0
	.word z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F ;@ 1
	.word z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F ;@ 1
	.word z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F ;@ 2
	.word z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F ;@ 2
	.word z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F ;@ 3
	.word z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F,z80_in_00_3F ;@ 3
	.word z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127                 ;@ 4
	.word z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127                 ;@ 4
	.word z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127                 ;@ 5
	.word z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127                 ;@ 5
	.word z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127                 ;@ 6
	.word z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127                 ;@ 6
	.word z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127                 ;@ 7
	.word z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127,z80_in_126,z80_in_127                 ;@ 7
	.word z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191                 ;@ 8
	.word z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191                 ;@ 8
	.word z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191                 ;@ 9
	.word z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191                 ;@ 9
	.word z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191                 ;@ A
	.word z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191                 ;@ A
	.word z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191                 ;@ B
	.word z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191,z80_in_190,z80_in_191                 ;@ B
	.word z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221                 ;@ C
	.word z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221                 ;@ C
	.word z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221                 ;@ D
	.word z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221                 ;@ D
	.word z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221                 ;@ E
	.word z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221                 ;@ E
	.word z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221                 ;@ F
	.word z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221,z80_in_220,z80_in_221                 ;@ F

z80_in_00_05:	     
     eor r0,r0,r0
     mov pc,lr
     
z80_in_00_3F:
     ldrb r0,[r6,#-1]
     mov pc,lr

z80_in_0:
    ldr drsms,drsms_local1
    ldr r0,[drsms,#pad]
	and r0,r0,#0x80  ;@ mask of PAUSE flag
	eor r0,r0,#0xFF
    mov pc,lr
z80_in_1:
     ldr drsms,drsms_local1
     ldrb r0,[drsms,#sms_port1]
     mov pc,lr

z80_in_126:
     ldr drsms,drsms_local1
     ldrh r0,[drsms,#vdp_line]
     cmp r0,#0xFF
     movgt r0,#0xFF
     mov pc,lr
z80_in_127:
     ldr drsms,drsms_local1
     eor r0,r4,#0xFF
     mov pc,lr
z80_in_190:
     ldr drsms,drsms_local1
     eor r0,r0,r0
     strb r0,[drsms,#vdp_pending]
     ldr r2,[drsms,#vdp_memory]
     ldrh r0,[drsms,#vdp_addr]
     ldrb r1,[r2,r0]
     add r0,r0,#1
     bic r0,r0,#0xFC000
     strh r0,[drsms,#vdp_addr]
     ldrb r0,[drsms,#vdp_buffer]
     strb r1,[drsms,#vdp_buffer]
     mov pc,lr
z80_in_191:
     ldr drsms,drsms_local1
     ldrb r0,[drsms,#vdp_status]
     orr r0,r0,#1<<5   ;@ set sprite collision flag
	 stmfd sp!,{r0,lr}
     eor r0,r0,r0
     strb r0,[drsms,#vdp_pending]
     strb r0,[drsms,#vdp_status]

	 ;@ lower irq
	 mov lr,pc
	 ldr pc,[drsms,#z80_set_irq]
	 
     ldmfd sp!,{r0,pc}
	 
z80_in_220:
     ldr drsms,drsms_local1
     ldr r0,[drsms,#pad]
	 and r0,r0,#0x3F  ;@ mask of PAUSE flag
	 eor r0,r0,#0xFF
     mov pc,lr

z80_in_221:
     ldr drsms,drsms_local1
     ldrb r0,[drsms,#sms_port63]
     and r0,r0,#0xC0
     orr r0,r0,#0x3F
     mov pc,lr

    ;@ address port = 0xBF = 191
    ;@ data port = 0xBE = 190
    ;@ set address first
DrSMS_Z80_Out:  ;@r0=port   ;@r1=data
     and r0,r0,#0xFF 
	 and r1,r1,#0xFF 
     ldr pc,[pc,r0, lsl #2]
z80_out_lookup: .word 0
			.word z80_out_nothing,z80_out_1,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63 ;@ 0
			.word z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63 ;@ 0
	       .word z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63 ;@ 1
	       .word z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63 ;@ 1
	       .word z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63 ;@ 2
	       .word z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63 ;@ 2
	       .word z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63 ;@ 3
	       .word z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63,z80_out_nothing,z80_out_63 ;@ 3
	       .word z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127             ;@ 4
	       .word z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127             ;@ 4
	       .word z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127             ;@ 5
	       .word z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127             ;@ 5
	       .word z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127             ;@ 6
	       .word z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127             ;@ 6
	       .word z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127             ;@ 7
	       .word z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127,z80_out_127             ;@ 7
	       .word z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191             ;@ 8
	       .word z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191             ;@ 8
	       .word z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191             ;@ 9
	       .word z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191             ;@ 9
	       .word z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191             ;@ A
	       .word z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191             ;@ A
	       .word z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191             ;@ B
	       .word z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191,z80_out_190,z80_out_191             ;@ B
	       .word z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing ;@ C
	       .word z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing ;@ C
	       .word z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing ;@ D
	       .word z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing ;@ D
	       .word z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing ;@ E
	       .word z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing ;@ E
	       .word z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing ;@ F
	       .word z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing,z80_out_nothing ;@ F
	       
z80_out_nothing:
     mov pc,lr

z80_out_1:
    ldr drsms,drsms_local1
    strb r1,[drsms,#sms_port1]
    mov pc,lr
    
z80_out_63:
    ldr drsms,drsms_local1
    and r0,r1,#0x80
    and r1,r1,#0x20
    orr r0,r0,r1, lsl #1
    and r0,r0,#0xC0
    ldrb r1,[drsms,#drsms_country]
    tst r1,r1
    eorne r0,r0,#0xC0
    strb r0,[drsms,#sms_port63]
    mov pc,lr

            
z80_out_127:
    ldr drsms,drsms_local1
    mov r0,r1
    ldr pc,[drsms,#psg_write]

   
z80_out_190:
    ldr drsms,drsms_local1
    eor r0,r0,r0
    strb r0,[drsms,#vdp_pending]
    ldrb r0,[drsms,#vdp_type]
    ldr pc,[pc,r0, lsl #2]
z80_out_190_jumptable:   .word 0x00
                        .word z80_out_190_writevram
                        .word z80_out_190_writevram
                        .word z80_out_190_writevram
                        .word z80_out_190_writecram 

z80_out_190_writecram:
    ldrb r2,[drsms,#gg_mode]
	tst r2,r2
	bne z80_out_190_writecram_gg
z80_out_190_writecram_sms:
    ldrh r2,[drsms,#vdp_addr]
    add r0,r2,#1
    bic r0,r0,#0xFC000 
    strh r0,[drsms,#vdp_addr]
    and r2,r2,#0x1F
    ldr r0,[drsms,#vdp_cram]
    strb r1,[r0,r2]
    mov pc,lr 

z80_out_190_writecram_gg:
    ldrh r2,[drsms,#vdp_addr]
    add r0,r2,#1
    bic r0,r0,#0xFC000 
    strh r0,[drsms,#vdp_addr]
    tst r2,#1
    streqb r1,[drsms,#cram_gg_latch]
    moveq pc,lr
    ldrb r0,[drsms,#cram_gg_latch]
    orr r1,r0,r1, lsl #8
    and r2,r2,#0x3E
    ldr r0,[drsms,#vdp_cram]
    strh r1,[r0,r2]
	;@ now do conversion to native pal
	bic r1,r1,#0xF000
	ldr r0,pal_lookup_local
	ldr r1,[r0,r1,lsl#2]
	ldr r0,[drsms,#local_pal]
	str r1,[r0,r2,lsl#1]
    mov pc,lr 
	
pal_lookup_local: .word pal_lookup
z80_out_190_writevram:
	
    stmfd sp!,{r3,r4,r5}
    strb r1,[drsms,#vdp_buffer]
    ldr r5,[drsms,#vdp_memory]
    ldrh r2,[drsms,#vdp_addr]
    ldrb r0,[r5,r2]
    add r4,r2,#1
    bic r4,r4,#0xFC000
    strh r4,[drsms,#vdp_addr]
    cmp r0,r1
    ldmeqfd sp!,{r3,r4,r5}
    moveq pc,lr
    strb r1,[r5,r2]

    ;@ tile is different so update tile cache
    and r0,r2,#3 ;@ get the shift amount
    bic r2,r2,#3 ;@ get start of tile line
    ldr r5,[drsms,#tile_cache]
    add r2,r5,r2 
    ldr r5,tile_decode  ;@ holds tile data patterns
    ldr r1,[r5,r1,lsl #2]
    ldr r4,bit_mask
    ldr r5,[r2]
    bic r5,r5,r4,lsl r0
    orr r5,r5,r1,lsl r0
    str r5,[r2]
    ldmfd sp!,{r3,r4,r5}
    mov pc,lr

update_tile_cache:
    stmfd sp!,{r0-r12,lr}
    ldr drsms,drsms_local1
    ldr r2,[drsms,#tile_cache]
    ldr r4,bit_mask
    ldr r7,[drsms,#vdp_memory]
	ldr r3,tile_decode
    add r8,r7,#0x4000
1:
    mov r6,#0
    
    ;@ pixel 1
    ldrb r1,[r7],#1
    ldr r1,[r3,r1,lsl#2]
    orr r6,r6,r1
    
    ;@ pixel 2
    ldrb r1,[r7],#1
    ldr r1,[r3,r1,lsl#2]
    orr r6,r6,r1,lsl#1
    
    ;@ pixel 3
    ldrb r1,[r7],#1
    ldr r1,[r3,r1,lsl#2]
    orr r6,r6,r1,lsl#2
    
    ;@ pixel 4
    ldrb r1,[r7],#1
    ldr r1,[r3,r1,lsl#2]
    orr r6,r6,r1,lsl#3
    
    str r6,[r2],#4
    
    cmp r7,r8
    blt 1b

    ldmfd sp!,{r0-r12,pc}
    
bit_mask: .word 0x11111111
tile_decode: .word tile_decode_data
tile_decode_data:   .word 0x00000000,0x10000000,0x01000000,0x11000000,0x00100000
   .word 0x10100000,0x01100000,0x11100000,0x00010000,0x10010000
   .word 0x01010000,0x11010000,0x00110000,0x10110000,0x01110000
   .word 0x11110000,0x00001000,0x10001000,0x01001000,0x11001000
   .word 0x00101000,0x10101000,0x01101000,0x11101000,0x00011000
   .word 0x10011000,0x01011000,0x11011000,0x00111000,0x10111000
   .word 0x01111000,0x11111000,0x00000100,0x10000100,0x01000100
   .word 0x11000100,0x00100100,0x10100100,0x01100100,0x11100100
   .word 0x00010100,0x10010100,0x01010100,0x11010100,0x00110100
   .word 0x10110100,0x01110100,0x11110100,0x00001100,0x10001100
   .word 0x01001100,0x11001100,0x00101100,0x10101100,0x01101100
   .word 0x11101100,0x00011100,0x10011100,0x01011100,0x11011100
   .word 0x00111100,0x10111100,0x01111100,0x11111100,0x00000010
   .word 0x10000010,0x01000010,0x11000010,0x00100010,0x10100010
   .word 0x01100010,0x11100010,0x00010010,0x10010010,0x01010010
   .word 0x11010010,0x00110010,0x10110010,0x01110010,0x11110010
   .word 0x00001010,0x10001010,0x01001010,0x11001010,0x00101010
   .word 0x10101010,0x01101010,0x11101010,0x00011010,0x10011010
   .word 0x01011010,0x11011010,0x00111010,0x10111010,0x01111010
   .word 0x11111010,0x00000110,0x10000110,0x01000110,0x11000110
   .word 0x00100110,0x10100110,0x01100110,0x11100110,0x00010110
   .word 0x10010110,0x01010110,0x11010110,0x00110110,0x10110110
   .word 0x01110110,0x11110110,0x00001110,0x10001110,0x01001110
   .word 0x11001110,0x00101110,0x10101110,0x01101110,0x11101110
   .word 0x00011110,0x10011110,0x01011110,0x11011110,0x00111110
   .word 0x10111110,0x01111110,0x11111110,0x00000001,0x10000001
   .word 0x01000001,0x11000001,0x00100001,0x10100001,0x01100001
   .word 0x11100001,0x00010001,0x10010001,0x01010001,0x11010001
   .word 0x00110001,0x10110001,0x01110001,0x11110001,0x00001001
   .word 0x10001001,0x01001001,0x11001001,0x00101001,0x10101001
   .word 0x01101001,0x11101001,0x00011001,0x10011001,0x01011001
   .word 0x11011001,0x00111001,0x10111001,0x01111001,0x11111001
   .word 0x00000101,0x10000101,0x01000101,0x11000101,0x00100101
   .word 0x10100101,0x01100101,0x11100101,0x00010101,0x10010101
   .word 0x01010101,0x11010101,0x00110101,0x10110101,0x01110101
   .word 0x11110101,0x00001101,0x10001101,0x01001101,0x11001101
   .word 0x00101101,0x10101101,0x01101101,0x11101101,0x00011101
   .word 0x10011101,0x01011101,0x11011101,0x00111101,0x10111101
   .word 0x01111101,0x11111101,0x00000011,0x10000011,0x01000011
   .word 0x11000011,0x00100011,0x10100011,0x01100011,0x11100011
   .word 0x00010011,0x10010011,0x01010011,0x11010011,0x00110011
   .word 0x10110011,0x01110011,0x11110011,0x00001011,0x10001011
   .word 0x01001011,0x11001011,0x00101011,0x10101011,0x01101011
   .word 0x11101011,0x00011011,0x10011011,0x01011011,0x11011011
   .word 0x00111011,0x10111011,0x01111011,0x11111011,0x00000111
   .word 0x10000111,0x01000111,0x11000111,0x00100111,0x10100111
   .word 0x01100111,0x11100111,0x00010111,0x10010111,0x01010111
   .word 0x11010111,0x00110111,0x10110111,0x01110111,0x11110111
   .word 0x00001111,0x10001111,0x01001111,0x11001111,0x00101111
   .word 0x10101111,0x01101111,0x11101111,0x00011111,0x10011111
   .word 0x01011111,0x11011111,0x00111111,0x10111111,0x01111111
   .word 0x11111111
   
drsms_local2: .word drsms
z80_out_191:
    ldr drsms,drsms_local2
    ldrb r0,[drsms,#vdp_pending]
    tst r0,r0
    bne z80_out_191_byte2
z80_out_191_byte1 :
    orr r0,r0,#1
    strb r0,[drsms,#vdp_pending]   
    strb r1,[drsms,#first_byte]    
    mov pc,lr
z80_out_191_byte2:
    eor r0,r0,r0
    strb r0,[drsms,#vdp_pending] 
    mov r0,r1, lsr #6
    and r0,r0,#3
    ldrb r2,[drsms,#first_byte]   
    orr r1,r2,r1, lsl #8 
    ldr pc,[pc,r0, lsl #2]
z80_out_191_jumptable: .word 0x00
                      .word z80_out_191_vram
                      .word z80_out_191_vram
                      .word z80_out_191_reg
                      .word z80_out_191_pallet
z80_out_191_vram:
    eor r2,r2,r2
    strb r2,[drsms,#vdp_type]
    bic r1,r1,#0xFC000 ;@ keep vdp addr is range
    ;@ need vdp memory base address
    tst r0,r0
    strneh r1,[drsms,#vdp_addr]
    movne pc,lr 
    ldr r2,[drsms,#vdp_memory]
    ldrb r2,[r2,r1]
    strb r2,[drsms,#vdp_buffer]
    add r1,r1,#1
    bic r1,r1,#0xFC000
    strh r1,[drsms,#vdp_addr]
    mov pc,lr
z80_out_191_pallet:
    strb r0,[drsms,#vdp_type]
    bic r1,r1,#0xFC000 ;@ keep vdp addr is range
    strh r1,[drsms,#vdp_addr]
    mov pc,lr
z80_out_191_reg  :
    eor r0,r0,r0
    strb r0,[drsms,#vdp_type]
    and r1,r1,#0xF<<8
    add r0,drsms,#vdp_reg0
    strb r2,[r0,r1, lsr #8]
    ldr pc,[pc,r1, lsr #6]
reg_lookup: .word 0x00000000
    .word z80_out_191_reg_0
    .word z80_out_191_reg_1
    .word z80_out_191_reg_2
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none
    .word z80_out_191_reg_5
    .word z80_out_191_reg_6
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none
    .word z80_out_191_reg_none  

z80_out_191_reg_none:
    mov pc,lr

z80_out_191_reg_0:
    tst r2,#1<<4
    movne pc,lr
	mov r0,#0
	ldr pc,[drsms,#z80_set_irq]
    
z80_out_191_reg_1:
    tst r2,#1<<5
	movne pc,lr
	mov r0,#0
	ldr pc,[drsms,#z80_set_irq]
    
z80_out_191_reg_2:
    mov r0,#0x3800
    and r0,r0,r2,lsl #10
    strh r0,[drsms,#map_addr]
    mov pc,lr

z80_out_191_reg_5:
    mov r0,#0x3F00
    and r0,r0,r2,lsl #7
    strh r0,[drsms,#sprite_addr]
    mov pc,lr

z80_out_191_reg_6:
    mov r2,r2,lsl #11
    and r2,r2,#0x2000
    strh r2,[drsms,#sprite_tiles]
    mov pc,lr



      

     
     