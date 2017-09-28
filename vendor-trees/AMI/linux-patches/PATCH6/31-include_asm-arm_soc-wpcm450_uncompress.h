--- linux-2.6.15.4-cpu/include/asm-arm/soc-wpcm450/uncompress.h	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/include/asm-arm/soc-wpcm450/uncompress.h	2007-06-18 23:33:03.000000000 -0400
@@ -0,0 +1,68 @@
+/*
+ * linux/include/asm-arm/arch-xxx/uncompress.h
+ *
+ * Copyright (C) 2005 American Megatrends Inc.
+ */
+
+#include <asm/arch/hardware.h>
+#include <asm/arch/lowlevel_serial.h>
+
+#define flush() do { } while (0)
+
+static void
+putc(const char c)
+{
+	volatile unsigned int status=0;
+
+	/* Wait for Ready */
+	do
+    {
+    	status = *(volatile unsigned char *)(UART_BASE+SERIAL_LSR);
+    }
+    while (!((status & SERIAL_LSR_THRE)==SERIAL_LSR_THRE) );
+
+	/* Write Character */
+    *(volatile unsigned char *)(UART_BASE+SERIAL_THR) = c;
+
+	return;
+}
+
+
+#if 0
+static void
+putstr(const char *s)
+{
+	while (*s)
+	{
+		putc(*s);
+		if (*s == '\n')
+			putc('\r');
+		s++;
+	}
+}
+#endif
+
+static __inline__ void
+arch_decomp_setup(void)
+{
+
+	/* Enable DLAB, 8 Bits and One Stop Bit */
+    *(volatile unsigned char *)(UART_BASE+SERIAL_LCR) = 0x83;
+
+	/* Set the Baud Rate */
+    *(volatile unsigned char *)(UART_BASE+SERIAL_DLM) = (((CONFIG_UART_CLK/(BAUD_RATE *16))-2) & 0xFF00) >> 8;
+    *(volatile unsigned char *)(UART_BASE+SERIAL_DLL) = (((CONFIG_UART_CLK/(BAUD_RATE *16))-2) & 0x00FF);
+
+	/* Disable  DLAB, 8 Bits and One Stop Bit */
+    *(volatile unsigned char *)(UART_BASE+SERIAL_LCR) = 0x03;
+
+	/* Enable FIFO */
+    *(volatile unsigned char *)(UART_BASE+SERIAL_FCR) = 0xC1;
+
+	return;
+}
+
+/*
+ * nothing to do
+ */
+#define arch_decomp_wdog()
