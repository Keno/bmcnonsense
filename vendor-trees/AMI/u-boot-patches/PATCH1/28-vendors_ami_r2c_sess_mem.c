--- uboot/vendors/ami/r2c/sess_mem.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.nonet/vendors/ami/r2c/sess_mem.c	2008-04-01 08:37:34.000000000 -0400
@@ -0,0 +1,462 @@
+# include <common.h>
+# include <config.h>
+#if (CONFIG_COMMANDS & CFG_CMD_NET)
+# include <command.h>
+# include <net.h>
+# include "r2c.h"
+# include "externs.h"
+
+uchar AllocMemory = 0;
+
+int
+IsFlash(unsigned long Dest)
+{
+#ifdef CFG_NO_FLASH
+	return 0;
+#else
+	if (addr2info(Dest) != NULL)	
+		return 1;
+	return 0;
+#endif	
+}
+
+int 
+Write2MemByte(unsigned char *Dest, unsigned char Data)
+{
+	flash_info_t *info;
+	int rc;
+	
+	if (IsFlash((unsigned long)Dest))
+	{
+		info = addr2info((unsigned long)Dest);
+		rc = write_buff(info,(uchar *)&Data,(ulong)Dest,sizeof(Data));
+		if (rc != 0) 
+			return 0;		
+	}
+	else
+		*Dest = Data;
+	
+	if (*Dest == Data)		
+		return 1;
+	return 0;	
+}
+
+int
+Write2MemWord(unsigned short *Dest,unsigned short Data)
+{
+	flash_info_t *info;
+	int rc;
+
+	if (IsFlash((unsigned long)Dest))
+	{
+		info = addr2info((unsigned long)Dest);
+		rc = write_buff(info,(uchar *)&Data,(ulong)Dest,sizeof(Data));
+		if (rc != 0) 
+			return 0;		
+	}		
+	else
+		*Dest = Data;
+	
+	if (*Dest == Data)		
+		return 1;
+	return 0;	
+
+}
+
+int
+Write2MemDword(unsigned long *Dest,unsigned long Data)
+{
+	flash_info_t *info;
+	int rc;
+	
+	if (IsFlash((unsigned long)Dest))
+	{
+		info = addr2info((unsigned long)Dest);
+		rc = write_buff(info,(uchar *)&Data,(ulong)Dest,sizeof(Data));
+		if (rc != 0) 
+			return 0;		
+	}		
+	else
+		*Dest = Data;
+	
+	if (*Dest == Data)		
+		return 1;
+	return 0;	
+}
+		
+		
+
+
+int
+ProcessReadMemory(uchar *pkt,int len)
+{
+	volatile R2C_ReadMemoryResponse *ResPkt;
+	R2C_ReadMemory *ReqPkt = (R2C_ReadMemory *)pkt;
+	unsigned short Count,CpLen,i;
+	unsigned char *bSrcAddr, *bDestAddr;
+	unsigned short *wSrcAddr, *wDestAddr;
+	unsigned long *dSrcAddr, *dDestAddr;
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_ReadMemoryResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Check for length */
+	Count = le16_to_host(ReqPkt->Size);
+	if (Count > 256)
+	{
+		printf("ERROR: Length should be <= 256 bytes\n");
+		Count = 0;
+	}
+	else
+	{	
+		/* Do the actual operation here */
+		switch (ReqPkt->Width)
+		{
+			case 8:
+				CpLen = Count/1;
+				bSrcAddr = (unsigned char *)le32_to_host(ReqPkt->Address);
+				bDestAddr = (unsigned char*)(&ResPkt->Data[0]);
+				for(i=0;i<CpLen;i++)
+					bDestAddr[i] = bSrcAddr[i];
+				break;
+			case 16:
+				CpLen = Count/2;
+				wSrcAddr = (unsigned short *)le32_to_host(ReqPkt->Address);
+				wDestAddr = (unsigned short*)(&ResPkt->Data[0]);
+				for(i=0;i<CpLen;i++)
+					wDestAddr[i] = wSrcAddr[i];
+				break;	
+			case 32:
+				CpLen = Count/4;
+				dSrcAddr = (unsigned long *)le32_to_host(ReqPkt->Address);
+				dDestAddr = (unsigned long *)(&ResPkt->Data[0]);
+				for(i=0;i<CpLen;i++)
+					dDestAddr[i] = dSrcAddr[i];
+				break;
+			default:
+				printf ("ERROR:Bus Width should be 8 or 16 or 32 \n");
+				Count = 0;	
+				break;
+		}
+	}
+	
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_ReadMemoryResponse) 
+					- sizeof(R2C_Header) -256 + Count);
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return Count;
+}
+
+int
+ProcessWriteMemory(uchar *pkt,int len)
+{
+	volatile R2C_WriteMemoryResponse *ResPkt;
+	R2C_WriteMemory *ReqPkt = (R2C_WriteMemory *)pkt;
+	unsigned short Count,CpLen,i;
+	unsigned char *bSrcAddr, *bDestAddr;
+	unsigned short *wSrcAddr, *wDestAddr;
+	unsigned long *dSrcAddr, *dDestAddr;
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_WriteMemoryResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Check for length */
+	Count = le16_to_host(ReqPkt->Header.PktLen) 
+						- sizeof(long) - sizeof(char);
+	if (Count > 256)
+	{
+		printf("ERROR: Length should be <= 256 bytes\n");
+		i = 0;
+	}
+	else
+	{	
+		/* Do the actual operation here */
+		switch (ReqPkt->Width)
+		{
+			case 8:
+				CpLen = Count/1;
+				bDestAddr = (unsigned char *)le32_to_host(ReqPkt->Address);
+				bSrcAddr = (unsigned char*)(&ReqPkt->Data[0]);
+				for(i=0;i<CpLen;i++)
+				{
+					if (Write2MemByte(&bDestAddr[i], bSrcAddr[i]) == 0)
+						break;							
+				}
+				i=i*1;
+				break;
+			case 16:
+				CpLen = Count/2;
+				wDestAddr = (unsigned short *)le32_to_host(ReqPkt->Address);
+				wSrcAddr = (unsigned short*)(&ReqPkt->Data[0]);
+				for(i=0;i<CpLen;i++)
+				{
+					if (Write2MemWord(&wDestAddr[i],wSrcAddr[i]) == 0)
+						break;							
+				}
+				i=i*2;
+				break;	
+			case 32:
+				CpLen = Count/4;
+				dDestAddr = (unsigned long *)le32_to_host(ReqPkt->Address);
+				dSrcAddr = (unsigned long *)(&ReqPkt->Data[0]);
+				for(i=0;i<CpLen;i++)
+				{
+					if (Write2MemDword(&dDestAddr[i],dSrcAddr[i]) == 0)
+						break;							
+				}
+				i=i*4;
+				break;
+			default:
+				printf ("ERROR:Bus Width should be 8 or 16 or 32 \n");
+				i = 0;	
+				break;
+		}
+	}
+	
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	
+					sizeof(R2C_WriteMemoryResponse) - sizeof(R2C_Header));
+
+	ResPkt->SizeWritten	= host_to_le16(i);					
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return i;
+}
+
+int
+ProcessCopyMemory(uchar *pkt,int len)
+{
+	volatile R2C_CopyMemoryResponse *ResPkt;
+	R2C_CopyMemory *ReqPkt = (R2C_CopyMemory *)pkt;
+	unsigned long Count,i;
+	unsigned char *bSrcAddr, *bDestAddr;
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_CopyMemoryResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Check for length */
+	Count = le32_to_host(ReqPkt->Size);
+	bSrcAddr = (unsigned char *)le32_to_host(ReqPkt->Src);
+	bDestAddr = (unsigned char*)le32_to_host(ReqPkt->Dest);
+	for(i=0;i<Count;i++)
+	{
+		if (Write2MemByte(&bDestAddr[i], bSrcAddr[i]) == 0)
+			break;
+	}
+	
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	
+					sizeof(R2C_CopyMemoryResponse) - sizeof(R2C_Header));
+	ResPkt->SizeCopied 		= host_to_le32(i);					
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return i;
+}
+
+int
+ProcessCompareMemory(uchar *pkt,int len)
+{
+	volatile R2C_CompareMemoryResponse *ResPkt;
+	R2C_CompareMemory *ReqPkt = (R2C_CompareMemory *)pkt;
+	unsigned long Count,i;
+	unsigned char *bSrcAddr, *bDestAddr;
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_CompareMemoryResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Check for length */
+	Count = le32_to_host(ReqPkt->Size);
+	bSrcAddr = (unsigned char *)le32_to_host(ReqPkt->Addr1);
+	bDestAddr = (unsigned char*)le32_to_host(ReqPkt->Addr2);
+	for(i=0;i<Count;i++)
+	{
+		if (bDestAddr[i] != bSrcAddr[i])
+			break;
+	}
+	
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	
+					sizeof(R2C_CompareMemoryResponse) - sizeof(R2C_Header));
+	ResPkt->Offset 		= host_to_le32(i);					
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	if (i == Count)
+		return 1;
+	return 0;
+}
+
+int
+ProcessClearMemory(uchar *pkt,int len)
+{
+	volatile R2C_ClearMemoryResponse *ResPkt;
+	R2C_ClearMemory *ReqPkt = (R2C_ClearMemory *)pkt;
+	unsigned long Count,i;
+	unsigned char *bAddr;
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_ClearMemoryResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Check for length */
+	Count = le32_to_host(ReqPkt->Size);
+	bAddr = (unsigned char *)le32_to_host(ReqPkt->Addr);
+#if DEBUG	
+	printf("Clearing 0x%08lx of Size 0x%08lx\n",bAddr,Count);
+#endif	
+	for(i=0;i<Count;i++)
+	{
+		if (Write2MemByte(&bAddr[i],0) == 0)
+			break;
+	}
+#if DEBUG	
+	printf("Clearing Completed\n");
+#endif
+	
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	
+					sizeof(R2C_ClearMemoryResponse) - sizeof(R2C_Header));
+	ResPkt->SizeCleared 	= host_to_le32(i);					
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return i;
+}
+
+
+int
+ProcessAllocateMemory(uchar *pkt,int len)
+{
+	/* 
+	 * Only one allocation at a time is allowed. Start Allocating 
+	 * from 1M till TotalMemory - 2M  which means Max Memory that 
+	 * can be allocated is  TotalMemory - 3M
+	 */
+	DECLARE_GLOBAL_DATA_PTR;
+		
+	volatile R2C_AllocateMemoryResponse *ResPkt;
+	R2C_AllocateMemory *ReqPkt = (R2C_AllocateMemory *)pkt;
+	unsigned long AllocAddr = 0;
+#ifdef CONFIG_ARM
+	unsigned long MemSize = gd->bd->bi_dram[0].size;
+#else
+	unsigned long MemSize = gd->bd->bi_memsize;
+#endif
+
+	if (AllocMemory == 0)
+	{
+		if (le32_to_host(ReqPkt->Size) <= (MemSize - 0x300000))
+		{
+			AllocAddr = 0x100000;
+			AllocMemory = 1;
+		}
+	}		
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_AllocateMemoryResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	
+					sizeof(R2C_AllocateMemoryResponse) - sizeof(R2C_Header));
+	ResPkt->Addr 	= host_to_le32(AllocAddr);					
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return AllocAddr;		
+}
+
+
+
+int
+ProcessFreeMemory(uchar *pkt,int len)
+{
+	volatile R2C_FreeMemoryResponse *ResPkt;
+	R2C_FreeMemory *ReqPkt = (R2C_FreeMemory *)pkt;
+	unsigned char Status =1;
+
+	if ((le32_to_host(ReqPkt->Addr) == 0) || (AllocMemory == 0))
+		Status = 0;
+	else
+		AllocMemory = 0;
+	
+	
+	/* Response Packet */
+	ResPkt = (volatile R2C_FreeMemoryResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Fill the Response Packet */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	
+					sizeof(R2C_FreeMemoryResponse) - sizeof(R2C_Header));
+	ResPkt->Status 	= Status;					
+
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return Status;
+}
+
+#endif
