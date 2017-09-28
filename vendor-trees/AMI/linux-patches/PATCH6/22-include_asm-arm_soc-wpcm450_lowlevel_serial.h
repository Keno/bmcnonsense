--- linux-2.6.15.4-cpu/include/asm-arm/soc-wpcm450/lowlevel_serial.h	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/include/asm-arm/soc-wpcm450/lowlevel_serial.h	2007-06-20 17:48:27.000000000 -0400
@@ -0,0 +1,29 @@
+/*
+ * linux/include/asm-arm/arch-xxx/lowlevel_serial.h
+ *
+ * Copyright (C) 2005 American Megatrends Inc.
+ */
+
+/* Used during bootup till the serial driver is initialized */
+/* Included in debug_macro.S and uncompress.h */
+#include  "asm/arch/platform.h"
+
+#define UART_BASE  		WPCM_UART0_BASE			/* Before MMU */
+#define UART_MMU_BASE 	WPCM_UART0_VA_BASE		/* After MMU (actually after paging init)*/
+#define BAUD_RATE  38400
+
+
+/* Serial Registers */
+#define SERIAL_THR	0x00
+#define SERIAL_RBR	0x00
+#define SERIAL_IER	0x04
+#define SERIAL_IIR  0x08
+#define SERIAL_FCR  0x08
+#define SERIAL_LCR  0x0C
+#define SERIAL_MCR	0x10
+#define SERIAL_LSR  0x14
+#define SERIAL_MSR	0x18
+#define SERIAL_SPR	0x1C
+
+
+
