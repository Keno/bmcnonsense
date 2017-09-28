--- linux-2.6.15.4-cpu/include/asm-arm/soc-wpcm450/entry-macro.S	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/include/asm-arm/soc-wpcm450/entry-macro.S	2007-06-18 23:25:58.000000000 -0400
@@ -0,0 +1,32 @@
+/*
+ * include/asm-arm/arch-xxx/entry-macro.S
+ *
+ * Low-level IRQ helper macros for WPCM450  SOC  platforms
+ *
+ * This file is licensed under  the terms of the GNU General Public
+ * License version 2. This program is licensed "as is" without any
+ * warranty of any kind, whether express or implied.
+ */
+
+#include <asm/arch/platform.h>
+
+ 		.macro	disable_fiq
+		.endm
+
+		 .macro get_irqnr_preamble, base, tmp
+		.endm
+
+		 .macro  arch_ret_to_user, tmp1, tmp2
+ 		.endm 
+
+		.macro get_fiqnr_and_base, irqnr, irqstat, base, tmp
+		.endm
+
+		.macro get_irqnr_and_base, irqnr, irqstat, base, tmp
+		ldr		\base, =(AIC_IPER)
+		ldr		\irqnr, [\base]
+		mov		\irqnr,\irqnr, lsr #2
+		teq		\irqnr, #0
+		.endm
+
+
