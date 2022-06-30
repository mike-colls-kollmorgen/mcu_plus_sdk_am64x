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

    .if    !$defined("__adc_preprocess_macros_h")
__adc_preprocess_macros_h    .set 1

;************************************************************************************
;   Macro: m_sign_ext_32bit
;
;   For sign extension from 18 to 32 bits
;
;   PEAK cycles:
;           3 cycle
;
;   Pseudo code:
;       ========== C ==========
;       if (18th bit == 1)
;           set(bits 18 to 32 HIGH)
;       =======================
;
;   Parameters:
;           sample_reg
;
;   Returns:
;           sample_reg
;************************************************************************************
m_sign_ext_32bit    .macro  sample_reg
    qbbc   SKIP_SIGN_EXT?, sample_reg, 17    ; bit position: (1<<17)
    or     sample_reg.b3, sample_reg.b3, 0xFF
    or     sample_reg.b2, sample_reg.b2, 0xFE
SKIP_SIGN_EXT?:
    .endm

;************************************************************************************
;   Macro: m_sign_ext_16bit
;
;   For sign extension from 14 to 16 bits
;
;   PEAK cycles:
;           2 cycle
;
;   Pseudo code:
;       ========== C ==========
;       if (14th bit == 1)
;           set(bits 14 to 16 HIGH)
;       =======================
;
;   Parameters:
;           sample_reg
;
;   Returns:
;           sample_reg
;************************************************************************************
m_sign_ext_16bit    .macro  sample_reg
    qbbc   SKIP_SIGN_EXT?, sample_reg, 13    ; bit position: (1<<13)
    or     sample_reg.b1, sample_reg.b1, 0xE0
SKIP_SIGN_EXT?:
    .endm

;************************************************************************************
;   Macro: m_sign_ext_16to32_bits
;
;   For sign extension from 16 to 32 bits
;
;   PEAK cycles:
;           2 cycle
;
;   Pseudo code:
;       ========== C ==========
;       if (16th bit == 1)
;           set(bits 17 to 32 HIGH)
;       =======================
;
;   Parameters:
;           sample_reg
;
;   Returns:
;           sample_reg
;************************************************************************************
m_sign_ext_16to32_bits      .macro  sample_reg
    qbbc   SKIP_SIGN_EXT?, sample_reg, 15    ; bit position: (1<<15)
    ldi    sample_reg.w2, 0xFFFF
SKIP_SIGN_EXT?:
    .endm

;************************************************************************************
;   Macro: m_sign_ext_24to32_bits
;
;   For sign extension from 24 to 32 bits
;
;   PEAK cycles:
;           2 cycle
;
;   Pseudo code:
;       ========== C ==========
;       // set(bits 25 to 32 LOW)
;       if (24th bit == 1)
;           set(bits 25 to 32 HIGH)
;       =======================
;
;   Parameters:
;           sample_reg
;
;   Returns:
;           sample_reg
;************************************************************************************
m_sign_ext_24to32_bits      .macro  sample_reg
    ; ldi    sample_reg.b3, 0x00
    qbbc   SKIP_SIGN_EXT?, sample_reg, 23    ; bit position: (1<<23)
    ldi    sample_reg.b3, 0xFF
SKIP_SIGN_EXT?:
    .endm

    .endif  ; __adc_preprocess_macros_h
