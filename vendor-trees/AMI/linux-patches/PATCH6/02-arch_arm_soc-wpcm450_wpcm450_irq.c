--- linux-2.6.15.4-cpu/arch/arm/soc-wpcm450/wpcm450_irq.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/arch/arm/soc-wpcm450/wpcm450_irq.c	2007-06-18 17:53:24.000000000 -0400
@@ -0,0 +1,97 @@
+/*
+ *  linux/arch/arm/mach-xxx/xxx-irq.c
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
+ *  WPCM450  SOC IRQ handling functions
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
+
+#include <linux/init.h>
+#include <linux/interrupt.h>
+#include <linux/list.h>
+
+#include <asm/mach/irq.h>
+#include <asm/irq.h>
+#include <asm/hardware.h>
+
+#include <asm/mach-types.h>
+
+static void
+wpcm450_irq_ack (unsigned int irq)
+{
+	if (irq > NR_IRQS)
+		return;
+	
+	/* Clear pending interrupt */
+	*((unsigned long *)(AIC_EOSCR)) = 0;
+
+	return;
+}
+
+static void
+wpcm450_irq_mask (unsigned int irq)
+{
+	if (irq > NR_IRQS)
+		return;
+
+	*((unsigned long *)(AIC_MDCR)) = (1 << irq);
+	return;
+
+}
+
+static void
+wpcm450_irq_unmask (unsigned int irq)
+{
+	if (irq > NR_IRQS)
+		return;
+
+	*((unsigned long *)(AIC_MECR)) = (1 << irq);
+	return;
+}
+
+
+struct irq_chip ext_chip =
+{
+	.name	= "HERMON",
+	.ack    = wpcm450_irq_ack,
+	.mask   = wpcm450_irq_mask,
+	.unmask = wpcm450_irq_unmask,
+};
+
+
+void __init
+wpcm450_init_irq(void)
+{
+	int i;
+
+	printk("WPCM450 SOC Interrupt Controller Enabled\n");
+
+	/* Mask all interrupts */
+	*((unsigned long *)(AIC_MDCR)) = 0xFFFFFFFF;
+
+	/* Clear pending interrupt */
+	*((unsigned long *)(AIC_EOSCR)) = 0;
+
+	for(i = 0; i < NR_IRQS; i++)
+	{
+		set_irq_chip(i, &ext_chip);
+		set_irq_handler(i, handle_level_irq);
+		set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
+	}
+	return;
+}
+
