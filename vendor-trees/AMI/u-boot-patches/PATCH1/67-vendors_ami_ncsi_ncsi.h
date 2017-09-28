--- uboot/vendors/ami/ncsi/ncsi.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.ncsi/vendors/ami/ncsi/ncsi.h	2007-09-20 17:37:23.000000000 -0400
@@ -0,0 +1,717 @@
+/****************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2005-2007, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        6145-F, Northbelt Parkway, Norcross,                **
+ **                                                            **
+ **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ ****************************************************************/
+/****************************************************************
+  Author	: Samvinesh Christopher
+
+  Module	: NC-SI Definitions
+
+  Revision	: 1.0  
+
+  Changelog : 1.0 - Initial Version  [SC]
+
+ *****************************************************************/
+#ifndef __NCSI_H__
+#define __NCSI_H__
+
+#include "types.h"
+
+#define NCSI_RETRIES		3	/* Command retries for response */
+#define NCSI_1G_SUPPORT		0
+#define NCSI_10G_SUPPORT	0
+
+/*---------------------NCSI Commands  ----------------------------*/
+#define CMD_RESPONSE						0x80
+#define CMD_REQUEST							0x00
+
+#define CMD_CLEAR_INITIAL_STATE				0x00
+#define CMD_SELECT_PACKAGE					0x01
+#define CMD_DESELECT_PACKAGE				0x02
+#define CMD_ENABLE_CHANNEL					0x03
+#define CMD_DISABLE_CHANNEL					0x04
+#define CMD_RESET_CHANNEL					0x05
+#define CMD_ENABLE_CHANNEL_TX				0x06
+#define CMD_DISABLE_CHANNEL_TX				0x07
+#define CMD_AEN_ENABLE						0x08
+#define CMD_SET_LINK						0x09
+#define CMD_GET_LINK_STATUS					0x0A
+#define CMD_SET_VLAN_FILTER					0x0B
+#define CMD_ENABLE_VLAN						0x0C
+#define CMD_DISABLE_VLAN					0x0D
+#define CMD_SET_MAC_ADDR					0x0E
+/* No Command with Number 0x0F */
+#define CMD_ENABLE_BCAST_FILTER				0x10
+#define CMD_DISABLE_BCAST_FILTER			0x11
+#define CMD_ENABLE_MCAST_FILTER				0x12
+#define CMD_DISABLE_MCAST_FILTER			0x13
+#define CMD_SET_NCSI_FLOW_CONTROL			0x14
+#define CMD_GET_VERSION_ID					0x15
+#define CMD_GET_CAPABILITIES				0x16
+#define CMD_GET_PARAMETERS					0x17
+#define CMD_GET_CONTROLLER_PKT_STATS		0x18
+#define CMD_GET_NCSI_STATS					0x19
+#define CMD_GET_NCSI_PASSTHRU_STATS			0x1A
+
+#define CMD_OEM_CMD							0x50
+
+
+
+/*---------------------NCSI Response Code ----------------------------*/
+#define COMMAND_COMPLETED					0x00
+#define COMMAND_FAILED						0x01
+#define COMMAND_UNAVAILABLE					0x02	/* Busy */
+#define COMMAND_UNSUPPORTED					0x03
+
+/*---------------------NCSI Reason Code -----------------------------*/
+#define REASON_NONE							0x00
+#define REASON_INIT_REQUIRED				0x01
+#define REASON_PARAMS_INVALID				0x02
+#define REASON_CHANNEL_NOT_READY			0x03
+#define REASON_PACKAGE_NOT_READY			0x04
+#define REASON_LENGTH_INVALID				0x05
+#define REASON_MAC_ADDR_ZERO				0x08	/* For Command 0xE*/
+
+
+/*---------------------NCSI Related defines -------------------------*/
+#define NCSI_ETHER_TYPE					0x88F8
+#define NCSI_MC_ID						0x00
+#define NCSI_HDR_REV					0x01
+#define MAC_ADDR_LEN					6
+
+#define PACKED __attribute__ ((packed))
+
+
+/*----------------------- Ethernet Header ---------------------------*/
+typedef struct
+{
+	UINT8	DestMACAddr [MAC_ADDR_LEN];
+	UINT8  	SrcMACAddr [MAC_ADDR_LEN];
+	UINT16	EtherType;
+} PACKED ETH_HDR;			/* 14 Bytes */
+
+#define MIN_ETH_SIZE		(64)	
+#define ETH_FCS_SIZE		(4)
+#define PAD_SIZE 	(MIN_ETH_SIZE - (sizeof(ETH_HDR) + ETH_FCS_SIZE))
+/*------------------------ AEN Header  -----------------------------*/
+typedef struct
+{
+	UINT8	MC_ID;		/* 0x00*/
+	UINT8	ONE;		/* 0x01*/
+	UINT8   Reserved1;	
+	UINT8	IID;		/* 0x00*/
+	UINT8   ALL_FF;		/* 0xFF*/
+	UINT8	OrgChannelID;
+	UINT8	Reserved2;  	/* 0x00*/
+	UINT8	PayLen;		/* Minimum 0x4 */
+	UINT32	Reserved3;
+	UINT32	Reserved4;
+} PACKED AEN_HDR;
+
+typedef struct
+{
+	AEN_HDR hdr;
+	UINT8	Reserved5[3];
+	UINT8	AEN_Type;
+	/* Optional Extra Data */
+	/* CheckSum */
+} PACKED AEN_PKT;
+
+
+#define AEN_TYPE_LINK_STATUS_CHANGE		0x00
+#define AEN_TYPE_LINK_STATUS_CHANGE_LEN		12
+#define AEN_TYPE_LINK_STATUS_CHANGE_CHKSUM	28
+
+#define AEN_TYPE_CONFIG_REQUIRED		0x01
+#define AEN_TYPE_CONFIG_REQUIRED_LEN		4
+#define AEN_TYPE_CONFIG_REQUIRED_CHKSUM		20
+
+#define AEN_TYPE_HOST_DRIVER_CHANGE		0x02
+#define AEN_TYPE_HOST_DRIVER_CHANGE_LEN		8
+#define AEN_TYPE_HOST_DRIVER_CHANGE_CHKSUM	24
+
+/*------------------------ NCSI Header  -----------------------------*/
+typedef struct
+{
+	UINT8	MC_ID;
+	UINT8 	HdrRev;
+	UINT8	Reserved1;
+	UINT8	I_ID;
+	UINT8   Command;
+	UINT8   CH_ID;
+	UINT16	PayloadLen;		/* 12 Bits only */
+	UINT16 	Reserved2[4];
+} PACKED NCSI_HDR;			/* 16 Bytes */
+
+#define MK_CH_ID(p,c)    ((((p) & 0x7) << 5) | ((c) & 0x1F))
+#define GET_PACKAGE_ID(id)    (((id) >> 5) & 0x7) 	
+#define GET_CHANNEL_ID(id)    ((id) & 0x1F)
+#define MAX_CHANNEL_ID	 0x1F
+#define MAX_PACKAGE_ID	 0x7 
+
+/*----------- NCSI Default (No Payload) Request and Response  -------*/
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		ResponseCode;
+	UINT16		ReasonCode;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(2+2+4+sizeof(NCSI_HDR))];
+} PACKED NCSI_DEFAULT_RES_PKT;
+
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(4+sizeof(NCSI_HDR))];
+} PACKED NCSI_DEFAULT_REQ_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef NCSI_DEFAULT_REQ_PKT CLEAR_INITIAL_STATE_REQ_PKT;
+typedef NCSI_DEFAULT_RES_PKT CLEAR_INITIAL_STATE_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT8		Reserved[3];
+	UINT8		HwArbitDisable;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(3+1+4+sizeof(NCSI_HDR))];
+} PACKED SELECT_PACKAGE_REQ_PKT;
+
+#define HW_ARBITRATION_ENABLE  0x00
+#define HW_ARBITRATION_DISABLE 0x01
+
+typedef NCSI_DEFAULT_RES_PKT SELECT_PACKAGE_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef NCSI_DEFAULT_REQ_PKT DESELECT_PACKAGE_REQ_PKT;
+typedef NCSI_DEFAULT_RES_PKT DESELECT_PACKAGE_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef NCSI_DEFAULT_REQ_PKT ENABLE_CHANNEL_REQ_PKT;
+typedef NCSI_DEFAULT_RES_PKT ENABLE_CHANNEL_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT8		Reserved[3];
+	UINT8		AllowLinkDown;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(3+1+4+sizeof(NCSI_HDR))];
+} PACKED DISABLE_CHANNEL_REQ_PKT;
+
+#define KEEP_LINK_UP  0x00
+#define ALLOW_LINK_DOWN 0x01
+
+typedef NCSI_DEFAULT_RES_PKT DISABLE_CHANNEL_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT32		Reserved;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(4+4+sizeof(NCSI_HDR))];
+} PACKED RESET_CHANNEL_REQ_PKT;
+
+typedef NCSI_DEFAULT_RES_PKT RESET_CHANNEL_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef NCSI_DEFAULT_REQ_PKT ENABLE_CHANNEL_TX_REQ_PKT;
+typedef NCSI_DEFAULT_RES_PKT ENABLE_CHANNEL_TX_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef NCSI_DEFAULT_REQ_PKT DISABLE_CHANNEL_TX_REQ_PKT;
+typedef NCSI_DEFAULT_RES_PKT DISABLE_CHANNEL_TX_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT8		Reserved[3];
+	UINT8		AEN_MC_ID;
+	UINT32		AenCtrl;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(3+1+4+4+sizeof(NCSI_HDR))];
+} PACKED AEN_ENABLE_REQ_PKT;
+
+#define ENABLE_LINK_STATUS_AEN		0x01
+#define ENABLE_CFG_REQ_AEN			0x02
+#define ENABLE_HOST_DRIVER_AEN		0x04
+
+typedef NCSI_DEFAULT_RES_PKT AEN_ENABLE_RES_PKT;
+
+/*-------------------------------------------------------------------*/
+
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT32		LinkSettings;
+	UINT32		OemLinkSettings;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(4+4+4+sizeof(NCSI_HDR))];
+} PACKED SET_LINK_REQ_PKT;
+
+#define	LINK_ENABLE_AUTO_NEG			0x0001
+#define LINK_ENABLE_10_MBPS				0x0002
+#define LINK_ENABLE_100_MBPS			0x0004
+#define LINK_ENABLE_1000_MBPS			0x0008
+#define LINK_ENABLE_10_GBPS				0x0010
+#define LINK_ENABLE_HALF_DUPLEX			0x0100
+#define LINK_ENABLE_FULL_DUPLEX			0x0200
+#define LINK_ENABLE_PAUSE_CAPS			0x0400
+#define LINK_ENABLE_ASYNC_PAUSE_CAPS	0x0800
+#define LINK_OEM_LINK_FIELD_VALID		0x1000
+
+typedef NCSI_DEFAULT_RES_PKT SET_LINK_RES_PKT;
+
+#define SET_LINK_REASON_HOST_CONFLICT		0x01
+#define SET_LINK_REASON_MEDIA_CONFLICT		0x02
+#define SET_LINK_REASON_PARAMETER_CONFLICT	0x03
+#define SET_LINK_REASON_POWERMODE_CONFLICT	0x04
+#define SET_LINK_REASON_SPEED_CONFLICT		0x05
+#define SET_LINK_REASON_HW_ACCESS_FAILED	0x06
+/*-------------------------------------------------------------------*/
+
+typedef NCSI_DEFAULT_REQ_PKT GET_LINK_STATUS_REQ_PKT;
+
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		ResponseCode;
+	UINT16		ReasonCode;
+	UINT32		LinkStatus;
+	UINT32		OtherIndicators;
+	UINT32		OEMLinkStatus;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(2+2+4+4+4+4+sizeof(NCSI_HDR))];
+} PACKED GET_LINK_STATUS_RES_PKT;
+
+/* Link Partner values valid SerDes=0 AutoNegFlag=1 and AutoNegComplete=1 */
+
+#define LINK_STATUS_UP			0x00000001
+#define LINK_AUTO_NEG_FLAG		0x00000020
+#define LINK_AUTO_NEG_COMPLETE	0x00000040
+#define LINK_PARALLEL_DET_FLAG	0x00000080
+#define LINK_PARTNER_1000TFD	0x00000200
+#define LINK_PARTNER_1000THD	0x00000400
+#define LINK_PARTNER_100T4		0x00000800
+#define LINK_PARTNER_100TXFD	0x00001000
+#define LINK_PARTNER_100TXHD	0x00002000
+#define LINK_PARTNER_10TXFD		0x00004000
+#define LINK_PARTNER_10TXHD		0x00008000
+#define LINK_TX_FLOW_FLAG		0x00010000
+#define LINK_RX_FLOW_FLAG		0x00020000
+#define LINK_SERDES				0x00100000
+#define LINK_OEM_FIELD_VALID	0x00200000
+
+#define LINK_PARTNER_FLOW_MASK	0x000C0000
+#define LINK_PARTNER_NO_PAUSE	0x00000000
+#define LINK_PARTNER_SYM_PAUSE	0x00040000
+#define LINK_PARTNER_ASYM_PAUSE	0x00080000
+#define LINK_PARTNER_BOTH_PAUSE	0x000C0000
+
+#define LINK_SPEED_DUPLEX_MASK	0x0000001E
+#define LINK_10THD			(0x1 << 1)			
+#define LINK_10TFD			(0x2 << 1)			
+#define LINK_100THD			(0x3 << 1)			
+#define LINK_100T4			(0x4 << 1)			
+#define LINK_100TFD			(0x5 << 1)			
+#define LINK_1000THD			(0x6 << 1)			
+#define LINK_1000TFD		(0x7 << 1)			
+#define LINK_10GT			(0x8 << 1)			
+
+#define OTHER_INDICATOR_HOST_UP		0x01
+
+/*-------------------------------------------------------------------*/
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		Reserved1;
+	UINT16		UserPri_CFI_VLANID;		/* 3:1:12 */
+	UINT16		Reserved2;
+	UINT8		FilterSelector;
+	UINT8		Enable;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(2+2+2+1+1+4+sizeof(NCSI_HDR))];
+} PACKED SET_VLAN_FILTER_REQ_PKT;
+
+
+typedef NCSI_DEFAULT_RES_PKT SET_VLAN_FILTER_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef struct 
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT8		Reserved[3];
+	UINT8		Mode;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(3+1+4+sizeof(NCSI_HDR))];
+} PACKED ENABLE_VLAN_REQ_PKT;
+
+#define VLAN_MODE_RESERVED			0x00
+#define VLAN_MODE_VLAN_ONLY			0x01
+#define VLAN_MODE_VLAN_NON_VLAN		0x02
+#define VLAN_MODE_ANY_VLAN_NON_VLAN	0x03
+
+
+typedef NCSI_DEFAULT_RES_PKT ENABLE_VLAN_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT DISABLE_VLAN_REQ_PKT;
+typedef NCSI_DEFAULT_RES_PKT DISABLE_VLAN_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT8		MACAddr5;
+	UINT8		MACAddr4;
+	UINT8		MACAddr3;
+	UINT8		MACAddr2;
+	UINT8		MACAddr1;
+	UINT8		MACAddr0;
+	UINT8		MacAddrNum;
+	UINT8		TypeEnable;		/*	3:4:1 = Type:Reserved:Enable */
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(6+1+1+4+sizeof(NCSI_HDR))];
+} PACKED SET_MAC_ADDR_REQ_PKT;
+
+#define ENABLE_MAC_FILTER	0x01
+#define DISABLE_MAC_FILTER	0x00
+
+#define UNICAST_MAC_ADDR	(0x00 << 5)
+#define MULTICAST_MAC_ADDR	(0x01 << 5)
+
+typedef NCSI_DEFAULT_RES_PKT SET_MAC_ADDR_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT32		Settings;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(4+4+sizeof(NCSI_HDR))];
+} PACKED ENABLE_BCAST_FILTER_REQ_PKT;
+
+#define FORWARD_ARP_PACKETS				0x01
+#define FORWARD_DHCP_CLIENT_PACKETS		0x02
+#define FORWARD_DHCP_SERVER_PACKETS		0x04
+#define FORWARD_NETBIOS_PACKETS			0x08
+
+
+typedef NCSI_DEFAULT_RES_PKT ENABLE_BCAST_FILTER_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT DISABLE_BCAST_FILTER_REQ_PKT;
+typedef NCSI_DEFAULT_RES_PKT DISABLE_BCAST_FILTER_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT32		Settings;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(4+4+sizeof(NCSI_HDR))];
+} PACKED ENABLE_MCAST_FILTER_REQ_PKT;
+
+#define FORWARD_IPV6_NEIGHBOR_ADV		0x01
+#define FORWARD_IPV6_ROUTER_ADV			0x02
+#define FORWARD_DHCPV6_RELAY_SERVER		0x04
+
+typedef NCSI_DEFAULT_RES_PKT ENABLE_MCAST_FILTER_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT DISABLE_MCAST_FILTER_REQ_PKT;
+typedef NCSI_DEFAULT_RES_PKT DISABLE_MCAST_FILTER_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT8		Reserved[3];
+	UINT8		FlowControl;
+	UINT32		CheckSum;
+	UINT8		Pad[PAD_SIZE-(3+1+4+sizeof(NCSI_HDR))];
+} PACKED SET_NCSI_FLOW_CONTROL_REQ_PKT;
+
+#define DISABLE_FLOW_CONTROL				0x00
+#define ENABLE_NC_TO_MC_FLOW_CONTROL		0x01
+#define ENABLE_MC_TO_NC_FLOW_CONTROL		0x02
+#define ENABLE_BI_DIR_FLOW_CONTROL			0x03
+
+typedef NCSI_DEFAULT_RES_PKT SET_NCSI_FLOW_CONTROL_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT GET_VERSION_ID_REQ_PKT;
+
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		ResponseCode;
+	UINT16		ReasonCode;
+	UINT8		NCSIVerMajor;
+	UINT8		NCSIVerMinor;
+	UINT8		NCSIVerUpdate;
+	UINT8		NCSIVerAlpha1;
+	UINT8		Reserved[3];
+	UINT8		NCSIVerAlpha2;
+	UINT8		FirmwareName[12];
+	UINT8		FirmwareVerByte3;
+	UINT8		FirmwareVerByte2;
+	UINT8		FirmwareVerByte1;
+	UINT8		FirmwareVerByte0;
+	UINT16		PCI_DID;
+	UINT16		PCI_VID;
+	UINT16		PCI_SSID;
+	UINT16		PCI_SVID;
+	UINT32		IANA_ManID;
+	UINT32		CheckSum;
+} PACKED GET_VERSION_ID_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT GET_CAPABILITIES_REQ_PKT;
+
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		ResponseCode;
+	UINT16		ReasonCode;
+	UINT32		CapFlags;
+	UINT32		BcastFilterCaps;
+	UINT32		McastFilterCaps;
+	UINT32		BufferingCaps;
+	UINT32		AENControlSupport;
+	UINT8		VlanFilterCount;
+	UINT8		MixedFilterCount;
+	UINT8		McastFilterCount;
+	UINT8		UcastFilterCount;
+	UINT16		Reserved;
+	UINT8		VlanModeSupport;
+	UINT8		ChannelCount;
+	UINT32		CheckSum;
+} PACKED GET_CAPABILITIES_RES_PKT;
+
+#define HW_ARBITRATION_SUPPORT			0x00000001
+#define OS_PRESENCE_SUPPORT				0x00000002
+#define NC_TO_MC_FLOW_SUPPORT			0x00000004
+#define MC_TO_NC_FLOW_SUPPORT			0x00000008
+#define ALL_MCAST_ADDR_SUPPORT			0x00000010
+
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT GET_PARAMETERS_REQ_PKT;
+
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		ResponseCode;
+	UINT16		ReasonCode;
+	UINT8		MacAddrCount;
+	UINT8		Reserved1[2];
+	UINT8		MacAddrFlags;
+	UINT8		VLAN_TagCount;
+	UINT8		Reserved2;
+	UINT16		VLAN_TagFlags;
+	UINT32		LinkSettings;
+	UINT32		BcastFilterSettings;
+	UINT32		ConfigurationFlags;
+	UINT8		VLAN_Mode;
+	UINT8		FlowControlEnable;
+	UINT16		Reserved3;
+	UINT32		AENControl;
+	UINT8		MacAddr1[6];	
+	UINT8		MacAddr2[6];	/* Can be from 1 to  8  */
+	UINT16		VLAN1_Tag;	/* Can be from 1 to 15  */
+	UINT16		VLAN2_Tag;	
+	UINT16		VLAN3_Tag;	
+	UINT16		VLAN4_Tag;	
+	UINT16		VLAN5_Tag;	
+	UINT16		VLAN6_Tag;	
+	UINT16		VLAN7_Tag;	
+	UINT16		VLAN8_Tag;	
+	/* May require Padding if not aligned to dword boundary */
+	UINT32		CheckSum;
+} PACKED GET_PARAMETERS_RES_PKT;
+/*TODO : The above structure is variable sized. So create a proper structure and 
+         in the code, handle this variable fields, paddding and checksum location */
+
+#define MAC_ADDRESS_ENABLED_FLAG(x)		(1 <<((x)-1))	/* 1 to 8 */
+#define VLAN_TAG_ENABLED_FLAG(x)		(1 <<((x)-1))	/* 1 to 15 */
+
+#define BCAST_FILTER_ENABLED			0x01
+#define CHANNEL_ENABLED					0x02
+#define CHANNEL_NETWORK_TX_ENABLED		0x04
+#define MCAST_FILTER_ENABLED			0x08
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT GET_CTRL_PKT_STATS_REQ_PKT;
+
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		ResponseCode;
+	UINT16		ReasonCode;
+	UINT32		ClearedCounter_MS;			/* Bites 0-6 = Counters 32-38*/
+	UINT32		ClearedCounter_LS;			/* Bites 0-31 = Counters 0-31*/
+	UINT32		RecvBytes;
+	UINT32		XmitBytes;
+	UINT32		RecvUnicastPkts;
+	UINT32		RecvMcastPkts;
+	UINT32		RecvBcastPkts;
+	UINT32		XmitUnicastPkts;
+	UINT32		XmitMcastPkts;
+	UINT32		XmitBcastPkts;
+	UINT32		RecvFcsErrs;
+	UINT32		AlignmentErrs;
+	UINT32		FalseCarrierDetections;
+	UINT32		RecvRuntPkts;
+	UINT32		RecvJabberPkts;
+	UINT32		RecvPauseXON;
+	UINT32		RecvPauseXOFF;
+	UINT32		XmitPauseXON;
+	UINT32		XmitPauseXOFF;
+	UINT32		XmitSingleCollisions;
+	UINT32		XmitMultipleCollisions;
+	UINT32		LateCollisions;
+	UINT32		ExcessiveCollisions;
+	UINT32		RecvControlFrames;
+	UINT32		RecvFrameSize_64;
+	UINT32		RecvFrameSize_127;
+	UINT32		RecvFrameSize_255;
+	UINT32		RecvFrameSize_511;
+	UINT32		RecvFrameSize_1023;
+	UINT32		RecvFrameSize_1522;
+	UINT32		RecvFrameSize_9022;
+	UINT32		XmitFrameSize_64;
+	UINT32		XmitFrameSize_127;
+	UINT32		XmitFrameSize_255;
+	UINT32		XmitFrameSize_511;
+	UINT32		XmitFrameSize_1023;
+	UINT32		XmitFrameSize_1522;
+	UINT32		XmitFrameSize_9022;
+	UINT32		RecvValidPkts;
+	UINT32		RecvErrRuntPkts;
+	UINT32		RecvErrJabbePkts;
+	UINT32		CheckSum;
+} PACKED GET_CTRL_PKT_STATS_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT GET_NCSI_STATS_REQ_PKT;
+
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		ResponseCode;
+	UINT16		ReasonCode;
+	UINT32		RecvCommands;
+	UINT32		DroppedCtrlPkts;;
+	UINT32		CommandTypeErrs;
+	UINT32		CommandChecksumErrs;
+	UINT32		RecvCtrlPkts;
+	UINT32		XmitCtrlPkts;
+	UINT32		AENsSent;
+	UINT32		CheckSum;
+} PACKED GET_NCSI_STATS_RES_PKT;
+/*-------------------------------------------------------------------*/
+typedef NCSI_DEFAULT_REQ_PKT GET_NCSI_PASS_THRUSTATS_REQ_PKT;
+
+typedef struct
+{
+	NCSI_HDR	NcsiHdr;	
+	UINT16		ResponseCode;
+	UINT16		ReasonCode;
+	UINT32		XmitPkts;		/* Received on NCSI RX interface of NC*/
+	UINT32		XmitPktsDropped;
+	UINT32		XmitPktsChannelStateErr;
+	UINT32		XmitPktsUndersizedErrs;
+	UINT32		XmitPktsOversizedErrs;
+	UINT32		RecvPkts;		/* Received on LAN interface of NC */
+	UINT32		RecvPktsDropped;
+	UINT32		RecvPktsChannelStateErr;
+	UINT32		RecvPktsUndersizedErrs;
+	UINT32		RecvPktsOversizedErrs;
+	UINT32		CommandTypeErrs;
+	UINT32		CommandChecksumErrs;
+	UINT32		RecvCtrlPkts;
+	UINT32		XmitCtrlPkts;
+	UINT32		AENsSent;
+	UINT32		CheckSum;
+} PACKED GET_NCSI_PASS_THRU_STATS_RES_PKT;
+/*-------------------------------------------------------------------*/
+
+
+/*---------------------------- AEN Packets ------------------------------*/
+#define AEN_TYPE_LINK_STATUS_CHANGE				0x00
+#define AEN_TYPE_CONFIG_REQUIRED				0x01
+#define AEN_TYPE_HOST_NC_DRIVER_STATUS_CHANGE	0x02
+typedef struct
+{
+	AEN_HDR		AenHdr;
+	UINT8		Reserved[3];
+	UINT8		AENType;
+	UINT32		LinkStatus;
+	UINT32		OEMLinkStatus;
+	UINT32		CheckSum;
+} PACKED AEN_LINK_STATUS_CHANGE_PKT;
+
+typedef struct
+{
+	AEN_HDR		AenHdr;
+	UINT8		Reserved[3];
+	UINT8		AENType;
+	UINT32		CheckSum;
+} PACKED AEN_CONFIG_REQUIRED_PKT;
+
+typedef struct
+{
+	AEN_HDR		AenHdr;
+	UINT8		Reserved[3];
+	UINT8		AENType;
+	UINT32		HostNC_DriverStatus;
+	UINT32		CheckSum;
+} PACKED AEN_HOST_NC_DRIVER_STATUS_CHANGE_PKT;
+
+/*------------------------------- NCSI Functions ----------------------------*/
+int NCSI_Issue_SelectPackage(UINT8 PackageID,UINT8 HwArbitDisable);
+int NCSI_Issue_DeSelectPackage(UINT8 PackageID);
+int NCSI_Issue_ClearInitialState(UINT8 PackageID, UINT8 ChannelID);
+int NCSI_Issue_DisableChannel(UINT8 PackageID, UINT8 ChannelID, UINT8 AllowLinkDown);
+int NCSI_Issue_ResetChannel(UINT8 PackageID, UINT8 ChannelID);
+int NCSI_Issue_ChannelCommands(UINT8 Command,UINT8 PackageID, UINT8 ChannelID);
+int NCSI_Issue_GetVersionID(UINT8 PackageID,UINT8 ChannelID,UINT32 *Ver1,UINT32 *Ver2);
+int NCSI_Issue_GetCapabiliites(UINT8 PackageID,UINT8 ChannelID, UINT32 *Caps);
+int NCSI_Issue_SetMacAddress(UINT8 PackageID,UINT8 ChannelID,UINT8 *MacAddr,
+							UINT8 MacFilterNo,UINT8 MacType);
+int NCSI_Issue_EnableBcastFilter(UINT8 PackageID,UINT8 ChannelID, 
+					UINT8 Arp,UINT8 DhcpC,UINT8 DhcpS,UINT8 NetBios);
+int NCSI_Issue_DisableBcastFilter(UINT8 PackageID,UINT8 ChannelID);
+int NCSI_Issue_EnableMcastFilter(UINT8 PackageID,UINT8 ChannelID, 
+		UINT8 IPv6_Neighbour_Adv, UINT8 IPv6_Router_Adv, UINT8 IPv6_Multicast);
+int NCSI_Issue_DisableMcastFilter(UINT8 PackageID,UINT8 ChannelID);
+int NCSI_Issue_EnableAEN(UINT8 PackageID,UINT8 ChannelID,UINT8 LinkAEN,
+						UINT8 ConfigAEN,UINT8 HostAEN);
+int NCSI_Issue_GetLinkStatus(UINT8 PackageID,UINT8 ChannelID,UINT32 *LinkStatus);
+int NCSI_Issue_GetParameters(UINT8 PackageID,UINT8 ChannelID);
+int NCSI_Issue_SetLink(UINT8 PackageID,UINT8 ChannelID,UINT8 AutoNeg,
+						UINT32 Speed, UINT32 Duplex);
+
+void NCSI_Start(void);
+void NCSIHandler(unsigned char *Buffer,unsigned Unused1,unsigned Unused2, unsigned Len);
+int  EnableChannel(UINT8 PackageID,UINT8 ChannelID); 
+int  EnableChannelOnLink(UINT8 PackageID,UINT8 ChannelID); /* Called on Link Change AEN */
+
+/* Miscellaneous functions */
+#define DUMP_BUFFER 	0x02
+#define SHOW_MESSAGES	0x01
+int EnableVerbose(int level);
+
+#endif
