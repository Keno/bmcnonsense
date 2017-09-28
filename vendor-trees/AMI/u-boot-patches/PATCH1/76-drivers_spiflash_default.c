--- uboot/drivers/spiflash/default.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/drivers/spiflash/default.c	2008-09-05 13:57:53.788495614 -0400
@@ -0,0 +1,104 @@
+/*
+ * Copyright (C) 2007 American Megatrends Inc
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
+ * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
+ */
+
+
+#ifdef __UBOOT__	
+#include <common.h>
+#endif
+#include "spiflash.h"
+#ifdef	CFG_FLASH_SPI_DRIVER
+
+#ifndef CONFIG_DEFAULT_SPI_NAME  
+#define CONFIG_DEFAULT_SPI_NAME "Unknown SPI Device"
+#endif
+
+#ifndef CONFIG_DEFAULT_SPI_SIZE
+#define CONFIG_DEFAULT_SPI_SIZE 0
+#endif
+
+#ifndef CONFIG_DEFAULT_SPI_ERASE_SIZE
+#define CONFIG_DEFAULT_SPI_ERASE_SIZE	(64 *1024)
+#endif
+
+
+/* This driver is called at end when all probe failed. Some chips don't support read id
+   commands and user can provide the information here */
+
+/* Name, ID1, ID2 , Size, Erase regions, { Offset, Erase Size, Erase Block Count } */
+static struct spi_flash_info default_data [] = 
+{
+	{ CONFIG_DEFAULT_SPI_NAME , 0xFF, 0x0FFFF, CONFIG_DEFAULT_SPI_SIZE , 1, 
+			{{ 0, CONFIG_DEFAULT_SPI_ERASE_SIZE, CONFIG_DEFAULT_SPI_SIZE/CONFIG_DEFAULT_SPI_ERASE_SIZE },} },
+};
+
+
+static
+int 
+default_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	memcpy(chip_info,&default_data[0],sizeof(struct spi_flash_info));
+
+	if (spi_generic_write_status(bank,ctrl_drv,0x0) < 0)
+		printk("%s: Unable to Unprotect all sectors\n",CONFIG_DEFAULT_SPI_NAME);
+	return 0;
+}
+
+struct spi_chip_driver default_driver =
+{
+	.name 		= "default",
+	.module 	= THIS_MODULE,
+	.probe	 	= default_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+};
+
+
+
+int 
+default_init(void)
+{
+	init_MUTEX(&default_driver.lock);
+#ifdef __UBOOT__	/* MIPS */
+	default_driver.probe	 		= default_probe;
+	default_driver.erase_sector 	= spi_generic_erase;
+	default_driver.read_bytes		= spi_generic_read;
+	default_driver.write_bytes	= spi_generic_write;
+#endif
+	register_spi_chip_driver(&default_driver);
+	return 0;
+}
+
+
+void 
+default_exit(void)
+{
+	init_MUTEX(&default_driver.lock);
+	unregister_spi_chip_driver(&default_driver);
+	return;
+}
+
+
+module_init(default_init);
+module_exit(default_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Default flash chips");
+
+#endif
