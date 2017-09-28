--- uboot/vendors/ami/r2c/sess_flash.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.nonet/vendors/ami/r2c/sess_flash.c	2008-04-01 08:36:59.000000000 -0400
@@ -0,0 +1,436 @@
+# include <common.h>
+# include <config.h>
+#if (CONFIG_COMMANDS & CFG_CMD_NET)
+# include <command.h>
+# include <net.h>
+# include "r2c.h"
+# include "fmh.h"
+# include "externs.h"
+
+extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
+
+const unsigned long PowerTable[] =
+{
+	0x00000001, 0x00000002, 0x00000004, 0x00000008, 
+	0x00000010, 0x00000020, 0x00000040, 0x00000080,
+	0x00000100, 0x00000200, 0x00000400, 0x00000800,
+	0x00001000, 0x00002000, 0x00004000, 0x00008000,
+	0x00010000, 0x00020000, 0x00040000, 0x00080000,
+	0x00100000, 0x00200000, 0x00400000, 0x00800000,
+	0x01000000, 0x02000000, 0x04000000, 0x08000000,
+	0x10000000, 0x20000000, 0x40000000, 0x80000000,
+};	
+
+const unsigned char PowerTableSize = sizeof(PowerTable)/sizeof(unsigned long);
+
+static
+unsigned char
+GetPower2(unsigned long size)
+{
+	unsigned char i;		
+	for (i=0;i<PowerTableSize;i++)
+	{
+		if (PowerTable[i] == size)			
+			return i;
+	}		
+
+	return 255; /* Error */					
+}					
+
+static
+unsigned long
+GetEraseBlockSize(void)
+{
+	flash_info_t *flinfo;
+	int i,bank;
+	unsigned long EraseSize,size;
+	
+	EraseSize = 0;
+	for (bank=0;bank<CFG_MAX_FLASH_BANKS;bank++)
+	{
+		flinfo = &flash_info[bank];
+		if (flinfo->size == 0)
+			break;
+		for (i=0;i<flinfo->sector_count;i++)
+		{			
+			if (i != (flinfo->sector_count-1))
+				size = flinfo->start[i+1]-flinfo->start[i];
+			else
+				size = flinfo->start[0] + flinfo->size - flinfo->start[i];		
+			EraseSize = (size > EraseSize)?size:EraseSize;
+		}		
+	}
+	return EraseSize;
+	
+}
+
+
+static
+unsigned short  
+GetModuleOffsets(unsigned long *ModuleOffset)
+{	
+	flash_info_t *flinfo;
+	unsigned long SectorSize;
+	unsigned long SectorCount;	/* Uniform Size Sector Count */	
+	int i,bank;
+	unsigned short count;
+	FMH	*fmh;
+	
+	count = 0;
+	for (bank=0;bank<CFG_MAX_FLASH_BANKS;bank++)
+	{
+		flinfo = &flash_info[bank];
+		if (flinfo->size == 0)
+			break;
+		SectorCount = flinfo->sector_count;
+
+		/* Normally Flash those have a boot sectors have one top/bottom
+		 * sector divided into 8 smaller sectors, which means the flash 
+	 	* will have 7 more sectors leading to a odd number of sectors.
+	 	* Use this to find the actual number of uniform size sectors 
+	 	*/
+		if (SectorCount % 2)	
+		{
+			printf("This flash has non uniform sectors\n");
+			SectorCount = SectorCount - 8 + 1;
+		}
+		SectorSize = flinfo->size/SectorCount;
+
+		for (i=0;i<SectorCount;)
+		{
+			fmh = ScanforFMH((unsigned char *)flinfo->start[i],SectorSize);
+			if (fmh == NULL)
+			{
+				i++;
+				continue;
+			}
+
+			/* Save the fmh location in the array */
+			ModuleOffset[count++] = host_to_le32((unsigned long)fmh);	
+		
+			/* Skip the Sectors occupied by the Module */		
+			i+=(le32_to_host(fmh->FMH_AllocatedSize)/SectorSize);
+		}
+	}	
+	return count;
+}	
+
+int
+ProcessGetFlashInfo(uchar *pkt,int len)
+{
+	volatile R2C_GetFlashInfoResponse *ResPkt;
+	R2C_GetFlashInfo *ReqPkt = (R2C_GetFlashInfo *)pkt;
+	unsigned short ExtraLen = 0;
+	flash_info_t *flinfo;
+	unsigned char *EraseSizeList;
+	unsigned long i,size,bank;
+	unsigned long fullsize;
+
+	fullsize = 0;
+	for (bank=0;bank<CFG_MAX_FLASH_BANKS;bank++)
+		fullsize+=flash_info[bank].size;
+
+	flinfo = &flash_info[0];
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_GetFlashInfoResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Fill the Response Packet Data */
+	ResPkt->FlashSize 	= host_to_le32(fullsize);
+	ResPkt->FlashAddr 	= host_to_le32(flinfo->start[0]);
+	ResPkt->FlashWidth 	= 2;
+	ResPkt->FMHCompliance 	= 1;
+	ResPkt->EraseBlockCount = host_to_le16(flinfo->sector_count);
+
+	/* High Byte is Vendor and Low Byte is Type */
+	ResPkt->ProductID 	= (flinfo->flash_id & FLASH_TYPEMASK) |
+						  ((flinfo->flash_id & FLASH_VENDMASK) >> 8);
+	ResPkt->ProductID   = host_to_le16(ResPkt->ProductID);
+	
+	/* Fill the Erase Block Information */
+	EraseSizeList =  (unsigned char *)(ResPkt + 1);
+	ResPkt->EraseSize 	= 0;
+	for (bank=0;bank<CFG_MAX_FLASH_BANKS;bank++)
+	{
+		flinfo = &flash_info[bank];
+		if (flinfo->size == 0)
+			break;
+		ExtraLen = flinfo->sector_count;
+		for (i=0;i<flinfo->sector_count;i++)
+		{			
+			if (i != (flinfo->sector_count-1))
+				size = flinfo->start[i+1]-flinfo->start[i];
+			else
+				size = flinfo->start[0] + flinfo->size - flinfo->start[i];		
+			EraseSizeList[i] = GetPower2(size);	
+			ResPkt->EraseSize = (size > ResPkt->EraseSize)?size:ResPkt->EraseSize;
+		}		
+	}
+	ResPkt->EraseSize = host_to_le32(ResPkt->EraseSize);
+	
+	/* Fill the Response Packet Header */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_GetFlashInfoResponse) 
+								- sizeof(R2C_Header) + ExtraLen);
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return 1;
+}
+
+int
+ProcessEraseFlash(uchar *pkt,int len)
+{
+	volatile R2C_EraseFlashResponse *ResPkt;
+	R2C_EraseFlash *ReqPkt = (R2C_EraseFlash *)pkt;
+	flash_info_t *flinfo;
+	unsigned char Status = 1;
+	int rc;
+	unsigned long BlockNo;
+
+	flinfo = &flash_info[0];
+	BlockNo = le32_to_host(ReqPkt->BlockNumber);
+	
+	if (BlockNo != 0xFFFFFFFF)	
+	{
+		/* Validate the block No for single block erase */
+		if (BlockNo > flinfo->sector_count)
+			rc = 1;
+		else
+			rc= flash_erase(flinfo,BlockNo,BlockNo);
+	}
+	else
+	{
+		/* Full Erase */			
+		rc =flash_erase(flinfo,0,flinfo->sector_count-1);	
+	}
+
+	if (rc != 0)
+		Status = 0;			
+			
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_EraseFlashResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Fill the Response Packet Header */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_EraseFlashResponse) 
+								- sizeof(R2C_Header));
+	ResPkt->Status 			= Status;
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return 1;
+}
+
+int
+ProcessProtectFlash(uchar *pkt,int len)
+{
+	volatile R2C_ProtectFlashResponse *ResPkt;
+	R2C_ProtectFlash *ReqPkt = (R2C_ProtectFlash *)pkt;
+	flash_info_t *flinfo;
+	unsigned char Status = 1;
+	unsigned long BlockNo,i;
+
+	flinfo = &flash_info[0];
+	BlockNo = le32_to_host(ReqPkt->BlockNumber);
+	
+	if (BlockNo != 0xFFFFFFFF)	
+	{
+		/* Validate the block No for single block erase */
+		if (BlockNo > flinfo->sector_count)
+			Status = 0;
+		else
+		{
+#if defined(CFG_FLASH_PROTECTION)
+			flash_real_protect(flinfo,BlockNo,ReqPkt->Switch);
+#else
+			flinfo->protect[BlockNo] = ReqPkt->Switch;				
+#endif			
+		}			
+	}
+	else
+	{
+			for(i=0;i<flinfo->sector_count;i++)
+			{
+#if defined(CFG_FLASH_PROTECTION)
+				flash_real_protect(flinfo,i,ReqPkt->Switch);
+#else
+				flinfo->protect[i] = ReqPkt->Switch;				
+#endif			
+			}
+	}
+
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_ProtectFlashResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Fill the Response Packet Header */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_ProtectFlashResponse) 
+								- sizeof(R2C_Header));
+	ResPkt->Status 			= Status;
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return 1;
+}
+
+int
+ProcessGetFMHInfo(uchar *pkt,int len)
+{
+	volatile R2C_GetFMHInfoResponse *ResPkt;
+	R2C_GetFMHInfo *ReqPkt = (R2C_GetFMHInfo *)pkt;
+	unsigned short ExtraLen = 0;
+//	flash_info_t *flinfo;
+	unsigned long *ModuleOffsetList;
+
+//	flinfo = &flash_info[0];
+
+	/* Response Packet */
+	ResPkt = (volatile R2C_GetFMHInfoResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Fill the Response Packet Data */
+	ModuleOffsetList =  (unsigned long *)(ResPkt + 1);
+	ExtraLen = GetModuleOffsets(ModuleOffsetList);
+	ResPkt->ModuleCount = host_to_le16(ExtraLen);
+	ResPkt->BootBlockSize 	= 0;
+	ResPkt->BootBlockOffset = 0;
+	
+	ExtraLen *= sizeof(unsigned long);
+	
+	/* Fill the Response Packet Header */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_GetFMHInfoResponse) 
+								- sizeof(R2C_Header) + ExtraLen);
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return 1;
+}
+
+int 
+ProcessEraseCopyFlash(uchar *pkt,int len)
+{
+	volatile R2C_EraseCopyFlashResponse *ResPkt;
+	R2C_EraseCopyFlash *ReqPkt = (R2C_EraseCopyFlash *)pkt;
+	flash_info_t *flinfo;
+	unsigned long BlockNo,Copied;
+	unsigned long EraseSize;
+	unsigned long StartBlockNo,EndBlockNo,AlignedSize;
+	unsigned long Count;
+	unsigned char *bSrcAddr,*bDestAddr;		
+	
+
+	/* Get Erase Block Size of the Flash */
+	flinfo = &flash_info[0];
+	EraseSize = GetEraseBlockSize();
+
+	/* Convert Required Values to Host format */	
+	Count = le32_to_host(ReqPkt->Size);
+	bSrcAddr = (unsigned char *)le32_to_host(ReqPkt->SrcMemory);
+	bDestAddr = (unsigned char*)le32_to_host(ReqPkt->DestFlash);
+#if DEBUG	
+	printf("Src = 0x%08lX Dest = 0x%08lX Count = 0x%08lX\n", 
+				(unsigned long)bSrcAddr, (unsigned long)bDestAddr, Count);
+#endif	
+	
+	/* If Destination is not the Flash Addr, return Error */
+	if (!IsFlash((unsigned long)bDestAddr))
+	{
+		Copied = 0;
+		goto RetEraseCopyFlash;
+	}				
+	
+	/* Align the DestAddr and Size to the EraseBlockSize */
+	StartBlockNo = ((unsigned long)bDestAddr / EraseSize) * EraseSize;
+	AlignedSize  = Count + ((unsigned long)bDestAddr - StartBlockNo);
+    AlignedSize  = ((AlignedSize + EraseSize -1 )/ EraseSize) * EraseSize;
+#if DEBUG	
+	printf("Aligned Dest = 0x%08lX Aligned Size = 0x%08lX\n",
+										StartBlockNo,AlignedSize);
+#endif	
+	
+	/* Convert Flash Addr Into Block Number */
+	StartBlockNo-=flinfo->start[0];
+	StartBlockNo = StartBlockNo/EraseSize;
+	EndBlockNo = StartBlockNo + (AlignedSize/EraseSize) - 1;
+#if DEBUG	
+	printf("Start Block = 0x%08lX End Block = 0x%08lx\n",
+									StartBlockNo,EndBlockNo);
+#endif	
+
+	/* Unprotect the Required Blocks */
+	for (BlockNo = StartBlockNo; BlockNo < EndBlockNo;BlockNo++)
+	{	
+#if defined(CFG_FLASH_PROTECTION)
+			flash_real_protect(flinfo,BlockNo,0);
+#else
+			flinfo->protect[BlockNo] = 0;				
+#endif			
+	}
+
+	/* Erase the Required Blocks */	
+	if (flash_erase(flinfo,StartBlockNo,EndBlockNo) != 0)
+		Copied = 0;			
+	else
+	{
+		/* Copy the Data to Flash */
+		for(Copied=0;Copied<Count;Copied++)
+		{
+			if (Write2MemByte(&bDestAddr[Copied], bSrcAddr[Copied]) == 0)
+			break;
+		}
+	}
+			
+RetEraseCopyFlash:
+	
+	/* Response Packet */
+	ResPkt = (volatile R2C_EraseCopyFlashResponse *)
+				(NetTxPacket + ETHER_HDR_SIZE + IP_HDR_SIZE);
+
+	/* Fill the Response Packet Header */
+	ResPkt->Header.SeqNo 	= ReqPkt->Header.SeqNo;
+	ResPkt->Header.PktType 	= ReqPkt->Header.PktType;
+	ResPkt->Header.PktLen 	= host_to_le16(	sizeof(R2C_EraseCopyFlashResponse) 
+								- sizeof(R2C_Header));
+	ResPkt->SizeCopied		= host_to_le32(Copied);
+	ResPkt->Header.Crc32= 
+			host_to_le32(Calculate_R2C_Crc32((uchar *)ResPkt,
+									le16_to_host(ResPkt->Header.PktLen)));
+			
+	/* Send the Response Pkt */
+	NetSendUDPPacket(ConsoleEther,ConsoleIP,ConsoleUDPPort,	R2C_DEVICE_PORT,
+					le16_to_host(ResPkt->Header.PktLen)+ sizeof(R2C_Header));
+
+	return 1;
+}	
+#endif
