--- uboot/vendors/ami/IPMI/cmdhandler.h	2008-12-29 14:22:13.000000000 +0530
+++ uboot/vendors/ami/IPMI/cmdhandler.h	2009-03-10 13:21:57.000000000 +0530
@@ -91,4 +91,5 @@
 #define CMD_AMI_YAFU_PROTECT_FLASH (0x0025)
 #define CMD_AMI_YAFU_ERASE_COPY_FLASH (0x0026)
+#define CMD_AMI_YAFU_GET_ECF_STATUS (0x0028)
 #define CMD_AMI_YAFU_WRITE_MEMORY (0x0031)
 #define CMD_AMI_YAFU_GET_BOOT_CONFIG (0x0040)
@@ -128,4 +129,5 @@
 #define AMI_YAFU_PROTECT_FLASH           		AMIYAFUProtectFlash
 #define AMI_YAFU_ERASE_COPY_FLASH     		AMIYAFUEraseCopyFlash
+#define AMI_YAFU_GET_ECF_STATUS		        AMIYAFUGetECFStatus
 #define AMI_YAFU_WRITE_MEMORY            		AMIYAFUWriteMemory
 #define AMI_YAFU_GET_BOOT_CONFIG      		AMIYAFUGetBootConfig        
@@ -676,4 +678,25 @@
 typedef struct
 {
+    YafuHeader GetECFStatusReq;
+         	
+} PACKED AMIYAFUGetECFStatusReq_T;
+
+/**
+ * @struct AMIYAFUGetECFStatusRes_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
+    INT8U   CompletionCode;
+    YafuHeader GetECFStatusRes;
+	INT32U Status;
+	INT16U Progress;
+}PACKED AMIYAFUGetECFStatusRes_T;
+/**
+ * @struct AMIYAFUWriteMemoryReq_T
+ * @brief Flash info structure
+ */
+typedef struct
+{
     YafuHeader WriteMemReq;
     INT32U Memoffset;
@@ -827,4 +850,5 @@
 #define YAFU_INVALID_CHKSUM                           0x12
 #define YAFU_PROTECT_ERR                                 0x14
+#define YAFU_ECF_SUCCESS	0x15
 /**
  * @def DEVICE_ID
