; Copyright (C) 2022 Texas Instruments Incorporated - http://www.ti.com/
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions
; are met:
;
; Redistributions of source code must retain the above copyright
; notice, this list of conditions and the following disclaimer.
;
; Redistributions in binary form must reproduce the above copyright
; notice, this list of conditions and the following disclaimer in the
; documentation and/or other materials provided with the
; distribution.
;
; Neither the name of Texas Instruments Incorporated nor the names of
; its contributors may be used to endorse or promote products derived
; from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
; "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
; LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
; A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
; OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
; SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
; LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
; THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

;************************************************************************************
;   File:     parallel_interface_macros.inc
;
;   Brief:    Parallel Interface related macro definitions
;
;   Includes:
;            icss_pin_macros.inc
;************************************************************************************
    .if    !$defined("__parallel_interface_macros_inc")
__parallel_interface_macros_inc    .set 1

    .include "icss_pin_macros.inc"

; PRU_CLK_CYC_PRD_CONST  will be imported from sysconfig generated file

;************************************************************************************
;   Macro: m_wait_cycles
;
;   Wait for specified number of cpu cycles
;
;   PEAK cycles:
;           count cycles
;
;   Pseudo code:
;       ========== C ==========
;       wait(count-cycles);
;       =======================
;
;   Parameters:
;           count       - number of cycles to wait
;************************************************************************************
m_wait_cycles       .macro count
    loop    endloop?, count - 1
    NOP
endloop?:
    .endm

;************************************************************************************
;   Macro: m_wait_nano_sec
;
;   Wait for specified number of nano seconds
;
;   PEAK cycles:
;           $ceil(time/PRU_CLK_CYC_PRD_CONST) cycles
;
;   Pseudo code:
;       ========== C ==========
;       wait(time);
;       =======================
;
;   Parameters:
;           time            - number of nano seconds to wait
;************************************************************************************
m_wait_nano_sec     .macro  time
    m_wait_cycles   (time + PRU_CLK_CYC_PRD_CONST -1)/PRU_CLK_CYC_PRD_CONST; $ceil(time/PRU_CLK_CYC_PRD_CONST) not working
    .endm

;************************************************************************************
;   Macro: m_parallel_read_packet
;
;   Read as data packet from parallel interface with following configurations
;    - INTERFACE, MODE, DATAWIDTH
;
;   PEAK cycles:
;           ~ cycle - depends on configuration
;
;   Pseudo code:
;       ========== C ==========
;       data_reg = value;
;       =======================
;
;   Parameters:
;           MODE       - MSB : MSB Bits are transferred first
;                        LSB : LSB Bits are transferred first
;           INTERFACE  - PARALLEL16 : 16 bit paralllel interface
;                        PARALLEL8  : 8 bit paralllel interface
;           DATAWIDTH  - 1 to 32
;           TRIG_PRD   - Period to wait, in ns, after setting control (CS, RD) pins
;           data_reg   - Register to store read data packet (32 bit)
;************************************************************************************
m_parallel_read_packet .macro  MODE, INTERFACE, DATAWIDTH, TRIG_PRD, data_reg, RD_PIN
    .if     $symcmp(MODE,"MSB") = 0
    .if     $symcmp(INTERFACE,"PARALLEL16") = 0
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD  ; 15ns
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD  ; 15ns
    ; read data
    mov     data_reg.w2, r31.w0

    .if     DATAWIDTH > 16
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.w0, r31.w0
    .endif  ; DATAWIDTH > 16

    lsr     data_reg, data_reg, (32 - DATAWIDTH)

    .elseif     $symcmp(INTERFACE,"PARALLEL8") = 0
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.b3, r31.b0

    .if     DATAWIDTH > 8
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.b2, r31.b0
    .endif  ; DATAWIDTH > 186

    .if     DATAWIDTH > 16
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.b1, r31.b0
    .endif  ; DATAWIDTH > 16

    .if     DATAWIDTH > 24
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.b0, r31.b0
    .endif  ; DATAWIDTH > 24

    lsr     data_reg, data_reg, (32 - DATAWIDTH)
    .endif  ; INTERFACE = PARALLEL8

    .elseif     $symcmp(MODE,"LSB") = 0
    .if     $symcmp(INTERFACE,"PARALLEL16") = 0

    .if     DATAWIDTH <= 16
    ; clear data_reg
    ldi     data_reg, 0
    .endif  ; DATAWIDTH <= 16

    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.w0, r31.w0

    .if     DATAWIDTH > 16
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.w2, r31..w0
    .endif  ; DATAWIDTH > 16

    .elseif     $symcmp(INTERFACE,"PARALLEL8") = 0

    .if     DATAWIDTH <= 24
    ; clear data_reg
    ldi     data_reg, 0
    .endif  ; DATAWIDTH <= 24

    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.b0, r31.b0

    .if     DATAWIDTH > 8
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.b1, r31.b0
    .endif  ; DATAWIDTH > 186

    .if     DATAWIDTH > 16
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.b2, r31.b0
    .endif  ; DATAWIDTH > 16

    .if     DATAWIDTH > 24
    ; send read signal
    m_pru_clr_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    m_pru_set_pin           RD_PIN
    m_wait_nano_sec         TRIG_PRD
    ; read data
    mov     data_reg.b3, r31.b0
    .endif  ; DATAWIDTH > 24

    .endif  ; INTERFACE = PARALLEL8
    .endif  ; MODE      = LSB

    .endm

