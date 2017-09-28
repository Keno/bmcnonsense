--- uboot.aviator/common/exports.c	2007-06-26 17:33:48.000000000 -0400
+++ uboot.ump.multi-spi-flash/common/exports.c	2007-06-12 21:31:57.000000000 -0400
@@ -32,4 +32,11 @@
 	gd->jt[XF_i2c_write] = (void *) i2c_write;
 	gd->jt[XF_i2c_read] = (void *) i2c_read;
 #endif	/* CFG_CMD_I2C */
+#ifdef CONFIG_EXPORT_ETH_FNS
+	gd->jt[XF_eth_init] = (void *) eth_init;
+	gd->jt[XF_eth_halt] = (void *) eth_halt;
+	gd->jt[XF_eth_send] = (void *) eth_send;
+	gd->jt[XF_eth_rx] = (void *) eth_rx;
+	gd->jt[XF_NetSetHandler] = (void *) NetSetHandler;
+#endif /* EXPORT_ETH_FNS */
 }
