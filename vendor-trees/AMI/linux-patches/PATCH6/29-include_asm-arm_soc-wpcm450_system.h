--- linux-old/include/asm-arm/soc-wpcm450/system.h	1969-12-31 17:00:00.000000000 -0700
+++ linux-new/include/asm-arm/soc-wpcm450/system.h	2008-08-02 18:37:49.000000000 -0500
@@ -0,0 +1,51 @@
+/*
+ *  linux/include/asm-arm/arch-xxx/system.h
+ *
+ *  Copyright (C) 1999 ARM Limited
+ *  Copyright (C) 2000 Deep Blue Solutions Ltd
+ *	Copyright (C) 2005 American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+#ifndef __ASM_ARCH_SYSTEM_H
+#define __ASM_ARCH_SYSTEM_H
+
+#include <asm/arch/platform.h>
+
+/* Some Debug Functions */
+void CheckPrintk(char *);
+
+static inline void arch_idle(void)
+{
+	/*
+	 * This should do all the clock switching
+	 * and wait for interrupt tricks
+	 */
+	cpu_do_idle();
+}
+
+static inline void arch_reset(char mode)
+{
+	printk("arch_reset() called with mode %d \n",mode);
+
+	if((*(volatile unsigned char*)(GCR_PDID + 0x03)) < 4 )
+	{
+		*(volatile unsigned long *)TIMER_WTCR = 0x01 ; // reset watch dog timer
+	}
+	*(volatile unsigned long *)TIMER_WTCR = 0x482 ; // enable watchdog timer & enable reset
+	while(1);
+}
+
+#endif
