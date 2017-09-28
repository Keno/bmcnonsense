--- u-boot-1.1.4/include/flash.h	2005-12-16 11:39:27.000000000 -0500
+++ u-boot-1.1.6/include/flash.h	2006-11-02 09:15:01.000000000 -0500
@@ -45,6 +45,7 @@
 	ushort	vendor;			/* the primary vendor id		*/
 	ushort	cmd_reset;		/* Vendor specific reset command	*/
 	ushort	interface;		/* used for x8/x16 adjustments		*/
+	ushort	legacy_unlock;		/* support Intel legacy (un)locking	*/
 #endif
 } flash_info_t;
 
@@ -208,6 +209,9 @@
 #define AMD_ID_GL064M_3 0x22012201	/* 3rd ID word for S29GL064M-R6 */
 #define AMD_ID_GL064MT_2 0x22102210	/* 2nd ID word for S29GL064M-R3 (top boot sector) */
 #define AMD_ID_GL064MT_3 0x22012201	/* 3rd ID word for S29GL064M-R3 (top boot sector) */
+#define AMD_ID_GL128N_2	0x22212221	/* 2nd ID word for S29GL128N */
+#define AMD_ID_GL128N_3	0x22012201	/* 3rd ID word for S29GL128N */
+
 
 #define AMD_ID_LV320B_2 0x221A221A	/* 2d ID word for AM29LV320MB at 0x38 */
 #define AMD_ID_LV320B_3 0x22002200	/* 3d ID word for AM29LV320MB at 0x3c */
@@ -233,6 +237,7 @@
 #define SST_ID_xF3202	0x235A235A	/* 39xF3202 ID (32M =	2M x 16 )	*/
 #define SST_ID_xF6401	0x236B236B	/* 39xF6401 ID (64M =	4M x 16 )	*/
 #define SST_ID_xF6402	0x236A236A	/* 39xF6402 ID (64M =	4M x 16 )	*/
+#define SST_ID_xF020	0xBFD6BFD6	/* 39xF020 ID (256KB = 2Mbit x 8)	*/
 #define SST_ID_xF040	0xBFD7BFD7	/* 39xF040 ID (512KB = 4Mbit x 8)	*/
 
 #define STM_ID_F040B	0xE2		/* M29F040B ID ( 4M = 512K x 8	)	*/
@@ -242,6 +247,7 @@
 #define STM_ID_29W320DT 0x22CA22CA	/* M29W320DT ID (32 M, top boot sector) */
 #define STM_ID_29W320DB 0x22CB22CB	/* M29W320DB ID (32 M, bottom boot sect)	*/
 #define STM_ID_29W040B	0x00E300E3	/* M29W040B ID (4M = 512K x 8)	*/
+#define FLASH_PSD4256GV 0x00E9		/* PSD4256 Flash and CPLD combination	*/
 
 #define INTEL_ID_28F016S    0x66a066a0	/* 28F016S[VS] ID (16M = 512k x 16)	*/
 #define INTEL_ID_28F800B3T  0x88928892	/*  8M = 512K x 16 top boot sector	*/
@@ -269,10 +275,17 @@
 #define INTEL_ID_28F320J3A  0x00160016	/*  32M = 128K x  32	*/
 #define INTEL_ID_28F640J3A  0x00170017	/*  64M = 128K x  64	*/
 #define INTEL_ID_28F128J3A  0x00180018	/* 128M = 128K x 128	*/
+#define INTEL_ID_28F256J3A  0x001D001D	/* 256M = 128K x 256	*/
 #define INTEL_ID_28F256L18T 0x880D880D	/* 256M = 128K x 255 + 32k x 4 */
 #define INTEL_ID_28F64K3    0x88018801	/*  64M =  32K x 255 + 32k x 4 */
 #define INTEL_ID_28F128K3   0x88028802	/* 128M =  64K x 255 + 32k x 4 */
 #define INTEL_ID_28F256K3   0x88038803	/* 256M = 128K x 255 + 32k x 4 */
+#define INTEL_ID_28F64P30T  0x88178817	/*  64M =  32K x 255 + 32k x 4 */
+#define INTEL_ID_28F64P30B  0x881A881A	/*  64M =  32K x 255 + 32k x 4 */
+#define INTEL_ID_28F128P30T 0x88188818	/* 128M =  64K x 255 + 32k x 4 */
+#define INTEL_ID_28F128P30B 0x881B881B	/* 128M =  64K x 255 + 32k x 4 */
+#define INTEL_ID_28F256P30T 0x88198819	/* 256M = 128K x 255 + 32k x 4 */
+#define INTEL_ID_28F256P30B 0x881C881C	/* 256M = 128K x 255 + 32k x 4 */
 
 #define INTEL_ID_28F160S3   0x00D000D0	/*  16M = 512K x  32 (64kB x 32)	*/
 #define INTEL_ID_28F320S3   0x00D400D4	/*  32M = 512K x  64 (64kB x 64)	*/
@@ -334,6 +347,7 @@
 #define FLASH_SST160A	0x0046		/* SST 39xF160A ID ( 16M =   1M x 16 )	*/
 #define FLASH_SST320	0x0048		/* SST 39xF160A ID ( 16M =   1M x 16 )	*/
 #define FLASH_SST640	0x004A		/* SST 39xF160A ID ( 16M =   1M x 16 )	*/
+#define FLASH_SST020	0x0024		/* SST 39xF020 ID (256KB = 2Mbit x 8 )	*/
 #define FLASH_SST040	0x000E		/* SST 39xF040 ID (512KB = 4Mbit x 8 )	*/
 
 #define FLASH_STM800AB	0x0051		/* STM M29WF800AB  (  8M = 512K x 16 )	*/
@@ -401,10 +415,12 @@
 #define FLASH_28F320J3A 0x00C0		/* INTEL 28F320J3A ( 32M = 128K x  32)	*/
 #define FLASH_28F640J3A 0x00C2		/* INTEL 28F640J3A ( 64M = 128K x  64)	*/
 #define FLASH_28F128J3A 0x00C4		/* INTEL 28F128J3A (128M = 128K x 128)	*/
+#define FLASH_28F256J3A 0x00C6		/* INTEL 28F256J3A (256M = 128K x 256)	*/
 
 #define FLASH_FUJLV650	0x00D0		/* Fujitsu MBM 29LV650UE/651UE		*/
 #define FLASH_MT28S4M16LC 0x00E1	/* Micron MT28S4M16LC			*/
 #define FLASH_S29GL064M 0x00F0		/* Spansion S29GL064M-R6		*/
+#define FLASH_S29GL128N 0x00F1		/* Spansion S29GL128N			*/
 
 #define FLASH_UNKNOWN	0xFFFF		/* unknown flash type			*/
 
