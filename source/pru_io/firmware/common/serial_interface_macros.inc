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
;   File:     serial_interface_macros.inc
;
;   Brief:    Serial Interface related macro definitions
;
;   Includes:
;            icss_constant_defines.inc
;************************************************************************************
    .if    !$defined("__adc_serial_interface_macros_inc")
__adc_serial_interface_macros_inc       .set 1

    .include "icss_constant_defines.inc"

;************************************************************************************
;   Macro: m_serial_read_packet
;
;   Read 1 data packet from the serial interface.
;
;   PEAK cycles:
;           ~ cycle
;
;   Pseudo code:
;       ========== C ==========
;       dataReg = 0
;       bitId = PACKETSIZE - 1
;       loop:   READ_PACKET
;           set   SCLK_PIN
;           wait  16 cycles
;           clear SCLK_PIN
;       =======================
;
;   Parameters:
;           MODE        -  MSB: MSB First, LSB: LSB First
;           PACKETSIZE  -  Size of data packet in bits
;           dataReg     -  To store read serial data packet
;           bitId       -  To track Bit index (8 bits wide)
;************************************************************************************
m_serial_read_packet   .macro  MODE, PACKETSIZE, dataReg, bitId
    ; clear dataReg which is used as temp data register
    ; bitId is bit position pointer
    zero    &dataReg, 4
    .if     $symcmp(MODE,"MSB") = 0
    ldi     bitId, PACKETSIZE - 1
    .elseif $symcmp(MODE,"LSB") = 0
    ldi     bitId, 0
    .endif
READ_PACKET_LOOP?:
    m_set_pru_pin   SCLK_PIN
    m_wait_cycles   16
    m_clr_pru_pin   SCLK_PIN
    ; store data in register
    qbbc    skip_data_high, r31, SDI_PIN
    set     dataReg, dataReg, bitId     ; FIXME: does this require specific register for bitId
    qba     skip_data_low
skip_data_high:
    m_wait_cycles   2
skip_data_low:
    m_wait_cycles   11
    .if     $symcmp(MODE,"MSB") = 0
    sub     bitId, bitId, 1
    qbne    READ_PACKET_LOOP?, bitId, 0xFF ; should overflow to 0xff after 0
    .elseif $symcmp(MODE,"LSB") = 0
    add     bitId, bitId, 1
    qbne    READ_PACKET_LOOP?, bitId, PACKETSIZE
    .endif
    .endm

;************************************************************************************
;   Macro: m_serial_read_frame
;
;   Read 1 data packet from the serial interface.
;
;   PEAK cycles:
;           ~ cycle
;
;   Pseudo code:
;       ========== C ==========
;       dataReg = 0
;       bitId = PACKETSIZE - 1
;       loop:   READ_PACKET
;           set   SCLK_PIN
;           wait  16 cycles
;           clear SCLK_PIN
;       =======================
;
;   Parameters:
;       MODE             -  MSB: MSB First, LSB: LSB First
;       PACKETSIZE       -  Size of data packet in bits
;       NO_OF_PACKETS    -  Number of data packets per frame
;       tempDataReg      -  To store read serial data packet temporarily
;       tempReg          -  To track Bit and Packet indices (16 bits wide)
;       dataRegBaseAddr  -  (8 bits) Address of starting data register
;                                   => register R1.bx must be used
;************************************************************************************
m_serial_read_frame    .macro  MODE, PACKETSIZE, NO_OF_PACKETS, dataRegBaseAddr, tempDataReg, tempReg
    .var    packetId, bitId
    .asg    tempReg.b0,    packetId
    .asg    tempReg.b1,    bitId

    ; set /CS low
    m_clr_pru_pin   CS_PIN
    ; wait 30 ns; min 16 ns according to ADS spec
    m_wait_cycles   10
    ldi     packetId, 0
READ_FRAME_LOOP:
    m_serial_read_packet  MODE, PACKETSIZE, tempDataReg, bitId
    mvid    *dataRegBaseAddr++, tempDataReg
    add     packetId, packetId, 1
    qbne    READ_FRAME_LOOP, packetId, NO_OF_PACKETS
    ; set /CS high
    m_set_pru_pin   CS_PIN
    ; wait 30 ns; min 16 ns according to ADS spec
    m_wait_cycles   10
    .endm

    .endif  ; __adc_serial_interface_macros_inc
