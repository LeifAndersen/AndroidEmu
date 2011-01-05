
      .global RenderSound
      
      .extern PSG
      
      psg .req r8

;@ ################################
;@ psg context
;@ ################################   
      .equiv psg_sn_volume0,                  0  
      .equiv psg_sn_volume1,                  psg_sn_volume0+4
      .equiv psg_sn_volume2,                  psg_sn_volume1+4 
      .equiv psg_sn_volume3,                  psg_sn_volume2+4 
      .equiv psg_sn_count0,                   psg_sn_volume3+4
      .equiv psg_sn_count1,                   psg_sn_count0+4
      .equiv psg_sn_count2,                   psg_sn_count1+4
      .equiv psg_sn_count3,                   psg_sn_count2+4
      .equiv psg_sn_output0,                  psg_sn_count3+4
      .equiv psg_sn_output1,                  psg_sn_output0+4
      .equiv psg_sn_output2,                  psg_sn_output1+4
      .equiv psg_sn_output3,                  psg_sn_output2+4
      .equiv psg_sn_period0,                  psg_sn_output3+4
      .equiv psg_sn_period1,                  psg_sn_period0+4
      .equiv psg_sn_period2,                  psg_sn_period1+4
      .equiv psg_sn_period3,                  psg_sn_period2+4
      .equiv psg_sn_register0,                psg_sn_period3+4
      .equiv psg_sn_register1,                psg_sn_register0+4
      .equiv psg_sn_register2,                psg_sn_register1+4
      .equiv psg_sn_register3,                psg_sn_register2+4
      .equiv psg_sn_register4,                psg_sn_register3+4
      .equiv psg_sn_register5,                psg_sn_register4+4
      .equiv psg_sn_register6,                psg_sn_register5+4
      .equiv psg_sn_register7,                psg_sn_register6+4
      .equiv psg_sn_voltable0,                psg_sn_register7+4
      .equiv psg_sn_voltable1,                psg_sn_voltable0+4
      .equiv psg_sn_voltable2,                psg_sn_voltable1+4
      .equiv psg_sn_voltable3,                psg_sn_voltable2+4
      .equiv psg_sn_voltable4,                psg_sn_voltable3+4
      .equiv psg_sn_voltable5,                psg_sn_voltable4+4
      .equiv psg_sn_voltable6,                psg_sn_voltable5+4
      .equiv psg_sn_voltable7,                psg_sn_voltable6+4
      .equiv psg_sn_voltable8,                psg_sn_voltable7+4
      .equiv psg_sn_voltable9,                psg_sn_voltable8+4
      .equiv psg_sn_voltable10,               psg_sn_voltable9+4
      .equiv psg_sn_voltable11,               psg_sn_voltable10+4
      .equiv psg_sn_voltable12,               psg_sn_voltable11+4
      .equiv psg_sn_voltable13,               psg_sn_voltable12+4
      .equiv psg_sn_voltable14,               psg_sn_voltable13+4
      .equiv psg_sn_voltable15,               psg_sn_voltable14+4
      .equiv psg_sn_lastregister,             psg_sn_voltable15+4
      .equiv psg_sn_noisefb,                  psg_sn_lastregister+4
      .equiv psg_sn_rng,                      psg_sn_noisefb+4

     
psg_local: .word PSG

