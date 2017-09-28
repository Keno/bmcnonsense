--- uboot/vendors/ami/r2c/phases.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.nonet/vendors/ami/r2c/phases.c	2008-04-01 08:36:33.000000000 -0400
@@ -0,0 +1,616 @@
+# include <common.h>
+# include <config.h>
+#if (CONFIG_COMMANDS & CFG_CMD_NET)
+# include <command.h>
+# include <net.h>
+# include "fmh.h"		/* For CRC32 functions */
+# include "r2c.h"
+# include "externs.h"
+
+
+/* Used from outside */
+void R2C_Initiate(uchar *ethaddr);		/* from NetLoop() in net.c */
+uchar CallFromNetLoop 	= 1;			/* from cmd_r2c.c */
+unsigned short ConsoleUDPPort = 0;		/* from session.c */
+IPaddr_t ConsoleIP = 0;	 				/* from session.c */
+uchar ConsoleEther[6]= {0,0,0,0,0,0}; 	/* from session.c */ /*Filled by ARP */
+uchar RemoteTerminal = 0;				/* from session.c */
+
+
+
+/* Local Variables and Functions */
+static unsigned long MaxRetries = 0xFFFFFFFF; /* Almost infinity*/
+static unsigned long SeqNo 	= 0;			
+static enum r2c_phase PhaseMachine 	= R2C_PHASE_INITIALIZATION;
+static uchar OurEtherAddr[6] 	= {0,0,0,0,0,0};
+
+
+static void	 R2C_Timeout(void);
+static void  R2C_Handler(uchar *pkt,unsigned dest,unsigned src,unsigned len);
+
+static void  ProcessPhaseInitialize(uchar *pkt,unsigned len,IPaddr_t SrcIp);
+static void  ProcessPhaseAddrAssigment(uchar *pkt,unsigned len);
+static void  ProcessPhaseSessionEstablishment(uchar *pkt,unsigned len);
+static void  ProcessPhaseSessionTermination(uchar *pkt,unsigned len);
+
+static int	 FormR2CInitiatePkt(void);
+static int	 FormR2CAddrReqPkt(void);
+static int	 FormR2CHeaderOnlyPkt(ushort PktType);
+
+static void R2C_AddrRequest(unsigned char);
+static void R2C_Session_Establish(unsigned char);
+static void R2C_Session_Terminate(unsigned char);
+
+
+static
+void
+ProcessPhaseInitialize(uchar *pkt,unsigned len,IPaddr_t SrcIp)
+{
+	R2C_Header *Header;
+	R2C_NakInitiatePkt *NakPkt;
+	R2C_AckInitiatePkt *AckPkt;
+	
+	Header = (R2C_Header *)pkt;
+#if DEBUG	
+	printf("Packet with SeqNo = 0x%08X got in R2C Initiation Phase\n",
+					le32_to_host(Header->SeqNo));
+#endif	
+
+	/* Check if it is a NAK, if so terminate */
+	if (le16_to_host(Header->PktType) == R2C_NAK_INITIATE)
+	{
+		NakPkt = (R2C_NakInitiatePkt *)pkt;
+		if (memcmp(&NakPkt->UniqueId[0],OurEtherAddr,6) != 0)
+		{
+			printf("UniqueId Mismatches in R2C_NAK_INITIATE. Discarding Pkt\n");
+			return;
+		}
+		/* Terminate R2C */
+		printf("R2C_NAK_INITIATE got. Terminating R2C\n");
+		NetState = NETLOOP_FAIL;		
+		return;
+	}
+
+	/* If it is a ACK Packet */	
+	if (le16_to_host(Header->PktType) == R2C_ACK_INITIATE)
+	{
+		AckPkt = (R2C_AckInitiatePkt *)pkt;
+		if (memcmp(&AckPkt->UniqueId[0],OurEtherAddr,6) != 0)
+		{
+			printf("UniqueId Mismatches in R2C_ACK_INITIATE. Discarding Pkt\n");
+			return;
+		}
+		
+		if (AckPkt->IpVer != 4)
+		{
+			printf("Currently we support only IPv4. Discarding Pkt\n");
+			return;
+		}		
+
+		/* Get the console's UDP Port and IP Addr */
+		printf("R2C_ACK_INITIATE got\n\n");
+		ConsoleUDPPort = le16_to_host(AckPkt->ConsoleUDPPort);
+		ConsoleIP = *(IPaddr_t *)(&AckPkt->ConsoleIP[2]);	 
+		if (ConsoleIP == 0)
+		{
+#if DEBUG				
+			printf("Console Ip is not set, Using SrcIp as Console IP \n");
+#endif			
+			ConsoleIP = SrcIp;
+		}
+		
+		/* Kill current timeout */
+		NetSetTimeout(0,R2C_Timeout);
+
+		printf("Console IP      = %ld.%ld.%ld.%ld Console UDP Port = %d\n",	
+					((ulong)ConsoleIP>>24)&0xFF,	
+					((ulong)ConsoleIP>>16)&0xFF,	
+					((ulong)ConsoleIP>>8)&0xFF,	
+					((ulong)ConsoleIP>>0)&0xFF,
+					ConsoleUDPPort);
+
+		/* Switch to next phase */		
+		PhaseMachine = R2C_PHASE_ADDR_ASSIGNMENT;
+		R2C_AddrRequest(1);
+		return;
+	}
+
+	printf("Invalid Packet (0x%04X) in R2C Initiation Phase. Discarding\n",
+										le16_to_host(Header->PktType));
+	return;										
+}
+
+static
+void
+ProcessPhaseAddrAssigment(uchar *pkt,unsigned len)
+{
+	R2C_Header *Header;
+	R2C_NakAddrReqPkt *NakPkt;
+	R2C_AckAddrReqPkt *AckPkt;
+	
+	Header = (R2C_Header *)pkt;
+#if DEBUG	
+	printf("Packet with SeqNo = 0x%08X got in R2C Addr Assignment Phase\n",
+					le32_to_host(Header->SeqNo));
+#endif	
+
+	/* Check if it is a NAK, if so terminate */
+	if (le16_to_host(Header->PktType) == R2C_NAK_ADDR_REQUEST)
+	{
+		NakPkt = (R2C_NakAddrReqPkt *)pkt;
+		if (memcmp(&NakPkt->DeviceMacAddr[0],OurEtherAddr,6) != 0)
+		{
+			printf("Device MAC Addr Mismatches in R2C_NAK_ADDR_REQUEST. Discarding Pkt\n");
+			return;
+		}
+		/* Terminate R2C */
+		printf("R2C_NAK_ADDR_REQUEST got. Terminating R2C\n");
+		NetState = NETLOOP_FAIL;		
+		return;
+	}
+
+	/* If it is a ACK Packet */	
+	if (le16_to_host(Header->PktType) == R2C_ACK_ADDR_REQUEST)
+	{
+		AckPkt = (R2C_AckAddrReqPkt *)pkt;
+		if (memcmp(&AckPkt->DeviceMacAddr[0],OurEtherAddr,6) != 0)
+		{
+			printf("Device MAC Addr Mismatches in R2C_ACK_ADDR_REQUEST. Discarding Pkt\n");
+			return;
+		}
+		
+		if (AckPkt->IpVer != 4)
+		{
+			printf("Currently we support only IPv4. Discarding Pkt\n");
+			return;
+		}		
+
+		/* Get the console's UDP Port and IP Addr */
+		printf("R2C_ACK_ADDR_REQUEST got\n\n");
+		NetOurIP			= *(IPaddr_t *)(&AckPkt->DeviceIP[2]);	 
+		NetOurSubnetMask 	= *(IPaddr_t *)(&AckPkt->SubnetMask[2]);	 
+		NetOurGatewayIP 	= *(IPaddr_t *)(&AckPkt->Gateway[2]);
+		
+		/* Check if non zero IP is got */
+		if (NetOurIP == 0)
+		{
+			printf("R2C_ACK_ADDR_REQUEST got a NULL IP. Terminating R2C\n");
+			NetState = NETLOOP_FAIL;		
+			return;
+		}
+		printf("Device IP      = %ld.%ld.%ld.%ld\n",	
+					((ulong)NetOurIP>>24)&0xFF,	
+					((ulong)NetOurIP>>16)&0xFF,	
+					((ulong)NetOurIP>>8)&0xFF,	
+					((ulong)NetOurIP>>0)&0xFF);
+		printf("Device Mask    = %ld.%ld.%ld.%ld\n",	
+					((ulong)NetOurSubnetMask>>24)&0xFF,
+					((ulong)NetOurSubnetMask>>16)&0xFF,	
+					((ulong)NetOurSubnetMask>>8)&0xFF,
+					((ulong)NetOurSubnetMask>>0)&0xFF);
+		printf("Device Gateway = %ld.%ld.%ld.%ld\n",	
+					((ulong)NetOurGatewayIP>>24)&0xFF,
+					((ulong)NetOurGatewayIP>>16)&0xFF,	
+					((ulong)NetOurGatewayIP>>8)&0xFF,
+					((ulong)NetOurGatewayIP>>0)&0xFF);
+		
+		/* Kill current timeout */
+		NetSetTimeout(0,R2C_Timeout);
+
+
+		/* Switch to next phase */		
+		PhaseMachine = R2C_PHASE_SESSION_ESTABLISHMENT;
+		R2C_Session_Establish(1);
+		return;
+	}
+
+	printf("Invalid Packet (0x%04X) in R2C Address Assigment Phase. Discarding\n",
+										le16_to_host(Header->PktType));
+	return;										
+}
+
+
+static 
+void  
+ProcessPhaseSessionEstablishment(uchar *pkt,unsigned len)
+{
+	R2C_Header *Header;
+	
+	Header = (R2C_Header *)pkt;
+#if DEBUG	
+	printf("Packet with SeqNo = 0x%08X got in R2C Session Establishment Phase\n",
+					le32_to_host(Header->SeqNo));
+#endif	
+
+	/* Check if it is a NAK, if so terminate */
+	if (le16_to_host(Header->PktType) == R2C_NAK_SESSION_ESTABLISH)
+	{
+		/* Terminate R2C */
+		printf("R2C_NAK_SESSION_ESTABLISH got. Terminating R2C\n");
+		NetState = NETLOOP_FAIL;		
+		return;
+	}
+	
+	/* If it is a ACK Packet */	
+	if (le16_to_host(Header->PktType) == R2C_ACK_SESSION_ESTABLISH)
+	{
+		printf("R2C_ACK_SESSION_ESTABLISH got\n\n");
+		
+		/* Kill current timeout */
+		NetSetTimeout(0,R2C_Timeout);
+
+		/* Switch to next phase */		
+		PhaseMachine = R2C_PHASE_SESSION;
+		EnterR2CSession();
+		return;
+	}
+
+	printf("Invalid Packet (0x%04X) in R2C Session Establisment Phase. Discarding\n",
+										le16_to_host(Header->PktType));
+	return;										
+}
+
+static 
+void  
+ProcessPhaseSessionTermination(uchar *pkt,unsigned len)
+{
+	R2C_Header *Header;
+	
+	Header = (R2C_Header *)pkt;
+#if DEBUG	
+	printf("Packet with SeqNo = 0x%08X got in R2C Session Termination Phase\n",
+					le32_to_host(Header->SeqNo));
+#endif	
+
+	/* Check if it is a NAK, if so terminate */
+	if (le16_to_host(Header->PktType) == R2C_NAK_SESSION_TERMINATE)
+	{
+		/* Terminate R2C */
+		printf("R2C_NAK_SESSION_TERMINATE got. Terminating R2C\n");
+		NetState = NETLOOP_FAIL;		
+		return;
+	}
+	
+	/* If it is a ACK Packet */	
+	if (le16_to_host(Header->PktType) == R2C_ACK_SESSION_TERMINATE)
+	{
+		printf("R2C_ACK_SESSION_TERMINATE got\n\n");
+		
+		/* Kill current timeout */
+		NetSetTimeout(0,R2C_Timeout);
+
+		/* Terminate R2C */
+		printf("R2C Session Completed Successfully\n");
+		NetBootFileXferSize = 0;
+		NetState = NETLOOP_SUCCESS;		
+		return;
+	}
+
+	printf("Invalid Packet (0x%04X) in R2C Session Termination Phase. Discarding\n",
+										le16_to_host(Header->PktType));
+	return;										
+}
+
+/* Note: This routine will receive all broadcast and unicast (to us) 
+         UDP Packets  */
+static
+void
+R2C_Handler(uchar *pkt,unsigned dest,unsigned src,unsigned len)
+{
+	IP_t *ip;
+	R2C_Header *Header = (R2C_Header *)pkt;
+	
+	/* Find if it is the required packet (Filter based on udp port)*/
+	if ((dest != R2C_DEVICE_PORT) && (dest != R2C_TERMINAL_PORT))
+		return;
+
+	/* Check if it is console port */
+	if (dest == R2C_TERMINAL_PORT)
+	{
+		if (!RemoteTerminal)
+		{
+			printf("Remote Terminal is disabled. Discarding Terminal pkt\n");
+			return;				
+		}
+#if DEBUG	
+		printf("Packet got in R2C Terminal Port\n") ;
+#endif		
+		AddtoInputQueue(pkt,len);
+		return;
+	}
+
+	/* @ Here it is a R2C Packet */
+	
+#if DEBUG	
+	printf("\n");
+	printf("Packet got in R2C Device Port\n") ;
+#endif	
+	
+	/* Validate The packet . CRC32 CheckSum */
+	if (Calculate_R2C_Crc32(pkt,le16_to_host(Header->PktLen)) 
+					!= le32_to_host(Header->Crc32))
+	{
+		printf("Checksum Invalid in the R2C packet. Discarding pkt\n");
+		return;		
+	}	
+
+	
+	/* Get the IP of Sender */
+	ip = (IP_t *)(pkt - IP_HDR_SIZE);
+	/* Check the packet got is from our console*/
+	if (PhaseMachine != R2C_PHASE_INITIALIZATION)
+	{
+		if (ip->ip_src != ConsoleIP)
+		{
+			printf("Packet from different console %ld.%ld.%ld.%ld. Discarding Pkt\n",
+				((ulong)ip->ip_src>>24)&0xFF,((ulong)ip->ip_src>>16)&0xFF,
+				((ulong)ip->ip_src>>8)&0xFF,((ulong)ip->ip_src>>0)&0xFF);
+		}
+	}
+	
+	switch (PhaseMachine)
+	{
+		case R2C_PHASE_INITIALIZATION:
+			ProcessPhaseInitialize(pkt,len,ip->ip_src);
+			break;
+		case R2C_PHASE_ADDR_ASSIGNMENT:
+			ProcessPhaseAddrAssigment(pkt,len);
+			break;			
+		case R2C_PHASE_SESSION_ESTABLISHMENT:
+			ProcessPhaseSessionEstablishment(pkt,len);
+			break;			
+		case R2C_PHASE_SESSION:
+			if (ProcessSessionCommands(pkt,len) != 0)
+			{
+				PhaseMachine = R2C_PHASE_SESSION_TERMINATION;
+				R2C_Session_Terminate(1);
+			}
+			break;
+		case R2C_PHASE_SESSION_TERMINATION:						
+			ProcessPhaseSessionTermination(pkt,len);					
+			break;
+
+		default:
+			Header = (R2C_Header *)pkt;
+			printf("Unknown R2C Phase (%d). Packet Type 0x%04X SeqNo 0x%08X\n",
+						PhaseMachine,le16_to_host(Header->PktType),
+								le32_to_host(Header->SeqNo));
+			break;
+	}
+
+#if DEBUG	
+	printf("Console Ether Addr = %02X:%02X:%02X:%02X:%02X:%02X\n",
+						ConsoleEther[0],ConsoleEther[1],ConsoleEther[2],
+						ConsoleEther[3],ConsoleEther[4],ConsoleEther[5]);
+#endif	
+	return;
+}
+
+static
+void
+R2C_Timeout(void)
+{
+	printf("R2C TimeOut\n");
+	MaxRetries --;
+	if (MaxRetries == 0)
+	{
+		printf("Max Retry Reached. R2C Failed. Resetting Hardware...\n");
+		NetState = NETLOOP_FAIL;		
+		do_reset(NULL,0,0,NULL);
+		return;
+	}
+	if (PhaseMachine == R2C_PHASE_INITIALIZATION)
+			R2C_Initiate(OurEtherAddr);
+	if (PhaseMachine == R2C_PHASE_ADDR_ASSIGNMENT)
+			R2C_AddrRequest(0);
+	if (PhaseMachine == R2C_PHASE_SESSION_ESTABLISHMENT)
+			R2C_Session_Establish(0);
+	if (PhaseMachine == R2C_PHASE_SESSION_TERMINATION)
+			R2C_Session_Terminate(0);
+			
+}
+
+
+static
+int
+FormR2CHeaderOnlyPkt(ushort PktType)
+{
+	volatile R2C_HeaderOnlyPkt *pkt;
+	
+	pkt = (volatile R2C_HeaderOnlyPkt *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	pkt->Header.SeqNo 	= host_to_le32(SeqNo);
+	pkt->Header.PktType = host_to_le16(PktType); 
+	pkt->Header.PktLen  = host_to_le16(0);
+
+	pkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)pkt,
+									le16_to_host(pkt->Header.PktLen)));
+	
+	SeqNo++;
+
+	printf(" with SeqNo = 0x%08x ...\n",le32_to_host(pkt->Header.SeqNo));
+	return sizeof(R2C_HeaderOnlyPkt);
+
+}	
+
+static
+int
+FormR2CInitiatePkt(void)
+{
+	volatile R2C_InitiatePkt *pkt;
+	
+	pkt = (volatile R2C_InitiatePkt *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	pkt->Header.SeqNo 	= host_to_le32(SeqNo);
+	pkt->Header.PktType = host_to_le16(R2C_INITIATE); 
+	pkt->Header.PktLen  = host_to_le16(
+					sizeof(R2C_InitiatePkt) - sizeof(R2C_Header));
+	pkt->DeviceUDPPort	= host_to_le16(R2C_DEVICE_PORT);
+	memcpy((void *)(&(pkt->UniqueId[0])),OurEtherAddr,6);
+
+	pkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)pkt,
+									le16_to_host(pkt->Header.PktLen)));
+	SeqNo++;
+
+	printf(" with SeqNo = 0x%08x ...\n",le32_to_host(pkt->Header.SeqNo));
+	return sizeof(R2C_InitiatePkt);
+
+}
+
+static
+int
+FormR2CAddrReqPkt(void)
+{
+	volatile R2C_AddrReqPkt *pkt;
+	
+	pkt = (volatile R2C_AddrReqPkt *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	pkt->Header.SeqNo 	= host_to_le32(SeqNo);
+	pkt->Header.PktType = host_to_le16(R2C_ADDR_REQUEST); 
+	pkt->Header.PktLen  = host_to_le16(
+					sizeof(R2C_AddrReqPkt) - sizeof(R2C_Header));
+	pkt->IpVer			= 4;
+	pkt->ConsoleIP[0]	= 0;
+	pkt->ConsoleIP[1]	= 0;
+	*(IPaddr_t *)(&pkt->ConsoleIP[2]) = ConsoleIP;	 
+	memcpy((void *)(&(pkt->DeviceMacAddr[0])),OurEtherAddr,6);
+
+	pkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)pkt,
+									le16_to_host(pkt->Header.PktLen)));
+	
+	SeqNo++;
+
+	printf(" with SeqNo = 0x%08x ...\n",le32_to_host(pkt->Header.SeqNo));
+	return sizeof(R2C_AddrReqPkt);
+
+}
+
+static
+void
+R2C_Session_Establish(unsigned char Call)
+{
+	int len;
+	
+	/* Form  Session Establishment Packet and unicast to console */
+	printf("Sending R2C Session Establishment Packet");
+	len  = FormR2CHeaderOnlyPkt(R2C_SESSION_ESTABLISH);
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,R2C_DEVICE_PORT,len);
+	
+	/* Set Timeout */
+	if (Call == 1)
+		MaxRetries = 10;
+	NetSetTimeout(R2C_TIMEOUT * CFG_HZ,R2C_Timeout);
+	return;
+
+}
+		
+static
+void
+R2C_Session_Terminate(unsigned char Call)
+{
+	int len;
+	
+	/* Form  Session Establishment Packet and unicast to console */
+	printf("Sending R2C Session Termination Packet");
+	len  = FormR2CHeaderOnlyPkt(R2C_SESSION_TERMINATE);
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,R2C_DEVICE_PORT,len);
+	
+	/* Set Timeout */
+	if (Call == 1)
+		MaxRetries = 10;
+	NetSetTimeout(R2C_TIMEOUT * CFG_HZ,R2C_Timeout);
+	return;
+}
+
+static
+void
+R2C_AddrRequest(unsigned char Call)
+{
+	int len;
+	
+	/* Form Address Request Packet and broadcast */
+	printf("Sending R2C Addr Request Packet");
+	len  = FormR2CAddrReqPkt();
+	NetSendUDPPacket(ConsoleEther,0xFFFFFFFF,ConsoleUDPPort,R2C_DEVICE_PORT,len);
+	
+	/* Set Timeout */
+	if (Call == 1)
+		MaxRetries = 10;
+	NetSetTimeout(R2C_TIMEOUT * CFG_HZ,R2C_Timeout);
+	return;
+}
+
+
+
+void
+R2C_Initiate(uchar *ethaddr)
+{
+	int len;
+
+	if (CallFromNetLoop)
+	{
+		printf("R2CInitiate Called\n");
+		printf("Our ethaddr = %02X:%02X:%02X:%02X:%02X:%02X\n",
+			ethaddr[0],ethaddr[1],ethaddr[2],ethaddr[3],ethaddr[4],ethaddr[5]);	
+	
+		/* Initialize Variables */
+		SeqNo = 0;
+		PhaseMachine = R2C_PHASE_INITIALIZATION;
+		NetOurIP			= 0;
+		NetOurSubnetMask 	= 0;
+		NetOurGatewayIP 	= 0;
+	
+		/* Save the etheraddr */
+		memcpy(OurEtherAddr,ethaddr,6);
+
+		/* Set Console Ether to Zero => a ARP will be sent on first send() */
+		memset(ConsoleEther,0,6);
+	
+		/* Hook Our Handler */
+		NetSetHandler(R2C_Handler);
+	
+		CallFromNetLoop = 0;
+		MaxRetries = 0xFFFFFFFF;
+	}
+	
+
+	/* Form Initiate Packet and Broadcast it out */
+	printf("Sending R2C Initiate Packet");
+	len  = FormR2CInitiatePkt();
+	NetSendUDPPacket(ConsoleEther,0xFFFFFFFF,R2C_UDP_PORT,R2C_DEVICE_PORT,len);
+	
+	/* Set Timeout */
+	NetSetTimeout(R2C_TIMEOUT * CFG_HZ,R2C_Timeout);
+	return;
+}
+
+unsigned long
+Calculate_R2C_Crc32(uchar *pkt,int len)
+{
+	unsigned long Crc32;
+	int i,Index=0;
+
+	BeginCRC32(&Crc32);
+
+	/* Calculate CRC for header excluding the field containing the CRC */	
+	for (i=0;i<(sizeof(R2C_Header)-sizeof(unsigned long));i++,Index++)
+			DoCRC32(&Crc32,pkt[Index]);
+
+	/* Skip over the CRC Field in header */
+	Index +=sizeof(unsigned long);
+
+	/* Calculate CRC for data part */
+	for (i=0;i<len;i++,Index++)
+		DoCRC32(&Crc32,pkt[Index]);
+	
+
+	EndCRC32(&Crc32);
+	return Crc32;
+}
+#endif
