
      ;@ .global advance_eg_channel
       .global YM2612UpdateOne
      
      
      .extern PSG
      .extern sl_table;
      .extern dt_tab;
      .extern opn_fktable;
      .extern eg_rate_select;
      .extern eg_rate_shift;
      .extern eg_inc;
      .extern lfo_pm_output;
      .extern lfo_samples_per_step;
      .extern lfo_ams_depth_shift;
      .extern lfo_pm_table;
      .extern YMOPN_ST_dt_tab;
      .extern OPN_fn_table;	/* fnumber->increment counter */
      .extern OPN_lfo_freq;	/* LFO FREQ table */
      .extern tl_tab;
      .extern sin_tab;
      .extern OPN_pan
      .extern SL3;
      .extern ST;
      .extern OPN;
      .extern CH;
      .extern dacout
      .extern dac_buffer;
      .extern dacen;
      .extern sound_buffer_size;
      .extern algofuncs;
      
      chan .req r5
      slot .req r6
      opn .req r7
      psg .req r8
      
      .equiv env_bits,        10
      .equiv env_len,         (1<<env_bits)
      .equiv env_step,        (128/env_len)
      
      .equiv max_att_index,   (env_len-1)
      .equiv min_att_index,   0
      
      .equiv freq_sh,         16
      .equiv eg_sh,           16
      .equiv lfo_sh,          24
      .equiv timer_sh,        16
      
      .equiv freq_mask,       ((1<<freq_sh)-1)
      
      .equiv eg_att,          4
      .equiv eg_dec,          3
      .equiv eg_sus,          2
      .equiv eg_rel,          1
      .equiv eg_off,          0
      
      .equiv sin_bits,        10
      .equiv sin_len,         (1<<sin_bits)
      .equiv sin_mask,        (sin_len-1)
      
      .equiv tl_res_len,      256
      
      .equiv maxout,          (+32767)
      .equiv minout,          (-32768)
      
      .equiv tl_tab_len,      (13*2*tl_res_len)
      .equiv env_quiet,       (tl_tab_len>>3)
      
      .equiv rate_steps,      8
      
      .equiv out_buffer_size, 138
      
      .equiv max_output,      0x7FFF
      .equiv step,            0x10000
      .equiv fb_wnoise,       0x12000
      .equiv fb_pnoise,       0x08000
      .equiv ng_preset,       0x0F35
      
      
;@ ################################
;@ fm single slot
;@ a fm channel has 4 of these slots
;@ ################################
      ;@ unsigned ints
      .equiv slot_eg_sh_active_mask,   0
      .equiv slot_sl,                  slot_eg_sh_active_mask+4
      .equiv slot_eg_sh_d1r_mask,      slot_sl+4
      .equiv slot_eg_sh_d2r_mask,      slot_eg_sh_d1r_mask+4
      .equiv slot_eg_sh_rr_mask,       slot_eg_sh_d2r_mask+4
      .equiv slot_eg_sh_ar_mask,       slot_eg_sh_rr_mask+4
      .equiv slot_tl,                  slot_eg_sh_ar_mask+4
      .equiv slot_vol_out,             slot_tl+4
      .equiv slot_AMmask,              slot_vol_out+4
      .equiv slot_phase,               slot_AMmask+4
      .equiv slot_Incr,                slot_phase+4
      .equiv slot_mul,                 slot_Incr+4
      .equiv slot_key,                 slot_mul+4
      .equiv slot_ar,                  slot_key+4
      .equiv slot_d1r,                 slot_ar+4
      .equiv slot_d2r,                 slot_d1r+4
      .equiv slot_rr,                  slot_d2r+4
      ;@########################################
      ;@ signed ints
      .equiv slot_volume,              slot_rr+4
      .equiv slot_dt,                  slot_volume+4
      ;@######################################## 
      ;@ unsigned bytes
      .equiv slot_state,               slot_dt+4
      .equiv slot_eg_sel_ar,           slot_state+1
      .equiv slot_eg_sh_ar,            slot_eg_sel_ar+1
      .equiv slot_eg_sel_d1r,          slot_eg_sh_ar+1
      .equiv slot_eg_sh_d1r,           slot_eg_sel_d1r+1
      .equiv slot_eg_sel_d2r,          slot_eg_sh_d1r+1
      .equiv slot_eg_sh_d2r,           slot_eg_sel_d2r+1
      .equiv slot_eg_sel_rr,           slot_eg_sh_d2r+1
      .equiv slot_eg_sh_rr,            slot_eg_sel_rr+1
      .equiv slot_ssg,                 slot_eg_sh_rr+1
      .equiv slot_ssgn,                slot_ssg+1 
      .equiv slot_KSR,                 slot_ssgn+1
      .equiv slot_ksr,                 slot_KSR+1
      .equiv slot_size,                0x5C

