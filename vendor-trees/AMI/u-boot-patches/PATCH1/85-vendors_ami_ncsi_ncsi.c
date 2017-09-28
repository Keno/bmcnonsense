--- uboot-pristine/vendors/ami/ncsi/ncsi.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/vendors/ami/ncsi/ncsi.c	2009-05-14 10:30:24.000000000 -0700
@@ -1268,4 +1268,64 @@
 	/* Check the Response code and reason */
 	return ProcessSetLinkResponseCode(ResPkt->ResponseCode,ResPkt->ReasonCode);
 }
+/*
+*********************************************************************************************************
+*                                            NCSI_Issue_EnableVLAN()
+*
+* Description : Enable VLAN filter mode to accept VLAN tagged packets:
+
+* Argument(s) : NCSI_IF_INFO pointer, PackageID, ChannelID, VLANFilterMode .
+*
+* Return(s)   : Returns the Command response code.
+*
+* Caller(s)   : FormNCSIPacket(), SendNCSICommand(), NCSIValidateCheckSum(),ProcessResponseCode() 
+*
+* Note(s)     : none.
+*********************************************************************************************************
+*/
+int 
+NCSI_Issue_EnableVLAN(UINT8 PackageID, UINT8 ChannelID, 
+								UINT8 VLANFilterMode)
+{
+	ENABLE_VLAN_REQ_PKT Pkt;
+	ENABLE_VLAN_RES_PKT *ResPkt;
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
+	/*Set the Enable VLAN filter  */
+	Channel = MK_CH_ID(PackageID,ChannelID);
+	Pkt.Mode = VLANFilterMode;
+	
+	/* Form NCSI Packet and send */
+	FormNCSIPacket(CMD_ENABLE_VLAN,Channel,PayLoad,cs_offset);
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
 #endif
