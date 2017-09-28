--- u-boot-1.1.4-cpu/cpu/arm926ejs/WPCM450/hermon_timer.c	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/WPCM450/hermon_timer.c	2007-06-27 18:50:05.000000000 -0400
@@ -0,0 +1,162 @@
+/*
+ * (C) Copyright 2006
+ * American Megatrends Inc.
+ *
+ * Timer functions for the WPCM450 SOC
+ *
+ * See file CREDITS for list of people who contributed to this
+ * project.
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ */
+
+#include <common.h>
+#include <arm926ejs.h>
+#include <asm/proc-armv/ptrace.h>
+#include "soc_hw.h"
+
+#define READ_TIMER (*(volatile ulong *)(TIMER_TDR0));
+#define TIMER_LOAD_VAL 24000000
+#define TIMER_PRESCALE_VALUE	4
+
+static ulong timestamp;
+static ulong lastdec;
+
+int timer_init (void)
+{
+
+	/* Load the counter and start timer */
+	*(volatile ulong *)(TIMER_TICR0) = TIMER_LOAD_VAL;
+
+	*(volatile ulong *)(TIMER_TCSR0) = TIMER_CEN | TIMER_PERIODICAL;
+
+	/* init the timestamp and lastdec value */
+	reset_timer_masked();
+
+	return (0);
+}
+
+/*
+ * timer without interrupts
+ */
+
+void reset_timer (void)
+{
+	reset_timer_masked ();
+}
+
+ulong get_timer (ulong base)
+{
+	return get_timer_masked () - base;
+}
+
+void set_timer (ulong t)
+{
+	timestamp = t;
+}
+
+/* delay x useconds AND perserve advance timstamp value */
+void udelay (unsigned long usec)
+{
+	udelay_masked(usec);
+}
+
+void reset_timer_masked (void)
+{
+	/* reset time */
+	*(volatile ulong *)(TIMER_TCSR0) &= ~TIMER_CEN; // can only be read when stopped
+	lastdec = READ_TIMER;  /* capure current decrementer value time */
+	*(volatile ulong *)(TIMER_TCSR0) = TIMER_CEN | TIMER_PERIODICAL ;
+	timestamp = 0;	       /* start "advancing" time stamp from 0 */
+}
+
+ulong get_timer_masked (void)
+{
+	*(volatile ulong *)(TIMER_TCSR0) &= ~TIMER_CEN; // can only be read when stopped
+	ulong now = READ_TIMER;		/* current tick value */
+	*(volatile ulong *)(TIMER_TCSR0) = TIMER_CEN | TIMER_PERIODICAL ;
+
+	if (lastdec >= now) {		/* normal mode (non roll) */
+		/* normal mode */
+		timestamp += lastdec - now; /* move stamp fordward with absoulte diff ticks */
+	} else {			/* we have overflow of the count down timer */
+		/* nts = ts + ld + (TLV - now)
+		 * ts=old stamp, ld=time that passed before passing through -1
+		 * (TLV-now) amount of time after passing though -1
+		 * nts = new "advancing time stamp"...it could also roll and cause problems.
+		 */
+		timestamp += lastdec + TIMER_LOAD_VAL - now;
+	}
+	lastdec = now;
+
+	return timestamp;
+}
+
+/* waits specified delay value and resets timestamp */
+void udelay_masked (unsigned long usec)
+{
+	ulong tmo;
+	ulong endtime;
+	signed long diff;
+
+	if (usec >= 1000) {		/* if "big" number, spread normalization to seconds */
+		tmo = usec / 1000;	/* start to normalize for usec to ticks per sec */
+#if (CFG_HZ > 1000)
+		tmo *=(CFG_HZ/1000);
+#else
+		tmo *= CFG_HZ;		/* find number of "ticks" to wait to achieve target */
+		tmo /= 1000;		/* finish normalize. */
+#endif
+	} else {			/* else small number, don't kill it prior to HZ multiply */
+#if (CFG_HZ > 1000)
+		tmo = usec *(CFG_HZ/1000);
+		tmo /= 1000;
+#else
+		tmo = usec * CFG_HZ;
+		tmo /= (1000*1000);
+#endif
+	}
+
+	endtime = get_timer_masked () + tmo;
+
+	do {
+		ulong now = get_timer_masked ();
+		diff = endtime - now;
+	} while (diff >= 0);
+}
+
+
+/*
+ * This function is derived from PowerPC code (read timebase as long long).
+ * On ARM it just returns the timer value.
+ */
+unsigned long long get_ticks(void)
+{
+	return get_timer(0);
+}
+
+/*
+ * This function is derived from PowerPC code (timebase clock frequency).
+ * On ARM it returns the number of timer ticks per second.
+ */
+ulong get_tbclk (void)
+{
+	ulong tbclk;
+
+	tbclk = CFG_HZ;
+	return tbclk;
+}
+
