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
 * \defgroup tisci_firewall tisci_firewall
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
 * TISCI Data structures and macros for firewall messages
 *
 */

#ifndef TISCI_FIREWALL_H_
#define TISCI_FIREWALL_H_


/**
 * \brief Request for a change owner operation
 *
 * \param hdr Common TI-SCI header
 *
 * \param fwl_id firewall ID in question
 * \param region Region or channel number if applicable
 * \param owner_index New owner index to transfer ownership to
 */
struct tisci_msg_fwl_change_owner_info_req {
    struct tisci_header    hdr;
    uint16_t            fwl_id;
    uint16_t            region;
    uint8_t            owner_index;
} __attribute__((__packed__));

/**
 * \brief Response for a change owner operation
 *
 * \param hdr Common TI-SCI header
 *            ACK response indicates that change owner operation is succesful.
 *            NACK response indicates that change owner operation failed.
 *
 * \param fwl_id firewall ID specified in request
 * \param region Region or channel number specified in request
 * \param owner_index owner index specified in request
 * \param owner_privid new owner priv-ID returned by DMSC. This field is currently
 *        initialized to zero by DMSC.
 * \param owner_permission_bits new owner permission bits returned by DMSC.
 *        This field is currently initialized to zero by DMSC.
 */
struct tisci_msg_fwl_change_owner_info_resp {
    struct tisci_header    hdr;
    uint16_t            fwl_id;
    uint16_t            region;
    uint8_t            owner_index;
    uint8_t            owner_privid;
    uint16_t            owner_permission_bits;
} __attribute__((__packed__));

/**
 * \brief Request for configuring the firewall permissions.
 *
 * \param hdr Common TI-SCI header
 *
 * \param fwl_id firewall ID in question. fwl_id is defined in the TRM.
 *
 * \param region Region or channel number to set config info
 *   This field is unused in case of a simple firewall  and must be initialized
 *   to zero.  In case of a region based firewall, this field indicates the
 *   region in question. (index starting from 0) In case of a channel based
 *   firewall, this field indicates the channel in question (index starting
 *   from 0)
 *
 * \param n_permission_regs Number of permission registers to set
 *
 * \param control Contents of the firewall CONTROL register to set
 *
 * \param permissions Contents of the firewall PERMISSION register to set
 *
 * \param start_address Contents of the firewall START_ADDRESS register to set
 *
 * \param end_address Contents of the firewall END_ADDRESS register to set
 */

struct tisci_msg_fwl_set_firewall_region_req {
    struct tisci_header    hdr;
    uint16_t            fwl_id;
    uint16_t            region;
    uint32_t            n_permission_regs;
    uint32_t            control;
    uint32_t            permissions[FWL_MAX_PRIVID_SLOTS];
    uint64_t        start_address;
    uint64_t        end_address;
} __attribute__((__packed__));

/**
 * \brief Response for configuring the firewall permissions.
 *
 * \param hdr Generic TISCI message header.
 *      ACK response indicates that firewall configuration is succesful.
 *      NACK response indicates that the configuration was not performed.
 */
struct tisci_msg_fwl_set_firewall_region_resp {
    struct tisci_header hdr;
} __attribute__((__packed__));

/**
 * \brief Request for retrieving the firewall permissions.
 *
 * \param hdr Generic TISCI message header.
 *
 * \param fwl_id firewall ID in question. fwl_id is defined in the TRM.
 *
 * \param region Region or channel number to set config info
 *
 *   This field is unused in case of a simple firewall  and must be initialized
 *   to zero.  In case of a region based firewall, this field indicates the
 *   region in question (index starting from 0). In case of a channel based
 *   firewall, this field indicates the channel in question (index starting
 *   from 0).
 *
 * \param n_permission_regs Number of permission registers to retrieve
 */
struct tisci_msg_fwl_get_firewall_region_req {
    struct tisci_header    hdr;
    uint16_t            fwl_id;
    uint16_t            region;
    uint32_t            n_permission_regs;
} __attribute__((__packed__));

/**
 * \brief Response for retrieving the firewall permissions.
 *
 * \param hdr Generic TISCI message header.
 *      A ACK response in the hdr indicates that the message was processed.
 *      successfully.
 *     A NACK response indicates failure to retrieve firewall permissions.
 *
 * \param fwl_id firewall ID in question. fwl_id is defined in the TRM.
 *
 * \param region Region or channel number to set config info This field is
 * unused in case of a simple firewall  and must be initialized to zero.  In
 * case of a region based firewall, this field indicates the region in
 * question. (index starting from 0) In case of a channel based firewall, this
 * field indicates the channel in question (index starting from 0)
 *
 * \param n_permission_regs Number of permission registers retrieved
 *
 * \param control Contents of the firewall CONTROL register
 *
 * \param permissions Contents of the firewall PERMISSION registers
 *
 * \param start_address Contents of the firewall START_ADDRESS register
 * This is not applicable for channelized firewalls.
 *
 * \param end_address Contents of the firewall END_ADDRESS register
 * This is not applicable for channelized firewalls.
 */

struct tisci_msg_fwl_get_firewall_region_resp {
    struct tisci_header    hdr;
    uint16_t            fwl_id;
    uint16_t            region;
    uint32_t            n_permission_regs;
    uint32_t            control;
    uint32_t            permissions[FWL_MAX_PRIVID_SLOTS];
    uint64_t        start_address;
    uint64_t        end_address;
} __attribute__((__packed__));

#endif

/** @} */
