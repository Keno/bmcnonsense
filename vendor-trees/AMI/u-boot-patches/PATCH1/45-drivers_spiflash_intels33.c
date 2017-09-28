--- u-boot-1.1.4.ami/drivers/spiflash/intels33.c	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-vsc/drivers/spiflash/intels33.c	2008-08-19 12:17:34.003293961 -0400
@@ -0,0 +1,97 @@
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
+/* Name, ID1, ID2 , Size, Erase regions, { Offset, Erase Size, Erase Block Count } */
+static struct spi_flash_info s33_data [] = 
+{
+	/* Intel S33  64 K Sectors */
+	{ "Intel S33 16Mb"  , 0x89, 0x1189, 0x200000 , 1, {{ 0, 64  * 1024, 32  },} },
+	{ "Intel S33 32Mb"  , 0x89, 0x1289, 0x400000 , 1, {{ 0, 64  * 1024, 64  },} },
+	{ "Intel S33 64Mb"  , 0x89, 0x1389, 0x800000 , 1, {{ 0, 64  * 1024, 128 },} },
+};
+
+
+static
+int 
+s33_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"intel s33",
+						s33_data,ARRAY_SIZE(s33_data));	
+
+	if (retval == -1)
+		return retval;
+
+	/* UnProctect all sectors */
+ 	/* SRWD=0 (Bit 7)  BP0,BP1,BP2 = 0 (Bit 2,3,4) */
+	if (spi_generic_write_status(bank,ctrl_drv,0x0) < 0)
+		printk("intel s33: Unable to Unprotect all sectors\n");
+	return retval;
+}
+
+struct spi_chip_driver s33_driver =
+{
+	.name 		= "intel s33",
+	.module 	= THIS_MODULE,
+	.probe	 	= s33_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+};
+
+
+
+int 
+s33_init(void)
+{
+	init_MUTEX(&s33_driver.lock);
+#ifdef __UBOOT__	/* MIPS */
+	s33_driver.probe	 		= s33_probe;
+	s33_driver.erase_sector 	= spi_generic_erase;
+	s33_driver.read_bytes		= spi_generic_read;
+	s33_driver.write_bytes		= spi_generic_write;
+#endif
+	register_spi_chip_driver(&s33_driver);
+	return 0;
+}
+
+
+void 
+s33_exit(void)
+{
+	init_MUTEX(&s33_driver.lock);
+	unregister_spi_chip_driver(&s33_driver);
+	return;
+}
+
+
+module_init(s33_init);
+module_exit(s33_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Intel S33 flash chips");
+
+#endif
