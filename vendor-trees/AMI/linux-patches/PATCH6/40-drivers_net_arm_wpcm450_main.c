--- linux-pristine/drivers/net/arm/wpcm450_main.c	2009-05-14 14:10:28.000000000 -0700
+++ linux-changed/drivers/net/arm/wpcm450_main.c	2009-05-14 13:41:09.000000000 -0700
@@ -23,6 +23,8 @@
 
 extern unsigned long wpcm_mac_io[MAX_WPCM_MAC]; 
 extern unsigned int  wpcm_mac_irq[MAX_WPCM_MAC];
+extern int isMACInitialized;
+extern unsigned long init_wpcm_mac_eth(void);
 
 void
 FindMACAddress(int unit, char *mac_addr)
@@ -62,6 +64,8 @@
 	sprintf(dev->name, "eth%d", unit);
 	netdev_boot_setup_check(dev);
 
+	if(!isMACInitialized) init_wpcm_mac_eth();
+
 	dev->base_addr = wpcm_mac_io[unit];
 	dev->irq = wpcm_mac_irq[unit];
 	dev->dma = 0;
