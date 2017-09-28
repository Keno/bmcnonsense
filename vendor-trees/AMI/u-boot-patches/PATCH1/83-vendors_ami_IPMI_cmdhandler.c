--- uboot-pristine/vendors/ami/IPMI/cmdhandler.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/vendors/ami/IPMI/cmdhandler.c	2009-05-14 10:30:24.000000000 -0700
@@ -884,12 +884,12 @@
 {
 
 
-       AMIYAFUGetFlashInfoReq_T *pAMIYAFUFlashInfoReq = (AMIYAFUGetFlashInfoReq_T *)pReq;
-       AMIYAFUGetFlashInfoRes_T* pAMIYAFUGetFlashInfo = (AMIYAFUGetFlashInfoRes_T*)pRes;  
+	AMIYAFUGetFlashInfoReq_T *pAMIYAFUFlashInfoReq = (AMIYAFUGetFlashInfoReq_T *)pReq;
+	AMIYAFUGetFlashInfoRes_T* pAMIYAFUGetFlashInfo = (AMIYAFUGetFlashInfoRes_T*)pRes;  
 
      	
 	pAMIYAFUGetFlashInfo->CompletionCode = YAFU_CC_NORMAL;
-       pAMIYAFUGetFlashInfo->FlashInfoRes.Seqnum = pAMIYAFUFlashInfoReq->FlashInfoReq.Seqnum;
+	pAMIYAFUGetFlashInfo->FlashInfoRes.Seqnum = pAMIYAFUFlashInfoReq->FlashInfoReq.Seqnum;
 	pAMIYAFUGetFlashInfo->FlashInfoRes.YafuCmd= pAMIYAFUFlashInfoReq->FlashInfoReq.YafuCmd;
 	
 	pAMIYAFUGetFlashInfo->FlashInfo.FlashSize = CFG_PROJ_USED_FLASH_SIZE;
@@ -899,20 +899,20 @@
 	pAMIYAFUGetFlashInfo->FlashInfo.FlashWidth = 8;  
 	pAMIYAFUGetFlashInfo->FlashInfo.FMHCompliance = 0x01;
 
-       if(pAMIYAFUGetFlashInfo->FlashInfo.FMHCompliance == 0x01)
-	   	FMHComp = 1;
+	if(pAMIYAFUGetFlashInfo->FlashInfo.FMHCompliance == 0x01)
+		FMHComp = 1;
 	
-      	pAMIYAFUGetFlashInfo->FlashInfo.Reserved = 0;
+	pAMIYAFUGetFlashInfo->FlashInfo.Reserved = 0;
 	pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks = CFG_PROJ_USED_FLASH_SIZE/CFG_PROJ_ERASE_BLOCK_SIZE; 
 
-       if((pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks * pAMIYAFUGetFlashInfo->FlashInfo.FlashEraseBlkSize) == pAMIYAFUGetFlashInfo->FlashInfo.FlashSize)
-             pAMIYAFUGetFlashInfo->FlashInfoRes.Datalen= 0x20; 
-       else	   
-             pAMIYAFUGetFlashInfo->FlashInfoRes.Datalen = 0x20 + pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks;
+	if((pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks * pAMIYAFUGetFlashInfo->FlashInfo.FlashEraseBlkSize) == pAMIYAFUGetFlashInfo->FlashInfo.FlashSize)
+		pAMIYAFUGetFlashInfo->FlashInfoRes.Datalen= 0x20; 
+	else	   
+		pAMIYAFUGetFlashInfo->FlashInfoRes.Datalen = 0x20 + pAMIYAFUGetFlashInfo->FlashInfo.NoEraseBlks;
 
-      pAMIYAFUGetFlashInfo->FlashInfoRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetFlashInfo->FlashInfo,sizeof(pAMIYAFUGetFlashInfo->FlashInfo));				 	   
+	pAMIYAFUGetFlashInfo->FlashInfoRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetFlashInfo->FlashInfo,sizeof(pAMIYAFUGetFlashInfo->FlashInfo));				 	   
 	
-       return( sizeof( AMIYAFUGetFlashInfoRes_T ) );
+	return( sizeof( AMIYAFUGetFlashInfoRes_T ) );
 
 }
 
