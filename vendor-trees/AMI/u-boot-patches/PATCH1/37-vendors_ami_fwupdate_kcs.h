--- uboot.org/vendors/ami/fwupdate/kcs.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot/vendors/ami/fwupdate/kcs.h	2007-04-05 12:40:10.000000000 -0400
@@ -0,0 +1,75 @@
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
+ * KCS.h
+ * KCS Functions and Macros Definitions.
+ *
+ *  Author: Rama Rao Bisa <ramab@ami.com>
+ *			Modified by Anurag Bhatia for SP porting		
+ ******************************************************************/
+#ifndef KCS_H
+#define KCS_H
+
+
+#define PACKED __attribute__ ((packed))
+
+/** 
+ * @def MAX_KCS_PKT_LEN
+ * @brief Max KCS Packet Length
+**/
+#define MAX_KCS_PKT_LEN			 (64*1024)
+
+/** 
+ * @def MAX_KCS_DATA_LEN
+ * @brief Max KCS Data Length
+**/
+#define MAX_KCS_DATA_LEN		(MAX_KCS_PKT_LEN - 2)
+
+/**
+ * @brief Initialize KCS interface.
+ * @return 0 if success, -1 if error.
+**/
+extern int InitKCS (void);
+
+/**
+ * @brief Receive KCS Request
+ * @return 0 if success if full KCS packet is received and -1 otherwise
+**/
+extern int RecvKCSRequest (unsigned char *pKCSPkt);
+
+/**
+ * @brief Send KCS Response
+**/
+extern void SendKCSResponse (unsigned char *Pkt, int Size);
+
+#pragma pack(1)
+/* KCSCmd_T */
+typedef struct
+{
+	unsigned char NetFnLUN;
+	unsigned char Cmd;
+	unsigned char Data [MAX_KCS_DATA_LEN];
+
+} PACKED KCSCmd_T;
+#pragma pack()
+#endif	/* KCS_H */
