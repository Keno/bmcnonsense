--- uboot.aviator/include/_exports.h	2005-12-16 11:39:27.000000000 -0500
+++ uboot.ump.multi-spi-flash/include/_exports.h	2007-06-12 21:32:19.000000000 -0400
@@ -16,3 +16,10 @@
 EXPORT_FUNC(i2c_write)
 EXPORT_FUNC(i2c_read)
 #endif	/* CFG_CMD_I2C */
+#ifdef CONFIG_EXPORT_ETH_FNS
+EXPORT_FUNC(eth_init)
+EXPORT_FUNC(eth_halt)
+EXPORT_FUNC(eth_send)
+EXPORT_FUNC(eth_rx)
+EXPORT_FUNC(NetSetHandler)
+#endif /* EXPORT_ETH_FNS */
