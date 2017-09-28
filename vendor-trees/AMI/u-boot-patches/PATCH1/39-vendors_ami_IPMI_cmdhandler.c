--- uboot-wpcm/vendors/ami/IPMI/cmdhandler.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot/vendors/ami/IPMI/cmdhandler.c	2008-05-22 19:26:59.000000000 -0400
@@ -0,0 +1,1795 @@
+/*****************************************************************
+ *****************************************************************
+ ***                                                            **
+ ***    (C)Copyright 2002-2003, American Megatrends Inc.        **
+ ***                                                            **
+ ***            All Rights Reserved.                            **
+ ***                                                            **
+ ***        6145-F, Northbelt Parkway, Norcross,                **
+ ***                                                            **
+ ***        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ ***                                                            **
+ *****************************************************************
+ *****************************************************************
+ *****************************************************************
+ * $Header: $
+ *
+ * $Revision: $
+ *
+ * $Date: $
+ *
+ ******************************************************************
+ ******************************************************************
+ * 
+ * cmdhandler.c
+ * IPMI Command Handler Functions
+ *
+ *  Author: Rama Rao Bisa <ramab@ami.com>
+ *			Modified by Anurag Bhatia for SP porting		
+ ******************************************************************/
+
+#include <common.h>
+#include <config.h>
+
+#ifdef CFG_YAFU_SUPPORT
+
+#include <net.h>
+#include "IPMIDefs.h"
+#include "cmdhandler.h"
+#include "fmh.h"
+#include <command.h>
+#include <malloc.h>
+
+#include "flash.h"
+
+/* Global Variables */
+int 						gTFTPServerIP;
+int 						gReset	= 0;
+int 						gStartFirmwareUpdate = 0;
+int 						gStartFirmwareUpdation = 0;
+int 						gfirmwareUpgradeProcessStarted = 0;
+INT32U					gEraseBlock =0,gSizeToErase = 0;
+unsigned char 				gImageFileName [MAX_FILE_NAME_LEN];
+STRUCTURED_FLASH_PROGRESS	gAMIFlashStatus;
+unsigned int 				gSessionSeqNum = 0;
+unsigned int 				gSessionID	 = 0;
+FLASH_PARAMS				gFlashParams;
+
+unsigned long 				Responselen;
+int  FMHComp;
+int ActivateFlashStatus =0X00 ;
+int LastStatCode = 0x00;
+INT32U FlashUpgrade = 0x00;
+ INT8U *allocmem;
+char  		  *argv [4];
+unsigned char *pNewFirmwareImage;
+unsigned char *pNewFirmwareAddr; 
+
+extern int do_tftpb   (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
+extern int do_protect (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
+extern int do_flerase (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);
+extern volatile unsigned long  *cpld_base;
+extern volatile unsigned short	*fwupdateflagreg;
+
+static unsigned char *FlashDataBuffer = (unsigned char*) TEMP_SECTOR_BUFFER;
+
+static int GetConfigSection_FLASH(unsigned long *configStart, unsigned long *configSize);
+
+/*
+ * Validate Module100
+ */
+static
+unsigned char 
+ValidateModule100(unsigned char *Buffer, unsigned long Size)
+{
+	unsigned char Sum=0;
+
+	while (Size--)
+	{
+		Sum+=(*Buffer);
+		Buffer++;
+	}
+
+	return Sum;
+}
+
+/*
+ * Check For Normal FMH
+ */
+static
+FMH *
+CheckForNormalFMH(FMH *fmh)
+{
+
+	
+	if (strncmp(fmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
+			return NULL;
+
+
+	if (le16_to_host(fmh->FMH_End_Signature) != FMH_END_SIGNATURE)
+			return NULL;
+
+
+	if (ValidateModule100((unsigned char *)fmh,sizeof(FMH)) != 0)
+			return NULL;
+
+	
+	return fmh;
+			
+}
+
+/*
+ * Check For Alternate FMH
+ */
+static
+unsigned long 
+CheckForAlternateFMH(ALT_FMH *altfmh)
+{
+
+	if (strncmp(altfmh->FMH_Signature,FMH_SIGNATURE,sizeof(FMH_SIGNATURE)-1) != 0)
+			return INVALID_FMH_OFFSET;
+
+	if (le16_to_host(altfmh->FMH_End_Signature) != FMH_END_SIGNATURE)
+			return INVALID_FMH_OFFSET;
+
+	if (ValidateModule100((unsigned char *)altfmh,sizeof(ALT_FMH)) != 0)
+			return INVALID_FMH_OFFSET;
+	
+	return le32_to_host(altfmh->FMH_Link_Address);
+
+}
+
+/*
+ * Scan for FMH in the RAM
+ */
+static FMH*
+ScanforFMH_RAM(unsigned char *SectorAddr, unsigned long SectorSize)
+{
+	FMH *fmh;
+	ALT_FMH *altfmh;
+	unsigned long FMH_Offset;
+
+
+	/* Check if Normal FMH is found */
+	fmh = (FMH *)SectorAddr;
+	fmh = CheckForNormalFMH(fmh);
+	if (fmh != NULL)
+	{
+		return fmh;
+	}
+
+	/* If Normal FMH is not found, check for alternate FMH */
+	altfmh = (ALT_FMH *)(SectorAddr+SectorSize - sizeof(ALT_FMH));
+	FMH_Offset = CheckForAlternateFMH(altfmh);
+	if (FMH_Offset == INVALID_FMH_OFFSET)
+	{
+		return NULL;
+	}
+	fmh = (FMH *)(SectorAddr +FMH_Offset);
+	
+	/* If alternate FMH is found, validate it */
+	fmh = CheckForNormalFMH(fmh);
+	return fmh;
+}
+
+
+/*---------------------------------------
+ * GetIMCStatus
+ *---------------------------------------*/
+int
+GetIMCStatus (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+
+    GetIMCStatus_T*  pGetIMCStatus = (GetIMCStatus_T*) pRes;
+
+    pGetIMCStatus->CompletionCode   = CC_NORMAL;
+    pGetIMCStatus->StatusByte1      = 0x01;
+    pGetIMCStatus->StatusByte2      = 0x02;
+    pGetIMCStatus->StatusByte3      = 0x03;
+    pGetIMCStatus->StatusByte4      = 0x04;
+
+    return (sizeof (GetIMCStatus_T));
+}
+
+/*---------------------------------------
+ * inet_ltoa
+ *---------------------------------------*/
+char *inet_ltoa(int a)
+{
+    static char ret[16];
+    sprintf(ret, "%d.%d.%d.%d", (ntohl(a) >> 24) & 0xFF,
+	     (ntohl(a) >> 16) & 0xFF, (ntohl(a) >> 8) & 0xFF,
+	     ntohl(a) & 0xFF);
+    return ret;
+}
+
+#if 0 
+/*---------------------------------------
+ * SetTFTPInfo
+ *---------------------------------------*/
+int
+SetTFTPInfo (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+	char *s;		/* pointer to env value */
+    SetTFTPInfo_T*  pSetTFTPInfo = (SetTFTPInfo_T*) pReq;
+
+	printf ("ReqLen = 0x%x IP_ADDR_LEN = 0x%x\n", ReqLen, IP_ADDR_LEN);
+	
+	if (ReqLen <= IP_ADDR_LEN)
+	{
+		*pRes = CC_REQ_INV_LEN;
+		return sizeof (*pRes);
+	}	
+    memcpy (&gTFTPServerIP, (unsigned char*)&pSetTFTPInfo->ServerIPAddr, IP_ADDR_LEN);
+	memset (gImageFileName, 0, MAX_FILE_NAME_LEN);
+	memcpy (gImageFileName, pSetTFTPInfo->ImageFileName, (ReqLen - IP_ADDR_LEN));
+
+	s = inet_ltoa (gTFTPServerIP);
+	printf ("TFTP Server IP address is ...%s\n", s);
+	setenv ("serverip", s);
+	printf ("Image File Name... %s\n", gImageFileName);
+
+
+	*pRes   = CC_NORMAL;
+	
+	/* Turn off flash erase/write Protection */
+	argv[0] = "protect";
+	argv[1] = "off";
+	argv[2] = "all";
+	argv[3] = NULL;
+	if (do_protect (NULL,0,3,argv))
+	{
+		printf ("Protect off all Failed\n");
+		return sizeof (*pRes);
+	}
+
+	/* Erase two Blocks (512K Bytes) */
+	argv[0] = "erase";
+	argv[1] = "0x20000000";
+	argv[2] = "0x2007FFFF";
+	argv[3] = NULL;
+
+	if (0 != do_flerase(NULL, 0, 3, argv))
+	{
+		printf ("Flash Erase Failed\n");
+		return sizeof (*pRes);
+	}
+
+	/* save environment */
+	saveenv ();
+
+	/* load the new Image */
+	argv[0] = "tftpboot";
+	argv[1] = "0x20000000";
+	argv[2] = gImageFileName;
+	argv[3] = NULL;
+	
+	if (0 != do_tftpb (NULL, 0, 3, argv))
+	{
+		printf ("Image Update Failed\n");
+		return sizeof (*pRes);
+	}
+
+    return sizeof (*pRes);
+}
+#endif
+
+/*---------------------------------------
+ * GetChAuthCaps
+ *---------------------------------------*/
+int
+GetChAuthCaps (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+
+    GetChAuthCapReq_T* pGetChAuthCapReq = (GetChAuthCapReq_T*)pReq;
+    GetChAuthCapRes_T* pGetChAuthCapRes = (GetChAuthCapRes_T*)pRes;
+    unsigned char              PrivLevel, ChannelNum;
+
+    PrivLevel   = pGetChAuthCapReq->PrivLevel & 0x0F;
+    ChannelNum  = pGetChAuthCapReq->ChannelNum & 0x0F;
+	if (ChannelNum == 0x0e) ChannelNum = 1;
+
+    pGetChAuthCapRes->CompletionCode    = CC_NORMAL; /* completion code */
+    pGetChAuthCapRes->ChannelNum        = ChannelNum; /* channel No */
+
+    if ((0 == PrivLevel) || (PrivLevel > 5))
+    {
+        pGetChAuthCapRes->CompletionCode = CC_INV_DATA_FIELD;/* Privilege is Reserved */
+        return sizeof (*pRes);
+    }
+    /* Authentication Type Supported for given Privilege */
+    pGetChAuthCapRes->AuthType = 0x01;
+
+
+    pGetChAuthCapRes->PerMsgUserAuthLoginStatus = 0;
+
+	/* Null User Null Password Enabled  */
+	pGetChAuthCapRes->PerMsgUserAuthLoginStatus |= 0x01;
+	/* Null User with Password Disabled */
+	pGetChAuthCapRes->PerMsgUserAuthLoginStatus &= ~0x02;
+
+    /* User level Authentication */
+    pGetChAuthCapRes->PerMsgUserAuthLoginStatus |= 0x08;
+
+    /* PerMessage Authentication */
+    pGetChAuthCapRes->PerMsgUserAuthLoginStatus |= 0x10;
+
+    return sizeof (GetChAuthCapRes_T);
+
+}
+
+
+
+/*---------------------------------------
+ * GetSessionChallenge
+ *---------------------------------------*/
+int
+GetSessionChallenge (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+
+	printf ("Get Session Challenge\n");
+    GetSesChallengeRes_T* pGetSesChalRes = (GetSesChallengeRes_T*)pRes;
+           unsigned char         ChallengeString[16] = { 1,2,3,4,5,6,7,8,9,0,1,2,3,4,5,6 };
+           unsigned int          TempSessId = 0x12345678;
+
+   	pGetSesChalRes->CompletionCode = CC_NORMAL;
+    pGetSesChalRes->TempSessionID  = TempSessId;
+    memcpy(pGetSesChalRes->ChallengeString, ChallengeString, 16);
+
+    return sizeof (GetSesChallengeRes_T);
+}
+
+
+/*---------------------------------------
+ * ActivateSession
+ *---------------------------------------*/
+int
+ActivateSession (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+     ActivateSesReq_T*   pAcvtSesReq = (ActivateSesReq_T*)pReq;
+     ActivateSesRes_T*   pAcvtSesRes = (ActivateSesRes_T*)pRes;
+     unsigned char       AuthType, Privilege;
+
+    AuthType  = pAcvtSesReq->AuthType  & 0x0F;
+    Privilege = pAcvtSesReq->Privilege & 0x0F;
+
+    if ((0 == Privilege) || (Privilege > 5))
+    {
+        pAcvtSesRes->CompletionCode = CC_INV_DATA_FIELD;
+        return sizeof (*pRes);
+    }
+
+    pAcvtSesRes->CompletionCode = CC_NORMAL;
+    pAcvtSesRes->AuthType       = 0x00;
+    pAcvtSesRes->SessionID      = 0x12345678;
+	pAcvtSesRes->InboundSeq		= 0x00;
+    pAcvtSesRes->Privilege      = 0x04;
+
+	gSessionID					= pAcvtSesRes->SessionID;
+	gSessionSeqNum				= pAcvtSesRes->InboundSeq;
+
+    return sizeof (ActivateSesRes_T);
+}
+
+
+/*---------------------------------------
+ * SetSessionPrivLevel
+ *---------------------------------------*/
+int
+SetSessionPrivLevel (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+    SetSesPrivLevelRes_T*   pSetSesPrivLevelRes = (SetSesPrivLevelRes_T*)pRes;
+
+    pSetSesPrivLevelRes->CompletionCode = CC_NORMAL;
+    pSetSesPrivLevelRes->Privilege      = *pReq & 0x0F;;
+
+    return sizeof (SetSesPrivLevelRes_T);
+}
+
+/*---------------------------------------
+ * CloseSession
+ *---------------------------------------*/
+int
+CloseSession (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+
+	CloseSesReq_T*	pCloseSesReq = (CloseSesReq_T*) pReq;
+
+	printf ("Close Session ID Receive is %x\n", pCloseSesReq->SessionID);
+    /* The SessionInfo is deleted form session table  from interface */
+    *pRes = CC_NORMAL;
+	if (gSessionID == pCloseSesReq->SessionID)
+	{
+		gSessionSeqNum = 0;
+		gSessionID	 = 0;
+	}
+	else
+	{
+		*pRes = CC_CLOSE_INVALID_SESSION_ID;
+	}	
+
+    return sizeof (*pRes);
+}
+extern flash_info_t flash_info[];
+unsigned long gFlashconfigStart, gFlashconfigSize;
+unsigned long gImageconfigStart, gImageconfigSize;
+/*---------------------------------------
+ * GetDevID
+ *---------------------------------------*/
+int
+GetDevID (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+	flash_info_t *pFlashInfo;
+ 	pFlashInfo = &flash_info[0];
+	FMH *FMHPtr = NULL;
+	MODULE_INFO *ModulePtr = NULL;
+    GetDevIDRes_T*  pGetDevIDRes = (GetDevIDRes_T*) pRes;
+    unsigned int Major,Minor;
+	unsigned char MfgID [3] = MFG_ID_AMI;
+
+	FMHPtr = ScanforFMH((unsigned char*)pFlashInfo->start[(CFG_PROJ_USED_FLASH_SIZE/CFG_PROJ_ERASE_BLOCK_SIZE)-1], (unsigned long)CFG_PROJ_ERASE_BLOCK_SIZE);
+	if (FMHPtr == NULL)
+	{
+		printf ("FMH not found\n");
+	}
+	ModulePtr = &(FMHPtr->Module_Info);
+	Major = ModulePtr->Module_Ver_Major;
+	Minor = ModulePtr->Module_Ver_Minor;
+
+    pGetDevIDRes->CompletionCode      = CC_NORMAL;
+    pGetDevIDRes->DeviceID            = DEVICE_ID;
+    pGetDevIDRes->DevRevision         = IPMI_DEV_REVISION;
+    pGetDevIDRes->FirmwareRevision1   = Major;
+    pGetDevIDRes->FirmwareRevision2   = Minor;
+    pGetDevIDRes->IPMIVersion         = IPMI_VERSION;
+    pGetDevIDRes->DevSupport          = DEV_SUPPORT;
+    pGetDevIDRes->ProdID              = htoipmi_u16 (0xaabb);
+    pGetDevIDRes->AuxFirmwareRevision = 0x00;
+    memcpy (pGetDevIDRes->MfgID, MfgID, sizeof (MfgID));
+
+    return (sizeof (GetDevIDRes_T));
+}
+
+
+static 
+int GetConfigSection_FLASH(unsigned long *configStart, unsigned long *configSize)
+{
+    unsigned long offs = 0;
+    FMH *FMHPtr = NULL;
+    MODULE_INFO *ModulePtr = NULL;
+	unsigned char*	TempBuf = (unsigned char*) TEMP_SECTOR_BUFFER;
+	int i;
+	flash_info_t *pFlashInfo;
+ 	pFlashInfo = &flash_info[0];
+
+
+    for (i = 0; i < (CFG_PROJ_USED_FLASH_SIZE/CFG_PROJ_ERASE_BLOCK_SIZE); i++)
+    {
+		if (0 != flash_read (pFlashInfo->start[i], CFG_PROJ_ERASE_BLOCK_SIZE, TempBuf))
+		{
+			printf ("flash read failed\n");
+			return -1;
+		}
+
+		/* Look for the FMH of the config section */
+		FMHPtr = ScanforFMH_RAM(TempBuf, CFG_PROJ_ERASE_BLOCK_SIZE);
+		if (FMHPtr != NULL)
+		{
+			/* FMH module found!!    */
+			ModulePtr = &(FMHPtr->Module_Info);
+			ModulePtr->Module_Name[8] = 0;
+			if(strcmp((char*)ModulePtr->Module_Name,(char*)CONFIG_SECTION_NAME) == 0)
+			{
+				*configStart = offs;
+				*configSize = FMHPtr->FMH_AllocatedSize;
+				return 0;
+			}
+		}
+
+		offs += CFG_PROJ_ERASE_BLOCK_SIZE;
+    }
+
+    return -1;
+}
+
+static 
+int GetConfigSection_RAM(unsigned long *configStart, unsigned long *configSize)
+{
+    unsigned long offs = 0;
+    FMH *FMHPtr = NULL;
+    MODULE_INFO *ModulePtr = NULL;
+    char name[10];
+	unsigned char*	TempBuf = (unsigned char*) TEMP_SECTOR_BUFFER;
+	int i;
+
+    for (i = 0; i < (CFG_PROJ_USED_FLASH_SIZE/CFG_PROJ_ERASE_BLOCK_SIZE); i++)
+    {
+		memcpy (TempBuf, (unsigned char*)(IMAGE_UPLOAD_LOCATION + (i * CFG_PROJ_ERASE_BLOCK_SIZE)), CFG_PROJ_ERASE_BLOCK_SIZE);
+		/* Look for the FMH of the config section */
+		FMHPtr = ScanforFMH_RAM(TempBuf, CFG_PROJ_ERASE_BLOCK_SIZE);
+		if (FMHPtr != NULL)
+		{
+			/* FMH module found!!    */
+			ModulePtr = &(FMHPtr->Module_Info);
+			memcpy(name,ModulePtr->Module_Name,8);
+			ModulePtr->Module_Name[8] = 0;
+			if(strcmp((char*)ModulePtr->Module_Name,(char*)CONFIG_SECTION_NAME) == 0)
+			{
+				*configStart = offs;
+				*configSize = FMHPtr->FMH_AllocatedSize;
+				break;
+			}
+		}
+		offs += CFG_PROJ_ERASE_BLOCK_SIZE;
+
+    }
+    return 0;
+}
+
+
+/*---------------------------------------
+ * AmiInitFlash
+ *---------------------------------------*/
+int
+AmiInitFlash (unsigned char* pReq, unsigned char ReqLen, unsigned char* pRes)
+{
+	AMIInitFlashReq_T*	pAMIInitFlashReq = (AMIInitFlashReq_T*) pReq;
+	AMIInitFlashRes_T*	pAMIInitFlashRes = (AMIInitFlashRes_T*) pRes;
+	FMH 				*FMHPtr = NULL;
+	MODULE_INFO 		*ModulePtr = NULL;
+	flash_info_t 		*pFlashInfo;
+
+ 	pFlashInfo = &flash_info[0];
+ 
+//	printf("AmiInitFlash\n");
+	if (gfirmwareUpgradeProcessStarted)
+	{
+		printf("gfirmwareUpgradeProcessStarted\n");
+		pAMIInitFlashRes->CompletionCode = CC_NODE_BUSY;
+		return sizeof (*pRes);
+	}
+	gfirmwareUpgradeProcessStarted = 1;
+
+	FMHPtr = ScanforFMH((unsigned char*)pFlashInfo->start[(CFG_PROJ_USED_FLASH_SIZE/CFG_PROJ_ERASE_BLOCK_SIZE)-1], (unsigned long)CFG_PROJ_ERASE_BLOCK_SIZE);
+	if (FMHPtr == NULL)
+	{
+		printf ("Error: in AMI Init Flash FMH not found\n");
+		//pAMIInitFlashRes->CompletionCode = CC_NODE_BUSY;
+		//return sizeof (*pRes);
+	}
+
+	ModulePtr = &(FMHPtr->Module_Info);
+	memset (pAMIInitFlashRes, 0, sizeof (AMIInitFlashRes_T));
+	pNewFirmwareImage = (unsigned char*)IMAGE_UPLOAD_LOCATION;
+
+    pAMIInitFlashRes->CompletionCode 			= CC_NORMAL;
+	pAMIInitFlashRes->Params.FlashStartAddr 	= htoipmi_u32(CFG_PROJ_FLASH_START);
+	pAMIInitFlashRes->Params.FlashSize		 	= htoipmi_u32(CFG_PROJ_FLASH_SIZE);
+	pAMIInitFlashRes->Params.UsedFlashStartAddr = htoipmi_u32(CFG_PROJ_USED_FLASH_START);
+	pAMIInitFlashRes->Params.UsedFlashSize		= htoipmi_u32(CFG_PROJ_USED_FLASH_SIZE);
+	pAMIInitFlashRes->Params.EnvStartAddr		= htoipmi_u32(CFG_PROJ_UBOOT_ENV_START);
+	pAMIInitFlashRes->Params.EnvSize			= htoipmi_u32(CFG_PROJ_UBOOT_ENV_SIZE);
+	pAMIInitFlashRes->Params.EraseBlockSize		= htoipmi_u32(CFG_PROJ_ERASE_BLOCK_SIZE);
+	pAMIInitFlashRes->Params.CurrentImageMajor 	= ModulePtr->Module_Ver_Major;
+	pAMIInitFlashRes->Params.CurrentImageMinor 	= ModulePtr->Module_Ver_Minor;
+	pAMIInitFlashRes->Params.RetainConfig = pAMIInitFlashReq->Params.RetainConfig;
+	pAMIInitFlashRes->Params.UpdateUboot  = pAMIInitFlashReq->Params.UpdateUboot;
+
+	memcpy (&gFlashParams, &pAMIInitFlashRes->Params, sizeof (FLASH_PARAMS));
+
+	if(GetConfigSection_FLASH(&gFlashconfigStart, &gFlashconfigSize) != 0)
+	{
+		printf("Error: Could not get config section\n");
+		gFlashconfigStart = 0;
+		gFlashconfigSize = 0;
+	}
+
+    return sizeof (AMIInitFlashRes_T);
+}
+
+/*---------------------------------------
+ * AMIExitFlash
+ *---------------------------------------*/
+int
+AMIExitUploadImage (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes)
+{
+	FMH 				*FMHPtr    = NULL;
+	MODULE_INFO 		*ModulePtr = NULL;
+	unsigned long 		ImgChksum;
+	unsigned long 		i;
+	unsigned char		*TmpBuff = (unsigned char*)IMAGE_UPLOAD_LOCATION;
+	unsigned long 		CRC32Val;
+//	AMIExitFlashReq_T	*pAMIExitFlashReq = (AMIExitFlashReq_T*)pReq;
+    AMIExitFlashRes_T 	*pAMIExitFlashRes = (AMIExitFlashRes_T*)pRes;
+
+	//memcpy ((unsigned char*)&pAMIExitFlashRes->Params, (unsigned char*)&pAMIExitFlashReq->Params, sizeof (FLASH_PARAMS));
+	pAMIExitFlashRes->CompletionCode = CC_NORMAL;
+	FMHPtr = ScanforFMH_RAM ((unsigned char*)(IMAGE_UPLOAD_LOCATION + CFG_PROJ_USED_FLASH_SIZE - CFG_PROJ_ERASE_BLOCK_SIZE), 	
+											(unsigned long)CFG_PROJ_ERASE_BLOCK_SIZE);
+	if (NULL == FMHPtr)
+	{
+		printf ("Error: In AMI Exit Upload Image command FMH Not found in the uploaded image\n");
+		pAMIExitFlashRes->CompletionCode = CC_NODE_BUSY;
+		return (sizeof (AMIExitFlashRes_T));
+	}
+	
+	ModulePtr = &(FMHPtr->Module_Info);
+	ImgChksum = ModulePtr->Module_Checksum;
+
+	BeginCRC32 (&CRC32Val);
+	
+	for (i = 0; i < CFG_PROJ_USED_FLASH_SIZE; i++)
+	{
+		if((i >= FMH_MODULE_CHECKSUM_START_OFFSET && i <= FMH_MODULE_CHCKSUM_END_OFFSET) || (i == FMH_FMH_HEADER_CHECKSUM_OFFSET))
+		{
+			TmpBuff++;
+			continue;
+		}
+
+		DoCRC32 (&CRC32Val, *TmpBuff);
+		TmpBuff++;
+	}
+	
+	EndCRC32 (&CRC32Val);
+
+	GetConfigSection_RAM(&gImageconfigStart, &gImageconfigSize);
+	//printf ("gImageconfigStart = 0x%lx\n", gImageconfigStart);
+	//printf ("gImageconfigSize  = 0x%lx\n", gImageconfigSize);
+
+	pAMIExitFlashRes->CompletionCode = CC_NORMAL;
+	if (CRC32Val != ImgChksum)
+	{
+		printf("Image Checksum mismtach\n");
+		pAMIExitFlashRes->CompletionCode = 0xFF;
+	}
+
+    return (sizeof (AMIExitFlashRes_T));
+}
+
+/*---------------------------------------
+ * AMIResetCard
+ *---------------------------------------*/
+int
+AMIResetCard(unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )
+{
+    AMIResetCardRes_T *pAMIResetCardRes = (AMIResetCardRes_T *)pRes;
+
+	pAMIResetCardRes->CompletionCode = CC_NORMAL;
+	gReset = 1;
+
+    return (sizeof (AMIResetCardRes_T));
+}
+
+/*---------------------------------------
+ * AMIStartFirmwareUpdate
+ *---------------------------------------*/
+int
+AMIStartFirmwareUpdate(unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )
+{
+    AMIStartFirmwareUpdateReq_T *pAMIStartFirmwareUpdateReq = (AMIStartFirmwareUpdateReq_T *)pReq;
+    AMIStartFirmwareUpdateRes_T *pAMIStartFirmwareUpdateRes = (AMIStartFirmwareUpdateRes_T *)pRes;
+
+	gFlashParams.UpdateUboot = pAMIStartFirmwareUpdateReq->Params.UpdateUboot;
+	gFlashParams.RetainConfig = pAMIStartFirmwareUpdateReq->Params.RetainConfig;
+
+	/* If we did not find config section on the flash */
+	if(gFlashconfigSize == 0)
+		gFlashParams.RetainConfig = 0;
+
+	if ((gImageconfigStart != gFlashconfigStart) || (gImageconfigSize != gFlashconfigSize))
+	{
+		gFlashParams.RetainConfig = 0;
+		printf ("NOTE: config start or config size not same in the flash & image. So overriding the preserve config option, if given, with no preserve config option");
+	}
+
+//	printf("AMIStartFirmwareUpdate: RetainConfig = %d,UpdateUboot = %d\n",gFlashParams.RetainConfig,gFlashParams.UpdateUboot);
+
+	pAMIStartFirmwareUpdateRes->CompletionCode = CC_NORMAL;
+	if ((1 == gStartFirmwareUpdate) || (0 == gfirmwareUpgradeProcessStarted))
+	{
+		pAMIStartFirmwareUpdateRes->CompletionCode = CC_NODE_BUSY;
+	}
+	else
+	{
+		gStartFirmwareUpdate = 1;
+	}
+
+    return (sizeof (AMIStartFirmwareUpdateRes_T));
+}
+
+/*---------------------------------------
+ * AMIEnterFirmwareUpdateMode
+ *---------------------------------------*/
+int
+AMIEnterFirmwareUpdateMode(unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )
+{
+	unsigned short  				old_data, new_data;
+	AMIEnterFirmwareUpdateModeRes_T *pAMIEnterFirmwareUpdateModeRes = (AMIEnterFirmwareUpdateModeRes_T *)pRes;
+
+	pAMIEnterFirmwareUpdateModeRes->CompletionCode = CC_NORMAL;
+
+	cpld_base [0] = CPLD_CONFIG_REG_INIT_VALUE;
+	cpld_base [1] = CPLD_TIMING_REG_INIT_VALUE;
+
+	old_data = fwupdateflagreg [0];
+	old_data |= 0x02;
+	new_data = ((old_data << 8) | (old_data & 0xFF));
+	fwupdateflagreg [0] = new_data;
+
+	/* verift CPLD firmware update bit */
+	new_data = 0;
+	new_data = fwupdateflagreg [0];
+	if (0 == (new_data & 0x02))
+	{
+		printf ("Enter Firmware Update Mode Failed\n");
+		pAMIEnterFirmwareUpdateModeRes->CompletionCode = CC_NODE_BUSY;
+		return (sizeof (AMIEnterFirmwareUpdateModeRes_T));
+	}
+	
+	gReset = 1;
+    return (sizeof (AMIEnterFirmwareUpdateModeRes_T));
+}
+
+
+/*---------------------------------------
+ * AMIGetStatus
+ *---------------------------------------*/
+int
+AMIGetStatus(unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )
+{
+    AMIGetStatusRes_T *pAMIGetStatusRes = (AMIGetStatusRes_T *)pRes;
+
+	pAMIGetStatusRes->CompletionCode = CC_NORMAL;
+	memcpy (&pAMIGetStatusRes->FlashProgess, &gAMIFlashStatus,sizeof (STRUCTURED_FLASH_PROGRESS));
+
+    return (sizeof(AMIGetStatusRes_T));
+}
+
+/*---------------------------------------
+ * ActualEraseBlockSize
+ *---------------------------------------*/
+static int 
+ActualEraseBlockSize (void)
+{
+	return (CFG_PROJ_ERASE_BLOCK_SIZE);
+}
+
+/*---------------------------------------
+ * CalculateChksum
+ *---------------------------------------*/
+unsigned long
+CalculateChksum (char *data, unsigned long size)
+{
+    unsigned long crc32val = 0;
+    unsigned int i = 0;
+
+    BeginCRC32 (&crc32val);
+
+    /*  Begin calculating CRC32 */
+	for(i = 0;i < size;i++)
+    {
+		DoCRC32 (&crc32val, data[i]);
+    }
+
+    EndCRC32 (&crc32val);
+
+	return crc32val;
+}
+
+/*---------------------------------------
+ * UpgradeBlock
+ *---------------------------------------*/
+static int
+UpgradeBlock(unsigned char* FlashDataBuf, unsigned long FlashBlkAdd)
+{
+	printf ("#");
+	memcpy ((pNewFirmwareImage + FlashBlkAdd), FlashDataBuf, ActualEraseBlockSize());
+	return 0;
+}
+
+/*---------------------------------------
+ * AMIUpgradeBlock
+ *---------------------------------------*/
+int AMIUpgradeBlock(unsigned char* pReq, unsigned char ReqLen, unsigned char *pRes )
+{
+    int retval = 0;
+    AMIUpgradeBlockReq_T *pAMIUpgradeBlockReq = (AMIUpgradeBlockReq_T *)pReq;
+    AMIUpgradeBlockRes_T *pAMIUpgradeBlockRes = (AMIUpgradeBlockRes_T *)pRes;
+    static ulong PrevBlkAdd = NO_ADDRESS;
+	static ulong FlashBlkAdd = NO_ADDRESS;
+
+    static unsigned char *PrevBuffAdd = (unsigned char *)TEMP_SECTOR_BUFFER;
+    unsigned long tmp;
+
+	if (0 == gfirmwareUpgradeProcessStarted)
+	{
+		pAMIUpgradeBlockRes->CompletionCode = CC_NODE_BUSY;
+		return sizeof (*pRes);
+	}
+
+    tmp = ipmitoh_u32(pAMIUpgradeBlockReq->blkInfo.BlockStartAdd);
+    pAMIUpgradeBlockReq->blkInfo.BlockStartAdd = tmp;
+
+    tmp = ipmitoh_u32(pAMIUpgradeBlockReq->blkInfo.CRC32);
+    pAMIUpgradeBlockReq->blkInfo.CRC32 = tmp;
+
+    if(pAMIUpgradeBlockReq->blkInfo.EndOfData)
+    {
+	   	/* If this data is in continuation with previously received data */
+		if(pAMIUpgradeBlockReq->blkInfo.BlockStartAdd == (PrevBlkAdd + ActualEraseBlockSize()/TOTAL_PACKETS_XMIT_IN_A_BLOCK))
+		{	/* Complete 256K received */
+			/* Append this data to FlashDataBuffer and then upgrade */
+			if(pAMIUpgradeBlockReq->blkInfo.CRC32 !=
+					CalculateChksum(pAMIUpgradeBlockReq->blkInfo.BlockData,ActualEraseBlockSize()/TOTAL_PACKETS_XMIT_IN_A_BLOCK))
+			{
+				printf("Corrupted data received\n");
+				pAMIUpgradeBlockRes->CompletionCode = OEMCC_CORRUPT_DATA_CHKSUM;
+			}
+			else
+			{
+				memcpy(PrevBuffAdd,&(pAMIUpgradeBlockReq->blkInfo.BlockData),ActualEraseBlockSize()/TOTAL_PACKETS_XMIT_IN_A_BLOCK);
+
+				retval = UpgradeBlock(FlashDataBuffer,FlashBlkAdd);
+				if( retval != 0 )
+				{
+					printf("IPMI:AMIUpgradeBlock() failed.\n");
+					pAMIUpgradeBlockRes->CompletionCode = (unsigned char)(OEMCC_FLASH_UPGRADE_FAILURE);
+				}
+				else
+				{
+					pAMIUpgradeBlockRes->CompletionCode = CC_NORMAL;
+				}
+				PrevBlkAdd = NO_ADDRESS;
+				FlashBlkAdd = NO_ADDRESS;
+				PrevBuffAdd = FlashDataBuffer;
+			}
+		}
+		else
+		{
+			printf("IPMI:Corrupted Data received.\n");
+			printf("PrevBlkAdd = 0x%08lX,  CurrentBlkAdd = 0x%08lX\n",PrevBlkAdd,pAMIUpgradeBlockReq->blkInfo.BlockStartAdd);
+			pAMIUpgradeBlockRes->CompletionCode = (unsigned char)(OEMCC_CORRUPT_FLASH_DATA);
+			PrevBlkAdd = NO_ADDRESS;
+			FlashBlkAdd = NO_ADDRESS;
+			PrevBuffAdd = FlashDataBuffer;
+		}
+   }
+    else
+    {	/* First part of 256K received */
+		/* Store it in the buffer */
+		if((pAMIUpgradeBlockReq->blkInfo.BlockStartAdd % ActualEraseBlockSize()) == 0)
+		{
+			FlashBlkAdd = pAMIUpgradeBlockReq->blkInfo.BlockStartAdd;
+		}
+		if(pAMIUpgradeBlockReq->blkInfo.CRC32 != CalculateChksum(pAMIUpgradeBlockReq->blkInfo.BlockData,ActualEraseBlockSize()/TOTAL_PACKETS_XMIT_IN_A_BLOCK))
+		{
+			printf("Corrupted data received. Recieved Chksum = 0x%0X\n",pAMIUpgradeBlockReq->blkInfo.CRC32);
+			pAMIUpgradeBlockRes->CompletionCode = OEMCC_CORRUPT_DATA_CHKSUM;
+		}
+		else
+		{
+			memcpy(PrevBuffAdd,&(pAMIUpgradeBlockReq->blkInfo.BlockData),ActualEraseBlockSize()/TOTAL_PACKETS_XMIT_IN_A_BLOCK);
+			PrevBuffAdd += ActualEraseBlockSize()/TOTAL_PACKETS_XMIT_IN_A_BLOCK;
+			PrevBlkAdd = pAMIUpgradeBlockReq->blkInfo.BlockStartAdd;
+			pAMIUpgradeBlockRes->CompletionCode = CC_NORMAL;
+		}
+	}
+	return( sizeof( *pAMIUpgradeBlockRes ) );
+}
+
+/*---------------------------------------
+ * AMIYAFUGetFlashInfo
+ *---------------------------------------*/
+int AMIYAFUGetFlashInfo ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+
+
+       AMIYAFUGetFlashInfoReq_T *pAMIYAFUFlashInfoReq = (AMIYAFUGetFlashInfoReq_T *)pReq;
+       AMIYAFUGetFlashInfoRes_T* pAMIYAFUGetFlashInfo = (AMIYAFUGetFlashInfoRes_T*)pRes;  
+
+     	
+	pAMIYAFUGetFlashInfo->CompletionCode = YAFU_CC_NORMAL;
+       pAMIYAFUGetFlashInfo->FlashInfoRes.Seqnum = pAMIYAFUFlashInfoReq->FlashInfoReq.Seqnum;
+	pAMIYAFUGetFlashInfo->FlashInfoRes.YafuCmd= pAMIYAFUFlashInfoReq->FlashInfoReq.YafuCmd;
+	
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashSize = CFG_PROJ_USED_FLASH_SIZE;
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashAddress = CFG_PROJ_USED_FLASH_START;
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashEraseBlkSize = CFG_PROJ_ERASE_BLOCK_SIZE;
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashProductID = 0; 
+	pAMIYAFUGetFlashInfo->FlashInfo.FlashWidth = 8;  
+	pAMIYAFUGetFlashInfo->FlashInfo.FMHCompliance = 0x01;
+
+       if(pAMIYAFUGetFlashInfo->FlashInfo.FMHCompliance == 0x01)
+	   	FMHComp = 1;
+	
+      	pAMIYAFUGetFlashInfo->FlashInfo.Reserved = 0;
+	pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks = CFG_PROJ_USED_FLASH_SIZE/CFG_PROJ_ERASE_BLOCK_SIZE; 
+
+       if((pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks * pAMIYAFUGetFlashInfo->FlashInfo.FlashEraseBlkSize) == pAMIYAFUGetFlashInfo->FlashInfo.FlashSize)
+             pAMIYAFUGetFlashInfo->FlashInfoRes.Datalen= 0x20; 
+       else	   
+             pAMIYAFUGetFlashInfo->FlashInfoRes.Datalen = 0x20 + pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks;
+
+      pAMIYAFUGetFlashInfo->FlashInfoRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetFlashInfo->FlashInfo,sizeof(pAMIYAFUGetFlashInfo->FlashInfo));				 	   
+	
+       return( sizeof( AMIYAFUGetFlashInfoRes_T ) );
+
+}
+
+
+/*---------------------------------------
+ * AMIYAFUGetFMHInfo
+ *---------------------------------------*/
+ int AMIYAFUGetFMHInfo ( unsigned char* pReq, unsigned char ReqLen, unsigned char *pRes ) 
+{
+      DWORD i=0,m=0;
+      INT8U *Buf;
+      FlashMH      FlashModHeader;
+      ALT_FMHead    AltFmh;  
+      char *FMHDetails;	  
+    
+
+      AMIYAFUGetFMHInfoReq_T *pAMIYAFUGetFMHInfoReq = (AMIYAFUGetFMHInfoReq_T *)pReq;
+      AMIYAFUGetFMHInfoRes_T* pAMIYAFUGetFMHInfo = (AMIYAFUGetFMHInfoRes_T*)pRes;
+
+  	pAMIYAFUGetFMHInfo->NumFMH = 0x00;
+
+      FMHDetails = malloc(1200);
+      if(FMHDetails == NULL)
+      {
+             printf("Error in malloc of FMHDetails");
+	      return -1;		 
+      }
+
+
+   for(i=0;i<(CFG_PROJ_USED_FLASH_SIZE/CFG_PROJ_ERASE_BLOCK_SIZE);i++) 			
+   {
+	Buf= (INT8U *)malloc(64);
+
+	if(Buf == NULL)
+	{
+	      printf("Error in alloc\n");
+	      AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+	      pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+	      pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	      pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	      pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+	      pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_ALLOC_ERR;
+	      LastStatCode = YAFU_CC_ALLOC_ERR;
+
+             return (sizeof(AMIYAFUNotAck));
+   	}
+
+
+       if(flash_read( CFG_PROJ_FLASH_START+(i*CFG_PROJ_ERASE_BLOCK_SIZE),64,Buf) != 0)
+	{
+
+		AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+		pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+		pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+		pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+		pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+		pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_READ_ERR;
+			 
+		return (sizeof(AMIYAFUNotAck));
+	}
+    
+       
+      memcpy((char*)&FlashModHeader,Buf,64);
+
+      if(strncmp(FlashModHeader.FmhSignature,"$MODULE$",(sizeof(FlashModHeader.FmhSignature) -1)) == 0 )	//Check FMH Signature
+      {
+
+            if(FlashModHeader.EndSignature != 21930)
+	      	  	continue;
+
+	     memcpy((FMHDetails + m),(char *)&FlashModHeader,64);
+           
+       
+              printf("Module Name = %s\n",FlashModHeader.ModuleName);
+
+	     m +=64;
+	     pAMIYAFUGetFMHInfo->NumFMH += 0x1;
+
+	     free(Buf);
+
+	}
+      else
+      {
+
+         if(flash_read(CFG_PROJ_FLASH_START+((i*CFG_PROJ_ERASE_BLOCK_SIZE) +(CFG_PROJ_ERASE_BLOCK_SIZE -sizeof(AltFmh))),sizeof(AltFmh),Buf) != 0)
+	  {
+
+		AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+		pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+		pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+		pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+		pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+		pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_READ_ERR;
+		 
+		return (sizeof(AMIYAFUNotAck));
+          }	
+
+	  
+	      memcpy((char *)&AltFmh,Buf,sizeof(AltFmh));
+             if(strncmp(AltFmh.Signature,"$MODULE$",(sizeof(AltFmh.Signature) -1)) == 0 )	//Check FMH Signature
+             	{
+			 
+	         	if(flash_read(CFG_PROJ_FLASH_START +(i*CFG_PROJ_ERASE_BLOCK_SIZE) +AltFmh.LinkAddress,64,Buf) != 0)
+	  		{
+
+				AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+				pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+				pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+				pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+				pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+				pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_READ_ERR;
+					 
+				return (sizeof(AMIYAFUNotAck));
+         		 }
+
+					  
+	      		memcpy((char*)&FlashModHeader,Buf,64);	
+
+	      		if(strncmp(FlashModHeader.FmhSignature,"$MODULE$",(sizeof(FlashModHeader.FmhSignature) -1)) == 0 )	//Check FMH Signature
+            		{
+
+	      			if(FlashModHeader.EndSignature != 21930)
+	      	  			continue;
+
+                            memcpy((FMHDetails +m),(char *)&FlashModHeader,64);
+           
+              		printf("Module Name = %s\n",FlashModHeader.ModuleName);
+	
+	     			m +=64;
+	     			pAMIYAFUGetFMHInfo->NumFMH += 0x1;
+
+	     			
+
+	   		}	
+          		
+             	}	
+	  free(Buf);
+      	}
+    }
+
+
+      if(pAMIYAFUGetFMHInfoReq->FMHReq.Datalen == 0x00)
+		pAMIYAFUGetFMHInfo->FMHRes.Datalen= 0x04 + (pAMIYAFUGetFMHInfo->NumFMH * 64);
+	else
+	{
+		pAMIYAFUGetFMHInfo->CompletionCode = CC_INV_DATA_FIELD;
+		return sizeof (*pRes);
+	}
+
+      memcpy (( INT8U*) (pAMIYAFUGetFMHInfo + 1),
+                   ( INT8U*)FMHDetails,(pAMIYAFUGetFMHInfo->NumFMH * 64) );	
+
+      pAMIYAFUGetFMHInfo->CompletionCode = YAFU_CC_NORMAL;
+      pAMIYAFUGetFMHInfo->FMHRes.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+      pAMIYAFUGetFMHInfo->FMHRes.YafuCmd= pAMIYAFUGetFMHInfoReq->FMHReq.YafuCmd;
+      pAMIYAFUGetFMHInfo->Reserved = 0x00; 	  
+      pAMIYAFUGetFMHInfo->FMHRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetFMHInfo->Reserved,pAMIYAFUGetFMHInfo->FMHRes.Datalen);
+     
+      return( sizeof( AMIYAFUGetFMHInfoRes_T ) + pAMIYAFUGetFMHInfo->NumFMH * 64 );
+}
+ 
+/*---------------------------------------
+ * AMIYAFUGetStatus
+ *---------------------------------------*/
+
+int AMIYAFUGetStatus  (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+    char *s;
+    int Mode = 0;
+
+    AMIYAFUGetStatusReq_T *pAMIYAFUGetStatusReq =(AMIYAFUGetStatusReq_T *)pReq;
+    AMIYAFUGetStatusRes_T* pAMIYAFUGetStatus = (AMIYAFUGetStatusRes_T*)pRes;
+
+    pAMIYAFUGetStatus->CompletionCode = YAFU_CC_NORMAL;
+    pAMIYAFUGetStatus->GetStatusRes.Seqnum = pAMIYAFUGetStatusReq->GetStatusReq.Seqnum;
+    pAMIYAFUGetStatus->GetStatusRes.YafuCmd= pAMIYAFUGetStatusReq->GetStatusReq.YafuCmd;
+    pAMIYAFUGetStatus->LastStatusCode =(INT16U) LastStatCode;
+    pAMIYAFUGetStatus->YAFUState = 0x00;
+
+
+    s = getenv("mode");
+    if (s != NULL)
+    {
+		Mode = (int)simple_strtoul(s,NULL,0);
+		printf("Mode is %d\n",Mode);
+    }
+
+
+    pAMIYAFUGetStatus->Mode = Mode;
+    pAMIYAFUGetStatus->Reserved = 0x00;
+    pAMIYAFUGetStatus->GetStatusRes.Datalen= 8;
+    pAMIYAFUGetStatus->Message[0] = 0;
+    pAMIYAFUGetStatus->GetStatusRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetStatus->LastStatusCode,(INT32U)pAMIYAFUGetStatus->GetStatusRes.Datalen);
+
+     return ( sizeof( AMIYAFUGetStatusRes_T ) );
+}
+
+/*---------------------------------------
+ * AMIYAFUActivateFlashMode
+ *---------------------------------------*/
+int AMIYAFUActivateFlashMode ( unsigned char *pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+    AMIYAFUActivateFlashModeReq_T *pAMIYAFUActivateFlashReq = (AMIYAFUActivateFlashModeReq_T *)pReq;
+    AMIYAFUActivateFlashModeRes_T* pAMIYAFUActivateFlash = (AMIYAFUActivateFlashModeRes_T*)pRes;
+
+   if(CalculateChksum((char *)&pAMIYAFUActivateFlashReq->Mode,sizeof(INT16U)) != pAMIYAFUActivateFlashReq->ActivateflashReq.CRC32chksum)
+  {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUActivateFlashReq->ActivateflashReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+  }
+
+	if(pAMIYAFUActivateFlashReq->ActivateflashReq.Datalen == 0x02)
+		pAMIYAFUActivateFlash->ActivateflashRes.Datalen= 0x02;
+	else
+	{
+	     AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+            pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUActivateFlashReq->ActivateflashReq.Seqnum;
+	     pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	     pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	     pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+            pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+            LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+            return (sizeof(AMIYAFUNotAck));
+	}
+
+    ActivateFlashStatus = 0x01;
+    pNewFirmwareAddr = (unsigned char*)IMAGE_UPLOAD_LOCATION;
+
+    pAMIYAFUActivateFlash->CompletionCode = YAFU_CC_NORMAL;
+    pAMIYAFUActivateFlash->ActivateflashRes.Seqnum = pAMIYAFUActivateFlashReq->ActivateflashReq.Seqnum;
+    pAMIYAFUActivateFlash->ActivateflashRes.YafuCmd= pAMIYAFUActivateFlashReq->ActivateflashReq.YafuCmd;
+
+    pAMIYAFUActivateFlash->ActivateflashRes.CRC32chksum = 0x00;
+    pAMIYAFUActivateFlash->Delay = 0x00;
+
+    return ( sizeof( AMIYAFUActivateFlashModeRes_T ) );
+}
+
+/*---------------------------------------
+ * AMIYAFUProtectFlash
+ *---------------------------------------*/
+int AMIYAFUProtectFlash ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes ) 
+{
+
+    AMIYAFUProtectFlashReq_T *pAMIYAFUProtectFlashReq = (AMIYAFUProtectFlashReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+   AMIYAFUProtectFlashRes_T* pAMIYAFUProtectFlash = (AMIYAFUProtectFlashRes_T*)pRes;
+
+   if(CalculateChksum((char *)&pAMIYAFUProtectFlashReq->Blknum,pAMIYAFUProtectFlashReq->ProtectFlashReq.Datalen) != pAMIYAFUProtectFlashReq->ProtectFlashReq.CRC32chksum)
+   {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+   }
+
+    if(pAMIYAFUProtectFlashReq->ProtectFlashReq.Datalen== 0x05)
+          pAMIYAFUProtectFlash->ProtectFlashRes.Datalen= 0x01;
+    else
+    {
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+	   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+	   return (sizeof(AMIYAFUNotAck));
+   }
+
+    argv[0] = "protect";
+    argv[1] = "off";
+     argv[2] = "all";
+     argv[3] = NULL;
+
+     if (0 != do_protect (NULL,0,3,argv))
+     {
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+	   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_PROTECT_ERR;
+	   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+	   return (sizeof(AMIYAFUNotAck));
+
+     }
+
+
+    pAMIYAFUProtectFlash->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = pAMIYAFUProtectFlash->CompletionCode;
+    pAMIYAFUProtectFlash->ProtectFlashRes.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+    pAMIYAFUProtectFlash->ProtectFlashRes.YafuCmd= pAMIYAFUProtectFlashReq->ProtectFlashReq.YafuCmd;
+    pAMIYAFUProtectFlash->ProtectFlashRes.CRC32chksum =  CalculateChksum(&(pAMIYAFUProtectFlash->Status),sizeof(INT8U));
+
+
+   return (sizeof(AMIYAFUProtectFlashRes_T));
+}
+else
+{
+     AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+     pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUProtectFlashReq->ProtectFlashReq.Seqnum;
+     pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+     pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+     pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+     pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+     LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+     return (sizeof(AMIYAFUNotAck));
+}
+
+}
+
+/*---------------------------------------
+ * AMIYAFUEraseCopyFlash
+ *---------------------------------------*/
+int AMIYAFUEraseCopyFlash ( unsigned char *pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+
+    AMIYAFUEraseCopyFlashReq_T *pAMIYAFUEraseCopyFlashReq = (AMIYAFUEraseCopyFlashReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+
+    AMIYAFUEraseCopyFlashRes_T* pAMIYAFUEraseCopyFlash = (AMIYAFUEraseCopyFlashRes_T*)pRes;
+
+    if(CalculateChksum((char *)&pAMIYAFUEraseCopyFlashReq->Memoffset,pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Datalen) != pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.CRC32chksum)
+    {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+    }
+
+    if(pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Datalen == 0x0c)
+          pAMIYAFUEraseCopyFlash->EraseCpyFlashRes.Datalen = 0x04;
+    else
+    {
+
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+          pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode = YAFU_CC_INVALID_DATLEN;
+
+	   return (sizeof(AMIYAFUNotAck));
+    }
+  
+    gEraseBlock = pAMIYAFUEraseCopyFlashReq->Flashoffset;
+    gSizeToErase = 	pAMIYAFUEraseCopyFlashReq->Sizetocopy;
+    pNewFirmwareAddr = (unsigned char *)IMAGE_UPLOAD_LOCATION;
+
+
+   gStartFirmwareUpdation = 0x01;
+
+    pAMIYAFUEraseCopyFlash->Sizecopied =  pAMIYAFUEraseCopyFlashReq->Sizetocopy;
+    pAMIYAFUEraseCopyFlash->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode =  (INT16U)pAMIYAFUEraseCopyFlash->CompletionCode;
+    pAMIYAFUEraseCopyFlash->EraseCpyFlashRes.Seqnum = pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Seqnum;
+    pAMIYAFUEraseCopyFlash->EraseCpyFlashRes.YafuCmd= pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.YafuCmd;
+    pAMIYAFUEraseCopyFlash->EraseCpyFlashRes.CRC32chksum = CalculateChksum((char *)&(pAMIYAFUEraseCopyFlash->Sizecopied),sizeof(INT32U));
+
+    return (sizeof(AMIYAFUEraseCopyFlashRes_T));
+}
+else
+{
+    AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+    pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUEraseCopyFlashReq->EraseCpyFlashReq.Seqnum;
+    pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+    pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+    pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+    pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+    LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+    return (sizeof(AMIYAFUNotAck));
+}
+
+}
+
+
+
+/*---------------------------------------
+ * AMIYAFUWriteMemory
+ *---------------------------------------*/
+int AMIYAFUWriteMemory ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+
+    AMIYAFUWriteMemoryReq_T *pAMIYAFUWriteMemoryReq = (AMIYAFUWriteMemoryReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+    INT8U *OffsetToWrite;
+
+    AMIYAFUWriteMemoryRes_T* pAMIYAFUWriteMemory = (AMIYAFUWriteMemoryRes_T*)pRes;
+
+    if(CalculateChksum((char *)&pAMIYAFUWriteMemoryReq->Memoffset,pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen) != pAMIYAFUWriteMemoryReq->WriteMemReq.CRC32chksum)
+   {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUWriteMemoryReq->WriteMemReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+         return (sizeof(AMIYAFUNotAck));
+
+   }
+
+
+   OffsetToWrite = pNewFirmwareAddr ;
+   
+    memcpy (( INT8U*) OffsetToWrite,
+	 	 (pReq + sizeof(AMIYAFUWriteMemoryReq_T)), 
+		 (pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen - 5)); 
+
+   pNewFirmwareAddr += pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen - 5;
+
+    pAMIYAFUWriteMemory->SizeWritten = (pAMIYAFUWriteMemoryReq->WriteMemReq.Datalen - 5);
+
+
+    pAMIYAFUWriteMemory->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U) pAMIYAFUWriteMemory->CompletionCode;
+    pAMIYAFUWriteMemory->WriteMemRes.Seqnum= pAMIYAFUWriteMemoryReq->WriteMemReq.Seqnum;
+    pAMIYAFUWriteMemory->WriteMemRes.YafuCmd= pAMIYAFUWriteMemoryReq->WriteMemReq.YafuCmd;
+    pAMIYAFUWriteMemory->WriteMemRes.Datalen=0x02;
+    pAMIYAFUWriteMemory->WriteMemRes.CRC32chksum =  CalculateChksum((char *)&pAMIYAFUWriteMemory->SizeWritten,sizeof(INT16U));
+
+    return (sizeof(AMIYAFUWriteMemoryRes_T));
+}
+else
+{
+   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUWriteMemoryReq->WriteMemReq.Seqnum;
+   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+   return (sizeof(AMIYAFUNotAck));
+}
+
+
+}
+
+/*---------------------------------------
+ * AMIYAFUGetBootConfig
+ *---------------------------------------*/
+int AMIYAFUGetBootConfig ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+
+    AMIYAFUGetBootConfigReq_T *pAMIYAFUGetBootConfigReq = (AMIYAFUGetBootConfigReq_T *)pReq;
+  
+if( ActivateFlashStatus == 0x01)
+{
+     int len = 0;
+     char *Buffer;		 
+
+    AMIYAFUGetBootConfigRes_T* pAMIYAFUGetBootConfig = (AMIYAFUGetBootConfigRes_T*)pRes;
+
+    if(CalculateChksum((char *)&pAMIYAFUGetBootConfigReq->VarName[0],pAMIYAFUGetBootConfigReq->GetBootReq.Datalen) != pAMIYAFUGetBootConfigReq->GetBootReq.CRC32chksum)
+   {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetBootConfigReq->GetBootReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+         return (sizeof(AMIYAFUNotAck));
+
+   }
+
+    if(pAMIYAFUGetBootConfigReq->GetBootReq.Datalen == 0x41)
+          pAMIYAFUGetBootConfig->GetBootRes.Datalen= 0x42;
+    else
+    {
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+          pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetBootConfigReq->GetBootReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode =  pAMIYAFUNotAcknowledge->ErrorCode;
+						
+	   return (sizeof(AMIYAFUNotAck)); 
+
+    }
+
+    Buffer = getenv((char *)&pAMIYAFUGetBootConfigReq->VarName);
+
+    len = strlen(Buffer);
+    len++; 	
+	
+
+    pAMIYAFUGetBootConfig->Status = 0x01;
+
+    pAMIYAFUGetBootConfig->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U)pAMIYAFUGetBootConfig->CompletionCode;	
+    pAMIYAFUGetBootConfig->GetBootRes.Seqnum = pAMIYAFUGetBootConfigReq->GetBootReq.Seqnum;
+    pAMIYAFUGetBootConfig->GetBootRes.YafuCmd=pAMIYAFUGetBootConfigReq->GetBootReq.YafuCmd;
+      
+    memcpy (( INT8U*) (pAMIYAFUGetBootConfig + 1),
+                   ( INT8U*)Buffer,len );
+    pAMIYAFUGetBootConfig->GetBootRes.Datalen = len + 1;  
+    pAMIYAFUGetBootConfig->GetBootRes.CRC32chksum= CalculateChksum((char *)&pAMIYAFUGetBootConfig->Status,pAMIYAFUGetBootConfig->GetBootRes.Datalen);
+
+    return (sizeof(AMIYAFUGetBootConfigRes_T) + len);
+}
+else
+{
+    AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+    pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetBootConfigReq->GetBootReq.Seqnum;
+    pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+    pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+    pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+    pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+    LastStatCode =   pAMIYAFUNotAcknowledge->ErrorCode;	
+						
+    return (sizeof(AMIYAFUNotAck));   
+}
+	
+}
+
+/*---------------------------------------
+ * AMIYAFUSetBootConfig
+ *---------------------------------------*/
+int AMIYAFUSetBootConfig ( unsigned char *pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+    AMIYAFUSetBootConfigReq_T *pAMIYAFUSetBootConfigReq = (AMIYAFUSetBootConfigReq_T *)pReq;
+
+if( ActivateFlashStatus == 0x01)
+{
+   
+    char *BootVal = (char *)TEMP_SECTOR_BUFFER;	
+	
+    AMIYAFUSetBootConfigRes_T* pAMIYAFUSetBootConfig = (AMIYAFUSetBootConfigRes_T*)pRes;
+  
+    if(CalculateChksum((char *)&pAMIYAFUSetBootConfigReq->VarName[0],pAMIYAFUSetBootConfigReq->SetBootReq.Datalen) != pAMIYAFUSetBootConfigReq->SetBootReq.CRC32chksum)
+    {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUSetBootConfigReq->SetBootReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+	  LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+    }
+
+   
+    pReq += sizeof(AMIYAFUSetBootConfigReq_T);
+    memcpy(BootVal,pReq,(pAMIYAFUSetBootConfigReq->SetBootReq.Datalen - 65));
+    memset(pReq,0,(pAMIYAFUSetBootConfigReq->SetBootReq.Datalen - 65)); 	
+    pReq -= sizeof(AMIYAFUSetBootConfigReq_T);	
+
+    if(strcmp(pAMIYAFUSetBootConfigReq->VarName,"baudrate") != 0)
+    	    setenv(pAMIYAFUSetBootConfigReq->VarName,BootVal);
+  
+    saveenv();
+
+    pAMIYAFUSetBootConfig->Status = 0x01;
+
+    pAMIYAFUSetBootConfig->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U)pAMIYAFUSetBootConfig->CompletionCode;	
+    pAMIYAFUSetBootConfig->SetBootRes.Seqnum = pAMIYAFUSetBootConfigReq->SetBootReq.Seqnum;
+    pAMIYAFUSetBootConfig->SetBootRes.YafuCmd = pAMIYAFUSetBootConfigReq->SetBootReq.YafuCmd;
+    pAMIYAFUSetBootConfig->SetBootRes.Datalen = 0x01;	
+    pAMIYAFUSetBootConfig->SetBootRes.CRC32chksum= CalculateChksum((char *)&pAMIYAFUSetBootConfig->Status,pAMIYAFUSetBootConfig->SetBootRes.Datalen ); 	
+
+    return (sizeof(AMIYAFUSetBootConfigRes_T));
+}
+else
+{
+   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUSetBootConfigReq->SetBootReq.Seqnum;
+   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+   LastStatCode =  pAMIYAFUNotAcknowledge->ErrorCode;
+							   
+   return (sizeof(AMIYAFUNotAck)); 
+}
+   
+}
+
+/*---------------------------------------
+ * AMIYAFUGetBootVars
+ *---------------------------------------*/
+int AMIYAFUGetBootVars ( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes ) 
+{
+    AMIYAFUGetBootVarsReq_T *pAMIYAFUGetBootVarsReq = (AMIYAFUGetBootVarsReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+    int RetVal;
+    int BootVarlen =0;	
+    char *Buffer = (char *)TEMP_SECTOR_BUFFER;	
+
+    AMIYAFUGetBootVarsRes_T* pAMIYAFUGetBootVars = (AMIYAFUGetBootVarsRes_T*)pRes;	 
+    pAMIYAFUGetBootVars->VarCount = 0X00;
+
+
+    RetVal = Getenvvar ((char *)&pAMIYAFUGetBootVars->VarCount,Buffer,&BootVarlen); 
+
+    memcpy (( INT8U*) (pAMIYAFUGetBootVars + 1),
+                   ( INT8U*)Buffer,BootVarlen );
+
+    pAMIYAFUGetBootVars->GetBootRes.Datalen = BootVarlen + 1; 
+
+    pAMIYAFUGetBootVars->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U)pAMIYAFUGetBootVars->CompletionCode;	
+    pAMIYAFUGetBootVars->GetBootRes.Seqnum = pAMIYAFUGetBootVarsReq->GetBootReq.Seqnum;
+    pAMIYAFUGetBootVars->GetBootRes.YafuCmd = pAMIYAFUGetBootVarsReq->GetBootReq.YafuCmd;
+    pAMIYAFUGetBootVars->GetBootRes.CRC32chksum= CalculateChksum((char *)&pAMIYAFUGetBootVars->VarCount,pAMIYAFUGetBootVars->GetBootRes.Datalen);
+     
+    return (sizeof(AMIYAFUGetBootVarsRes_T)+ BootVarlen);
+}
+else
+{
+   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+   pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetBootVarsReq->GetBootReq.Seqnum;
+   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_IN_DEACTIVATE;
+   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+							   
+   return (sizeof(AMIYAFUNotAck)); 
+} 
+
+}
+
+
+
+
+/*---------------------------------------
+ * AMIYAFUDeactivateFlash
+ *---------------------------------------*/
+int AMIYAFUDeactivateFlash (unsigned char*pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+
+    AMIYAFUDeactivateFlashReq_T *pAMIYAFUDeactivateFlashReq = (AMIYAFUDeactivateFlashReq_T *)pReq;
+
+if(ActivateFlashStatus == 0x01)
+{
+    AMIYAFUDeactivateFlashRes_T* pAMIYAFUDeactivateFlash = (AMIYAFUDeactivateFlashRes_T*)pRes;
+
+
+    if(pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.Datalen== 0x00)
+          pAMIYAFUDeactivateFlash->DeactivateFlashRes.Datalen= 0x01;
+    else
+    {
+
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+          pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+	   return (sizeof(AMIYAFUNotAck));
+
+    }
+
+    ActivateFlashStatus = 0x00;
+
+    pAMIYAFUDeactivateFlash->Status = 0x00;
+    pAMIYAFUDeactivateFlash->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U) pAMIYAFUDeactivateFlash->CompletionCode;
+    pAMIYAFUDeactivateFlash->DeactivateFlashRes.Seqnum = pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.Seqnum;
+    pAMIYAFUDeactivateFlash->DeactivateFlashRes.YafuCmd = pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.YafuCmd;
+    pAMIYAFUDeactivateFlash->DeactivateFlashRes.CRC32chksum= CalculateChksum((char *)&pAMIYAFUDeactivateFlash->Status,sizeof(INT8U));
+
+
+    return (sizeof(AMIYAFUDeactivateFlashRes_T));
+}
+else
+{
+    AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+    pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUDeactivateFlashReq->DeactivateFlashReq.Seqnum;
+    pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+    pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+    pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+    pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+    LastStatCode = pAMIYAFUNotAcknowledge->ErrorCode;
+
+    return (sizeof(AMIYAFUNotAck));
+}
+
+}
+
+
+/*---------------------------------------
+ * AMIYAFUResetDevice
+ *---------------------------------------*/
+int AMIYAFUResetDevice ( unsigned char *pReq, unsigned char ReqLen,unsigned char *pRes )  
+{
+
+    AMIYAFUResetDeviceReq_T *pAMIYAFUResetDeviceReq = (AMIYAFUResetDeviceReq_T *)pReq;
+    AMIYAFUResetDeviceRes_T* pAMIYAFUResetDevice = (AMIYAFUResetDeviceRes_T*)pRes;
+
+     if(CalculateChksum((char *)&pAMIYAFUResetDeviceReq->WaitSec,sizeof(INT16U)) != pAMIYAFUResetDeviceReq->ResetReq.CRC32chksum)
+   {
+         AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+         pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUResetDeviceReq->ResetReq.Seqnum;
+	  pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	  pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	  pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+         pAMIYAFUNotAcknowledge->ErrorCode = YAFU_INVALID_CHKSUM;
+         LastStatCode=pAMIYAFUNotAcknowledge->ErrorCode;
+
+         return (sizeof(AMIYAFUNotAck));
+
+   }
+
+    if(pAMIYAFUResetDeviceReq->ResetReq.Datalen== 0x02)
+          pAMIYAFUResetDevice->ResetRes.Datalen= 0x01;
+    else
+    {
+
+	   AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+          pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUResetDeviceReq->ResetReq.Seqnum;
+	   pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+	   pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+	   pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00;
+	   pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_INVALID_DATLEN;
+	   LastStatCode =   pAMIYAFUNotAcknowledge->ErrorCode;
+
+	   return (sizeof(AMIYAFUNotAck));
+
+    }
+
+    gReset = 1;
+
+    pAMIYAFUResetDevice->CompletionCode = YAFU_CC_NORMAL;
+    LastStatCode = (INT16U) pAMIYAFUResetDevice->CompletionCode;
+    pAMIYAFUResetDevice->ResetRes.Seqnum = pAMIYAFUResetDeviceReq->ResetReq.Seqnum;
+    pAMIYAFUResetDevice->ResetRes.YafuCmd = pAMIYAFUResetDeviceReq->ResetReq.YafuCmd;
+    pAMIYAFUResetDevice->ResetRes.CRC32chksum=CalculateChksum((char *)&pAMIYAFUResetDevice->Status,sizeof(INT8U));
+
+    return (sizeof(AMIYAFUResetDeviceRes_T));
+}
+
+/*-----------------------
+ * Command Handler Type
+ *----------------------*/
+typedef int (*pCmdHndlr_T) (unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes);
+
+/**** Command Handlers ****/
+typedef struct
+{
+	unsigned char	NetFnLUN;
+	unsigned char	Cmd;
+	pCmdHndlr_T		pHandler;
+	unsigned char	Len;
+
+} PACKED CmdInfo_T;
+
+/* Supported Commands */
+const CmdInfo_T	m_CmdInfo [] = 
+{
+    { NETFN_APP,		CMD_GET_DEV_ID,		            GET_DEV_ID, 	        	0x00    					},
+	{ NETFN_OEM_AMI,	CMD_AMI_INIT_FLASH,				AMI_INIT_FLASH,				0xFF	},
+	{ NETFN_OEM_AMI,	CMD_AMI_UPGRADE_BLOCK,			AMI_UPGRADE_BLOCK,			0xFF	},
+	{ NETFN_OEM_AMI,	CMD_AMI_EXIT_FLASH,				AMI_EXIT_FLASH,				0xFF	},
+	{ NETFN_OEM_AMI,	CMD_AMI_RESET_CARD,				AMI_RESET_CARD,				0xFF	},
+	{ NETFN_OEM_AMI,    CMD_AMI_GET_STATUS,				AMI_GET_STATUS,				0xFF	},
+	{ NETFN_OEM_AMI,    CMD_AMI_START_FW_UPDATE,		AMI_START_FW_UPDATE,		0xFF	},
+	{ NETFN_APP,		CMD_GET_CH_AUTH_CAPS,			GET_CH_AUTH_CAPS,			sizeof (GetChAuthCapReq_T) },	
+    { NETFN_APP,		CMD_GET_SESSION_CHALLENGE,      GET_SESSION_CHALLENGE,  	sizeof (GetSesChallengeReq_T) },
+    { NETFN_APP,		CMD_ACTIVATE_SESSION, 		    ACTIVATE_SESSION,       	sizeof (ActivateSesReq_T) },
+    { NETFN_APP,		CMD_SET_SESSION_PRIV_LEVEL,     SET_SESSION_PRIV_LEVEL, 	sizeof (unsigned char) },
+    { NETFN_APP,		CMD_CLOSE_SESSION,              CLOSE_SESSION,          	sizeof (CloseSesReq_T) },
+    { NETFN_OEM_AMI,   CMD_AMI_YAFU_GET_FLASH_INFO,                   AMI_YAFU_GET_FLASH_INFO,                sizeof(AMIYAFUGetFlashInfoReq_T)},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_GET_FMH_INFO, 				AMI_YAFU_GET_FMH_INFO,			  sizeof(AMIYAFUGetFMHInfoReq_T) },
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_GET_STATUS,			 	AMI_YAFU_GET_STATUS,			  sizeof(AMIYAFUGetStatusReq_T) },
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_ACTIVATE_FLASH,			AMI_YAFU_ACTIVATE_FLASH,		  sizeof(AMIYAFUActivateFlashModeReq_T)},
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_PROTECT_FLASH,			AMI_YAFU_PROTECT_FLASH, 		  sizeof(AMIYAFUProtectFlashReq_T) },
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_ERASE_COPY_FLASH, 		AMI_YAFU_ERASE_COPY_FLASH,		  sizeof(AMIYAFUEraseCopyFlashReq_T) },
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_WRITE_MEMORY, 			AMI_YAFU_WRITE_MEMORY,			  0xff },
+    { NETFN_OEM_AMI,   CMD_AMI_YAFU_GET_BOOT_CONFIG,               AMI_YAFU_GET_BOOT_CONFIG,            sizeof(AMIYAFUGetBootConfigReq_T) },
+    { NETFN_OEM_AMI,   CMD_AMI_YAFU_SET_BOOT_CONFIG,               AMI_YAFU_SET_BOOT_CONFIG,             0xff},
+    { NETFN_OEM_AMI,   CMD_AMI_YAFU_GET_BOOT_VARS,                  AMI_YAFU_GET_BOOT_VARS,                0xff}, 
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_DEACTIVATE_FLASH_MODE,	AMI_YAFU_DEACTIVATE_FLASH_MODE, sizeof(AMIYAFUDeactivateFlashReq_T) },
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_RESET_DEVICE, 			AMI_YAFU_RESET_DEVICE , 			  sizeof(AMIYAFUResetDeviceReq_T) },
+
+};
+
+/*
+ * Process IPMI Command
+ */
+void
+ProcessIPMICmd (IPMICmdData_T* pIPMICmd)
+{
+	int i;
+	unsigned char Err = CC_NORMAL;
+
+	/* Process this message through the SMM message handler */
+	for (i = 0; i < sizeof (m_CmdInfo) / sizeof (CmdInfo_T); i++)
+	{
+		if ((m_CmdInfo [i].NetFnLUN == (pIPMICmd->NetFnLUN >> 2)) &&
+			(m_CmdInfo [i].Cmd	    == pIPMICmd->Cmd))
+		{
+			break;
+		}
+	}
+
+	/* If we have found the handler - handle the command */
+	if (i != sizeof (m_CmdInfo) / sizeof (CmdInfo_T))
+	{
+		/* Check for the request length */
+		if (0xff != m_CmdInfo [i].Len)
+		{
+			if (pIPMICmd->ReqLen != m_CmdInfo [i].Len)
+			{
+				printf("INV LEN: Recv len = %d,exp len = %d\n",pIPMICmd->ReqLen,m_CmdInfo [i].Len);
+				Err = CC_REQ_INV_LEN;
+			}
+			
+		}
+
+	}
+	else
+	{
+		Err = CC_INV_CMD;
+	}
+
+    if (Err == CC_NORMAL)
+	{
+         pIPMICmd->ResLen = m_CmdInfo [i].pHandler (pIPMICmd->ReqData, pIPMICmd->ReqLen, pIPMICmd->ResData); 
+	  Responselen = pIPMICmd->ResLen;	 
+    }
+    else
+    {
+        pIPMICmd->ResData [0] = Err;
+		pIPMICmd->ResLen	  = 1;
+    }
+    
+}
+#endif
+
+
+
+
+
+
+
+
