      .global md_render_16_small

;@ --------------------------- Defines ----------------------------
     .include "DrMD_core_vars.s"
    
screen_off:
      mov r2,r6,lsr#1
      tst r2,#1
      moveq pc,lr

      tst r0,#0x8
      moveq r1,#0x74
      movne r1,#0x78
      rsb r0,r2,r1
      ldr r1,[drmd,#frame_buffer]
      add r0,r1,r0,lsl#1
      mov r1,#0x0
      mov r2,#5
screen_off_loop:      
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      str r1,[r0],#480
      subs r2,r2,#1
      bne screen_off_loop
      mov pc,lr
      
;@  gp32_vid_mem: .word 0x0C7B4000 
md_render_16_small:
      ;@ r6 = line
      ;@ r5 = Pointer to DrMD context
      tst r6,#1
      movne pc,lr
      ;@ r0 = vdp_reg1 which is read at start of render line
      ldrb r0,[drmd,#vdp_reg1]
      tst r0,#0x40
      beq screen_off
      stmfd sp!,{r0-r12,lr}
      mov r2,r6,lsr#1
      tst r0,#0x8
      moveq r1,#0x74
      movne r1,#0x78
      rsb r0,r2,r1
      ldr r12,[drmd,#frame_buffer]
      add r12,r12,r0,lsl#1
      ldrb r0,[drmd,#vdp_reg12]
      tst r0,#1
      addeq r12,r12,#0x1E00
      str r12,gp32_curr_line
      moveq r0,#33
      movne r0,#41
      str r0,tile_count
      
      
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
      str r0,window_flag
      ;@ r0 = plane
      ;@ r4 = high
      ;@ r12 = pointer to gp32 vid mem
      
      ;@ plane B low
draw_layerB_low:
      mov r0,#1
      mov r4,#0
      bl draw_plane_nontrans
      
      ;@plane A low
      ldr r12,gp32_curr_line
      ldrh r6,[drmd,#vdp_line]
      ldr r0,window_flag
      tst r0,r0
      bne draw_window_low
draw_layerA_low:
      mov r0,#0
      mov r4,#0
      bl draw_plane
      b draw_layerA_low_done
draw_window_low: 
      mov r4,#0
      bl draw_window 
draw_layerA_low_done:

      ldrh r6,[drmd,#vdp_line]
      bl parse_sprites
     
      mov r4,#0
      ldrh r6,[drmd,#vdp_line]
      bl render_sprite
      
      ;@plane B high
draw_layerB_high:
      ldr r12,gp32_curr_line
      mov r0,#1
      mov r4,#1
      ldrh r6,[drmd,#vdp_line]
      bl draw_plane
      
      ldr r12,gp32_curr_line  ;@ reload gp32 mem pointer
      ldrh r6,[drmd,#vdp_line] ;@ reload line
      ldr r0,window_flag
      tst r0,r0
      bne draw_window_high
draw_layerA_high:
      mov r0,#0
      mov r4,#1
      bl draw_plane
      b draw_layerA_high_done
draw_window_high:
      mov r4,#1
      bl draw_window 
draw_layerA_high_done:   
      
      mov r4,#1
      ldrh r6,[drmd,#vdp_line]
      bl render_sprite
draw_layer_skiphigh:
      ldrh r0,[drmd,#vdp_line]
      mov r0,r0,lsr#1
      tst r0,#1
      beq render_line_end
      
      add lr,pc,#12
      ldrb r0,[drmd,#vdp_reg12]
      tst r0,#1
      beq mask_screen_256
      bne mask_screen_320
render_line_end:      
      ldmfd sp!,{r0-r12,lr}
      mov pc,lr
gp32_curr_line: .word 0 
tile_count: .word 0 
window_flag: .word 0  
mask_screen_320:
      ldr r12,gp32_curr_line
      sub r12,r12,#0x780
      mov r0,#0x0     
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      add r12,r12,#0x12000
      add r12,r12,#0x00C00     
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12]
      mov pc,lr

mask_screen_256:
      ldr r12,gp32_curr_line
      sub r12,r12,#0x1E00
      mov r0,#0x0     
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12]
      add r12,r12,#0xF000    
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12],#480
      str r0,[r12]
      mov pc,lr
      
draw_plane_nontrans:
      ldrb r8,[drmd,#vdp_reg16]
      mov r9,r8,lsr#4
      and r9,r9,#0x3   ;@ get height of map
      and r8,r8,#0x3   ;@ get width of map
      ldr r10,shift_table2
      ldrb r11,[r10,r8]
      mov r3,#1
      mov r3,r3,lsl r11   ;@ r11 = width shift val
      sub r3,r3,#1         ;@ xmask
      ldrb r9,[r10,r9]
      mov r7,#0x8
      mov r9,r7,lsl r9
      sub r9,r9,#1         ;@ ymask
      
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
      add r11,r11,#1
      add r0,r0,r7,lsl r11
      add r0,r0,r10  ;@ base ntab in vram
      
      /*
       before drawing line need to know
       r0 - nametab
       r1 - line
       r2 - hscroll
       r3 - xmask
       r4 - high - high or low tiles
       r12 = gp32 vid mem
      */
draw_tile_nontrans:
      ;@ r6 = tilex
      ;@ r7 = ty
      ;@ r8 = dx
      
      rsb r6,r2,#0     ;@  get first tile
      and r8,r6,#7
      mov r8,r8,lsr#1
      mov r7,#480
      mul r10,r7,r8
      sub r12,r12,r10
      mov r6,r6,lsr#3
      and r6,r6,r3
      
      and r7,r1,#7
      mov r7,r7,lsl#2
      
      ldr r2,[drmd,#vram] 

      ;@ tile 0   
      ldr r8,tile_count    
draw_tile_nontrans_loop:     
      add r10,r0,r6,lsl#1
      ldrh r9,[r10]
      mov r10,r9,lsr#15
      ;@ 7FF = tile number
      ;@ 8 lines, each line = 4 byte
      ;@ 32 bytes per tile
      ;@ 0000 0000 0000 0000 0pp0 0111 1111 1111
      mov r10,r9,lsl#21  ;@ get tile number 7FF
      mov r10,r10,lsr#16 ;@ shift tile left by 5 (x32)
      tst r9,#0x1000
      rsbne r11,r7,#28
      addne r10,r10,r11
      addeq r10,r10,r7
      ldr r10,[r2,r10]  ;@ get MD tile data
      tst r9,#0x800
      mov r9,r9,lsr#7
      and r9,r9,#0xC0
      ldr r11,[drmd,#local_pal]
      add r9,r11,r9
      bne draw_tile_nontrans_flip
draw_tile_nontrans_norm:
      and r11,r10,#0x0000F000
      ldr r11,[r9,r11,lsr#10]
      strh r11,[r12]
      add r12,r12,#480
          
      and r11,r10,#0x000000F0
      ldr r11,[r9,r11,lsr#2]
      strh r11,[r12]
      add r12,r12,#480
      
      and r11,r10,#0xF0000000
      ldr r11,[r9,r11,lsr#26]
      strh r11,[r12]
      add r12,r12,#480
          
      and r11,r10,#0x00F00000
      ldr r11,[r9,r11,lsr#18]
      strh r11,[r12]
      add r12,r12,#480
            
      add r6,r6,#1
      and r6,r6,r3
      subs r8,r8,#1   
      bne draw_tile_nontrans_loop
      mov pc,lr
draw_tile_nontrans_flip:
      and r11,r10,#0x000F0000
      ldr r11,[r9,r11,lsr#14]
      strh r11,[r12]
      add r12,r12,#480
      
      and r11,r10,#0x0F000000
      ldr r11,[r9,r11,lsr#22]
      strh r11,[r12]
      add r12,r12,#480
      
      and r11,r10,#0x0000000F
      ldr r11,[r9,r11,lsl#2]
      strh r11,[r12]
      add r12,r12,#480
      
      and r11,r10,#0x00000F00
      ldr r11,[r9,r11,lsr#6]
      strh r11,[r12]
      add r12,r12,#480
       
      add r6,r6,#1
      and r6,r6,r3
      subs r8,r8,#1   
      bne draw_tile_nontrans_loop
      mov pc,lr   
      
draw_plane:
      ldrb r8,[drmd,#vdp_reg16]
      mov r9,r8,lsr#4
      and r9,r9,#0x3   ;@ get height of map
      and r8,r8,#0x3   ;@ get width of map
      ldr r10,shift_table2
      ldrb r11,[r10,r8]
      mov r3,#1
      mov r3,r3,lsl r11   ;@ r11 = width shift val
      sub r3,r3,#1         ;@ xmask
      ldrb r9,[r10,r9]
      mov r7,#0x8
      mov r9,r7,lsl r9
      sub r9,r9,#1         ;@ ymask
      
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
      add r11,r11,#1
      add r0,r0,r7,lsl r11
      add r0,r0,r10  ;@ base ntab in vram
      
      /*
       before drawing line need to know
       r0 - nametab
       r1 - line
       r2 - hscroll
       r3 - xmask
       r4 - high - high or low tiles
       r12 = gp32 vid mem
      */
draw_tile:
      ;@ r6 = tilex
      ;@ r7 = ty
      ;@ r8 = dx
      
      rsb r6,r2,#0     ;@  get first tile
      and r8,r6,#7
      mov r8,r8,lsr#1
      mov r7,#480
      mul r10,r7,r8
      sub r12,r12,r10
      mov r6,r6,lsr#3
      and r6,r6,r3
      
      and r7,r1,#7
      mov r7,r7,lsl#2
      
      ldr r2,[drmd,#vram]
      ldr r8,tile_count         
draw_tile_loop: 
      add r10,r0,r6,lsl#1
      ldrh r9,[r10]
      tst r9,r9
      beq draw_tile_skip
      mov r10,r9,lsr#15
      cmp r10,r4
      bne draw_tile_skip 
      ;@ 7FF = tile number
      ;@ 8 lines, each line = 4 byte
      ;@ 32 bytes per tile
      ;@ 0000 0000 0000 0000 0pp0 0111 1111 1111
      mov r10,r9,lsl#21  ;@ get tile number 7FF
      mov r10,r10,lsr#16 ;@ shift tile left by 5 (x32)
      tst r9,#0x1000
      rsbne r11,r7,#28
      addne r10,r10,r11
      addeq r10,r10,r7
      ldr r10,[r2,r10]  ;@ get MD tile data
      tst r10,r10
      beq draw_tile_skip ;@  whole tile blank no skip
      tst r9,#0x800
      mov r9,r9,lsr#7
      and r9,r9,#0xC0
      ldr r11,[drmd,#local_pal]
      add r9,r11,r9
      bne draw_tile_flip
draw_tile_norm:
      ands r11,r10,#0x0000F000
      ldrne r11,[r9,r11,lsr#10]
      strneh r11,[r12]
           
      ands r11,r10,#0x000000F0
      ldrne r11,[r9,r11,lsr#2]
      addne r1,r12,#480
      strneh r11,[r1]
      
      ands r11,r10,#0xF0000000
      ldrne r11,[r9,r11,lsr#26]
      addne r1,r12,#960
      strneh r11,[r1]
      
      ands r11,r10,#0x00F00000
      ldrne r11,[r9,r11,lsr#18]
      addne r1,r12,#1440
      strneh r11,[r1]

draw_tile_skip:      
      add r12,r12,#1920
      
      add r6,r6,#1
      and r6,r6,r3
      subs r8,r8,#1   
      bne draw_tile_loop
      mov pc,lr
draw_tile_flip:
      ands r11,r10,#0x000F0000
      ldrne r11,[r9,r11,lsr#14]
      strneh r11,[r12]
      
      ands r11,r10,#0x0F000000
      ldrne r11,[r9,r11,lsr#22]
      addne r1,r12,#480
      strneh r11,[r1]
      
      ands r11,r10,#0x0000000F
      ldrne r11,[r9,r11,lsl#2]
      addne r1,r12,#960
      strneh r11,[r1]
      
      ands r11,r10,#0x00000F00
      ldrne r11,[r9,r11,lsr#6]
      addne r1,r12,#1440
      strneh r11,[r1]
      
      add r12,r12,#1920
      
      add r6,r6,#1
      and r6,r6,r3
      subs r8,r8,#1   
      bne draw_tile_loop
      mov pc,lr 
      
shift_table2: .word shift_table2_data
shift_table2_data: .byte 5,6,6,7    

draw_window:    
      ldrb r0,[drmd,#vdp_reg12]
      tst r0,#1
      beq draw_window_32cell
draw_window_40cell:
      ldrb r0,[drmd,#vdp_reg3]
      and r0,r0,#0x3C
      mov r0,r0,lsl#10
      mov r1,r6,lsr#3
      add r0,r0,r1,lsl#7
      b draw_window_setnametab_done
draw_window_32cell:
      ldrb r0,[drmd,#vdp_reg3]
      and r0,r0,#0x3E
      mov r0,r0,lsl#10
      mov r1,r6,lsr#3
      add r0,r0,r1,lsl#6
      add r12,r12,#0x1E00 ;@ centre window
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


parse_sprites:
     ;@ scan sprites to find any on current scan line
     ;@ and are actually being displayed
     ;@ this needs to run even if frame is skipped
     stmfd sp!,{r4-r12,lr}
     add r6,r6,#0x80
     ldrb r0,[drmd,#vdp_reg12]
     tst r0,#6        ;@  test for interlace mode
     movne lr,#1
     moveq lr,#0
     
     tst r0,#1
     movne r1,#0x14  ;@ set max sprites on vdp line
     moveq r1,#0x10
     str r1,sprite_limit
     mov r1,#0
     str r1,sprite_count
     movne r8,#80      ;@ set total sprites to scan
     moveq r8,#64
     mov r9,#0         ;@ reset loop count
     ldr r10,[drmd,#vram]
     ldrb r0,[drmd,#vdp_reg5]
     andne r0,r0,#0x7E
     andeq r0,r0,#0x7F
     add r10,r10,r0,lsl#9   ;@ get sprite base address
     mov r11,#0             ;@ reset sprite link to 0
     ldr r12,sprite_table   ;@ where to write sprite data to
     ;@ r5 = pointer to drmd context
     ;@ r6 = line
     ;@ r9 = current sprite count
     ;@ r10 = sprite data in vram pointer
     ;@ r11 = link  
     ;@ lr = ymask     
parse_sprites_main_loop:
     add r0,r10,r11,lsl#3
     ldrh r1,[r0] ;@ get y
     mov r1,r1,lsr lr   ;@ if in interlace mode
                        ;@ divide sprite y by 2
     bic r1,r1,#0xFE00 ;@ mask y 0x1FF 
     ;@ sub r1,r1,#0x80   ;@ sub sprite y offset     
     cmp r6,r1
     blt parse_sprites_next
     ldrb r2,[r0,#3]   ;@ get height and width info
     and r3,r2,#3
     add r3,r3,#1
     add r4,r1,r3,lsl#3
     cmp r6,r4
     bge parse_sprites_next
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
     strh r4,[r12],#2  ;@ save code (pri,pal,xflip,yflip)
     
     sub r1,r6,r1  ;@ row
     tst r4,#0x1000
     movne r7,r3,lsl#3
     subne r7,r7,#1
     subne r1,r7,r1  ;@ row flip y
     
     
     
     
     tst r4,#0x800   ;@ delta flip
     bic r4,r4,#0xF800  ;@ get tile
     mov r7,r1,lsr#3
     ;@ and r7,r7,#3
     add r4,r4,r7
     mulne r7,r3,r2
     addne r4,r4,r7
     rsbne r3,r3,#0
     
     
     mov r4,r4,lsl#5
     and r1,r1,#7
     add r4,r4,r1,lsl#2
         
     strh r4,[r12],#2  ;@ save tile
     mov r3,r3,lsl#5
     strh r3,[r12],#2  ;@ save delta
     strh r2,[r12],#2  ;@ save width
     ldrh r1,[r0,#6]
     bic r1,r1,#0xFC00
     sub r1,r1,#0x78
     strh r1,[r12],#8 ;@ save x

     ldr r1,sprite_count
     ldr r2,sprite_limit
     add r1,r1,#1
     str r1,sprite_count
     cmp r1,r2
     blt parse_sprites_next
     ldrb r0,[drmd,#vint_pending]
     tst r0,r0
     bne parse_sprites_end
     ldrh r0,[drmd,#vdp_status]
     orr r0,r0,#0x40
     strh r0,[drmd,#vdp_status]
     b parse_sprites_end
parse_sprites_next:  
     ldrh r11,[r0,#2]   
     ands r11,r11,#0x7F  ;@  get next sprite link
     beq parse_sprites_end  ;@  exit if sprite 0
     add r9,r9,#1
     cmp r9,r8      ;@ compare sprite count against total sprite
     blt parse_sprites_main_loop
parse_sprites_end:
     ldmfd sp!,{r4-r12,lr}
     mov pc,lr    
sprite_limit: .word 0     
sprite_count: .word 0
sprite_table: .word sprite_table_data     
sprite_table_data:
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
     .space 20*(8*2)

render_sprite:
      
      ldr r1,sprite_count
      tst r1,r1
      moveq pc,lr
      stmfd sp!,{lr}
      sub r1,r1,#1
      ldr r2,[drmd,#vram]  ;@ create pointer to vram
      
render_sprite_sprite_loop:
      ldr r10,sprite_table
      add r0,r10,r1,lsl#4   ;@ create pointer to sprite data
      ldr r12,gp32_curr_line  ;@ reset pointer to start of vdp line
      ldrh r6,[r0]       ;@ get code
      cmp r4,r6,lsr#15   ;@ check priority
      bne render_sprite_getnext
      ldrh r7,[r0,#6]   ;@ get width
      ldrh r8,[r0,#2] 
      ldrsh r3,[r0,#8]  ;@ get signed x pos
      mov r3,r3,lsr#1
render_sprite_sprite_dofirst:
      cmp r3,#0
      ble render_sprite_sprite_offscreen
      cmp r3,#7
      ble render_sprite_part ;@ do partial sprite
      mov r10,#480
      sub r9,r3,#8
      mul r11,r10,r9
      add r12,r12,r11  ;@ get start xpos
      mov r9,r6,lsr#7
      and r9,r9,#0xC0   ;@ get pal
      ldr r11,[drmd,#local_pal]
      add r9,r11,r9
      b render_sprite_tile_loop
render_sprite_part:
      and r10,r3,#7
      eor r10,r10,#7
      mov r11,#480
      mul r9,r11,r10
      sub r12,r12,r9
      mov r9,r6,lsr#7
      and r9,r9,#0xC0   ;@ get pal
      ldr r11,[drmd,#local_pal]
      add r9,r11,r9
render_sprite_tile_loop:      
      cmp r3,#0xA4
      bge render_sprite_getnext       
      ldr r10,[r2,r8] ;@ get tile data
      tst r10,r10
      beq render_sprite_next_tile ;@  whole tile blank no skip
      tst r6,#0x800
      bne render_sprite_flip    
render_sprite_norm:
      ands r11,r10,#0x0000F000
      ldrne r11,[r9,r11,lsr#10]
      strneh r11,[r12]
      
      ands r11,r10,#0x000000F0
      ldrne r11,[r9,r11,lsr#2]
      addne lr,r12,#480
      strneh r11,[lr]
      
      ands r11,r10,#0xF0000000
      ldrne r11,[r9,r11,lsr#26]
      addne lr,r12,#960
      strneh r11,[lr]
      
      ands r11,r10,#0x00F00000
      ldrne r11,[r9,r11,lsr#18]
      addne lr,r12,#1440
      strneh r11,[lr]
      
      b render_sprite_next_tile  
      
render_sprite_flip:
      ands r11,r10,#0x000F0000
      ldrne r11,[r9,r11,lsr#14]
      strneh r11,[r12]
      
      ands r11,r10,#0x0F000000
      ldrne r11,[r9,r11,lsr#22]
      addne lr,r12,#480
      strneh r11,[lr]
      
      ands r11,r10,#0x0000000F
      ldrne r11,[r9,r11,lsl#2]
      addne lr,r12,#960
      strneh r11,[lr]
      
      ands r11,r10,#0x00000F00
      ldrne r11,[r9,r11,lsr#6]
      addne lr,r12,#1440
      strneh r11,[lr]
   
render_sprite_next_tile:
      subs r7,r7,#1
      bmi render_sprite_getnext
      ldrsh r10,[r0,#4]  ;@ get delta
      add r8,r8,r10  ;@ add 
      bic r8,r8,#0x00FF0000
      bic r8,r8,#0xFF000000
      add r3,r3,#8
      add r12,r12,#1920
      b render_sprite_tile_loop
render_sprite_getnext:
      subs r1,r1,#1
      bpl render_sprite_sprite_loop 
      ldmfd sp!,{lr}
      mov pc,lr

render_sprite_sprite_offscreen:
      subs r7,r7,#1
      bmi render_sprite_getnext
      ldrsh r10,[r0,#4]  ;@ get delta
      add r8,r8,r10  ;@ add 
      bic r8,r8,#0x00FF0000
      bic r8,r8,#0xFF000000
      add r3,r3,#8
      b render_sprite_sprite_dofirst  
      
      
      