--- uboot.org/vendors/ami/IPMI/IPMIDefs.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot/vendors/ami/IPMI/IPMIDefs.h	2007-04-04 17:03:28.000000000 -0400
@@ -0,0 +1,159 @@
+/****************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2005-2006, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        6145-F, Northbelt Parkway, Norcross,                **
+ **                                                            **
+ **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ ****************************************************************
+ ****************************************************************
+ ******************************************************************
+ *
+ * ipmi_defs.h
+ * IPMI Definitions
+ *
+ *  Author: Govind Kothandapani <govindk@ami.com>
+ *
+ ******************************************************************/
+#ifndef IPMI_DEFS_H
+#define IPMI_DEFS_H
+
+/*------- Net Functions  -------------------*/
+#define NETFN_CHASSIS					0x00
+#define NETFN_BRIDGE					0x02
+#define NETFN_SENSOR					0x04
+#define NETFN_APP					0x06
+#define NETFN_FIRMWARE					0x08
+#define NETFN_STORAGE					0x0A
+#define NETFN_TRANSPORT					0x0C
+#define NETFN_GROUP_EXT					0x2C
+#define NETFN_OEM_AMI					0x32
+
+/**** Command Completion Codes ****/
+#define	CC_NORMAL										0x00
+#define	CC_SUCCESS										0x00
+#define CC_NODE_BUSY									0xC0
+#define CC_INV_CMD										0xC1
+#define CC_INV_CMD_FOR_LUN								0xC2
+#define CC_TIMEOUT										0xC3
+#define CC_OUT_OF_SPACE									0xC4
+#define CC_INV_RESERVATION_ID							0xC5
+#define CC_REQ_TRUNCATED								0xC6
+#define CC_REQ_INV_LEN									0xC7
+#define CC_REQ_FIELD_LEN_EXCEEDED						0xC8
+#define CC_PARAM_OUT_OF_RANGE							0xC9
+#define CC_CANNOT_RETURN_REQ_BYTES						0xCA
+#define CC_SEL_REC_NOT_PRESENT							0xCB
+#define CC_SDR_REC_NOT_PRESENT							0xCB
+#define CC_FRU_REC_NOT_PRESENT							0xCB
+#define CC_INV_DATA_FIELD								0xCC
+#define CC_ILLEGAL_CMD_FOR_SENSOR_REC					0xCD
+#define CC_COULD_NOT_PROVIDE_RESP						0xCE
+#define CC_CANNOT_EXEC_DUPL_REQ							0xCF
+#define CC_SDR_REP_IN_UPDATE_MODE						0xD0
+#define CC_DEV_IN_FIRMWARE_UPDATE_MODE					0xD1
+#define CC_INIT_AGENT_IN_PROGRESS						0xD2
+#define CC_DEST_UNAVAILABLE								0xD3
+#define CC_INSUFFIENT_PRIVILEGE							0xD4
+#define CC_PARAM_NOT_SUP_IN_CUR_STATE					0xD5
+#define CC_UNSPECIFIED_ERR								0xFF
+#define CC_GET_MSG_QUEUE_EMPTY							0x80
+#define	CC_EVT_MSG_QUEUE_EMPTY							0x80
+#define CC_GET_SESSION_INVALID_USER						0x81
+#define CC_GET_SESSION_NULL_USER_DISABLED				0x82
+#define CC_ACTIVATE_SESS_NO_SESSION_SLOT_AVAILABLE		0x81
+#define CC_ACTIVATE_SESS_NO_SLOT_AVAILABLE_USER			0x82
+#define CC_ACTIVATE_SESS_REQ_LEVEL_EXCEEDS_LIMIT		0x83
+#define CC_ACTIVATE_SESS_INVALID_SESSION_ID				0x85
+#define CC_ACTIVATE_SESS_MAX_PRIVILEGE_EXCEEDS_LIMIT	0x86
+#define	CC_PASSWORD_TEST_FAILED							0x80
+#define	CC_SETPASSWORD_INVALID_USERID					0x81
+#define	CC_SETPASSWORD_CANNOT_DISABLE_USER				0x82
+#define	CC_NO_ACK_FROM_SLAVE							0x83
+#define	CC_GET_CH_COMMAND_NOT_SUPPORTED					0x82
+#define	CC_SET_CH_COMMAND_NOT_SUPPORTED					0x82
+#define	CC_SET_CH_ACCES_MODE_NOT_SUPPORTED				0x83
+#define	CC_SET_SESS_PREV_REQ_LEVEL_NOT_AVAILABLE		0x80
+#define	CC_SET_SESS_PREV_REQ_PRIVILEGE_EXCEEDS_LIMIT	0x81
+#define	CC_SET_SESS_PREV_INVALID_SESSION_ID				0x82
+#define CC_ACTIVATE_SESS_NO_SLOT_AVAILABLE_USER			0x82
+#define	CC_CLOSE_INVALID_SESSION_ID						0x87
+#define	CC_PEF_PARAM_NOT_SUPPORTED						0x80
+#define	CC_PEF_SET_IN_PROGRESS							0x81
+#define CC_PEF_ATTEMPT_TO_SET_READ_ONLY_PARAM			0x82
+#define	CC_SET_IN_PROGRESS								0x81
+#define CC_ATTEMPT_TO_SET_RO_PARAM						0x82
+#define	CC_PARAM_NOT_SUPPORTED							0x80
+#define CC_BIOS_NOT_READY								0x82
+#define CC_IFC_NOT_SUPPORTED							0x81
+#define CC_DISABLE_SM									0x81
+#define CC_BIOS_IS_BUSY									0x81
+
+#define CC_CALLBACK_REJ_SESSION_ACTIVE					0x82
+#define CC_WRITE_ONLY_PARAM								0x83
+
+#define CC_INST_ALREADY_ACTIVE							0x80
+#define CC_CANNOT_ACTIVATE_WITH_ENCR					0x83
+#define CC_PAYLOAD_NOT_ENABLED							0x81
+#define CC_INST_EXCEEDED								0x82
+#define CC_INST_ALREADY_INACTIVE						0x80
+#define CC_ENCRYPT_NOT_AVAILABLE						0x82
+#define CC_INST_NOT_ACTIVE								0x83
+#define	CC_KEYS_LOCKED									0x80
+#define	CC_INSUF_KEY_BYTES								0x81
+#define	CC_TOO_MANY_KEY_BYTES							0x82
+#define	CC_KEY_MISMATCH									0x83
+
+#define CC_OP_NOT_SUPPORTED								0x80
+#define CC_OP_NOT_ALLOWED								0x81
+
+#define CC_ATTEMPT_TO_RESET_UNIN_WATCHDOG 					0x80
+
+#define	CC_ILLEGAL_CONNECTOR_ID							0x81
+
+#define IPMI_EVM_REVISION								0x04
+#define IPMI_SENSOR_TEMP_TYPE							0x01
+#define IPMI_SENSOR_VOLT_TYPE							0x02
+#define IPMI_SENSOR_FAN_TYPE							0x04
+#define IPMI_SENSOR_PHYSICAL_SECURITY_TYPE					0x05
+#define IPMI_SENSOR_POWER_SUPPLY_TYPE						0x08
+#define IPMI_SENSOR_MEMORY_TYPE							0x0C
+#define IPMI_SENSOR_DRIVE_TYPE							0x0D
+#define IPMI_SENSOR_BUTTON_TYPE							0x14
+#define IPMI_SENSOR_ENTITY_PRESENCE_TYPE					0x25
+#define IPMI_SENSOR_OEM_TYPE							0xC0
+
+/******************OEM Completion codes*********************/
+#define OEMCC_INVALID_USERNAME							0x01
+#define OEMCC_PASSWD_MISMATCH							0x02
+#define OEMCC_INVALID_PASSWD							0x03
+#define OEMCC_DUPLICATE_USERNAME						0x04
+#define OEMCC_USER_EXISTS_AT_SLOT						0x05
+#define OEMCC_NOMEM										0x06
+#define OEMCC_FILE_ERR									0x07
+#define OEMCC_SSHKEY_UPDATE_FAILURE						0x08
+#define OEMCC_SENSOR_DISABLED							0x09
+#define OEMCC_INVALID_SDR_ENTRY							0x0a
+#define OEMCC_CORRUPT_FLASH_DATA						0x0b
+#define OEMCC_CORRUPT_DATA_CHKSUM						0x0c
+#define OEMCC_FLASH_UPGRADE_FAILURE						0x0d
+#define OEMCC_VERSION_MISMATCH							0x0e
+
+/*--------------------------------------------
+ * Macro to extract the net function.
+ *--------------------------------------------*/
+#define NET_FN(NetFnLUN)	((INT8U)((NetFnLUN & 0xFC) >> 2))
+
+#define htoipmi_u16(val)	(val)
+#define htoipmi_u32(val)	(val)
+
+#define ipmitoh_u16(val)	(val)
+#define ipmitoh_u32(val)	(val)
+
+
+#endif	/* IPMI_DEFS_H */
