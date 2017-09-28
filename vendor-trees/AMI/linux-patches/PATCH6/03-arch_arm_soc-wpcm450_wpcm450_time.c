--- linux-2.6.15.4-cpu/arch/arm/soc-wpcm450/wpcm450_time.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/arch/arm/soc-wpcm450/wpcm450_time.c	2007-06-26 17:10:39.000000000 -0400
@@ -0,0 +1,137 @@
+/*
+ *  linux/arch/arm/mach-xxx/xxx-time.c
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
+ *  WPCM450 SOC timer functions
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
+#include <linux/kernel.h>
+#include <linux/interrupt.h>
+#include <linux/time.h>
+#include <linux/init.h>
+#include <linux/timex.h>
+
+#include <asm/hardware.h>
+#include <asm/io.h>
+#include <asm/irq.h>
+#include <asm/uaccess.h>
+#include <asm/mach/irq.h>
+#include <asm/mach/time.h>
+
+
+static inline unsigned long get_elapsed(void)
+{
+	unsigned long counter;
+	counter = *(volatile unsigned long *)(TIMER_TDR0);
+	return LATCH-counter;
+}
+
+/* Returns number of us since last clock interrupt. Note the
+   interrupts will be disabled before calling this function */
+static unsigned long
+wpcm450_gettimeoffset(void)
+{
+	unsigned long elapsed, usec;
+	u32 tisr1, tisr2;
+
+	/*
+	 * If an interrupt was pending before we read the timer,
+	 * we've already wrapped.  Factor this into the time.
+	 * If an interrupt was pending after we read the timer,
+	 * it may have wrapped between checking the interrupt
+	 * status and reading the timer.  Re-read the timer to
+	 * be sure its value is after the wrap.
+	 */
+
+	tisr1 =	*(volatile unsigned long*)(TIMER_TISR);
+	elapsed = get_elapsed();
+	tisr2 =	*(volatile unsigned long*)(TIMER_TISR);
+
+	if(tisr1 & 1)
+		elapsed += LATCH;
+	else if (tisr2 & 1)
+		elapsed = LATCH + get_elapsed();
+
+	/* Now convert them to usec */
+	usec = (unsigned long)(elapsed / (CLOCK_TICK_RATE/1000000));
+
+	return usec;
+}
+
+//static int counter=0;
+
+static irqreturn_t
+wpcm450_timer_interrupt(int irq, void *dev_id)
+{
+	write_seqlock(&xtime_lock);
+/*
+	if ((counter++)== HZ)
+	{
+		counter=0;
+		printk("X");
+	}
+*/
+
+	/* Clear Interrupt */
+    	*((volatile unsigned long *)(TIMER_TISR)) = 1;
+
+	timer_tick();
+
+	write_sequnlock(&xtime_lock);
+
+	return IRQ_HANDLED;
+}
+
+static struct irqaction wpcm450_timer_irq =
+{
+	.name		= "WPCM450 Timer Tick",
+	.handler	= wpcm450_timer_interrupt,
+	.flags		= IRQF_DISABLED | IRQF_TIMER | IRQF_IRQPOLL,
+};
+
+static void __init
+wpcm450_timer_init(void)
+{
+
+	#define PRESCALE    4
+
+	/* Disable timer and interrups */
+	*((volatile unsigned long *)(TIMER_TCSR0)) = 0;
+	*((volatile unsigned long *)(TIMER_TCSR1)) = 0;
+	*((volatile unsigned long *)(TIMER_TCSR2)) = 0;
+	*((volatile unsigned long *)(TIMER_TCSR3)) = 0;
+
+	/* Load Counter Value */
+	*((volatile unsigned long *)(TIMER_TICR0)) = (LATCH/PRESCALE);
+
+	/* Setup Timer Interrupt routine */
+	setup_irq(IRQ_TIMER_0, &wpcm450_timer_irq);
+
+	/* Enable Timer and Intr */
+	*((volatile unsigned long *)(TIMER_TCSR0)) = 0xe8000000| (PRESCALE-1);
+
+	printk("WPCM450 SOC Timer Enabled\n");
+
+	return;
+}
+
+struct sys_timer wpcm450_timer =
+{
+	.init		= &wpcm450_timer_init,
+	.offset		= wpcm450_gettimeoffset,
+};
