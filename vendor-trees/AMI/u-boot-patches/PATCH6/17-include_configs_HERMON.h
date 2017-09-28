--- uboot-pristine/include/configs/HERMON.h	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/include/configs/HERMON.h	2009-05-14 10:30:23.000000000 -0700
@@ -15,7 +15,7 @@
 #define 	UART_CLOCK 					24000000			/* Uart clock is 24 MHz */
 #define 	CONFIG_SERIAL0				1					/* Use UART0 as output on HERMON*/
 #define 	CONFIG_NR_DRAM_BANKS		1
-#define 	CFG_MAX_FLASH_BANKS			4
+#define 	CFG_MAX_FLASH_BANKS			1
 
 //#define 	DEBUG 			1								/* Enable Debug Message			*/
 #undef  	DEBUG
@@ -27,6 +27,7 @@
 #define 	CFG_FLASH_SECTOR_SIZE		CFG_PROJ_ERASE_BLOCK_SIZE	/* Flash Sector Size	*/
 
 #define 	CONFIG_ETHADDR				00:00:00:00:00:00	/* Ethernet MAC Address 		*/
+#define 	CONFIG_ETH1ADDR			00:00:00:00:00:00	/* Ethernet MAC Address 		*/
 #define 	CONFIG_BOARD_NAME			CFG_PROJ_BOARD		/* Board Name 			*/
 #define 	CONFIG_MENUKEY			 	27					/* ESC Key to enter menu		*/
 #define 	CONFIG_SILENT_CONSOLE		1
@@ -227,8 +228,8 @@
  */
 #define CFG_ENV_SIZE 			(CFG_ENV_SECT_SIZE - 4)
 
-#ifdef CFG_PROJ_ENV_START
-#define CFG_ENV_ADDR			CFG_PROJ_ENV_START
+#ifdef CFG_PROJ_UBOOT_ENV_START
+#define CFG_ENV_ADDR			CFG_PROJ_UBOOT_ENV_START
 #else
 #define CFG_ENV_ADDR			(CFG_FLASH_BASE+CFG_MONITOR_LEN)
 #endif
@@ -244,6 +245,7 @@
  * Board Specific Configs
  *----------------------------------------------------------
  */
+#define  CONFIG_BIGPHYSAREA "1025"
 
 /*----------------------------------------------------------
  * YAFU support
@@ -251,4 +253,15 @@
  */
 #define CFG_YAFU_SUPPORT 1
 
+/*----------------------------------------------------------
+ * NCSI support
+ *----------------------------------------------------------
+ */
+
+#define CONFIG_NCSI_SUPPORT 1
+#define CONFIG_NCSI_FORCE_DEFAULT_PORT 1
+
+#define CONFIG_EXPORT_ETH_FNS 1
+#define CONFIG_ETHPRIME "wpcm_eth1"
+
 #endif							/* __CONFIG_H */