;@ ################################
;@ fm channel
;@ ################################     
      .equiv ch_fc,                    0
      .equiv ch_block_fnum,            ch_fc+4
      .equiv ch_pms,                   ch_block_fnum+4
      .equiv ch_opl_out0,              ch_pms+4
      .equiv ch_opl_out1,              ch_opl_out0+4
      .equiv ch_mem_value,             ch_opl_out1+4
      .equiv ch_ALGO,                  ch_mem_value+4
      .equiv ch_FB,                    ch_ALGO+1
      .equiv ch_ams,                   ch_FB+1
      .equiv ch_kcode,                 ch_ams+1
      .equiv ch_slot0,                 ch_kcode+1
      .equiv ch_slot1,                 ch_slot0+slot_size
      .equiv ch_slot2,                 ch_slot1+slot_size
      .equiv ch_slot3,                 ch_slot2+slot_size
      .equiv ch_size,                  0x18C

      ;@ slot = (19 * 4) + 13 = 89
      ;@ ch   = (89 * 4) + (6 * 4) + 8 = 32
;@ ################################
;@ fm_st
;@ ################################
      .equiv st_mode,                  0
      .equiv st_TA,                    st_mode+4
      .equiv st_TAC,                   st_TA+4
      .equiv st_TAC_Step,              st_TAC+4
      .equiv st_TBC,                   st_TAC_Step+4
      .equiv st_TBC_Step,              st_TBC+4
      .equiv st_address,               st_TBC_Step+4
      .equiv st_irq,                   st_address+1
      .equiv st_irqmask,               st_irq+1
      .equiv st_status,                st_irqmask+1
      .equiv st_prescaler_sel,         st_status+1
      .equiv st_fn_h,                  st_prescaler_sel+1
      .equiv st_TB,                    st_fn_h+1


;@ ################################
;@ fm_3slot
;@ ################################
      .equiv slot3_fc0,                  0
      .equiv slot3_fc1,                  slot3_fc0+4
      .equiv slot3_fc2,                  slot3_fc1+4
      .equiv slot3_block_fnum0,          slot3_fc1+4
      .equiv slot3_block_fnum1,          slot3_block_fnum0+4
      .equiv slot3_block_fnum2,          slot3_block_fnum1+4
      .equiv slot3_fn_h,                 slot3_block_fnum2+4
      .equiv slot3_kcode0,               slot3_fn_h+1
      .equiv slot3_kcode1,               slot3_kcode0+1
      .equiv slot3_kcode2,               slot3_kcode1+1


;@ ################################
;@ fm_opn
;@ ################################
      .equiv opn_lfo_inc,                  0      
      .equiv opn_lfo_cnt,                  opn_lfo_inc+4
      .equiv opn_eg_cnt,                   opn_lfo_cnt+4
      .equiv opn_eg_timer,                 opn_eg_cnt+4
      .equiv opn_eg_timer_add,             opn_eg_timer+4
      .equiv opn_eg_timer_overflow,        opn_eg_timer_add+4

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


      

      
eg_inc_local:    .word eg_inc
opn_local:       .word OPN
chan_local:      .word CH

      
sound_buffer_size_local: .word sound_buffer_size

psg_local: .word PSG
LFO_AM:    .word 0
LFO_PM:    .word 0
dac_buffer_local: .word dac_buffer
YM2612UpdateOne:
      stmfd sp!,{r4-r11,lr}
      str r0,buffer_curr_pos
      add r1,r0,r1,lsl#2
      str r1,buffer_max_pos
      ldr opn,opn_local
      ldr r0,dac_buffer_local
      str r0,dac_sample
