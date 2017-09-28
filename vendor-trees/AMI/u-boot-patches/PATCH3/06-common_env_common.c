--- uboot-pristine/common/env_common.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/common/env_common.c	2009-05-14 10:30:24.000000000 -0700
@@ -99,6 +99,9 @@
 #ifdef	CONFIG_ETH3ADDR
 	"eth3addr="	MK_STR(CONFIG_ETH3ADDR)		"\0"
 #endif
+#ifdef	CONFIG_ETHPRIME
+	"ethprime="	CONFIG_ETHPRIME			"\0"
+#endif
 #ifdef	CONFIG_IPADDR
 	"ipaddr="	MK_STR(CONFIG_IPADDR)		"\0"
 #endif
