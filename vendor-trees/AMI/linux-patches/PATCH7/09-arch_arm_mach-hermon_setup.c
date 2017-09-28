--- linux-2.6.15.4-wpcm/arch/arm/mach-hermon/setup.c	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wb/arch/arm/mach-hermon/setup.c	2007-06-25 14:31:11.000000000 -0400
@@ -0,0 +1,112 @@
+/*
+ *  linux/arch/arm/mach-hermon/setup.c
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
+ *
+ */
+#include <linux/mm.h>
+#include <linux/init.h>
+#include <linux/autoconf.h>
+#include <linux/init.h>
+#include <linux/major.h>
+#include <linux/fs.h>
+#include <linux/device.h>
+#include <linux/serial.h>
+#include <linux/tty.h>
+#include <linux/serial_core.h>
+
+#include <asm/io.h>
+#include <asm/pgtable.h>
+#include <asm/page.h>
+#include <asm/mach/map.h>
+#include <asm/setup.h>
+#include <asm/system.h>
+#include <asm/memory.h>
+#include <asm/hardware.h>
+#include <asm/mach-types.h>
+#include <asm/mach/arch.h>
+
+static struct map_desc hermon_std_desc[] __initdata =
+{
+	/* WPCM450 Register mappings */
+	WPCM450_MAP_DESC,
+	
+ 	/* HERMON Flash Space */
+ 	{WPCM_FLASH_VA_BASE, 	__phys_to_pfn(WPCM_FLASH_BASE), WPCM_FLASH_SZ, 	MT_DEVICE },
+};
+
+
+/*
+static struct uart_port early_serial_ports[] =
+{
+	{
+		.membase	= (char *)WPCM_UART1_VA_BASE,
+		.mapbase	= WPCM_UART1_BASE,
+		.irq		= IRQ_UART1,
+		.flags		= UPF_SKIP_TEST,
+		.iotype		= UPIO_PORT,
+		.regshift	= 2,
+		.uartclk	= CONFIG_UART_CLK,
+		.line		= 0,
+		.type		= PORT_16550A,
+		.fifosize	= 16
+	}
+};
+*/
+
+
+#ifdef CONFIG_DEBUG_LL
+extern void printascii(char *Str);
+void
+EarlyPrintk(char *Str)
+{
+	printascii(Str);
+}
+#else
+void
+EarlyPrintk(char *Str)
+{
+}
+#endif
+
+void __init
+hermon_init(void)
+{
+	/* Do any initial hardware setup if needed */
+}
+
+void __init
+hermon_map_io(void)
+{
+	/* Do any initial hardware setup if needed */
+	iotable_init(hermon_std_desc, ARRAY_SIZE(hermon_std_desc));
+//	early_serial_setup(&early_serial_ports[0]);
+}
+
+extern struct sys_timer wpcm450_timer;
+extern void wpcm450_init_irq(void);
+
+MACHINE_START(HERMON, "HERMON")
+	//.phys_ram		= 0,
+	.phys_io		= WPCM_UART0_BASE,
+	.io_pg_offst		= ((IO_ADDRESS(WPCM_UART0_BASE)) >> 18) & 0xFFFC,
+	.map_io			= hermon_map_io,
+	.init_irq		= wpcm450_init_irq,
+	.timer			= &wpcm450_timer,
+	.boot_params	= 0x00000100,
+	.init_machine	= hermon_init,
+MACHINE_END
