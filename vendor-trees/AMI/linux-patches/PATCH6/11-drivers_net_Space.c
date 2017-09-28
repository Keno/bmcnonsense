--- linux-2.6.15.4-cpu/drivers/net/Space.c	2006-02-10 02:22:48.000000000 -0500
+++ linux-2.6.15.4-wpcm/drivers/net/Space.c	2007-06-18 15:52:55.000000000 -0400
@@ -86,6 +86,9 @@
 extern struct net_device *mc32_probe(int unit);
 extern struct net_device *cops_probe(int unit);
 extern struct net_device *ltpc_probe(void);
+#ifdef CONFIG_WPCM450_MAC
+extern struct net_device *wpcm450_mac_probe(int unit);
+#endif
 
 /* Detachable devices ("pocket adaptors") */
 extern struct net_device *de620_probe(int unit);
@@ -239,6 +242,9 @@
 #ifdef CONFIG_NI65
 	{ni65_probe, 0},
 #endif
+#ifdef CONFIG_WPCM450_MAC
+	{wpcm450_mac_probe,0 },
+#endif
 	{NULL, 0},
 };
 
