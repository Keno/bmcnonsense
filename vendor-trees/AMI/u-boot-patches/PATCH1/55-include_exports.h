--- uboot.aviator/include/exports.h	2005-12-16 11:39:27.000000000 -0500
+++ uboot.ump.multi-spi-flash/include/exports.h	2007-06-12 21:32:21.000000000 -0400
@@ -5,6 +5,10 @@
 
 #include <common.h>
 
+#ifdef CONFIG_EXPORT_ETH_FNS
+typedef void	rxhand_func(uchar *, unsigned, unsigned, unsigned);
+#endif /* EXPORT_ETH_FNS */
+
 /* These are declarations of exported functions available in C code */
 unsigned long get_version(void);
 int  getc(void);
@@ -24,6 +28,13 @@
 int i2c_write (uchar, uint, int , uchar* , int);
 int i2c_read (uchar, uint, int , uchar* , int);
 #endif	/* CFG_CMD_I2C */
+#ifdef CONFIG_EXPORT_ETH_FNS
+void NetSetHandler(rxhand_func*);	
+int eth_init(bd_t *bis);			
+int eth_send(volatile void *packet, int length);	   
+int eth_rx(void);			
+void eth_halt(void);
+#endif /* EXPORT_ETH_FNS */
 
 void app_startup(char **);
 
