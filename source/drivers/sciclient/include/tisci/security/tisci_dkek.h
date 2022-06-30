/*
 *  Copyright (C) 2017-2022 Texas Instruments Incorporated
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
/**
 * \ingroup TISCI
 * \defgroup tisci_dkek tisci_dkek
 *
 * DMSC controls the power management, security and resource management
 * of the device.
 *
 *
 * @{
 */
/**
 *
 *  \brief  This file contains:
 *
 *          WARNING!!: Autogenerated file from SYSFW. DO NOT MODIFY!!
 * System Firmware
 *
 * TISCI Data structures and macros for DKEK access
 *
 */
#ifndef TISCI_DKEK_H_
#define TISCI_DKEK_H_


/**
 * \brief Derived key length 32 bytes always
 */
#define SA2UL_DKEK_KEY_LEN (32U)

/**
 * \brief key derivation label and context length
 */
#define KDF_LABEL_AND_CONTEXT_LEN_MAX       41U

/**
 * \brief TISCI Request for setting DKEK in SA2UL registers
 * \param hdr Generic TISCI message header.
 *
 * \param sa2ul_instance SA2UL instance number - set to 0
 * \param kdf_label_len Length of "Label" input to KDF
 * \param kdf_context_len Length of "Context" input to KDF
 * \param kdf_label_and_context "Label" and "Context" bytes are stored in this
 *                              array one after another. Maximum length of
 *                              this field is KDF_LABEL_AND_CONTEXT_LEN_MAX
 */
struct tisci_msg_sa2ul_set_dkek_req {
    struct tisci_header    hdr;
    uint8_t            sa2ul_instance;
    uint8_t            kdf_label_len;
    uint8_t            kdf_context_len;
    uint8_t            kdf_label_and_context[KDF_LABEL_AND_CONTEXT_LEN_MAX];
} __attribute__((__packed__));

/**
 * \brief TISCI Response for setting DKEK in SA2UL registers
 * \param hdr Generic TISCI message header.
 */
struct tisci_msg_sa2ul_set_dkek_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));

/**
 * \brief TISCI Request for releasing DKEK
 * \param hdr Generic TISCI message header.
 *
 * \param sa2ul_instance SA2UL instance number - set to 0
 */
struct tisci_msg_sa2ul_release_dkek_req {
    struct tisci_header    hdr;
    uint8_t            sa2ul_instance;
} __attribute__((__packed__));

/**
 * \brief TISCI Response for Releasing DKEK
 * \param hdr Generic TISCI message header.
 */
struct tisci_msg_sa2ul_release_dkek_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));

/**
 * \brief TISCI Request for getting DKEK via TISCI
 * \param hdr Generic TISCI message header.
 *
 * \param sa2ul_instance SA2UL instance number - set to 0
 * \param kdf_label_len Length of "Label" input to KDF
 * \param kdf_context_len Length of "Context" input to KDF
 * \param kdf_label_and_context "Label" and "Context" bytes are stored in this
 *                              array one after another. Maximum length of
 *                              this field is KDF_LABEL_AND_CONTEXT_LEN_MAX
 */
struct tisci_msg_sa2ul_get_dkek_req {
    struct tisci_header    hdr;
    uint8_t            sa2ul_instance;
    uint8_t            kdf_label_len;
    uint8_t            kdf_context_len;
    uint8_t            kdf_label_and_context[KDF_LABEL_AND_CONTEXT_LEN_MAX];
} __attribute__((__packed__));

/**
 * \brief TISCI Response for requesting DKEK
 * \param hdr Generic TISCI message header.
 * \param dkek Array containing Derived KEK. Length is 32 bytes
 */
struct tisci_msg_sa2ul_get_dkek_resp {
    struct tisci_header    hdr;
    uint8_t            dkek[SA2UL_DKEK_KEY_LEN];
} __attribute__((__packed__));

#endif

/** @} */