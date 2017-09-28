--- linux-pristine/drivers/net/arm/wpcm450_mac.h	2009-05-14 14:10:28.000000000 -0700
+++ linux-changed/drivers/net/arm/wpcm450_mac.h	2009-05-14 13:41:09.000000000 -0700
@@ -5,7 +5,7 @@
 #ifndef _WPCM450_MAC_H_
 #define _WPCM450_MAC_H_
 
-#if defined(CFG_PROJ_NETWORK_ETH0) || defined(CFG_PROJ_NETWORK_ETH1)
+#if defined(CFG_PROJ_NETWORK_ETH0) || defined(CFG_PROJ_NETWORK_ETH1) || defined(CFG_PROJ_NETWORK_BOOTSELECT)
 #define MAX_WPCM_MAC  	 1
 #elif defined(CFG_PROJ_NETWORK_BOTH)
 #define MAX_WPCM_MAC  	 2
@@ -224,12 +224,14 @@
 #define MGSTA_SQE		(1<<10)
 #define MGSTA_TXHA		(1<<11)
 
+extern int isMACInitialized;
+unsigned long init_wpcm_mac_eth(void);
 
 #define wpcm_WriteReg(reg,val,which) \
-  (*((volatile unsigned int *)(wpcm_mac_io[(which)]+(reg)))=(val))
+(!isMACInitialized && init_wpcm_mac_eth())?(*((volatile unsigned int *)(wpcm_mac_io[(which)]+(reg)))=(val)):(*((volatile unsigned int *)(wpcm_mac_io[(which)]+(reg)))=(val))
 
 #define wpcm_ReadReg(reg,which)      \
- (*((volatile unsigned int *)(wpcm_mac_io[(which)]+(reg))))
+  (!isMACInitialized && init_wpcm_mac_eth())?(*((volatile unsigned int *)(wpcm_mac_io[(which)]+(reg)))): (*((volatile unsigned int *)(wpcm_mac_io[(which)]+(reg))))
 
 #define wpcm_WriteCam0(which,x,lsw,msw) \
 		wpcm_WriteReg(CAM0L+(x)*CAM_ENTRY_SIZE,lsw,which);\
