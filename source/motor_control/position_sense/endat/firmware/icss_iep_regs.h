
;
; Copyright (C) 2021 Texas Instruments Incorporated
;
; Redistribution and use in source and binary forms, with or without
; modification, are permitted provided that the following conditions
; are met:
;
;   Redistributions of source code must retain the above copyright
;   notice, this list of conditions and the following disclaimer.
;
;   Redistributions in binary form must reproduce the above copyright
;   notice, this list of conditions and the following disclaimer in the
;   documentation and/or other materials provided with the
;   distribution.
;
;   Neither the name of Texas Instruments Incorporated nor the names of
;   its contributors may be used to endorse or promote products derived
;   from this software without specific prior written permission.
;
; THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
; "AS IS" AND ANY EXPgResS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
; LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
; A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
; OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
; SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
; LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
; DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
; THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
; (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
; OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
;

	.if	!$isdefed("__icss_iep_regs_h")
__icss_iep_regs_h	.set	1

ICSS_IEP_TIMER_BLK_INDEX	.set		0

ICSS_IEP_GLOBAL_CFG_REG	.set		0x0000
ICSS_IEP_GLOBAL_STATUS_REG	.set	0x0004
ICSS_IEP_COMPEN_REG	.set			0x0008
ICSS_IEP_COUNT_REG	.set			0x000C
ICSS_IEP_CAP_CFG_REG	.set		0x0010
ICSS_IEP_CAP_STATUS_REG	.set		0x0014
ICSS_IEP_CAPR0_REG	.set			0x0018
ICSS_IEP_CAPR1_REG	.set			0x001C
ICSS_IEP_CAPR2_REG	.set			0x0020
ICSS_IEP_CAPR3_REG	.set			0x0024
ICSS_IEP_CAPR4_REG	.set			0x0028
ICSS_IEP_CAPR5_REG	.set			0x002C
ICSS_IEP_CAPR6_REG	.set			0x0030
ICSS_IEP_CAPF6_REG	.set			0x0034
ICSS_IEP_CAPR7_REG	.set			0x0038
ICSS_IEP_CAPF7_REG	.set			0x003C
ICSS_IEP_CMP_CFG_REG	.set		0x0040
ICSS_IEP_CMP_STATUS_REG	.set		0x0044
ICSS_IEP_CMP0_REG	.set			0x0048
ICSS_IEP_CMP1_REG	.set			0x004C
ICSS_IEP_CMP2_REG	.set			0x0050
ICSS_IEP_CMP3_REG	.set			0x0054
ICSS_IEP_CMP4_REG	.set			0x0058
ICSS_IEP_CMP5_REG	.set			0x005C
ICSS_IEP_CMP6_REG	.set			0x0060
ICSS_IEP_CMP7_REG	.set			0x0064
ICSS_IEP_RXIPG0_REG	.set			0x0080
ICSS_IEP_RXIPG1_REG	.set			0x0084

ICSS_IEP_SYNC_BLK_INDEX	.set					1
ICSS_IEP_SYNC_CTRL_REG	.set				0x0000
ICSS_IEP_SYNC_FIRST_STATUS_REG	.set		0x0004
ICSS_IEP_SYNC0_STATUS_REG	.set			0x0008
ICSS_IEP_SYNC1_STATUS_REG	.set			0x000C
ICSS_IEP_SYNC_PWIDTH_REG	.set  			0x0010
ICSS_IEP_SYNC0_PERIOD_REG	.set			0x0014
ICSS_IEP_SYNC1_DELAY_REG	.set			0x0018
ICSS_IEP_SYNC_START_REG	.set				0x001C

ICSS_IEP_WD_BLK_INDEX	.set					2
ICSS_IEP_WD_PREDIV_REG	.set				0x0000
ICSS_IEP_PDI_WD_TIM_REG	.set				0x0004
ICSS_IEP_PD_WD_TIM_REG	.set				0x0008
ICSS_IEP_WD_STATUS_REG	.set				0x000C
ICSS_IEP_EXP_COUNTER_REG	.set			0x0010
ICSS_IEP_WD_CTRL_REG	.set				0x0014

ICSS_IEP_DIGIO_BLK_INDEX	.set				3
ICSS_IEP_DIGIO_CTRL_REG	.set				0x0000
ICSS_IEP_DIGIO_STATUS_REG	.set			0x0004
ICSS_IEP_DIGIO_DATA_IN_REG	.set			0x0008
ICSS_IEP_DIGIO_DATA_IN_RAW_REG	.set		0x000C
ICSS_IEP_DIGIO_DATA_OUT_REG	.set			0x0010
ICSS_IEP_DIGIO_DATA_OUT_EN_REG	.set		0x0014
ICSS_IEP_DIGIO_EXP_REG	.set				0x0018

ICSS_IEP_COUNT_RESET_REG		.set	0x00A8
ICSS_IEP_PWM_REG			.set	0x00AC

	.endif
