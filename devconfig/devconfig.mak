# Device type (HS/GP)
DEVICE_TYPE?=GP

# Path to the signing tools, keys etc
SIGNING_TOOL_PATH=$(MCU_PLUS_SDK_PATH)/tools/boot/signing

# Path to the keys
ROM_DEGENERATE_KEY:=$(SIGNING_TOOL_PATH)/rom_degenerateKey.pem
CUST_MPK=$(SIGNING_TOOL_PATH)/custMpk_am64x_am243x.pem
CUST_MEK=$(SIGNING_TOOL_PATH)/custMek_am64x_am243x.txt

# Encryption option (yes/no)
ENC_ENABLED?=no

# Generic macros to be used depending on the device type
APP_SIGNING_KEY=
APP_ENCRYPTION_KEY=

ifeq ($(DEVICE_TYPE),HS)
	APP_SIGNING_KEY=$(CUST_MPK)
	APP_ENCRYPTION_KEY=$(CUST_MEK)
else
	APP_SIGNING_KEY=$(ROM_DEGENERATE_KEY)
endif