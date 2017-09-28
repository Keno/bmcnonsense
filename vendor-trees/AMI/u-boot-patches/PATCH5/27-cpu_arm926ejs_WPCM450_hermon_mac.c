--- uboot-pristine/cpu/arm926ejs/WPCM450/hermon_mac.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/cpu/arm926ejs/WPCM450/hermon_mac.c	2009-05-14 10:30:26.000000000 -0700
@@ -13,7 +13,7 @@
 #define DEFAULT_CSR6   (CSR6_TR_VAL(0))
 #define DEFAULT_CSR0   (CSR0_BAR_RXH | CSR0_DSL_VAL(0) | CSR0_PBL_VAL(0) | CSR0_TAP_VAL(0))
 
-extern void DetectPhyAddr(char *devname);
+extern void DetectPhyAddr(char *devname); 
 extern unsigned char GetConfigPhyAddr(char *devname);
 extern int wpcm_miiphy_write (char *devname, unsigned char addr, unsigned char reg, unsigned short value);
 extern int wpcm_miiphy_read (char *devname, unsigned char addr, unsigned char reg,  unsigned short *value);
@@ -22,6 +22,10 @@
 extern int GetPhySpeed(char *devname);
 extern int GetPhyDuplex(char *devname);
 
+// joe --- begin
+#define MY_MAX_WPCM_ETH	2
+// joe --- end
+
 #define inl(addr) 			(*((volatile u32 *)(addr)))
 #define inw(addr)			(*((volatile u16 *)(addr)))
 #define outl(value, addr)  	(*((volatile u32 *)(addr)) = value)
@@ -694,7 +698,10 @@
 /*----------------------------------------------------------------------*/
 /*    				 U-Boot Interface Functions							*/
 /*----------------------------------------------------------------------*/
-static int eth_init_called[MAX_WPCM_ETH];
+// joe --- begin
+//static int eth_init_called[MAX_WPCM_ETH];
+static int eth_init_called[MY_MAX_WPCM_ETH];
+// joe --- end
 static
 int
 wpcm_eth_init(struct eth_device *dev,bd_t *bd)
@@ -760,7 +767,10 @@
 		return -1;
 	}
 
-	for(eth_num=0;eth_num < MAX_WPCM_ETH;eth_num++)
+// joe --- begin
+//	for(eth_num=0;eth_num < MAX_WPCM_ETH;eth_num++)
+	for(eth_num=0;eth_num < MY_MAX_WPCM_ETH;eth_num++)
+// joe --- end
 	{
 
 		/* Allocate Device Structure */
@@ -832,7 +842,7 @@
 		miiphy_register(dev->name,wpcm_miiphy_read,wpcm_miiphy_write);
 #endif
 		if (eth_num == 0)	// [SMC]
-      DetectPhyAddr(dev->name);
+      DetectPhyAddr(dev->name); 
 	}		/* for ethnum */
 
 	eth_initialized = 1;
