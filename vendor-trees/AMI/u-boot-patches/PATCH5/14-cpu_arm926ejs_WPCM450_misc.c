--- u-boot-1.1.4-cpu/cpu/arm926ejs/WPCM450/misc.c	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/WPCM450/misc.c	2007-06-27 18:14:17.000000000 -0400
@@ -0,0 +1,21 @@
+#include <common.h>
+#include <command.h>
+#include <linux/types.h>
+#include "macros.h"
+#include "soc_hw.h"
+
+void
+soc_init(void)
+{
+
+	/* Do any WPCM450 SOC Initialization here */
+	// joe --- begin 
+	// hw require this bit to 0 to select GPIO81 because hw desgin this pin as GPIO
+	// and must not affect ICH9
+	CLEAR_BIT(*(volatile unsigned long *)GCR_MFSEL2, 17);	
+	// joe --- end
+	/* Change CPU freq to max if possible */
+
+	return;
+}
+
