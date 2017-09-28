--- uboot/vendors/ami/r2c/sess_misc.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.nonet/vendors/ami/r2c/sess_misc.c	2008-04-01 08:37:48.000000000 -0400
@@ -0,0 +1,311 @@
+# include <common.h>
+# include <config.h>
+#if (CONFIG_COMMANDS & CFG_CMD_NET)
+# include <command.h>
+# include <net.h>
+//# include <cmd_elf.h>
+//# include <cmd_boot.h>
+# include "r2c.h"
+# include "externs.h"
+
+static char gImageType;
+static unsigned long gLoadAddr;
+
+void
+DelayedReset(void)
+{
+	do_reset(NULL,0,0,NULL);
+	return;
+}
+
+void
+DelayedExec(void)
+{
+	char *argv[4];
+	char Command[] = "FMHEXECUTE";
+	char AddrStr[] = "0xFFFFFFFF";
+
+	printf("Delayed Execution started\n");
+	
+	if (gImageType == IMAGE_BINARY)
+	{
+		((void (*)(void)) gLoadAddr ) ();
+		return;
+	}
+					
+	if (gImageType  == IMAGE_ELF)
+	{
+		/* Form Arguments */
+		sprintf(AddrStr,"0x%lx",gLoadAddr);
+		argv[0] = &Command[0];
+		argv[1] = &AddrStr[0];
+		argv[2] = NULL;
+		do_bootelf(NULL,0,2,argv);
+		return;
+	}	
+
+	return;
+}
+
+
+int
+ProcessExecuteProgram(uchar *pkt,int len)
+{
+	volatile R2C_ExecuteProgramResponse *ResPkt;
+	R2C_ExecuteProgram *ReqPkt = (R2C_ExecuteProgram *)pkt;
+	uchar Status = 1;
+	unsigned long LoadAddr;
+
+	/* Convert ExecAddr to host endian format */
+	LoadAddr = le32_to_host(ReqPkt->ExecAddr);
+
+	/* Currenly support only binary and elf formats */
+	if ((ReqPkt->ImageType != IMAGE_BINARY) &&
+		(ReqPkt->ImageType != IMAGE_ELF))
+		Status = 0;
+
+	/* Check if the image is a valid ELF */		
+	if (ReqPkt->ImageType == IMAGE_ELF)
+	{
+		if (!valid_elf_image(LoadAddr))
+			Status = 0;			
+	}
+	
+	/* Response Packet */
+	ResPkt = (volatile R2C_ExecuteProgramResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(
+					sizeof(R2C_ExecuteProgramResponse) - sizeof(R2C_Header));
+	ResPkt->Status = Status;
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,
+							R2C_DEVICE_PORT,sizeof(R2C_ExecuteProgramResponse));
+
+	if (Status)
+	{
+		gImageType = ReqPkt->ImageType;
+		gLoadAddr = LoadAddr;
+		NetSetTimeout(CFG_HZ,DelayedExec);	/* Execute after a second */
+	}		
+	
+	return (int)Status;
+}
+
+int
+ProcessResetDevice(uchar *pkt,int len)
+{
+	volatile R2C_ResetDeviceResponse *ResPkt;
+	R2C_ResetDevice *ReqPkt = (R2C_ResetDevice *)pkt;
+	uchar Status = 1;
+	unsigned short WaitTime;
+
+	/* TODO :Set Status to 0, if the session cannot be stopped now */
+	
+	/* Convert WaitTime to host endian format */
+	WaitTime = le16_to_host(ReqPkt->WaitTime);
+
+	/* WaitTime should be atleast one second */
+	WaitTime =  (WaitTime == 0)?1:WaitTime;
+	
+	/* Response Packet */
+	ResPkt = (volatile R2C_ResetDeviceResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(
+					sizeof(R2C_ResetDeviceResponse) - sizeof(R2C_Header));
+	ResPkt->Status = Status;
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,
+							R2C_DEVICE_PORT,sizeof(R2C_ResetDeviceResponse));
+
+	if (Status)
+	{
+		/* Execute Reset after the waittime */
+		NetSetTimeout(CFG_HZ*WaitTime,DelayedReset);	
+	}		
+	
+	return (int)Status;
+}
+
+int
+ProcessSetMacAddress(uchar *pkt,int len)
+{
+	volatile R2C_SetMacAddressResponse *ResPkt;
+	R2C_SetMacAddress *ReqPkt = (R2C_SetMacAddress *)pkt;
+	uchar Status = 1;
+	char MacAddress[]  = "00:00:00:00:00:00";
+	char SavedValue[]  = "00:00:00:00:00:00";
+			
+	
+	sprintf(MacAddress,"%02X:%02X:%02X:%02X:%02X:%02X",
+				ReqPkt->MACAddr[0],ReqPkt->MACAddr[1],ReqPkt->MACAddr[2],
+				ReqPkt->MACAddr[3],ReqPkt->MACAddr[4],ReqPkt->MACAddr[5]);
+	
+	printf("Writing MacAddress %s to Flash ...\n",MacAddress);
+	/* Set the MAC Address in the Flash */
+	setenv("ethaddr",MacAddress);
+	saveenv();
+
+	/* Read Back and see if we are succesful */
+	getenv_r("ethaddr",SavedValue,sizeof(SavedValue));
+#if DEBUG	
+	printf("Read Back written Value = %s\n",SavedValue);
+#endif	
+	if (strcmp(MacAddress,SavedValue) != 0)
+		Status = 0;								/* Failed */			
+	
+	
+	/* Response Packet */
+	ResPkt = (volatile R2C_SetMacAddressResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(
+					sizeof(R2C_SetMacAddressResponse) - sizeof(R2C_Header));
+	ResPkt->Status = Status;
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,
+							R2C_DEVICE_PORT,sizeof(R2C_SetMacAddressResponse));
+
+	if (Status)
+		NetSetTimeout(CFG_HZ,DelayedReset);	
+	
+	return (int)Status;
+}
+
+int
+ProcessGetVar(uchar *pkt,int len)
+{
+	volatile R2C_GetVarResponse *ResPkt;
+	R2C_GetVar *ReqPkt = (R2C_GetVar *)pkt;
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_GetVarResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	if (getenv_r((char *)&ReqPkt->Key[0],(char *)&ResPkt->Value[0],VALUE_SIZE) == -1)
+		ResPkt->Status = 0;
+	else
+		ResPkt->Status = 1;  /* Success */
+
+	
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_GetVarResponse) 
+									- sizeof(R2C_Header));
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return ResPkt->Status;
+}
+
+int
+ProcessSetVar(uchar *pkt,int len)
+{
+	volatile R2C_SetVarResponse *ResPkt;
+	R2C_SetVar *ReqPkt = (R2C_SetVar *)pkt;
+	unsigned char SavedValue[VALUE_SIZE+1];
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_SetVarResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	ResPkt->Status = 1;
+
+	/* Write, Save and Read Back to verify */
+	setenv(ReqPkt->Key,ReqPkt->Value);
+	saveenv();
+	if (getenv_r(ReqPkt->Key,SavedValue,sizeof(SavedValue)) == -1)
+			ResPkt->Status = 0;		/* Failed */
+	else
+	{
+		if (strcmp(ReqPkt->Value,SavedValue) != 0)
+			ResPkt->Status = 0;		/* Failed */
+	}
+			
+	
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_SetVarResponse) 
+								- sizeof(R2C_Header));
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return ResPkt->Status;
+}
+
+
+
+int
+ProcessSetVarNoSave(uchar *pkt,int len)
+{
+	volatile R2C_SetVarResponse *ResPkt;
+	R2C_SetVar *ReqPkt = (R2C_SetVar *)pkt;
+	unsigned char SavedValue[VALUE_SIZE+1];
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_SetVarResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	ResPkt->Status = 1;
+
+	/* Write, Save and Read Back to verify */
+	setenv(ReqPkt->Key,ReqPkt->Value);
+	if (getenv_r(ReqPkt->Key,SavedValue,sizeof(SavedValue)) == -1)
+			ResPkt->Status = 0;		/* Failed */
+	else
+	{
+		if (strcmp(ReqPkt->Value,SavedValue) != 0)
+			ResPkt->Status = 0;		/* Failed */
+	}
+			
+	
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_SetVarResponse) 
+								- sizeof(R2C_Header));
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return ResPkt->Status;
+}
+#endif