;************************************************************************************
;   Macro: m_parallel_read_frame
;
;   Read as data frame composed of data packets from parallel interface
;
;   PEAK cycles:
;           ~ cycle - depends on configuration
;
;   Pseudo code:
;       ========== C ==========
;       data_reg = value;
;       =======================
;
;   Parameters:
;           MODE       - MSB : MSB Bits are transferred first
;                        LSB : LSB Bits are transferred first
;           INTERFACE  - PARALLEL16 : 16 bit paralllel interface
;                        PARALLEL8  : 8 bit paralllel interface
;           DATAWIDTH  - 1 to 32
;           TRIG_PRD   - Period to wait, in ns, after setting control (CS, RD) pins
;           data_reg   - Register to store read data packet (32 bit)
;************************************************************************************
m_parallel_read_frame   .macro  NO_OF_PACKETS, dataRegBaseAddr, tempReg, MODE, INTERFACE, DATAWIDTH, TRIG_PRD, data_reg
    .if         0
    ; set /CS low
    m_pru_clr_pin       CS_PIN
    m_wait_nano_sec     WAIT_PRD
    .endif

    ;-----------------------------------------------
    ; Memory Optimized Code
    ;-----------------------------------------------
    .if         0
    .var    packetId
    .asg    tempReg.b0,    packetId

    ; Can we use this loop instruction? noninterruptible loop meaning?
    ; loop EndLoop, NO_OF_PACKETS
    ldi     packetId, 0
READ_FRAME_LOOP:
    m_parallel_read_packet  MODE, INTERFACE, DATAWIDTH, TRIG_PRD, data_reg
    ; .if         DATAWIDTH <= 8
    ; mvib    *dataRegBaseAddr++, data_reg
    ; .elseif     DATAWIDTH <= 16
    ; mviw    *dataRegBaseAddr++, data_reg
    ; .else
    mvid    *dataRegBaseAddr++, data_reg
    ; .endif  ; DATAWIDTH <= 8
    add     packetId, packetId, 1
    qbne    READ_FRAME_LOOP, packetId, NO_OF_PACKETS
    .endif

    ;-----------------------------------------------
    ; Speed Optimized Code
    ;-----------------------------------------------
    .if         1
    .eval       0, i
    .loop       NO_OF_PACKETS
    .if         0
    m_parallel_read_packet  MODE, INTERFACE, DATAWIDTH, TRIG_PRD, data_reg
    mvid    *dataRegBaseAddr++, data_reg
    .endif
    .if         1
    ; TODO: check if this works: data_reg_:i:
    m_parallel_read_packet  MODE, INTERFACE, DATAWIDTH, TRIG_PRD, data_reg_:i:
    .endif
    .eval       i+1, i
    .endloop    ; NO_OF_PACKETS
    .endif

    .if         0
    ; set /CS high
    m_pru_set_pin       CS_PIN
    m_wait_nano_sec     WAIT_PRD
    .endif

    .endm

    .endif  ; __parallel_interface_macros_inc