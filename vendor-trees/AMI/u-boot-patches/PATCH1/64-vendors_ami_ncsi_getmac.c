--- uboot/vendors/ami/ncsi/getmac.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/vendors/ami/ncsi/getmac.c	2007-10-04 15:32:12.000000000 -0400
@@ -0,0 +1,69 @@
+/****************************************************************
+ ****************************************************************
+ **                                                            **
+ **    (C)Copyright 2005-2006, American Megatrends Inc.        **
+ **                                                            **
+ **            All Rights Reserved.                            **
+ **                                                            **
+ **        6145-F, Northbelt Parkway, Norcross,                **
+ **                                                            **
+ **        Georgia - 30071, USA. Phone-(770)-246-8600.         **
+ **                                                            **
+ ****************************************************************
+ ****************************************************************/
+/****************************************************************
+
+  Author	: Samvinesh Christopher
+
+  Module	: NCSI - Get MAC Address from U-boot
+			
+  Revision	: 1.0  
+
+  Changelog : 1.0 - Initial Version [SC]
+
+*****************************************************************/
+#include <common.h>
+#ifdef CONFIG_NCSI_SUPPORT
+#include <exports.h>
+#include "types.h"
+
+int 
+GetDefaultPort(void)
+{
+	char PortNum[8];
+	if (getenv_r("ncsiport",PortNum,sizeof(PortNum)) == -1)
+		return 0;
+	if ((PortNum[0] < '0') || (PortNum[0] > '9'))
+		return 0;
+	return PortNum[0]-'0';
+}
+
+void GetMACAddr(UINT8 *MACAddr)
+{
+	UINT8 MacStr[]  = "00:00:00:00:00:00";
+	int index=5;
+	char *start,*end;
+
+	memset(MACAddr,0,6);
+	if (getenv_r("eth1addr",MacStr,sizeof(MacStr)) == -1)
+		return;
+	
+	end = &MacStr[0];
+	while(*end)
+	{
+		start = end;
+		MACAddr[index] = simple_strtoul(start,&end,16);
+		if (*end)
+			end++;	/* Skip : */
+		if (index == 0)
+		    break;
+		index--;
+	}		
+
+	printf("NC-SI MAC Address  = %02X:%02X:%02X:%02X:%02X:%02X\n",
+						MACAddr[5],MACAddr[4],MACAddr[3],
+					  	MACAddr[2],MACAddr[1],MACAddr[0]);
+	return;
+}
+
+#endif
