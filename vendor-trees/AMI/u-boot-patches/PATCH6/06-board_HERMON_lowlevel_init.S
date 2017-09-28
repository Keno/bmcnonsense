--- u-boot-1.1.4-wpcm/board/HERMON/lowlevel_init.S	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-good/board/HERMON/lowlevel_init.S	2007-06-28 00:38:51.000000000 -0400
@@ -0,0 +1,58 @@
+/* Low level initialization for the board */
+/* Copyright (c) American Megatrends, Inc */
+
+#include <config.h>
+#include <version.h>
+#include "wpcm450_hw.h"
+
+.extern hermon_init
+/*----------------------------- Setting up the memory and Flash ----------------------*/
+.globl lowlevel_init
+lowlevel_init:
+
+	/* Save Return PC */
+   	mov 	r10,lr
+
+	/* Memory and Clock Initialization */
+	bl			hermon_init
+
+
+/*	bl		flash_config  */
+
+	/* Return Back to caller */
+ 	mov		lr, r10
+	mov		pc, lr
+
+
+/*-----------------------------------------------------------------------------------*/
+/*							FLASH Configuration										 */
+/*-----------------------------------------------------------------------------------*/
+/* IMPORTANT: r10 should not be destroyed in this function*/
+
+flash_config:
+
+	/* Save Return PC */
+	mov		r9,lr
+
+	/* Return Back to caller */
+	mov		lr,r9
+	mov		pc, lr
+
+/* No remap function in WPCM450 */
+/*-----------------------------------------------------------------------------------*/
+/*					 Remap Memory -Swap Address of ROM and SDRAM					 */
+/*-----------------------------------------------------------------------------------*/
+.globl remap_memory
+remap_memory:
+
+	/* Save Return PC */
+	mov 	r9,lr
+
+	/* Add your SOC Specific Remap if any */
+
+	/* Return Back to caller */
+    mov 	lr,r9
+	mov		pc, lr
+
+
+
