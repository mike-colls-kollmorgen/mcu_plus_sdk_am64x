# Script to add x509 certificate to SBL
# for combined boot - SBL+SYSFW+
# The certificate used will be ROM format
#
# Python 3 script

import argparse
import os
import subprocess
from re import sub
from random import randint
from shutil import copyfileobj
from textwrap import dedent

g_sha_to_use = "sha512"

g_sha_oids = {
	"sha256" : "2.16.840.1.101.3.4.2.1",
	"sha384" : "2.16.840.1.101.3.4.2.2",
	"sha512" : "2.16.840.1.101.3.4.2.3",
	"sha224" : "2.16.840.1.101.3.4.2.4",
}

g_dbg_types = {
	"DBG_PERM_DISABLE"   : '0', 
	"DBG_SOC_DEFAULT"    : '1', 
	"DBG_PUBLIC_ENABLE"  : '2',
	"DBG_FULL_ENABLE"    : '4',
}

g_x509_template = '''
[ req ]
distinguished_name     = req_distinguished_name
x509_extensions        = v3_ca
prompt                 = no

dirstring_type = nobmp

[ req_distinguished_name ]
C                      = US
ST                     = SC
L                      = New York
O                      = Texas Instruments., Inc.
OU                     = DSP
CN                     = Albert
emailAddress           = Albert@gt.ti.com

[ v3_ca ]
basicConstraints = CA:true
1.3.6.1.4.1.294.1.3 = ASN1:SEQUENCE:swrv
1.3.6.1.4.1.294.1.9 = ASN1:SEQUENCE:ext_boot_info
{DBG_SEQ}
{EXT_ENC_SEQ}

[ swrv ]
swrv = INTEGER:{SWRV}

[ ext_boot_info ]
extImgSize = INTEGER:{EXT_IMAGE_SIZE}
numComp = INTEGER:{NUM_COMP}
sbl = SEQUENCE:sbl
fw = SEQUENCE:sysfw
{SYSFW_INNER_CERT_EXT}
bd2 = SEQUENCE:boardcfg

[ sbl ]

compType = INTEGER:1
bootCore = INTEGER:16
compOpts = INTEGER:0
destAddr = FORMAT:HEX,OCT:{SBL_LOAD_ADDR}
compSize = INTEGER:{SBL_SIZE}
shaType = OID:{SHA_OID}
shaValue = FORMAT:HEX,OCT:{SBL_SHA_VAL}

[ sysfw ]

compType = INTEGER:2
bootCore = INTEGER:0
compOpts = INTEGER:0
destAddr = FORMAT:HEX,OCT:{SYSFW_LOAD_ADDR}
compSize = INTEGER:{SYSFW_SIZE}
shaType = OID:{SHA_OID}
shaValue = FORMAT:HEX,OCT:{SYSFW_SHA_VAL}

{SYSFW_INNER_CERT_SEQ}

[ boardcfg ]

compType = INTEGER:18
bootCore = INTEGER:0
compOpts = INTEGER:0
destAddr = FORMAT:HEX,OCT:{BOARDCFG_LOAD_ADDR}
compSize = INTEGER:{BOARDCFG_SIZE}
shaType = OID:{SHA_OID}
shaValue = FORMAT:HEX,OCT:{BOARDCFG_SHA_VAL}
'''

g_inner_cert_ext = 'bd1 = SEQUENCE:sysfw_inner_cert'

g_inner_cert_seq = '''
[ sysfw_inner_cert ]

compType = INTEGER:3
bootCore = INTEGER:0
compOpts = INTEGER:0
destAddr = FORMAT:HEX,OCT:00000000
compSize = INTEGER:{SYSFW_INNER_CERT_SIZE}
shaType = OID:{SHA_OID}
shaValue = FORMAT:HEX,OCT:{SYSFW_INNER_CERT_SHA_VAL}
'''

g_dbg_seq = '''
[ debug ]
debugUID     =  FORMAT:HEX,OCT:{DBG_DEVICE}
debugType    =  INTEGER:{DBG_TYPE}
coreDbgEn    =  INTEGER:0
coreDbgSecEn =  INTEGER:0
'''

def get_sha_val(f_name, sha_type):
	sha_val = subprocess.check_output('openssl dgst -{} -hex {}'.format(sha_type, f_name), shell=True).decode()
	return sub("^.*= ", r'', sha_val).strip('\n')

