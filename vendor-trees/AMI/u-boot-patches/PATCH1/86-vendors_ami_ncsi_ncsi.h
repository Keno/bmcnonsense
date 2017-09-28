--- uboot-pristine/vendors/ami/ncsi/ncsi.h	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/vendors/ami/ncsi/ncsi.h	2009-05-14 10:30:24.000000000 -0700
@@ -27,7 +27,7 @@
 #include "types.h"
 
 #define NCSI_RETRIES		3	/* Command retries for response */
-#define NCSI_1G_SUPPORT		0
+#define NCSI_1G_SUPPORT		1
 #define NCSI_10G_SUPPORT	0
 
 /*---------------------NCSI Commands  ----------------------------*/
@@ -708,6 +708,7 @@
 void NCSIHandler(unsigned char *Buffer,unsigned Unused1,unsigned Unused2, unsigned Len);
 int  EnableChannel(UINT8 PackageID,UINT8 ChannelID); 
 int  EnableChannelOnLink(UINT8 PackageID,UINT8 ChannelID); /* Called on Link Change AEN */
+int NCSI_Issue_EnableVLAN(UINT8 PackageID, UINT8 ChannelID,UINT8 VLANFilterMode);
 
 /* Miscellaneous functions */
 #define DUMP_BUFFER 	0x02