@@ -922,46 +922,46 @@
  *---------------------------------------*/
  int AMIYAFUGetFMHInfo ( unsigned char* pReq, unsigned char ReqLen, unsigned char *pRes ) 
 {
-      DWORD i=0,m=0;
-      INT8U *Buf;
-      FlashMH      FlashModHeader;
-      ALT_FMHead    AltFmh;  
-      char *FMHDetails;	  
+	DWORD i=0,m=0;
+	INT8U *Buf;
+	FlashMH      FlashModHeader;
+	ALT_FMHead    AltFmh;  
+	char *FMHDetails;	  
     
 
-      AMIYAFUGetFMHInfoReq_T *pAMIYAFUGetFMHInfoReq = (AMIYAFUGetFMHInfoReq_T *)pReq;
-      AMIYAFUGetFMHInfoRes_T* pAMIYAFUGetFMHInfo = (AMIYAFUGetFMHInfoRes_T*)pRes;
+	AMIYAFUGetFMHInfoReq_T *pAMIYAFUGetFMHInfoReq = (AMIYAFUGetFMHInfoReq_T *)pReq;
+	AMIYAFUGetFMHInfoRes_T* pAMIYAFUGetFMHInfo = (AMIYAFUGetFMHInfoRes_T*)pRes;
 
-  	pAMIYAFUGetFMHInfo->NumFMH = 0x00;
+	pAMIYAFUGetFMHInfo->NumFMH = 0x00;
 
-      FMHDetails = malloc(1200);
-      if(FMHDetails == NULL)
-      {
-             printf("Error in malloc of FMHDetails");
-	      return -1;		 
-      }
+	FMHDetails = malloc(1200);
+	if(FMHDetails == NULL)
+	{
+		printf("Error in malloc of FMHDetails");
+		return -1;		 
+	}
 
 
    for(i=0;i<(CFG_PROJ_USED_FLASH_SIZE/CFG_PROJ_ERASE_BLOCK_SIZE);i++) 			
    {
-	Buf= (INT8U *)malloc(64);
+		Buf= (INT8U *)malloc(64);
 
-	if(Buf == NULL)
-	{
-	      printf("Error in alloc\n");
-	      AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
-	      pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
-	      pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
-	      pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
-	      pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
-	      pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_ALLOC_ERR;
-	      LastStatCode = YAFU_CC_ALLOC_ERR;
+		if(Buf == NULL)
+		{
+			printf("Error in alloc\n");
+			AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
+			pAMIYAFUNotAcknowledge->NotAck.Seqnum = pAMIYAFUGetFMHInfoReq->FMHReq.Seqnum;
+			pAMIYAFUNotAcknowledge->NotAck.YafuCmd = CMD_AMI_YAFU_COMMON_NAK;
+			pAMIYAFUNotAcknowledge->NotAck.Datalen = 0x02;
+			pAMIYAFUNotAcknowledge->NotAck.CRC32chksum = 0x00; 
+			pAMIYAFUNotAcknowledge->ErrorCode = YAFU_CC_ALLOC_ERR;
+			LastStatCode = YAFU_CC_ALLOC_ERR;
 
-             return (sizeof(AMIYAFUNotAck));
+			return (sizeof(AMIYAFUNotAck));
    	}
 
 
-       if(flash_read( CFG_PROJ_FLASH_START+(i*CFG_PROJ_ERASE_BLOCK_SIZE),64,Buf) != 0)
+	if(flash_read( CFG_PROJ_FLASH_START+(i*CFG_PROJ_ERASE_BLOCK_SIZE),64,Buf) != 0)
 	{
 
 		AMIYAFUNotAck* pAMIYAFUNotAcknowledge =(AMIYAFUNotAck*)pRes;
@@ -980,18 +980,18 @@
       if(strncmp(FlashModHeader.FmhSignature,"$MODULE$",(sizeof(FlashModHeader.FmhSignature) -1)) == 0 )	//Check FMH Signature
       {
 
-            if(FlashModHeader.EndSignature != 21930)
-	      	  	continue;
+			if(FlashModHeader.EndSignature != 21930)
+				continue;
 
-	     memcpy((FMHDetails + m),(char *)&FlashModHeader,64);
+			memcpy((FMHDetails + m),(char *)&FlashModHeader,64);
            
        
-              printf("Module Name = %s\n",FlashModHeader.ModuleName);
+			printf("Module Name = %s\n",FlashModHeader.ModuleName);
 
-	     m +=64;
-	     pAMIYAFUGetFMHInfo->NumFMH += 0x1;
+			m +=64;
+			pAMIYAFUGetFMHInfo->NumFMH += 0x1;
 
-	     free(Buf);
+			free(Buf);
 
 	}
       else
@@ -1347,7 +1347,7 @@
     pAMIYAFUGetECFStatus->GetECFStatusRes.YafuCmd= pAMIYAFUGetECFStatusReq->GetECFStatusReq.YafuCmd;
 	pAMIYAFUGetECFStatus->GetECFStatusRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetECFStatus->Status,(INT32U)pAMIYAFUGetECFStatus->GetECFStatusRes.Datalen);
     return ( sizeof( AMIYAFUGetECFStatusRes_T ) );
-
+ 
 }
 
 /*---------------------------------------
