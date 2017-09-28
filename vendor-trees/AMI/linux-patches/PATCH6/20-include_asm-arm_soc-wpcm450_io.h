--- linux-2.6.15.4-cpu/include/asm-arm/soc-wpcm450/io.h	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/include/asm-arm/soc-wpcm450/io.h	2007-06-18 15:46:01.000000000 -0400
@@ -0,0 +1,20 @@
+/*
+ * linux/include/asm-arm/arch-xxx/io.h
+ *
+ *  Copyright (C) 2001  MontaVista Software, Inc.
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License version 2 as
+ * published by the Free Software Foundation.
+ */
+
+#ifndef __ASM_ARM_ARCH_IO_H
+#define __ASM_ARM_ARCH_IO_H
+
+#define IO_SPACE_LIMIT      0xffffffff
+#define __io(a)				((void __iomem *)((a)))
+#define __mem_pci(a)		(a)
+
+extern void EarlyPrintk(char *);
+
+#endif
