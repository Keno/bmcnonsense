--- uboot-pristine/vendors/ami/ncsi/getmac.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/vendors/ami/ncsi/getmac.c	2009-05-14 10:30:24.000000000 -0700
@@ -45,7 +45,7 @@
 	char *start,*end;
 
 	memset(MACAddr,0,6);
-	if (getenv_r("eth1addr",MacStr,sizeof(MacStr)) == -1)
+	if (getenv_r("ethaddr",MacStr,sizeof(MacStr)) == -1)
 		return;
 	
 	end = &MacStr[0];
