--- u-boot/cpu/arm926ejs/WPCM450/serial.c	1969-12-31 17:00:00.000000000 -0700
+++ u-boot-new/cpu/arm926ejs/WPCM450/serial.c	2008-06-04 18:42:10.000000000 -0500
@@ -0,0 +1,137 @@
+/*
+ * (C) Copyright 2002
+ * Gary Jennejohn, DENX Software Engineering, <gj@denx.de>
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
+ *
+ */
+
+#include <config.h>
+#include <linux/types.h>
+#include <asm/u-boot.h>
+#include <asm/global_data.h>
+#include "soc_hw.h"
+#include "hermon_serial.h"
+
+
+
+#ifdef CONFIG_SERIAL0
+	UINT32 DebugSerialPort = WPCM_UART0_BASE ;
+#elif CONFIG_SERIAL1
+	UINT32 DebugSerialPort = WPCM_UART1_BASE ;
+#else
+	#error "Bad: you didn't configure serial ..."
+#endif
+
+
+//	UINT32 DebugSerialPort = 0xB8000000 ;
+
+void
+serialhandler(void *arg)
+{
+	WPCM_SerialIntIdentification(DebugSerialPort);
+}
+
+void
+ll_serial_init(void)
+{
+	WPCM_SerialInit (DebugSerialPort, WPCM_BAUD_38400, PARITY_NONE, 0, 8 );
+	WPCM_SetSerialFifoCtrl(DebugSerialPort, 1, ENABLE, ENABLE);
+}
+
+void
+serial_setbrg(void)
+{
+	DECLARE_GLOBAL_DATA_PTR;
+	unsigned int reg = 0;
+
+	switch (gd->baudrate)
+	{
+		case 9600:
+			reg = WPCM_BAUD_9600;
+			break;
+		case 19200:
+			reg = WPCM_BAUD_19200;
+			break;
+		case 38400:
+			reg = WPCM_BAUD_38400;
+			break;
+		case 57600:
+			reg = WPCM_BAUD_57600;
+			break;
+		case 115200:
+			reg = WPCM_BAUD_115200;
+			break;
+		default:
+			reg=WPCM_BAUD_38400;
+			break;
+	}
+
+	/* Set Baud Rate */
+	WPCM_SerialInit (DebugSerialPort, reg, PARITY_NONE, 0, 8 );
+
+	/* Enable FiFo */
+	WPCM_SetSerialFifoCtrl(DebugSerialPort, 1, ENABLE, ENABLE);
+
+
+#ifndef CONFIG_SERIAL1    
+	// Initialize the UART1 - Init as UART0 for the time being
+	/* Set Baud Rate */
+	WPCM_SerialInit (WPCM_UART1_BASE, reg, PARITY_NONE, 0, 8 );
+
+	/* Enable FiFo */
+	WPCM_SetSerialFifoCtrl(WPCM_UART1_BASE, 1, ENABLE, ENABLE);
+#endif
+
+	return;
+}
+
+int
+serial_init(void)
+{
+	serial_setbrg();
+	return 0;
+}
+
+
+int
+serial_getc(void)
+{
+	return WPCM_GetSerialChar( DebugSerialPort );
+}
+
+
+void
+serial_putc(const char c)
+{
+	WPCM_PutSerialChar( DebugSerialPort, c );
+   	if(c == '\n')
+	   		WPCM_PutSerialChar(DebugSerialPort,'\r');
+
+}
+
+int
+serial_tstc(void)
+{
+	return WPCM_TestSerialForChar(DebugSerialPort);
+}
+
+
+void
+serial_puts (char *s)
+{
+	WPCM_PutSerialStr(DebugSerialPort,s);
+}
+
