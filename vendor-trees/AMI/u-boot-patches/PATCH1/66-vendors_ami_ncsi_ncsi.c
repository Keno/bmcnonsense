diff -Naur uboot/vendors/ami/ncsi/ncsi.c uboot.new/vendors/ami/ncsi/ncsi.c
--- uboot/vendors/ami/ncsi/ncsi.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/vendors/ami/ncsi/ncsi.c	2007-08-31 12:25:16.000000000 -0400
@@ -0,0 +1,1271 @@
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
+  Module	: NCSI Core API
+
+  Revision	: 1.0  
+
+  Changelog : 1.0 - Initial Version  [SC]
+
+ *****************************************************************/
+#include <common.h>
+#include <exports.h>
+#ifdef CONFIG_NCSI_SUPPORT
+#include "endian.h"
+#include "ncsi.h"
+#ifdef NCSI_DEBUG
+#include "datadisp.h"
+#endif
+
+
+#define NCSI_ERR_SUCCESS	0
+#define NCSI_ERR_FAILED		1	/* Remote send Response error */
+#define NCSI_ERR_NETWORK	2
+#define NCSI_ERR_TIMEOUT	3
+#define NCSI_ERR_RESPONSE	4
+#define NCSI_ERR_CHECKSUM	5
+#define NCSI_ERR_SEQNO		6
+#define NCSI_ERR_RESPONSE_NUM	7
+
+
+
+#define BUFFER_SIZE  (2+1522+2)	
+/* First two bytes are not used to make the NCSI fileds align at boundaries*/
+
+/* Make sure these two buffers are aligned at 32 bir boundary */
+static UINT8  gSendBuffer[BUFFER_SIZE];
+static UINT8  gRecvBuffer[BUFFER_SIZE];
+
+static UINT8  *SendPkt = &gSendBuffer[2];
+static UINT8  *RecvPkt = &gRecvBuffer[2];
+
+//static UINT8  OurMacAddr[] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
+
+static UINT8  InstanceID=0x1;
+static int ReceivedNCSILen = 0;
+
+static int verbose = SHOW_MESSAGES;
+
+
+int
+EnableVerbose(int level)
+{
+	int prev = verbose;
+	verbose = level;
+	return prev;
+}
+
+int
+CheckIfAEN(unsigned char *RecvPkt,unsigned Len)
+{
+	AEN_PKT *aen;
+	
+	aen = (AEN_PKT *)RecvPkt;
+	
+	if (aen->hdr.MC_ID	!= 0x00)
+		return 0;
+	if (aen->hdr.ONE   	!= 0x01)
+		return 0;
+	if (aen->hdr.IID   	!= 0x00)
+		return 0;
+	if (aen->hdr.ALL_FF	!= 0xFF)
+		return 0;
+	printf("AEN Received : ");
+
+	switch (aen->AEN_Type)
+	{
+		case AEN_TYPE_LINK_STATUS_CHANGE:
+			if (aen->hdr.PayLen !=  AEN_TYPE_LINK_STATUS_CHANGE_LEN)
+				break;
+			printf("Link Status Change \n");
+	//		EnableChannelOnLink(GET_PACKAGE_ID(aen->hdr.OrgChannelID),
+	//				    GET_CHANNEL_ID(aen->hdr.OrgChannelID));
+			return 1;
+
+		case  AEN_TYPE_CONFIG_REQUIRED:
+			if (aen->hdr.PayLen !=  AEN_TYPE_CONFIG_REQUIRED_LEN)
+				break;
+			printf("Config Required\n");
+			return 1;
+
+		case  AEN_TYPE_HOST_DRIVER_CHANGE:
+			if (aen->hdr.PayLen !=  AEN_TYPE_HOST_DRIVER_CHANGE_LEN)
+				break;
+			printf("Host driver Status Change\n");
+			return 1;
+	}
+	printf("Unknown AEN %d\n",aen->AEN_Type);
+	return 0;
+}
+
+void
+NCSIHandler(unsigned char *Buffer,unsigned Unused1,unsigned Unused2, unsigned Len)
+{
+	ETH_HDR  *EthHdr;
+	int i;
+
+	if (ReceivedNCSILen !=0)
+		return;
+
+	Len-=(sizeof(ETH_HDR)+4) ; /* Strip Ethernet header, crc32 */
+	
+	EthHdr = (ETH_HDR *)Buffer;
+	if (be16_to_cpu(EthHdr->EtherType) != NCSI_ETHER_TYPE)
+	{
+//		printf("Received a Non NCSI Packet (EtherType = 0x%04X). Ignored\n",
+//			be16_to_cpu(EthHdr->EtherType));
+		return;
+	}	
+	for (i=0;i<MAC_ADDR_LEN;i++)
+	{	
+		if (EthHdr->DestMACAddr[i] != 0xFF)
+		{
+			printf("Received a Invalid NCSI Packet (Dest Mac != All 0xFF). Ignored\n");
+			return;
+		}
+	}
+//	printf("Received a NCSI Packet of Length %d\n",Len);
+#ifdef NCSI_DEBUG
+	if (verbose & DUMP_BUFFER)
+	{
+		printf ("NCSI: Receiving Response ...\n");
+		DumpContents(0,Buffer,Len+sizeof(ETH_HDR),FORMAT_BYTE,NULL);
+	}
+#endif
+	Buffer += sizeof(ETH_HDR);
+	memcpy(RecvPkt,Buffer,Len);
+
+	if (CheckIfAEN(RecvPkt,Len) == 0)
+		ReceivedNCSILen = Len;
+	return;
+}
+
+static
+int 
+SendNCSICommand (UINT8* pBuf, int Size)
+{
+	int i;
+	unsigned long counter;
+	int err;
+	NCSI_HDR *send_hdr,*recv_hdr;
+	
+	err = NCSI_ERR_SUCCESS;
+
+	Size+=sizeof(ETH_HDR);
+	send_hdr = (NCSI_HDR *)(pBuf +sizeof(ETH_HDR));
+
+#ifdef NCSI_DEBUG
+	if (verbose & DUMP_BUFFER)
+	{
+		printf ("NCSI: Sending Request ...\n");
+		DumpContents(0,pBuf,Size,FORMAT_BYTE,NULL);
+	}
+#endif
+	
+	for (i = 0; i < NCSI_RETRIES; i++)
+	{
+		counter=0x10000;
+		ReceivedNCSILen = 0;
+		if (-1 == eth_send(pBuf, Size))
+		{
+			printf ("NCSI: eth_send failed\n");
+			return NCSI_ERR_NETWORK;
+		}
+
+		while (counter--)
+		{
+			eth_rx();
+			if (ReceivedNCSILen != 0)
+			{
+			//	printf("NCSI: Received Response of Len %d\n",ReceivedNCSILen);
+				recv_hdr = (NCSI_HDR *)(RecvPkt);
+				err = NCSI_ERR_SUCCESS;
+				if (recv_hdr->I_ID != send_hdr->I_ID)
+				{
+					printf("NCSI: SeqNo Mismatch %02X != %02X\n",
+							send_hdr->I_ID, recv_hdr->I_ID);
+					err = NCSI_ERR_SEQNO;
+				}
+				if (recv_hdr->Command != (send_hdr->Command | 0x80))
+				{
+					printf("NCSI: Invalid Response Command. Expected = %02X, Got = %02X\n",
+						send_hdr->Command|0x80,recv_hdr->Command);
+					err = NCSI_ERR_RESPONSE_NUM;
+				}
+				if (err != NCSI_ERR_SUCCESS)
+				{
+					/* Ignore the junk packet and try receive next */
+					ReceivedNCSILen = 0;
+					continue;
+				}
+				return NCSI_ERR_SUCCESS;
+			}
+		}
+		if (verbose & SHOW_MESSAGES)
+			printf ("NCSI: Resending Command (Retry = %d)...\n",i+1);
+	}
+
+	if (verbose & SHOW_MESSAGES)
+		printf ("NCSI:Receive Timeout out\n");
+	if (err == NCSI_ERR_SUCCESS)
+		return NCSI_ERR_TIMEOUT;
+	return err;
+}
+
+static
+UINT32
+NCSICalculateCheckSum (UINT16* pData, int Size)
+{
+	UINT32 CheckSum = 0;
+	while (Size != 0)
+	{
+		CheckSum += be16_to_cpu(*pData);
+		pData++;
+		Size -= 2;
+	}
+
+	CheckSum = ~CheckSum + 1;
+	return cpu_to_be32(CheckSum);
+}
+
+static
+UINT32
+NCSIValidateCheckSum (UINT16* pData,int cs_offset)
+{
+	UINT32 CheckSum = 0, GotCS;
+	UINT32 i;
+
+	GotCS = *(UINT32*)(((UINT8*)pData) + cs_offset);
+	GotCS = be32_to_cpu(GotCS);
+	for(i=0;i<cs_offset;i+=2)
+	{
+		CheckSum += be16_to_cpu(*pData);
+		pData++;
+	}
+	CheckSum = ~CheckSum + 1;
+
+	if (GotCS == CheckSum)
+		return NCSI_ERR_SUCCESS;
+
+	printf("NCSI: CheckSum Error: Received = 0x%08x, Calculated = 0x%08x\n",
+									GotCS,CheckSum);
+	return NCSI_ERR_CHECKSUM;
+}
+
+static
+int
+ProcessResponseCode(UINT16 ResponseCode, UINT16 ReasonCode)
+{
+	if (!(verbose & SHOW_MESSAGES))
+	{
+		if (be16_to_cpu(ResponseCode) == COMMAND_COMPLETED)
+			return NCSI_ERR_SUCCESS;
+		return NCSI_ERR_FAILED;
+	}
+	
+	switch (be16_to_cpu(ResponseCode))
+	{
+		case  COMMAND_COMPLETED:
+			return NCSI_ERR_SUCCESS;
+		case  COMMAND_FAILED:
+			printf("NCSI:Command Failed.");		
+			break;
+		case  COMMAND_UNAVAILABLE:			
+			printf("NCSI:Command Unavailable.");		
+			break;
+		case  COMMAND_UNSUPPORTED:
+			printf("NCSI:Command Unsupported.");		
+			break;
+		default:
+			printf("NCSI:Unknown ResponseCode. ");		
+			break;
+	}
+
+	switch (be16_to_cpu(ReasonCode))
+	{
+		case  REASON_NONE:
+			printf("No Reason\n");
+			break;
+		case  REASON_INIT_REQUIRED:		
+			printf("Init Required\n");
+			break;
+		case  REASON_PARAMS_INVALID:				
+			printf("Parameters Invalid\n");
+			break;
+		case  REASON_CHANNEL_NOT_READY:			
+			printf("Channel Not Ready\n");
+			break;
+		case  REASON_PACKAGE_NOT_READY:			
+			printf("Package Not Ready\n");
+			break;
+		case  REASON_LENGTH_INVALID:				
+			printf("Invalid Length\n");
+			break;
+		case  REASON_MAC_ADDR_ZERO:
+			printf("MAC Address is zero\n");
+			break;
+		default:
+			printf("Unknown Reason\n");
+			break;			
+	}
+
+	return NCSI_ERR_FAILED;
+}
+
+static
+int
+ProcessSetLinkResponseCode(UINT16 ResponseCode, UINT16 ReasonCode)
+{
+	if (!(verbose & SHOW_MESSAGES))
+	{
+		if (be16_to_cpu(ResponseCode) == COMMAND_COMPLETED)
+			return NCSI_ERR_SUCCESS;
+		return NCSI_ERR_FAILED;
+	}
+	
+	switch (be16_to_cpu(ResponseCode))
+	{
+		case  COMMAND_COMPLETED:
+			return NCSI_ERR_SUCCESS;
+		case  COMMAND_FAILED:
+			printf("NCSI:Command Failed.");		
+			break;
+		case  COMMAND_UNAVAILABLE:			
+			printf("NCSI:Command Unavailable.");		
+			break;
+		case  COMMAND_UNSUPPORTED:
+			printf("NCSI:Command Unsupported.");		
+			break;
+		default:
+			printf("NCSI:Unknown ResponseCode. ");		
+			break;
+	}
+
+	switch (be16_to_cpu(ReasonCode))
+	{
+		case  REASON_NONE:
+			printf("No Reason\n");
+			break;
+		case  SET_LINK_REASON_HOST_CONFLICT:
+			printf("Host OS/Driver Conflict\n");
+			break;
+		case  SET_LINK_REASON_MEDIA_CONFLICT:
+			printf("Media Conflict\n");
+			break;
+		case  SET_LINK_REASON_PARAMETER_CONFLICT:
+			printf("Parameter Conflict\n");
+			break;
+		case  SET_LINK_REASON_POWERMODE_CONFLICT:
+			printf("Power Mode Conflict\n");
+			break;
+		case  SET_LINK_REASON_SPEED_CONFLICT:
+			printf("Speed  Conflict\n");
+			break;
+		case  SET_LINK_REASON_HW_ACCESS_FAILED:
+			printf("Hardware Failure\n");
+			break;
+		default:
+			printf("Unknown Reason\n");
+			break;			
+	}
+
+	return NCSI_ERR_FAILED;
+}
+
+
+static
+void
+FormNCSIPacket(UINT8 Command,UINT8 Channel,UINT8 *PayLoad, int cs_offset)
+{
+	ETH_HDR  		*EthHdr;
+	NCSI_HDR		*NcsiHdr;
+	UINT8   		*NcsiPkt,*NcsiPayLoad;
+	UINT32			*CheckSum;
+	UINT32			PayLoadLen;
+
+	memset(gSendBuffer,0,BUFFER_SIZE);
+	/* Get the locations of various fields */
+	EthHdr  	= (ETH_HDR *)(SendPkt);
+	NcsiPkt		=  SendPkt + sizeof(ETH_HDR);
+	NcsiHdr 	= (NCSI_HDR *)NcsiPkt;
+	NcsiPayLoad     =  SendPkt + sizeof(ETH_HDR) + sizeof(NCSI_HDR);
+	CheckSum	= (UINT32 *)(NcsiPkt+cs_offset);
+	PayLoadLen	=  (UINT32)CheckSum - (UINT32)NcsiPayLoad;
+
+	/* Fill in Ethernet Header */
+	memset(EthHdr->DestMACAddr,0xFF,6);
+	memset(EthHdr->SrcMACAddr,0xFF,6);
+	EthHdr->EtherType = cpu_to_be16(NCSI_ETHER_TYPE);
+	
+	/* Fill in NCSI Header */
+	memset(NcsiHdr,0, sizeof(NCSI_HDR));
+	NcsiHdr->MC_ID 		= NCSI_MC_ID;
+	NcsiHdr->HdrRev 	= NCSI_HDR_REV;
+	NcsiHdr->I_ID		= InstanceID++;
+	InstanceID= (InstanceID == 0)?1:InstanceID;	// IID = 1 to 0xFF . 0 is invalid
+	NcsiHdr->Command 	= Command;
+	NcsiHdr->CH_ID		= Channel;
+	NcsiHdr->PayloadLen = cpu_to_be16(PayLoadLen);
+
+	/* Copy the PayLoad */
+	memcpy(NcsiPayLoad,PayLoad,PayLoadLen);
+
+	/* Calculate the checksum for NCSI packet */
+	*CheckSum = NCSICalculateCheckSum((UINT16 *)(NcsiPkt),PayLoadLen+sizeof(NCSI_HDR));
+	return;
+}
+
+/*****************************************************************************************
+								NC-SI Commands
+*****************************************************************************************/
+
+int
+NCSI_Issue_SelectPackage(UINT8 PackageID,UINT8 HwArbitDisable)
+{
+	SELECT_PACKAGE_REQ_PKT Pkt;
+	SELECT_PACKAGE_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	/* "Select Package" Specific Parameters */
+	Channel = MK_CH_ID(PackageID,0x1F);
+	Pkt.HwArbitDisable = HwArbitDisable;
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_SELECT_PACKAGE,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (SELECT_PACKAGE_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(SELECT_PACKAGE_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(SELECT_PACKAGE_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+
+}
+
+
+int
+NCSI_Issue_DeSelectPackage(UINT8 PackageID)
+{
+	DESELECT_PACKAGE_REQ_PKT Pkt;
+	DESELECT_PACKAGE_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	/* "DeSelect Package" Specific Parameters */
+	Channel = MK_CH_ID(PackageID,0x1F);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_DESELECT_PACKAGE,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (DESELECT_PACKAGE_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(DESELECT_PACKAGE_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(DESELECT_PACKAGE_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+
+int
+NCSI_Issue_ClearInitialState(UINT8 PackageID,UINT8 ChannelID)
+{
+	CLEAR_INITIAL_STATE_REQ_PKT Pkt;
+	CLEAR_INITIAL_STATE_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_CLEAR_INITIAL_STATE,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (CLEAR_INITIAL_STATE_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(CLEAR_INITIAL_STATE_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(CLEAR_INITIAL_STATE_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+
+int
+NCSI_Issue_ChannelCommands(UINT8 Command, UINT8 PackageID,UINT8 ChannelID)
+{
+	NCSI_DEFAULT_REQ_PKT Pkt;
+	NCSI_DEFAULT_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(Command,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (NCSI_DEFAULT_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(NCSI_DEFAULT_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(NCSI_DEFAULT_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+
+int
+NCSI_Issue_DisableChannel(UINT8 PackageID,UINT8 ChannelID,UINT8 AllowLinkDown)
+{
+	DISABLE_CHANNEL_REQ_PKT Pkt;
+	DISABLE_CHANNEL_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	/* i"Disable Channel" Specific Parameters */
+	Channel = MK_CH_ID(PackageID,ChannelID);
+	Pkt.AllowLinkDown = AllowLinkDown;
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_DISABLE_CHANNEL,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (DISABLE_CHANNEL_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(DISABLE_CHANNEL_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(DISABLE_CHANNEL_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+int
+NCSI_Issue_ResetChannel(UINT8 PackageID,UINT8 ChannelID)
+{
+	RESET_CHANNEL_REQ_PKT Pkt;
+	RESET_CHANNEL_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	/* "Reset Channel" Specific Parameters */
+	Channel = MK_CH_ID(PackageID,ChannelID);
+	Pkt.Reserved = 0;
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_RESET_CHANNEL,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (RESET_CHANNEL_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(RESET_CHANNEL_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(RESET_CHANNEL_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+
+int 
+NCSI_Issue_GetVersionID(UINT8 PackageID, UINT8 ChannelID, UINT32 *Ver1,UINT32 *Ver2)
+{
+	GET_VERSION_ID_REQ_PKT Pkt;
+	GET_VERSION_ID_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	*Ver1 = *Ver2 = 0;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_GET_VERSION_ID,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (GET_VERSION_ID_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(GET_VERSION_ID_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(GET_VERSION_ID_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	retval =  ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+	if (retval == NCSI_ERR_SUCCESS)
+	{
+		*Ver1 |= ResPkt->NCSIVerMajor << 24;
+		*Ver1 |= ResPkt->NCSIVerMinor << 16;
+		*Ver1 |= ResPkt->NCSIVerUpdate<< 8;
+		*Ver1 |= ResPkt->NCSIVerAlpha1<< 0;
+
+		*Ver2 = ResPkt->NCSIVerAlpha2;
+	}
+
+	return retval;
+}
+
+int 
+NCSI_Issue_GetCapabiliites(UINT8 PackageID, UINT8 ChannelID,UINT32 *Caps)
+{
+
+	GET_CAPABILITIES_REQ_PKT Pkt;
+	GET_CAPABILITIES_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_GET_CAPABILITIES,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (GET_CAPABILITIES_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(GET_CAPABILITIES_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(GET_CAPABILITIES_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	retval =  ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+	if (retval == NCSI_ERR_SUCCESS)
+	{
+		*Caps = be32_to_cpu(ResPkt->CapFlags);
+	}
+
+	return retval;
+}
+
+int 
+NCSI_Issue_SetMacAddress(UINT8 PackageID, UINT8 ChannelID, UINT8 *MacAddr,
+					UINT8 MacFilterNo,UINT8 MacType)
+{
+	SET_MAC_ADDR_REQ_PKT Pkt;
+	SET_MAC_ADDR_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	/* "SetMacAddr" Specific Parameters */
+	Channel = MK_CH_ID(PackageID,ChannelID);
+	Pkt.MACAddr5   = MacAddr[5];
+	Pkt.MACAddr4   = MacAddr[4];
+	Pkt.MACAddr3   = MacAddr[3];
+	Pkt.MACAddr2   = MacAddr[2];
+	Pkt.MACAddr1   = MacAddr[1];
+	Pkt.MACAddr0   = MacAddr[0];
+	Pkt.MacAddrNum = MacFilterNo;
+	Pkt.TypeEnable  = (MacType << 5) | ENABLE_MAC_FILTER;	
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_SET_MAC_ADDR,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (SET_MAC_ADDR_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(SET_MAC_ADDR_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(SET_MAC_ADDR_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+
+int 
+NCSI_Issue_EnableBcastFilter(UINT8 PackageID,UINT8 ChannelID, 
+					UINT8 Arp,UINT8 DhcpC,UINT8 DhcpS,UINT8 NetBios)
+{
+	ENABLE_BCAST_FILTER_REQ_PKT Pkt;
+	ENABLE_BCAST_FILTER_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+	Pkt.Settings = cpu_to_be32(Arp | DhcpC << 1 | DhcpS << 2 | NetBios << 3);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_ENABLE_BCAST_FILTER,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (ENABLE_BCAST_FILTER_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(ENABLE_BCAST_FILTER_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(ENABLE_BCAST_FILTER_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+int 
+NCSI_Issue_DisableBcastFilter(UINT8 PackageID,UINT8 ChannelID)
+{
+	DISABLE_BCAST_FILTER_REQ_PKT Pkt;
+	DISABLE_BCAST_FILTER_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_DISABLE_BCAST_FILTER,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (DISABLE_BCAST_FILTER_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(DISABLE_BCAST_FILTER_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(DISABLE_BCAST_FILTER_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+int 
+NCSI_Issue_EnableMcastFilter(UINT8 PackageID,UINT8 ChannelID, 
+		UINT8 IPv6_Neighbour_Adv, UINT8 IPv6_Router_Adv, UINT8 IPv6_Multicast)
+{
+	ENABLE_MCAST_FILTER_REQ_PKT Pkt;
+	ENABLE_MCAST_FILTER_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+	Pkt.Settings =cpu_to_be32(IPv6_Neighbour_Adv |  IPv6_Router_Adv << 1 | IPv6_Multicast << 2);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_ENABLE_MCAST_FILTER,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (ENABLE_MCAST_FILTER_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(ENABLE_MCAST_FILTER_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(ENABLE_MCAST_FILTER_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+
+
+int 
+NCSI_Issue_DisableMcastFilter(UINT8 PackageID,UINT8 ChannelID)
+{
+	DISABLE_MCAST_FILTER_REQ_PKT Pkt;
+	DISABLE_MCAST_FILTER_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_DISABLE_MCAST_FILTER,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (DISABLE_MCAST_FILTER_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(DISABLE_MCAST_FILTER_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(DISABLE_MCAST_FILTER_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+
+int 
+NCSI_Issue_EnableAEN(UINT8 PackageID,UINT8 ChannelID,UINT8 LinkAEN,
+						UINT8 ConfigAEN,UINT8 HostAEN)
+{
+	AEN_ENABLE_REQ_PKT Pkt;
+	AEN_ENABLE_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+	Pkt.AenCtrl   = cpu_to_be32(LinkAEN | ConfigAEN << 1 | HostAEN << 2);
+	Pkt.AEN_MC_ID = NCSI_MC_ID;
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_AEN_ENABLE,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (AEN_ENABLE_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(AEN_ENABLE_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(AEN_ENABLE_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+
+
+
+
+int 
+NCSI_Issue_GetLinkStatus(UINT8 PackageID, UINT8 ChannelID, UINT32 *LinkStatus)
+{
+
+	GET_LINK_STATUS_REQ_PKT Pkt;
+	GET_LINK_STATUS_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* This command does not return proper link status if 
+           send immediatly after a NCSI command (I seen it after
+	   SelectPackage). So added a delay of one second */
+	udelay(1000*1000);
+
+	*LinkStatus = 0;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_GET_LINK_STATUS,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (GET_LINK_STATUS_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(GET_LINK_STATUS_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(GET_LINK_STATUS_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	retval =  ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+	if (retval == NCSI_ERR_SUCCESS)
+	{
+		*LinkStatus = be32_to_cpu(ResPkt->LinkStatus);
+//		printf("Link Status = 0x%08X\n",be32_to_cpu(ResPkt->LinkStatus));
+//		printf("OEM Link Status = 0x%08X\n",be32_to_cpu(ResPkt->OEMLinkStatus));
+//		printf("Other Indicators = 0x%08X\n",be32_to_cpu(ResPkt->OtherIndicators));
+	}
+
+	return retval;
+}
+
+int 
+NCSI_Issue_GetParameters(UINT8 PackageID, UINT8 ChannelID)
+{
+	GET_PARAMETERS_REQ_PKT Pkt;
+	GET_PARAMETERS_RES_PKT *ResPkt;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_GET_PARAMETERS,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (GET_PARAMETERS_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(GET_PARAMETERS_RES_PKT))
+	{
+			printf("NCSI: Expected Response Size = %d Got %d\n",
+				sizeof(GET_PARAMETERS_RES_PKT),ReceivedNCSILen);
+			return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	retval =  ProcessResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+	if (retval == NCSI_ERR_SUCCESS)
+	{
+			/*TODO: Send back some useful info ? */
+	}
+
+	return retval;
+}
+
+int
+NCSI_Issue_SetLink(UINT8 PackageID,UINT8 ChannelID, UINT8 AutoNeg,
+	UINT32 Speed, UINT32 Duplex)
+{
+	SET_LINK_REQ_PKT Pkt;
+	SET_LINK_RES_PKT *ResPkt;
+	UINT32 LinkSettings;
+
+	UINT8 Channel;
+	UINT8 *PayLoad;
+	int cs_offset;
+	int retval;
+
+	/* Basic Initialization */
+	memset(&Pkt ,0, sizeof(Pkt));
+	PayLoad = ((UINT8 *)&Pkt) + sizeof(NCSI_HDR);
+	cs_offset = (UINT32)(&(Pkt.CheckSum)) - (UINT32)(&Pkt);
+
+	/* "SetLink" Specific Parameters */
+	Channel = MK_CH_ID(PackageID,ChannelID);
+
+	LinkSettings = LINK_ENABLE_PAUSE_CAPS;
+	LinkSettings |= LINK_ENABLE_ASYNC_PAUSE_CAPS;
+	if (AutoNeg)
+	{
+		LinkSettings |= LINK_ENABLE_AUTO_NEG;
+		LinkSettings |= LINK_ENABLE_10_MBPS;
+		LinkSettings |= LINK_ENABLE_100_MBPS;
+#if NCSI_1G_SUPPORT
+		LinkSettings |= LINK_ENABLE_1000_MBPS;
+#endif
+#if NCSI_10G_SUPPORT
+		LinkSettings |= LINK_ENABLE_10_GBPS;
+#endif
+		LinkSettings |= LINK_ENABLE_HALF_DUPLEX;
+		LinkSettings |= LINK_ENABLE_FULL_DUPLEX;
+	}
+	else
+	{
+		LinkSettings |= Speed | Duplex;
+	}
+	Pkt.LinkSettings = cpu_to_be32(LinkSettings);
+	
+
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_SET_LINK,Channel,PayLoad,cs_offset);
+	retval = SendNCSICommand (SendPkt,sizeof(Pkt));
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Validate Response pkt */
+	ResPkt = (SET_LINK_RES_PKT *)RecvPkt;
+	if (ReceivedNCSILen != sizeof(SET_LINK_RES_PKT))
+	{
+		printf("NCSI: Expected Response Size = %d Got %d\n",
+			sizeof(DISABLE_CHANNEL_RES_PKT),ReceivedNCSILen);
+		return NCSI_ERR_RESPONSE;
+	}
+
+	/* Validate Response checksum */
+	cs_offset = (UINT32)(&(ResPkt->CheckSum)) - (UINT32)(ResPkt);
+	retval = NCSIValidateCheckSum((UINT16*)RecvPkt,cs_offset);
+	if (retval != NCSI_ERR_SUCCESS)
+		return retval;
+
+	/* Check the Response code and reason */
+	return ProcessSetLinkResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
+}
+#endif
