--- linux-2.6.15.4-cpu/include/asm-arm/soc-wpcm450/debug-macro.S	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/include/asm-arm/soc-wpcm450/debug-macro.S	2007-06-25 14:07:01.000000000 -0400
@@ -0,0 +1,50 @@
+/* linux/include/asm-arm/arch-xxx/debug-macro.S
+ *
+ * Debugging macro include header
+ *
+ * Copyright (C) 1994-1999 Russell King
+ *
+ * Ported for WPCM450 (Base code taken from arch-integrator)
+ * Copyright (C) 2005 American Megatrends Inc.
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License version 2 as
+ * published by the Free Software Foundation.
+ *
+*/
+
+#include <asm/arch/lowlevel_serial.h>
+
+uart_base:
+		.word  UART_BASE
+uart_mmu_base:
+		.word   UART_MMU_BASE
+
+		.macro	addruart,rx
+		mrc		p15,0,\rx,c1,c0
+		tst		\rx, #1		@ MMU enabled?
+
+		beq		1001f
+		ldr		\rx, uart_mmu_base 	@ virtual address
+		b		1002f
+1001:
+		ldr		\rx, uart_base	 	@  physical address
+1002:
+		.endm
+
+		.macro	senduart,rd,rx
+		strb	\rd, [\rx, #SERIAL_THR]
+		.endm
+
+		.macro	waituart,rd,rx
+1001:	ldr		\rd, [\rx, #SERIAL_LSR]
+		tst		\rd, #1 << 5			@ SERIAL_LSR_THRE
+		beq		1001b
+		.endm
+
+/* I dont know what busyuart should do. So I used the same as waituart */
+		.macro	busyuart,rd,rx
+1001:	ldr		\rd, [\rx, #SERIAL_LSR]
+		tst		\rd, #1 << 5			@ SERIAL_LSR_THRE
+		beq		1001b
+		.endm