RenderSound:
      stmfd sp!,{r4-r11,lr}
      mov r10,r0         ;@ buffer_curr_pos
      add r9,r0,r1,lsl#2 ;@ buffer_max_pos
      ldr psg,psg_local  ;@ r8 = psg pointer
    
      ldr r0,[psg,#psg_sn_volume0]
      tst r0,r0
      bne 1f
      ldr r0,[psg,#psg_sn_count0]
      cmp r0,r1,lsl#16
      bgt 1f
      add r0,r0,r1,lsl#16
      str r0,[psg,#psg_sn_count0]
1:
      ldr r0,[psg,#psg_sn_volume1]
      tst r0,r0
      bne 1f
      ldr r0,[psg,#psg_sn_count1]
      cmp r0,r1,lsl#16
      bgt 1f
      add r0,r0,r1,lsl#16
      str r0,[psg,#psg_sn_count1]
1:
      ldr r0,[psg,#psg_sn_volume2]
      tst r0,r0
      bne 1f
      ldr r0,[psg,#psg_sn_count2]
      cmp r0,r1,lsl#16
      bgt 1f
      add r0,r0,r1,lsl#16
      str r0,[psg,#psg_sn_count2]
1:
      ldr r0,[psg,#psg_sn_volume3]
      tst r0,r0
      bne 1f
      ldr r0,[psg,#psg_sn_count3]
      cmp r0,r1,lsl#16
      bgt 1f
      add r0,r0,r1,lsl#16
      str r0,[psg,#psg_sn_count3]
1:

RenderSound_loop:
            
      eor r11,r11,r11
      ;@ get PSG channel 0 sample
      ldrb r0,[psg,#psg_sn_output0]
      ldr r1,[psg,#psg_sn_count0]
      tst r0,r0
      eoreq r4,r4,r4
      movne r4,r1
      sub r1,r1,#0x10000
      cmp r1,#0
      bgt 1f
      ldr r2,[psg,#psg_sn_period0]
3:
      add r1,r1,r2
      cmp r1,#0
      bgt 2f
      add r1,r1,r2
      add r4,r4,r2
      cmp r1,#0
      ble 3b
      b 1f
2:
      eors r0,r0,#1
      addne r4,r4,r2
      strb r0,[psg,#psg_sn_output0]
      
1:        
      str r1,[psg,#psg_sn_count0]      
      tst r0,r0
      subne r4,r4,r1
      ldr r2,[psg,#psg_sn_volume0]
      mul r0,r4,r2
      add r11,r11,r0,asr#16
      
      ;@ get PSG channel 1 sample
      ldrb r0,[psg,#psg_sn_output1]
      ldr r1,[psg,#psg_sn_count1]
      tst r0,r0
      eoreq r4,r4,r4
      movne r4,r1
      sub r1,r1,#0x10000
      cmp r1,#0
      bgt 1f
      ldr r2,[psg,#psg_sn_period1]
3:
      add r1,r1,r2
      cmp r1,#0
      bgt 2f
      add r1,r1,r2
      add r4,r4,r2
      cmp r1,#0
      ble 3b
      b 1f
2:
      eors r0,r0,#1
      addne r4,r4,r2
      strb r0,[psg,#psg_sn_output1]
      
1:        
      
      str r1,[psg,#psg_sn_count1]      
      tst r0,r0
      subne r4,r4,r1
      ldr r2,[psg,#psg_sn_volume1]
      mul r0,r4,r2
      add r11,r11,r0,asr#16
      
      ;@ get PSG channel 2 sample
      ldrb r0,[psg,#psg_sn_output2]
      ldr r1,[psg,#psg_sn_count2]
      tst r0,r0
      eoreq r4,r4,r4
      movne r4,r1
      sub r1,r1,#0x10000
      cmp r1,#0
      bgt 1f
      ldr r2,[psg,#psg_sn_period2]
3:
      add r1,r1,r2
      cmp r1,#0
      bgt 2f
      add r1,r1,r2
      add r4,r4,r2
      cmp r1,#0
      ble 3b
      b 1f
2:
      eors r0,r0,#1
      addne r4,r4,r2
      strb r0,[psg,#psg_sn_output2]
      
1:        
      
      str r1,[psg,#psg_sn_count2]      
      tst r0,r0
      subne r4,r4,r1
      ldr r2,[psg,#psg_sn_volume2]
      mul r0,r4,r2
      add r11,r11,r0,asr#16
      
      ;@ update psg sound channel3 ( noise )
      mov r12,#0x10000
      ldr r1,[psg,#psg_sn_count3]
      ldrb r0,[psg,#psg_sn_output3]
      ldr r2,[psg,#psg_sn_rng]
      ldr r3,[psg,#psg_sn_period3]
      ldr r5,[psg,#psg_sn_noisefb]
      eor r4,r4,r4
1:      
      cmp r1,r12
      movlt lr,r1
      movge lr,r12
      
      tst r0,r0
      addne r4,r4,r1
      
      sub r1,r1,lr
      sub r12,r12,lr
      
      cmp r1,#0
      bgt 2f
      
      tst r2,#1
      eorne r2,r2,r5
      mov r2,r2,lsr#1
      ands r0,r2,#1
      addne r4,r4,r3
      add r1,r1,r3
2:
      tst r0,r0
      subne r4,r4,r1
      cmp r12,#0
      bgt 1b
      
      strb r0,[psg,#psg_sn_output3]
      str r1,[psg,#psg_sn_count3]
      str r2,[psg,#psg_sn_rng]
      ldr r1,[psg,#psg_sn_volume3]
      mul r0,r4,r1
      add r11,r11,r0,asr#16
      mov r11,r11,lsl#1
      mov r0,#0x8000
      cmp r11,r0
      subge r11,r0,#1  ;@ 7FFF
      bge 1f
      sub r0,r0,#0x10000
      cmp r11,r0
      movlt r11,r0
1:
      
      mov r11,r11,lsl#16
      orr r11,r11,r11,lsr#16
      str r11,[r10],#4
      ;@ strh r11,[r10],#2
      ;@ strh r11,[r10],#2
      cmp r10,r9
      blt RenderSound_loop

      ldmfd sp!,{r4-r11,pc}
buffer_curr_pos: .word 0
buffer_max_pos: .word 0 
  