def get_cert(args):
	swrev = args.swrv

	if(swrev is None):
		# Default to 1
		swrev = 1

	dbg_seq = ''

	if(args.debug is not None):
		if(args.debug in g_dbg_types):
			# all good
				dbg_seq = "1.3.6.1.4.1.294.1.8 = ASN1:SEQUENCE:debug"
		else:
			# Invalid debug extension, exit fail
			print("Invalid debug extension, exiting ...")
			exit(2)

	inner_c_ext = ''
	inner_c_seq = ''
	num_comp = 3

	full_image_size = os.path.getsize(args.sbl_bin) + os.path.getsize(args.sysfw_bin) + os.path.getsize(args.boardcfg_blob)

	if(args.sysfw_inner_cert is not None):
		if(not os.path.exists(args.sysfw_inner_cert)):
			# Invalid inner cert
			print("SYSFW Inner certificate file does not exist")
			exit(2)
		else:
			full_image_size += os.path.getsize(args.sysfw_inner_cert)
			num_comp += 1
			inner_c_ext = g_inner_cert_ext
			inner_c_seq = g_inner_cert_seq.format(
							SYSFW_INNER_CERT_SIZE = os.path.getsize(args.sysfw_inner_cert),
							SHA_OID = g_sha_oids[g_sha_to_use],
							SYSFW_INNER_CERT_SHA_VAL = get_sha_val(args.sysfw_inner_cert, g_sha_to_use),
							)

	ret_cert = g_x509_template.format(
				DBG_SEQ = dbg_seq,
				EXT_ENC_SEQ = '',
				NUM_COMP = num_comp,
				SHA_OID = g_sha_oids[g_sha_to_use],
				SWRV = swrev,
				SBL_LOAD_ADDR = '{:08X}'.format(int(args.sbl_loadaddr, 16)),
				SBL_SIZE = os.path.getsize(args.sbl_bin),
				SBL_SHA_VAL = get_sha_val(args.sbl_bin, g_sha_to_use),
				SYSFW_LOAD_ADDR = '{:08X}'.format(int(args.sysfw_loadaddr, 16)),
				SYSFW_SIZE = os.path.getsize(args.sysfw_bin),
				SYSFW_SHA_VAL = get_sha_val(args.sysfw_bin, g_sha_to_use),
				SYSFW_INNER_CERT_EXT = inner_c_ext,
				SYSFW_INNER_CERT_SEQ = inner_c_seq,
				BOARDCFG_LOAD_ADDR = '{:08X}'.format(int(args.bcfg_loadaddr, 16)),
				BOARDCFG_SIZE = os.path.getsize(args.boardcfg_blob),
				BOARDCFG_SHA_VAL = get_sha_val(args.boardcfg_blob, g_sha_to_use),
				EXT_IMAGE_SIZE = full_image_size,
				)

	if(dbg_seq != ''):
		ret_cert += g_dbg_seq.format(
					DBG_DEVICE='0000000000000000000000000000000000000000000000000000000000000000',
					DBG_TYPE=g_dbg_types[args.debug],
					)

	return dedent(ret_cert)

# MAIN
my_parser = argparse.ArgumentParser(description="Creates a ROM-boot-able combined image when the SBL, SYSFW and BoardCfg data are provided")

my_parser.add_argument('--swrv',             type=str, help='Software revision number')
my_parser.add_argument('--sbl-bin',          type=str, required=True, help='Path to the SBL binary')
my_parser.add_argument('--sysfw-bin',        type=str, required=True, help='Path to the sysfw binary')
my_parser.add_argument('--sysfw-inner-cert', type=str, help='Path to the sysfw inner certificate')
my_parser.add_argument('--boardcfg-blob',    type=str, required=True, help='Path to the boardcfg blob')
my_parser.add_argument('--sbl-loadaddr',     type=str, required=True, help='Load address at which SBL needs to be loaded')
my_parser.add_argument('--sysfw-loadaddr',   type=str, required=True, help='Load address at which SYSFW needs to be loaded')
my_parser.add_argument('--bcfg-loadaddr',    type=str, required=True, help='Load address at which BOARDCFG needs to be loaded')
my_parser.add_argument('--key',              type=str, required=True, help='Path to the signing key to be used while creating the certificate')
my_parser.add_argument('--rom-image',        type=str, required=True, help='Output file combined ROM image of SBL+SYSFW+Boardcfg')
my_parser.add_argument('--debug',            type=str, help='Debug options for the image')

args = my_parser.parse_args()

cert_str = get_cert(args)
# print(cert_str)

cert_file_name = "temp_cert"+str(randint(111, 999))

with open(cert_file_name, "w+") as f:
	f.write(cert_str)

cert_name = "cert"+str(randint(111, 999))

out_name = args.rom_image

# Generate the certificate
subprocess.check_output('openssl req -new -x509 -key {} -nodes -outform DER -out {} -config {} -{}'.format(args.key, cert_name, cert_file_name, g_sha_to_use), shell=True)

# Concatenate the certificate, SBL, SYSFW, and Boardcfg, SYSFW Inner Cert
final_fh = open(args.rom_image, 'wb+')
cert_fh = open(cert_name, 'rb')
sbl_fh = open(args.sbl_bin, 'rb')
sysfw_fh = open(args.sysfw_bin, 'rb')
if(args.sysfw_inner_cert is not None and os.path.exists(args.sysfw_inner_cert)):
	sysfw_inner_cert_fh = open(args.sysfw_inner_cert, 'rb')
bcfg_fh = open(args.boardcfg_blob, 'rb')

copyfileobj(cert_fh, final_fh)
copyfileobj(sbl_fh, final_fh)
copyfileobj(sysfw_fh, final_fh)
if(args.sysfw_inner_cert is not None and os.path.exists(args.sysfw_inner_cert)):
	copyfileobj(sysfw_inner_cert_fh, final_fh)
copyfileobj(bcfg_fh, final_fh)

final_fh.close()
cert_fh.close()
sbl_fh.close()
sysfw_fh.close()
bcfg_fh.close()
if(args.sysfw_inner_cert is not None and os.path.exists(args.sysfw_inner_cert)):
	sysfw_inner_cert_fh.close()

# Delete the temporary files
os.remove(cert_file_name)
os.remove(cert_name)