YM2612UpdateOne_buffer_loop:
      
      ;@ check for LFO support
      ldr r0,[opn,#opn_lfo_inc]
      tst r0,r0
      beq 1f
      ldr r1,[opn,#opn_lfo_cnt]
      add r1,r1,r0
      str r1,[opn,#opn_lfo_cnt]
      mov r1,r1,lsr#lfo_sh
      and r1,r1,#127
      cmp r1,#64
      movlt r0,r1,lsl#1
      andge r0,r1,#63
      movge r0,r0,lsl#1
      rsbge r0,r0,#126
      strb r0,LFO_AM
      mov r1,r1,lsr#2
      strb r1,LFO_PM
      b 2f
1:
      ;@ no LFO support
      eor r0,r0,r0
      strb r0,LFO_AM
      strb r0,LFO_PM
2:
      
advance_eg_channel:   ;@ r0 = pointer to first channel
      ldr r12,eg_inc_local
      ldr r4,[opn,#opn_eg_cnt]
      ldr r8,[opn,#opn_eg_timer]
      ldr r0,[opn,#opn_eg_timer_add]
      add r8,r8,r0
      ldr r9,[opn,#opn_eg_timer_overflow]
      cmp r8,r9
      blt advance_eg_end
      mov r10,#0x300
      orr r10,r10,#0xFF
advance_eg_timer_loop:
      add r4,r4,#1
      ldr chan,chan_local
      mov r11,#6
advance_eg_channel_loop:      
      add slot,chan,#ch_slot0
      mov lr,#4
      mov r3,#0       
advance_eg_slot_loop:
      ldr r1,[slot,#slot_eg_sh_active_mask]
      ands r1,r4,r1
      bne advance_eg_channel_nextslot2
      ldrb r0,[slot,#slot_state]
      ldr pc,[pc,r0,lsl#2]
      .word 0
      .word advance_eg_channel_nextslot2
      .word advance_eg_channel_EG_REL
      .word advance_eg_channel_EG_SUS
      .word advance_eg_channel_EG_DEC
      .word advance_eg_channel_EG_ATT

advance_eg_channel_EG_ATT:
       ldrb r1,[slot,#slot_eg_sh_ar]
       mov r0,r4,lsr r1
       and r0,r0,#7
       ldrb r1,[slot,#slot_eg_sel_ar]
       add r0,r0,r1
       ldrb r0,[r12,r0]
       ldr r1,[slot,#slot_volume]
       mvn r2,r1
       mul r0,r2,r0
       add r0,r1,r0,asr#4
       cmp r0,#0
       bgt advance_eg_channel_nextslot
       mov r0,#0
       mov r1,#3
       strb r1,[slot,#slot_state]
       ldr r1,[slot,#slot_eg_sh_d1r_mask]
       str r1,[slot,#slot_eg_sh_active_mask]
       b advance_eg_channel_nextslot

advance_eg_channel_EG_DEC:
       ldrb r1,[slot,#slot_eg_sh_d1r]
       mov r0,r4,lsr r1
       and r0,r0,#7
       ldrb r1,[slot,#slot_eg_sel_d1r]
       add r0,r0,r1
       ldrb r0,[r12,r0]
       ldrb r1,[slot,#slot_ssg]
       ldr r2,[slot,#slot_volume]
       tst r1,#0x8
       addne r0,r2,r0,lsl#2
       addeq r0,r2,r0
       ldr r1,[slot,#slot_sl]
       cmp r0,r1
       bcc advance_eg_channel_nextslot
       mov r1,#2
       strb r1,[slot,#slot_state]
       ldr r1,[slot,#slot_eg_sh_d2r_mask]
       str r1,[slot,#slot_eg_sh_active_mask]
       b advance_eg_channel_nextslot
       
advance_eg_channel_EG_SUS:
       ldrb r0,[slot,#slot_ssg]
       tst r0,#0x8
       beq 1f
       ldrb r1,[slot,#slot_eg_sh_d2r]
       mov r0,r4,lsr r1
       and r0,r0,#7
       ldrb r1,[slot,#slot_eg_sel_d2r]
       add r0,r0,r1
       ldrb r0,[r12,r0]
       ldr r1,[slot,#slot_volume]
       add r0,r1,r0,lsl#2
       cmp r0,r10
       blt advance_eg_channel_nextslot
       mov r0,r10
       ldrb r1,[slot,#slot_ssg]
       tst r1,#0x1
       beq 2f
       ldrb r2,[slot,#slot_ssgn]
       tst r2,#1
       bne advance_eg_channel_nextslot
       and r3,r1,#0x2
       orr r3,r3,#1
       b advance_eg_channel_nextslot
2:
       mov r2,#4
       strb r2,[slot,#slot_state]
       ldr r2,[slot,#slot_eg_sh_ar_mask]
       str r2,[slot,#slot_eg_sh_active_mask]
       and r3,r1,#0x2
       b advance_eg_channel_nextslot
1:
       ldrb r1,[slot,#slot_eg_sh_d2r]
       mov r0,r4,lsr r1
       and r0,r0,#7
       ldrb r1,[slot,#slot_eg_sel_d2r]
       add r0,r0,r1
       ldrb r0,[r12,r0]
       ldr r1,[slot,#slot_volume]
       add r0,r1,r0
       cmp r0,r10
       movgt r0,r10
       b advance_eg_channel_nextslot
       
advance_eg_channel_EG_REL:
       ldrb r1,[slot,#slot_eg_sh_rr]
       mov r0,r4,lsr r1
       and r0,r0,#7
       ldrb r1,[slot,#slot_eg_sel_rr]
       add r0,r0,r1
       ldrb r0,[r12,r0]
       ldr r1,[slot,#slot_volume]
       add r0,r1,r0
       cmp r0,r10
       blt advance_eg_channel_nextslot
       mov r0,r10
       mov r1,#0
       strb r1,[slot,#slot_state]
       str r1,[slot,#slot_eg_sh_active_mask]
       b advance_eg_channel_nextslot

advance_eg_channel_nextslot2:
      ldr r0,[slot,#slot_volume] ;@ reload as unsigned
advance_eg_channel_nextslot:
      str r0,[slot,#slot_volume]
      ldr r1,[slot,#slot_tl]
      add r0,r0,r1
      ldrb r1,[slot,#slot_ssg]
      tst r1,#0x8
      beq 1f
      ldrb r1,[slot,#slot_ssgn]
      tst r1,#0x2
      beq 1f
      eor r0,r0,r10
1:
      str r0,[slot,#slot_vol_out]
      ldrb r0,[slot,#slot_ssgn]
      eor r0,r0,r3
      strb r0,[slot,#slot_ssgn]
      
      add slot,slot,#slot_size
      subs lr,lr,#1
      bne advance_eg_slot_loop
      add chan,chan,#ch_size
      subs r11,r11,#1
      bne advance_eg_channel_loop  
      sub r8,r8,r9
      cmp r8,r9
      bge advance_eg_timer_loop
advance_eg_end:
      str r8,[r7,#opn_eg_timer]
      str r4,[r7,#opn_eg_cnt]
      
      eor r11,r11,r11
      ldr psg,psg_local
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
      add r11,r11,r0
      
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
      add r11,r11,r0
      
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
      add r11,r11,r0
      
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
      add r11,r11,r0
      
      mov r11,r11,asr#17
      ;@ mov r11,#0
      ;@ r11 current sample
      ;@ r7 - opn - opn context
process_channel:   
      ldr chan,chan_local
      mov r12,#5  ;@ channel counter
      eor r0,r0,r0
      str r0,chan_6_check
process_channel_loop:      
      ldrb r0,[chan,#ch_ALGO]
      ldr pc,[pc,r0,lsl#2]
      .word 0
      .word process_channel_algo0
      .word process_channel_algo1
      .word process_channel_algo2
      .word process_channel_algo3
      .word process_channel_algo4
      .word process_channel_algo5
      .word process_channel_algo6
      .word process_channel_algo7

process_channel_algo0:
      ldr r0,[chan,#ch_opl_out0]
      ldr r9,[chan,#ch_opl_out1]
      add r4,r0,r9
      str r9,[chan,#ch_opl_out0]
      
      ldrb r8,LFO_AM 
      ldrb r1,[chan,#ch_ams]
      mov r8,r8,lsr r1
      ;@ r8 = cached ams

      ;@ #################################
      ;@ #  Slot 1
      ;@ #################################
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                ;@ calc eg
      cmp r1,#0x340               ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      strge r0,[chan,#ch_opl_out1]
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldrb r2,[chan,#ch_FB]
      tst r2,r2
      addne r0,r0,r4,lsl r2
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      str r0,[chan,#ch_opl_out1]
1:      
      ;@ #################################
      ;@ #  Slot 2
      ;@ #################################
      ldr r4,[chan,#ch_mem_value] ;@ keep for later
      add slot,slot,#slot_size<<1  ;@ move to slot2(3)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r2,r9,asr#1
      ;@ r0 = phase
      ;@ r1 = env
      ;@ r2 = pm
      bl op_calc
1:
      str r0,[chan,#ch_mem_value]
      
      ;@ #################################
      ;@ #  Slot 4
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot4
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      eorge r0,r0,r0
      bge process_channel_next 
      ldr r0,[slot,#slot_phase]
      stmfd sp!,{r0,r1}
      sub slot,slot,#slot_size<<1  ;@ move to back to slot3
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      eorge r2,r2,r2
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r2,r4,asr#1
      bl op_calc
      mov r2,r0,asr#1
1:
      ldmfd sp!,{r0,r1}
      bl op_calc
      b process_channel_next    

     
process_channel_algo1:
      ldr r0,[chan,#ch_opl_out0]
      ldr r1,[chan,#ch_opl_out1]
      add r4,r0,r1
      str r1,[chan,#ch_opl_out0]
      
      ldrb r8,LFO_AM 
      ldrb r1,[chan,#ch_ams]
      mov r8,r8,lsr r1
      ;@ r8 = cached ams

      ;@ #################################
      ;@ #  Slot 1
      ;@ #################################
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                ;@ calc eg
      cmp r1,#0x340               ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      strge r0,[chan,#ch_opl_out1]
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldrb r2,[chan,#ch_FB]
      tst r2,r2
      addne r0,r0,r4,lsl r2
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      str r0,[chan,#ch_opl_out1]
1:      
      ;@ #################################
      ;@ #  Slot 2
      ;@ #################################
      ldr r4,[chan,#ch_mem_value] ;@ keep for later
      add slot,slot,#slot_size<<1  ;@ move to slot2(3)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      ldrge r0,[chan,#ch_opl_out0]
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r1,r1,lsr#1
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      ldr r1,[chan,#ch_opl_out0]
      add r0,r0,r1
1:
      str r0,[chan,#ch_mem_value]
      
      ;@ #################################
      ;@ #  Slot 4
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot4
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      eorge r0,r0,r0
      bge process_channel_next 
      ldr r0,[slot,#slot_phase]
      stmfd sp!,{r0,r1}
      sub slot,slot,#slot_size<<1  ;@ move to back to slot3
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      eorge r2,r2,r2
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r2,r4,asr#1
      bl op_calc
      mov r2,r0,asr#1
1:
      ldmfd sp!,{r0,r1}
      bl op_calc
      b process_channel_next  
      
process_channel_algo2:
      ldr r0,[chan,#ch_opl_out0]
      ldr r1,[chan,#ch_opl_out1]
      add r4,r0,r1
      str r1,[chan,#ch_opl_out0]
      
      ldrb r8,LFO_AM 
      ldrb r1,[chan,#ch_ams]
      mov r8,r8,lsr r1
      ;@ r8 = cached ams

      ;@ #################################
      ;@ #  Slot 1
      ;@ #################################
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                ;@ calc eg
      cmp r1,#0x340               ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      strge r0,[chan,#ch_opl_out1]
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldrb r2,[chan,#ch_FB]
      tst r2,r2
      addne r0,r0,r4,lsl r2
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      str r0,[chan,#ch_opl_out1]
1:      
      ;@ #################################
      ;@ #  Slot 2
      ;@ #################################
      ldr r4,[chan,#ch_mem_value] ;@ keep for later
      add slot,slot,#slot_size<<1  ;@ move to slot2(3)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      bge 1f
      ldr r0,[slot,#slot_phase]
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
1:
      str r0,[chan,#ch_mem_value]
      
      ;@ #################################
      ;@ #  Slot 4
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot4
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      eorge r0,r0,r0
      bge process_channel_next 
      ldr r0,[slot,#slot_phase]
      stmfd sp!,{r0,r1}
      sub slot,slot,#slot_size<<1  ;@ move to back to slot3
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      ldrge r2,[chan,#ch_opl_out0]
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r2,r4,asr#1
      bl op_calc
      ldr r1,[chan,#ch_opl_out0]
      add r0,r0,r1
      mov r2,r0,asr#1
1:
      ldmfd sp!,{r0,r1}
      bl op_calc
      b process_channel_next 
      
process_channel_algo3:
      ldr r0,[chan,#ch_opl_out0]
      ldr r1,[chan,#ch_opl_out1]
      add r4,r0,r1
      str r1,[chan,#ch_opl_out0]
      
      ldrb r8,LFO_AM 
      ldrb r1,[chan,#ch_ams]
      mov r8,r8,lsr r1
      ;@ r8 = cached ams

      ;@ #################################
      ;@ #  Slot 1
      ;@ #################################
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                ;@ calc eg
      cmp r1,#0x340               ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldrb r2,[chan,#ch_FB]
      tst r2,r2
      addne r0,r0,r4,lsl r2
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
1:      
      str r0,[chan,#ch_opl_out1]
      
      ;@ #################################
      ;@ #  Slot 3
      ;@ #################################
      ldr r4,[chan,#ch_mem_value] ;@ keep for later
      add slot,slot,#slot_size  ;@ move to slot3(2)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      eorge r9,r9,r9
      bge 1f
      ldr r0,[slot,#slot_phase]
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      add r4,r4,r0
      mov r9,r0  ;@ save r0 for later
1:
      ;@ #################################
      ;@ #  Slot 2
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot2(3)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldr r2,[chan,#ch_opl_out0]
      mov r2,r2,asr#1
      ;@ r0 = phase
      ;@ r1 = env
      ;@ r2 = pm
      bl op_calc
1:
      str r0,[chan,#ch_mem_value]
      
      ;@ #################################
      ;@ #  Slot 4
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot4
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      eorge r0,r0,r0
      bge process_channel_next 
      ldr r0,[slot,#slot_phase]
      add r2,r4,r9
      mov r2,r2,asr#1
      bl op_calc
      b process_channel_next 
      
process_channel_algo4:
      ldr r0,[chan,#ch_opl_out0]
      ldr r9,[chan,#ch_opl_out1]
      add r4,r0,r9
      str r9,[chan,#ch_opl_out0]
      
      ldrb r8,LFO_AM 
      ldrb r1,[chan,#ch_ams]
      mov r8,r8,lsr r1
      ;@ r8 = cached ams

      ;@ #################################
      ;@ #  Slot 1
      ;@ #################################
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                ;@ calc eg
      cmp r1,#0x340               ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldrb r2,[chan,#ch_FB]
      tst r2,r2
      addne r0,r0,r4,lsl r2
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
1:      
      str r0,[chan,#ch_opl_out1]
      ;@ #################################
      ;@ #  Slot 2
      ;@ #################################
      add slot,slot,#slot_size<<1  ;@ move to slot2(3)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      eorge r4,r4,r4
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r2,r9,asr#1
      ;@ r0 = phase
      ;@ r1 = env
      ;@ r2 = pm
      bl op_calc
      mov r4,r0
1:
      
      ;@ #################################
      ;@ #  Slot 4
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot4
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      movge r0,r4
      bge process_channel_next 
      ldr r0,[slot,#slot_phase]
      stmfd sp!,{r0,r1}
      sub slot,slot,#slot_size<<1  ;@ move to back to slot3
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1
      cmp r1,#0x340 ;@ ENV_QUIET
      eorge r2,r2,r2
      bge 1f
      ldr r0,[slot,#slot_phase]
      bl op_calc2
      mov r2,r0,asr#1
1:
      ldmfd sp!,{r0,r1}
      bl op_calc
      add r0,r0,r4
      b process_channel_next
      
process_channel_algo5:
      ldr r0,[chan,#ch_opl_out0]
      ldr r9,[chan,#ch_opl_out1]
      add r4,r0,r9
      str r9,[chan,#ch_opl_out0]
      
      ldrb r8,LFO_AM 
      ldrb r1,[chan,#ch_ams]
      mov r8,r8,lsr r1
      ;@ r8 = cached ams

      ;@ #################################
      ;@ #  Slot 1
      ;@ #################################
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                ;@ calc eg
      cmp r1,#0x340               ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldrb r2,[chan,#ch_FB]
      tst r2,r2
      addne r0,r0,r4,lsl r2
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
1:    
      str r0,[chan,#ch_opl_out1]
      ;@ #################################
      ;@ #  Slot 3
      ;@ #################################
      ldr r2,[chan,#ch_mem_value] ;@ keep for later
      str r9,[chan,#ch_mem_value]
      add slot,slot,#slot_size  ;@ move to slot3(2)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      eorge r4,r4,r4
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r2,r2,asr#1
      ;@ r0 = phase
      ;@ r1 = env
      ;@ r2 = pm
      bl op_calc
      mov r4,r0
1:
      
      ;@ #################################
      ;@ #  Slot 2
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot2(3)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r2,r9,asr#1
      ;@ r0 = phase
      ;@ r1 = env
      ;@ r2 = pm
      bl op_calc
      add r4,r4,r0
1:
      ;@ #################################
      ;@ #  Slot 4
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot4(4)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      movge r0,r4
      bge process_channel_next
      ldr r0,[slot,#slot_phase]
      mov r2,r9,asr#1
      ;@ r0 = phase
      ;@ r1 = env
      ;@ r2 = pm
      bl op_calc
      add r0,r4,r0
      b process_channel_next
      
process_channel_algo6:
      ldr r0,[chan,#ch_opl_out0]
      ldr r9,[chan,#ch_opl_out1]
      add r4,r0,r9
      str r9,[chan,#ch_opl_out0]
      
      ldrb r8,LFO_AM 
      ldrb r1,[chan,#ch_ams]
      mov r8,r8,lsr r1
      ;@ r8 = cached ams

      ;@ #################################
      ;@ #  Slot 1
      ;@ #################################
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                ;@ calc eg
      cmp r1,#0x340               ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldrb r2,[chan,#ch_FB]
      tst r2,r2
      addne r0,r0,r4,lsl r2
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
1:    
      str r0,[chan,#ch_opl_out1]
      
      ;@ #################################
      ;@ #  Slot 3
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot3(2)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      eorge r4,r4,r4
      bge 1f
      ldr r0,[slot,#slot_phase]
      ;@ r0 = phase
      ;@ r1 = env
      ;@ r2 = pm
      bl op_calc2
      mov r4,r0
1:
      
      ;@ #################################
      ;@ #  Slot 2
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot2(3)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      bge 1f
      ldr r0,[slot,#slot_phase]
      mov r2,r9,asr#1
      ;@ r0 = phase
      ;@ r1 = env
      ;@ r2 = pm
      bl op_calc
      add r4,r4,r0
1:
      ;@ #################################
      ;@ #  Slot 4
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot4(4)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      movge r0,r4
      bge process_channel_next
      ldr r0,[slot,#slot_phase]
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      add r0,r4,r0
      b process_channel_next
      
process_channel_algo7:
      ldr r0,[chan,#ch_opl_out0]
      ldr r9,[chan,#ch_opl_out1]
      add r4,r0,r9
      str r9,[chan,#ch_opl_out0]
      
      ldrb r8,LFO_AM 
      ldrb r1,[chan,#ch_ams]
      mov r8,r8,lsr r1
      ;@ r8 = cached ams

      ;@ #################################
      ;@ #  Slot 1
      ;@ #################################
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                ;@ calc eg
      cmp r1,#0x340               ;@ check eg < ENV_QUIET
      eorge r0,r0,r0
      bge 1f
      ldr r0,[slot,#slot_phase]
      ldrb r2,[chan,#ch_FB]
      tst r2,r2
      addne r0,r0,r4,lsl r2
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
1:    
      str r0,[chan,#ch_opl_out1]
      
      ;@ #################################
      ;@ #  Slot 3
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot3(2)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      movge r4,r9
      bge 1f
      ldr r0,[slot,#slot_phase]
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      add r4,r9,r0
1:
      
      ;@ #################################
      ;@ #  Slot 2
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot2(3)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      bge 1f
      ldr r0,[slot,#slot_phase]
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      add r4,r4,r0
1:
      ;@ #################################
      ;@ #  Slot 4
      ;@ #################################
      add slot,slot,#slot_size  ;@ move to slot4(4)
      ldr r0,[slot,#slot_AMmask]
      and r0,r0,r8                 ;@ mask with ams
      ldr r1,[slot,#slot_vol_out]
      add r1,r0,r1                 ;@ calc eg
      cmp r1,#0x340 ;@ ENV_QUIET   ;@ check eg < ENV_QUIET
      movge r0,r4
      bge process_channel_next
      ldr r0,[slot,#slot_phase]
      ;@ r0 = phase
      ;@ r1 = env
      bl op_calc2
      add r0,r4,r0
      b process_channel_next

tl_tab_local: .word tl_tab
sin_tab_local: .word sin_tab
    
op_calc:
      mov r0,r0,lsr#16
      add r0,r0,r2 
      tst r0,#256
      eorne r0,r0,#0xFF
      ldr r2,sin_tab_local
      and r3,r0,#0xFF
      add r2,r2,r3,lsl#1
      ldrh r3,[r2]
      add r1,r3,r1,lsl#3
      cmp r1,#0x1A00
      eorge r0,r0,r0
      movge pc,lr
      ldr r2,tl_tab_local
      bic r3,r1,#1
      mov r3,r3,lsl#23    ;@ 000001FE
      add r3,r2,r3,lsr#23
      ldrsh r2,[r3]
      mov r1,r1,lsr#9
      tst r0,#512
      mov r0,r2,asr r1
      rsbne r0,r0,#0
      mov pc,lr 

op_calc2:
      mov r0,r0,lsr#16
      tst r0,#256
      eorne r0,r0,#0xFF
      ldr r2,sin_tab_local
      and r3,r0,#0xFF
      add r2,r2,r3,lsl#1
      ldrh r3,[r2]
      add r1,r3,r1,lsl#3
      cmp r1,#0x1A00
      eorge r0,r0,r0
      movge pc,lr
      ldr r2,tl_tab_local
      bic r3,r1,#1
      mov r3,r3,lsl#23    ;@ 000001FE
      add r3,r2,r3,lsr#23
      ldrsh r2,[r3]
      mov r1,r1,lsr#9
      tst r0,#512
      mov r0,r2,asr r1
      rsbne r0,r0,#0
      mov pc,lr    
dac_sample: .word 0
chan_6_check: .word 0
process_channel_next:
      add r11,r11,r0
      
      ;@ calculate channel
      ldr r0,[chan,#ch_pms]
      tst r0,r0
      beq 1f
      
1:
      ;@ slot 1
      add slot,chan,#ch_slot0
      ldr r0,[slot,#slot_phase]
      ldr r1,[slot,#slot_Incr]
      add r0,r0,r1
      str r0,[slot,#slot_phase]
      ;@ slot 2
      add slot,slot,#slot_size
      ldr r0,[slot,#slot_phase]
      ldr r1,[slot,#slot_Incr]
      add r0,r0,r1
      str r0,[slot,#slot_phase]
      ;@ slot 3
      add slot,slot,#slot_size
      ldr r0,[slot,#slot_phase]
      ldr r1,[slot,#slot_Incr]
      add r0,r0,r1
      str r0,[slot,#slot_phase]
      ;@ slot 4
      add slot,slot,#slot_size
      ldr r0,[slot,#slot_phase]
      ldr r1,[slot,#slot_Incr]
      add r0,r0,r1
      str r0,[slot,#slot_phase]
      
      subs r12,r12,#1
      addne chan,chan,#ch_size 
      bne process_channel_loop
      
      ldr r0,chan_6_check
      tst r0,r0
      bne 1f   ;@ chan 6 already done
      
      mov r0,#1
      str r0,chan_6_check
      ldr r0,dac_sample
      ldrh r1,[r0],#2
      str r0,dac_sample
      tst r1,#0xFF00
      moveq r12,#1
      beq process_channel_loop
      mov r1,r1,lsl#24
      add r11,r11,r1,asr#18
      
1:
      ;@ add sample to buffer
      ;@ range check sample
      
      mov r11,r11,lsl#1
      mov r0,#0x8000
      cmp r11,r0
      subge r11,r0,#1  ;@ 7FFF
      bge 1f
      sub r0,r0,#0x10000
      cmp r11,r0
      movlt r11,r0
1:
      ldr r0,buffer_curr_pos
      strh r11,[r0],#2
      strh r11,[r0],#2
      ldr r1,buffer_max_pos
      cmp r0,r1
      strlt r0,buffer_curr_pos
      blt YM2612UpdateOne_buffer_loop

      ldmfd sp!,{r4-r11,pc}
buffer_curr_pos: .word 0
buffer_max_pos: .word 0 
  