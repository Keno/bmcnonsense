--- uboot/vendors/ami/IPMI/cmdhandler.c	2008-12-29 14:22:13.000000000 +0530
+++ uboot/vendors/ami/IPMI/cmdhandler.c	2009-03-10 13:22:14.000000000 +0530
@@ -1320,5 +1320,34 @@
 }
 
+int AMIYAFUGetECFStatus( unsigned char *pReq, unsigned char ReqLen, unsigned char *pRes )  
+{
+	AMIYAFUGetECFStatusReq_T *pAMIYAFUGetECFStatusReq = (AMIYAFUGetECFStatusReq_T *)pReq;
+    AMIYAFUGetECFStatusRes_T* pAMIYAFUGetECFStatus = (AMIYAFUGetECFStatusRes_T*)pRes;
+
+//    if(LastStatCode!=YAFU_CC_NORMAL&&LastStatCode!=YAFU_ECF_SUCCESS)
+//		{
+//			return (AMIYAFUNotAcks(pRes,LastStatCode,pAMIYAFUGetECFStatusReq->GetECFStatusReq.Seqnum));    
+//		}
+
+	if(gStartFirmwareUpdation==0)
+	{
+		LastStatCode=YAFU_ECF_SUCCESS;
+		pAMIYAFUGetECFStatus->Progress= 100;
+	}
+	else
+	{
+		LastStatCode=YAFU_CC_NORMAL;
+		pAMIYAFUGetECFStatus->Progress= 0x00;			
+	}
+	pAMIYAFUGetECFStatus->CompletionCode=YAFU_CC_NORMAL;
+	pAMIYAFUGetECFStatus->Status =(INT16U) LastStatCode;
+    
+    pAMIYAFUGetECFStatus->GetECFStatusRes.Datalen=0x00;
+	pAMIYAFUGetECFStatus->GetECFStatusRes.Seqnum = pAMIYAFUGetECFStatusReq->GetECFStatusReq.Seqnum;
+    pAMIYAFUGetECFStatus->GetECFStatusRes.YafuCmd= pAMIYAFUGetECFStatusReq->GetECFStatusReq.YafuCmd;
+	pAMIYAFUGetECFStatus->GetECFStatusRes.CRC32chksum = CalculateChksum((char *)&pAMIYAFUGetECFStatus->Status,(INT32U)pAMIYAFUGetECFStatus->GetECFStatusRes.Datalen);
+    return ( sizeof( AMIYAFUGetECFStatusRes_T ) );
 
+}
 
 /*---------------------------------------
@@ -1725,4 +1754,5 @@
     { NETFN_OEM_AMI,	CMD_AMI_YAFU_PROTECT_FLASH,			AMI_YAFU_PROTECT_FLASH, 		  sizeof(AMIYAFUProtectFlashReq_T) },
     { NETFN_OEM_AMI,	CMD_AMI_YAFU_ERASE_COPY_FLASH, 		AMI_YAFU_ERASE_COPY_FLASH,		  sizeof(AMIYAFUEraseCopyFlashReq_T) },
+    { NETFN_OEM_AMI,	CMD_AMI_YAFU_GET_ECF_STATUS, 		AMI_YAFU_GET_ECF_STATUS,		  sizeof(AMIYAFUGetECFStatusReq_T) },    
     { NETFN_OEM_AMI,	CMD_AMI_YAFU_WRITE_MEMORY, 			AMI_YAFU_WRITE_MEMORY,			  0xff },
     { NETFN_OEM_AMI,   CMD_AMI_YAFU_GET_BOOT_CONFIG,               AMI_YAFU_GET_BOOT_CONFIG,            sizeof(AMIYAFUGetBootConfigReq_T) },
