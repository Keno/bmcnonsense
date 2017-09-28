--- u-boot-1.1.4-cpu/include/configs/HERMON.h	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/include/configs/HERMON.h	2007-11-26 10:47:12.000000000 -0500
@@ -0,0 +1,254 @@
+/*
+ * (C) Copyright 2007 American Megaterends
+ * Configuration settings for the Hermon Eval Board
+ */
+
+#ifndef __CONFIG_H
+#define __CONFIG_H
+
+/*------------------------------------------------------------------------------------------*/
+/*--------------------------------- Configurable Setting -----------------------------------*/
+/*------------------------------------------------------------------------------------------*/
+#define 	CFG_SDRAM_PRE_REMAP_ADDR	0x00000000			/* SDRAM Location before Remap  */
+#define 	CFG_MONITOR_LEN		   		0x00040000			/* Size of U-Boot (without Env) */
+#define    	SYSTEM_CLOCK 				160000000      		/* System CPU Clock - 160 MHz   */
+#define 	UART_CLOCK 					24000000			/* Uart clock is 24 MHz */
+#define 	CONFIG_SERIAL0				1					/* Use UART0 as output on HERMON*/
+#define 	CONFIG_NR_DRAM_BANKS		1
+#define 	CFG_MAX_FLASH_BANKS			4
+
+//#define 	DEBUG 			1								/* Enable Debug Message			*/
+#undef  	DEBUG
+
+#define 	CFG_ROM_REMAP_BASE 			CFG_PROJ_FLASH_START	/* Location of ROM after Remap  */
+#define 	CFG_FLASH_BASE          	(CFG_ROM_REMAP_BASE)
+#define 	CFG_SDRAM_LEN				CFG_PROJ_MEMORY_SIZE 	/* SDRAM Size */
+#define 	CFG_FLASH_SIZE				CFG_PROJ_FLASH_SIZE		/* Flash Size*/
+#define 	CFG_FLASH_SECTOR_SIZE		CFG_PROJ_ERASE_BLOCK_SIZE	/* Flash Sector Size	*/
+
+#define 	CONFIG_ETHADDR				00:00:00:00:00:00	/* Ethernet MAC Address 		*/
+#define 	CONFIG_BOARD_NAME			CFG_PROJ_BOARD		/* Board Name 			*/
+#define 	CONFIG_MENUKEY			 	27					/* ESC Key to enter menu		*/
+#define 	CONFIG_SILENT_CONSOLE		1
+
+
+/* Various clocks derived from . Board specific */
+#define 	SYS_CLK                	SYSTEM_CLOCK
+#define 	AHB_CLK                 (SYS_CLK/2)
+#define 	P_CLK					(AHB_CLK/2)
+
+
+
+/*---------------------------------------------------------------
+ *			Board and Processor Identification
+ *---------------------------------------------------------------
+ */
+#define CONFIG_ARM926EJS	1	/* This is an ARM926EJS Core */
+#define CONFIG_WPCM450		1	/* WPCM450 SOC */
+
+
+/*---------------------------------------------------------------
+ *  			Default Memory Test Range
+ *---------------------------------------------------------------
+ */
+#define CFG_MEMTEST_START  0x300000 		/* After U-Boot		*/
+#define CFG_MEMTEST_END    CFG_SDRAM_LEN
+
+/*---------------------------------------------------------------
+ *				   Timer Clock Frequency
+ *---------------------------------------------------------------
+ */
+#define CFG_HZ	P_CLK
+
+
+/*---------------------------------------------------------------
+ *	             Serial Port Configuration
+ *---------------------------------------------------------------
+ */
+#define CFG_BAUDRATE_TABLE      { 9600, 19200, 38400, 57600, 115200 }
+
+
+/*---------------------------------------------------------------
+ *	           Required DHCP Vendor Extensions
+ *---------------------------------------------------------------
+ */
+#define CONFIG_BOOTP_MASK (CONFIG_BOOTP_SUBNETMASK | \
+						   CONFIG_BOOTP_GATEWAY    | \
+						   CONFIG_BOOTP_HOSTNAME)
+
+/*---------------------------------------------------------------
+ *	              ARM Linux Argument TAGS (ATAG)
+ *---------------------------------------------------------------
+ */
+#define CONFIG_CMDLINE_TAG	 		1
+#define CONFIG_SETUP_MEMORY_TAGS 	1
+#define CONFIG_INITRD_TAG	 		1
+#define CONFIG_ENETADDR_TAG			1
+//The following needs functions to be written
+//#define CONFIG_SERIAL_TAG				1		// get_board_serial()
+//#define CONFIG_REVISION_TAG			1		// get_board_rev()
+/*---------------------------------------------------------------
+ *	               Supported Commands
+ *---------------------------------------------------------------
+ */
+#define CONFIG_COMMANDS     								\
+	(CFG_CMD_DHCP|CFG_CMD_NET| CFG_CMD_PING|CFG_CMD_MII|	\
+	CFG_CMD_MEMORY|CFG_CMD_CONSOLE|CFG_CMD_FLASH|			\
+	CFG_CMD_AUTOSCRIPT|CFG_CMD_RUN|CFG_CMD_BDI|				\
+	CFG_CMD_ELF|CFG_CMD_IMLS|CFG_CMD_IMI|					\
+	CFG_CMD_RUN|CFG_CMD_SAVEENV |CFG_CMD_ASKENV |			\
+	CFG_CMD_CACHE | CFG_CMD_ENV |							\
+	CFG_CMD_IRQ | CFG_CMD_MISC | CFG_CMD_LOADB)
+
+/* CFG_CMD_DATE  | CFG_CMD_SNTP */
+
+
+
+
+
+/* IMPORTANT: This must be included AFTER the definition of
+               CONFIG_COMMANDS (if any) */
+#include <cmd_confdefs.h>
+
+/*---------------------------------------------------------------
+ * 				 Default Environment Values
+ *--------------------------------------------------------------*/
+#define CONFIG_BAUDRATE        	38400
+#define CONFIG_BOOTDELAY        1
+#define CFG_AUTOLOAD			"no"
+#define CONFIG_LOADS_ECHO		1
+#define CFG_LOADS_BAUD_CHANGE	1
+#define CFG_LOAD_ADDR			0x00100000
+#define CONFIG_BOOTCOMMAND		"bootfmh"
+//#define CONFIG_EXTRA_ENV_SETTINGS "menucmd=bootmenu\0silent=1"
+#define CONFIG_EXTRA_ENV_SETTINGS "menucmd=bootmenu"
+
+/*---------------------------------------------------------------
+ * 				Network Configuration
+ *--------------------------------------------------------------*/
+#define CONFIG_NET_MULTI			1
+#define CONFIG_MII					1
+#define CONFIG_PHY_ADDR_WPCM_ETH_1 0x1F
+#define CONFIG_PHY_ADDR_WPCM_ETH_2 0x0F
+#define CONFIG_OVERWRITE_ETHADDR_ONCE	1
+#define CONFIG_NET_RETRY_COUNT		25
+#define CONFIG_PHY_CMD_DELAY		1
+#define CONFIG_HAS_ETH1
+
+/*---------------------------------------------------------------
+ * Miscellaneous configurable options
+ *---------------------------------------------------------------*/
+#define CONFIG_BOARD_PRE_INIT	1
+
+#define CFG_LONGHELP			0					/* No Long Help    				*/
+#define CFG_PROMPT				CONFIG_BOARD_NAME">"	/* Monitor Command Prompt		*/
+#define CFG_PBSIZE				(CFG_CBSIZE+sizeof(CFG_PROMPT)+16)
+													/* Print Buffer Size 			*/
+#define CFG_MAXARGS				16					/* Max number of command args	*/
+#define CFG_BARGSIZE			CFG_CBSIZE			/* Boot Argument Buffer Size 	*/
+#define CFG_EXTBDINFO			1					/* Extended Board Info 			*/
+//#define CFG_ALT_MEMTEST		1					/* Allow Extended Memory Test 	*/
+#define CFG_DEVICE_NULLDEV		1					/* Enable Null Device 			*/
+#define CFG_CONSOLE_INFO_QUIET 	1
+#define CFG_DIRECT_FLASH_TFTP	1					/* Allow TFTP download to flash	*/
+
+/* Console I/O Buffer Size */
+#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
+#define CFG_CBSIZE		1024
+#else
+#define CFG_CBSIZE		256
+#endif
+
+#define CONFIG_USE_IRQ			1			/* U-Boot can use IRQ */
+
+/*-----------------------------------------------------------------
+ * Stack sizes
+ * The stack sizes are set up in start.S using the settings below
+ *-----------------------------------------------------------------
+ */
+#define CONFIG_STACKSIZE		(128*1024)	/* regular stack */
+#ifdef CONFIG_USE_IRQ
+#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
+#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
+#endif
+
+/*----------------------------------------------------------------
+ *             Physical Memory Map of SDRAM and Flash
+ *----------------------------------------------------------------
+ */
+#define CFG_SDRAM_BASE 			CFG_PROJ_MEMORY_START
+/* Cannot use SPD to detect SDRAM Information*/
+#undef CONFIG_SPD_EEPROM
+
+#ifdef CFG_PROJ_FLASH_TYPE_NOR
+	/* Use CFI Driver to detect the Flash Information */
+	#define CFG_FLASH_CFI    			1
+	#define CFG_FLASH_CFI_DRIVER    	1
+	#define CFG_ENV_IS_IN_FLASH 
+#endif
+
+#ifdef CFG_PROJ_FLASH_TYPE_SPI
+	/* Use SPI Driver to detect the Flash Information */
+	#define CONFIG_HAS_SPI				1
+	#define CFG_FLASH_SPI_DRIVER    	1
+	#define CFG_ENV_IS_IN_SPI			1
+#endif
+
+//Note: One of the following is goofing up flash writes to Spansion
+//      So till it is fixed don't use.
+//#define CFG_FLASH_PROTECTION	1		
+//#define CFG_FLASH_USE_BUFFER_WRITE 1
+
+
+/* Max number of sectors on one chip*/
+#define CFG_MAX_FLASH_SECT      512
+
+/*--------------------------------------------------------------
+ *					Monitor Information
+ *--------------------------------------------------------------
+*/
+#define CFG_MONITOR_BASE	   CFG_FLASH_BASE
+
+
+/*---------------------------------------------------------------
+ *                    Environment organization
+ *---------------------------------------------------------------
+ */
+//#define CFG_ENV_IS_NOWHERE		1
+
+#define CONFIG_ENV_OVERWRITE 	1
+#define CFG_ENV_SECT_SIZE	 	CFG_FLASH_SECTOR_SIZE
+
+/*
+ * Adjust Total Size of Environment so that it fits in a complete sector
+ * along with the variable "env_size". Actual Usable environment Size is
+ * CFG_ENV_SECT_SIZE - 2 * sizeof(unsigned long). The unsigned longs are
+ * "env_size" and crc32
+ */
+#define CFG_ENV_SIZE 			(CFG_ENV_SECT_SIZE - 4)
+
+#ifdef CFG_PROJ_ENV_START
+#define CFG_ENV_ADDR			CFG_PROJ_ENV_START
+#else
+#define CFG_ENV_ADDR			(CFG_FLASH_BASE+CFG_MONITOR_LEN)
+#endif
+
+/*----------------------------------------------------------------
+ *               Size of malloc() pool
+ *---------------------------------------------------------------
+ */
+#define CFG_MALLOC_LEN	  ((CFG_ENV_SIZE+4) + (128 *1024))
+#define CFG_GBL_DATA_SIZE	128	 /* Size for initial data */
+
+/*----------------------------------------------------------
+ * Board Specific Configs
+ *----------------------------------------------------------
+ */
+
+/*----------------------------------------------------------
+ * YAFU support
+ *----------------------------------------------------------
+ */
+#define CFG_YAFU_SUPPORT 1
+
+#endif							/* __CONFIG_H */
