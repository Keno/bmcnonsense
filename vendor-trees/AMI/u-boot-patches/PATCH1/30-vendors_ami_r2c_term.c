--- uboot/vendors/ami/r2c/term.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.nonet/vendors/ami/r2c/term.c	2008-04-01 08:38:03.000000000 -0400
@@ -0,0 +1,217 @@
+# include <common.h>
+# include <config.h>
+#if (CONFIG_COMMANDS & CFG_CMD_NET)
+# include <command.h>
+# include <net.h>
+# include <devices.h>
+# include "r2c.h"
+# include "externs.h"
+
+static device_t dev;
+
+#define TERMBUFF_SIZE 128
+static uchar TermBuff[TERMBUFF_SIZE];
+static volatile int BuffTail,BuffHead,BuffCount;
+
+
+
+void
+r2cdev_putc(const char c)
+{
+	volatile char *ch;
+	
+	ch = (volatile char *) 
+			(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+	*ch = c;
+	
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,R2C_TERMINAL_PORT,
+											R2C_TERMINAL_PORT,1);
+	return;
+}
+
+void
+r2cdev_puts(const char *s)
+{
+	int len,total;
+	volatile char *ch;
+	
+	total = strlen(s);
+
+	while (total > 0)
+	{
+		ch = (volatile char *) 
+			(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+		/* Send smaller packet (256 bytes) at a time to 
+		 * avoid UDP Packet loss */
+		len = (total < 256)?total:256;
+		memcpy((char *)ch,s,len);
+		NetSendUDPPacket(ConsoleEther,ConsoleIP,R2C_TERMINAL_PORT,
+											R2C_TERMINAL_PORT,len);
+		
+		/* Adjust buffer and total length */
+		total-=len;
+		s+=len;
+	}
+	return;
+}
+
+int
+r2cdev_getc(void)
+{
+	int ch;
+
+#if DEBUG
+	printf("getc called\n");
+#endif	
+	/* Wait for a character */
+	while (!BuffCount)
+		eth_rx();
+#if DEBUG	
+	printf("getc has a character 0x%x\n",TermBuff[BuffTail]);
+#endif	
+
+	/* Remove the character from buffer and return it */	
+	ch = TermBuff[BuffTail];
+	BuffTail++;
+	if (BuffTail == TERMBUFF_SIZE)
+		BuffTail = 0;
+	BuffCount--;
+	
+	return ch;
+}
+
+int 
+r2cdev_tstc(void)
+{
+	eth_rx();
+#if DEBUG		
+	printf("tstc called BuffCount = %d\n",BuffCount);
+#endif	
+	if (BuffCount == 0)
+		return 0;	
+	return 1;
+}
+
+static
+void
+AddtoTermBuffer(uchar ch)
+{
+	if (BuffCount == TERMBUFF_SIZE)
+		return;
+#if DEBUG	
+//	printf("%c",ch);
+#endif	
+	TermBuff[BuffHead] = ch;
+	BuffHead++;
+	if (BuffHead == TERMBUFF_SIZE)
+		BuffHead = 0;
+	BuffCount++;
+	return;
+}
+
+
+void
+AddtoInputQueue(uchar *pkt,int len)
+{
+#if DEBUG		
+//	printf("[");
+#endif	
+	while (len)
+	{
+		AddtoTermBuffer(*pkt);
+		pkt++,len--;
+	}		
+#if DEBUG		
+//	printf("]\n");
+#endif
+	return;
+}
+
+void
+RestoreTerminal(void)
+{
+	/* Stop and Deregister the device */		
+	RemoteTerminal = 0;
+#ifdef CFG_DEVICE_DEREGISTER			
+	device_deregister("r2cdev");
+#endif			
+
+	/* Switch back to the serial terminal */	
+	console_assign(stdin,"serial");
+	console_assign(stdout,"serial");
+	console_assign(stderr,"serial");
+
+	return;
+}
+
+void
+SetNetTerminal(void)
+{
+	/* Initialize Buffers */
+	BuffTail = BuffHead = BuffCount = 0;
+	/* Register the new device */
+	memset (&dev, 0, sizeof (dev));
+	strcpy (dev.name, "r2cdev");
+	dev.flags = DEV_FLAGS_OUTPUT | DEV_FLAGS_INPUT | DEV_FLAGS_SYSTEM;
+	dev.putc = r2cdev_putc;
+	dev.puts = r2cdev_puts;
+	dev.getc = r2cdev_getc;
+	dev.tstc = r2cdev_tstc;
+	device_register (&dev);
+
+	/* Switch console to the new device */	
+	console_assign(stdin,"r2cdev");
+	console_assign(stdout,"r2cdev");
+	console_assign(stderr,"r2cdev");
+	RemoteTerminal = 1;
+	
+	return;
+}
+
+
+int
+ProcessRemoteTerminal(uchar *pkt,int len)
+{
+	volatile R2C_RemoteTerminalResponse *ResPkt;
+	R2C_RemoteTerminal *ReqPkt = (R2C_RemoteTerminal *)pkt;
+	uchar Status = 1;
+
+	/* Redirect Terminal On or Off depending upon the Switch*/
+	if (ReqPkt->Switch)
+	{
+		if (!RemoteTerminal)
+			SetNetTerminal();
+		else
+			Status = 0;					
+	}
+	else
+	{
+		if (RemoteTerminal)
+			RestoreTerminal();
+		else
+			Status = 0;
+	}
+	
+	/* Response Packet */
+	ResPkt = (volatile R2C_RemoteTerminalResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(
+					sizeof(R2C_RemoteTerminalResponse) - sizeof(R2C_Header));
+	ResPkt->Status = Status;
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,
+							R2C_DEVICE_PORT,sizeof(R2C_RemoteTerminalResponse));
+
+	/* Set Status */
+	return (int)Status;
+}
+#endif
