--- uboot/vendors/ami/r2c/session.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.nonet/vendors/ami/r2c/session.c	2008-04-01 08:37:17.000000000 -0400
@@ -0,0 +1,158 @@
+# include <common.h>
+# include <config.h>
+#if (CONFIG_COMMANDS & CFG_CMD_NET)
+# include <command.h>
+# include <net.h>
+# include "r2c.h"
+# include "externs.h"
+
+
+void
+EnterR2CSession(void)
+{
+	/* Do any session related Initialization here */
+	RemoteTerminal = 0;
+	AllocMemory = 0;
+}
+
+int
+ProcessStopSession(uchar *pkt,int len)
+{
+	volatile R2C_StopSessionResponse *ResPkt;
+	R2C_StopSession *ReqPkt = (R2C_StopSession *)pkt;
+	uchar Status = 1;
+
+	/* TODO :Set Status to 0, if the session cannot be stopped now */
+	
+	/* Response Packet */
+	ResPkt = (volatile R2C_StopSessionResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(
+					sizeof(R2C_StopSessionResponse) - sizeof(R2C_Header));
+	ResPkt->Status = Status;
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,
+							R2C_DEVICE_PORT,sizeof(R2C_StopSessionResponse));
+	
+	/* If we are stopping and remote terminal is on, stop remote terminal */
+	if (Status && RemoteTerminal)
+		RestoreTerminal();
+	
+	return (int)Status;
+}
+	
+	
+/* Returns 0 on normal commands and 1 on successful R2C_STOP_SESSION */
+int
+ProcessSessionCommands(uchar *pkt,int len)
+{
+	R2C_Header *Header = (R2C_Header *)pkt;
+
+#if DEBUG	
+	printf("Session Command is 0x%04X\n",le16_to_host(Header->PktType));
+#endif	
+	
+	switch (le16_to_host(Header->PktType))
+	{
+		case R2C_GET_FLASH_INFO:
+			ProcessGetFlashInfo(pkt,len);
+			break;
+			
+		case R2C_GET_FMH_INFO:
+			ProcessGetFMHInfo(pkt,len);
+			break;
+
+		case R2C_READ_MEMORY:
+#ifndef CFG_NO_FLASH				
+		case R2C_READ_FLASH:				
+#endif				
+			ProcessReadMemory(pkt,len);
+			break;
+
+		case R2C_WRITE_MEMORY:
+#ifndef CFG_NO_FLASH				
+		case R2C_WRITE_FLASH:			
+#endif			
+			ProcessWriteMemory(pkt,len);
+			break;
+
+		case R2C_ERASE_FLASH:			
+			ProcessEraseFlash(pkt,len);
+			break;
+
+		case R2C_PROTECT_FLASH:			
+			ProcessProtectFlash(pkt,len);
+			break;
+
+		case R2C_COMPARE_MEMORY:
+			ProcessCompareMemory(pkt,len);
+			break;
+
+		case R2C_COPY_MEMORY:
+			ProcessCopyMemory(pkt,len);
+			break;
+			
+		case R2C_CLEAR_MEMORY:
+			ProcessClearMemory(pkt,len);
+			break;
+			
+		case R2C_ALLOC_MEMORY: 			
+			ProcessAllocateMemory(pkt,len);
+			break;
+
+		case R2C_FREE_MEMORY: 			
+			ProcessFreeMemory(pkt,len);
+			break;
+			
+		case R2C_ERASE_COPY_FLASH:			
+			ProcessEraseCopyFlash(pkt,len);
+			break;
+
+		case R2C_SET_MAC_ADDRESS:
+			ProcessSetMacAddress(pkt,len);
+			break;
+			
+		case R2C_EXECUTE_PROGRAM:
+			ProcessExecuteProgram(pkt,len);
+			break;					
+			
+		case R2C_STOP_SESSION:
+			/* This is the only command function which returns 
+			 * its return code to caller */
+			return ProcessStopSession(pkt,len);
+			break;
+
+		case R2C_RESET_DEVICE:
+			ProcessResetDevice(pkt,len);
+			break;
+
+		case R2C_REMOTE_TERMINAL:
+			ProcessRemoteTerminal(pkt,len);			
+			break;
+
+		case R2C_SET_VAR:
+			ProcessSetVar(pkt,len);
+			break;
+
+		case R2C_SET_VAR_NO_SAVE: 
+			ProcessSetVarNoSave( pkt,len);
+			break;
+
+		case R2C_GET_VAR:			
+			ProcessGetVar(pkt,len);
+			break;
+			
+		default:
+			break;
+	}
+	return 0;
+}
+#endif
