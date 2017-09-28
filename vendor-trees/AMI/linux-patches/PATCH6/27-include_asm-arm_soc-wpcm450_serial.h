--- linux-2.6.15.4-cpu/include/asm-arm/soc-wpcm450/serial.h	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/include/asm-arm/soc-wpcm450/serial.h	2007-06-18 23:52:20.000000000 -0400
@@ -0,0 +1,38 @@
+/*
+ *  linux/include/asm-arm/soc-xxx/serial.h
+ *
+ *  This file contains the WPCM450 Serial port configuration
+ *  The serial port of WPCM450 is similiar to UART 8250
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
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
+
+#include <linux/autoconf.h>
+#include <linux/serial_core.h>
+#include <asm/arch/hardware.h>
+
+#define BASE_BAUD	(CONFIG_UART_CLK/16)
+
+#define STD_COM_FLAGS (ASYNC_SKIP_TEST)
+
+#define SERIAL_PORT_DFNS			\
+ {0,BASE_BAUD,IO_ADDRESS(WPCM_UART0_BASE),IRQ_UART0,STD_COM_FLAGS,0,UPIO_PORT,0,2},	\
+ {0,BASE_BAUD,IO_ADDRESS(WPCM_UART1_BASE),IRQ_UART1,STD_COM_FLAGS,0,UPIO_PORT,0,2},
+
+
+
