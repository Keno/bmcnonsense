--- u-boot-1.1.4-cpu/net/eth.c	2007-06-27 14:52:06.000000000 -0400
+++ u-boot-1.1.4-wpcm/net/eth.c	2007-06-27 18:16:18.000000000 -0400
@@ -53,6 +53,9 @@
 extern int scc_initialize(bd_t*);
 extern int skge_initialize(bd_t*);
 extern int tsec_initialize(bd_t*, int, char *);
+#if defined(CONFIG_WPCM450)
+extern int 	wpcm_eth_initialize(bd_t *bis);
+#endif
 
 static struct eth_device *eth_devices, *eth_current;
 
@@ -232,6 +235,9 @@
 	rtl8169_initialize(bis);
 #endif
 
+#if defined(CONFIG_WPCM450)
+	wpcm_eth_initialize(bis);
+#endif
 	if (!eth_devices) {
 		puts ("No ethernet found.\n");
 	} else {
