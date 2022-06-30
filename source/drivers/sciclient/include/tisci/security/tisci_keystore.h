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
 * \defgroup tisci_keystore tisci_keystore
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
 * TISCI Data structures and macros for runtime keystore
 *
 */
#ifndef TISCI_KEYSTORE_H_
#define TISCI_KEYSTORE_H_


/** Keystore Major ABI revision value */
#define KEYSTORE_ABI_MAJ_VALUE                  0x00
/** Keystore Minor ABI revision value */
#define KEYSTORE_ABI_MIN_VALUE                  0x01

/**
 * \brief Request for writing the entire keystore contents
 *
 *  A user may unlock the keystore to write its contents using a signed and
 *  encrypted keystore structure. This operation is required for keystore
 *  initialization and is mandatorily the first keystore operation which must be
 *  executed in order to put the keystore in a valid state for subsequent
 *  operations.
 *
 * \param hdr Common TI-SCI header
 *
 * \param keystore_lo Lower 32-bit memory location of the signed and
 *                    encrypted keystore contents
 * \param keystore_hi Higher 32-bit memory location of the signed and
 *                    encrypted keystore contents
 */
struct tisci_msg_keystore_write_req {
    struct tisci_header    hdr;
    uint32_t            keystore_lo;
    uint32_t            keystore_hi;
} __attribute__((__packed__));

/**
 * \brief Response to the keystore write
 *
 * \param hdr Generic TISCI message header. ACK response indicates that the
 *            accompanying certificate was validated and the keystore contents
 *            were successfully decrypted, authenticated, and copied to the
 *            internal keystore memory. NACK response indicates that the
 *            keystore write could not be completed.
 */
struct tisci_msg_keystore_write_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));

/**
 * \brief Request for exporting the entire keystore contents
 *
 *  The host will make the request to encrypt and export the entire keystore
 *  contents for saving to some storage mechanism to be made available for
 *  future re-import. Only the host who owns the keystore structure may execute
 *  this operation. The host is responsible for pre-allocating enough memory to
 *  fit the keystore structure and all encryption parameters.
 *
 * \param hdr Common TI-SCI header
 *
 * \param keystore_lo Lower 32-bit memory location where the encrypted keystore
 *                    contents will be placed to pass back to the host
 * \param keystore_hi Higher 32-bit memory location where the encrypted keystore
 *                    contents will be placed to pass back to the host
 */
struct tisci_msg_keystore_export_req {
    struct tisci_header    hdr;
    uint32_t            keystore_lo;
    uint32_t            keystore_hi;
} __attribute__((__packed__));

/**
 * \brief Response to the keystore export
 *
 * \param hdr Generic TISCI message header. ACK response indicates that the
 *            keystore has been encrypted and copied into the host-provided
 *            memory. NACK response indicates that the keystore export could not
 *            be completed
 */
struct tisci_msg_keystore_export_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));

/**
 * \brief Request for importing a previously exported keystore
 *
 * \param hdr Common TI-SCI header
 *
 * \param keystore_lo Lower 32-bit memory location of the encrypted keystore
 *                    contents provided by the host
 * \param keystore_hi Higher 32-bit memory location of the encrypted keystore
 *                    contents provided by the host
 */
struct tisci_msg_keystore_import_req {
    struct tisci_header    hdr;
    uint32_t            keystore_lo;
    uint32_t            keystore_hi;
} __attribute__((__packed__));

/**
 * \brief Response to the keystore import
 *
 * \param hdr Generic TISCI message header. ACK response indicates that the
 *            imported keystore has been successfully decrypted and
 *            authenticated. NACK response indicates that the keystore import
 *            could not be completed.
 */
struct tisci_msg_keystore_import_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));

#endif /* TISCI_KEYSTORE_H_ */

/** @} */
