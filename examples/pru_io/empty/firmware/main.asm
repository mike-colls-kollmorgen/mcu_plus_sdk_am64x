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
;   File:     main.asm
;
;   Brief:    Template asm file example
;************************************************************************************

; CCS/makefile specific settings
    .retain     ; Required for building .out with assembly file
    .retainrefs ; Required for building .out with assembly file

    .global     main
    .sect       ".text"

;************************************* includes *************************************
; icss_constant_defines.inc: Defines symbols corresponding to Constant Table Entries
    .include "icss_constant_defines.inc"

    .asg	R2,	TEMP_REG

;********
;* MAIN *
;********

main:

init:
    zero	&r0, 120  ; Clear the register space

;----------------------------------------------------------------------------
;   Constant Table Entries Configuration
;   Sample code to configure Constant Table Entries.
;----------------------------------------------------------------------------

; Configure the Constant Table entry C28 to point to start of shared memory
; PRU_ICSSG Shared RAM (local-C28) : 00nn_nn00h, nnnn = c28_pointer[15:0]
    ldi     TEMP_REG, 0x0100
    sbco    &TEMP_REG, ICSS_PRU_CTRL_CONST, 0x28, 2

;----------------------------------------------------------------------------
;   Writing to PRU memories
;   Sample code to write to DMEM and SMEM.
;----------------------------------------------------------------------------

; Write 4 byte register value to DMEM0 at offset 0x14
    sbco    &TEMP_REG, ICSS_DMEM0_CONST, 0x14, 4

; Write 3 byte register value to DMEM1 at offset 0x10
    sbco    &TEMP_REG, ICSS_DMEM1_CONST, 0x10, 3

; Write 2 byte register value to SMEM0 at offset 0x22
    sbco    &TEMP_REG, ICSS_SMEM_CONST,  0x22, 2

    halt ; end of program
