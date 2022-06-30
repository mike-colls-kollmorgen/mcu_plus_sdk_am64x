/********************************************************************
 * Copyright (C) 2003-2019 Texas Instruments Incorporated.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
*/
/*********************************************************************
* file: cslr_cpsgmii.h
*
* Brief: This file contains the Register Description for cpsgmii
*
*********************************************************************/
#ifndef CSLR_CPSGMII_TOP_H
#define CSLR_CPSGMII_TOP_H


#if defined(SOC_K2K)||defined(SOC_K2H)||defined(SOC_C6678)

#include <sgmii/V0/cslr_cpsgmii.h>

#elif defined(SOC_AM65XX)||defined(SOC_J721E) || defined (SOC_J74202) || defined (SOC_J721S2)||defined(SOC_J7200) || defined(SOC_AM64X) || defined(SOC_AM243X) || defined (SOC_AM62X)

#include <sgmii/V5/cslr_cpsgmii.h>

#elif defined(SOC_C6657)

#include <sgmii/V3/cslr_sgmii.h>

#elif defined(SOC_K2E) ||defined(SOC_K2L) || defined(SOC_K2G)

#include <sgmii/V1/cslr_cpsgmii.h>

#endif /* SOC_XXXXX */

#endif /* CSLR_CPSGMII_TOP_H */
