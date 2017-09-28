--- linux.old/drivers/mtd/spichips/m25pxx.c	1969-12-31 19:00:00.000000000 -0500
+++ linux/drivers/mtd/spichips/m25pxx.c	2008-04-29 17:56:14.000000000 -0400
@@ -0,0 +1,112 @@
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
+static struct spi_flash_info m25pxx_data [] = 
+{
+	/* ST Micro 32K Sectors */
+	{ "ST Micro m25p05A" , 0x20, 0x1020, 0x010000 , 1, {{ 0, 32  * 1024, 2   },} },
+	{ "ST Micro m25p10A" , 0x20, 0x1120, 0x020000 , 1, {{ 0, 32  * 1024, 4   },} },
+
+	/* ST Micro 64 K Sectors */
+	{ "ST Micro m25p20"  , 0x20, 0x1220, 0x040000 , 1, {{ 0, 64  * 1024, 4   },} },
+	{ "ST Micro m25p40"  , 0x20, 0x1320, 0x080000 , 1, {{ 0, 64  * 1024, 8   },} },
+	{ "ST Micro m25p80"  , 0x20, 0x1420, 0x100000 , 1, {{ 0, 64  * 1024, 16  },} },
+	{ "ST Micro m25p16"  , 0x20, 0x1520, 0x200000 , 1, {{ 0, 64  * 1024, 32  },} },
+	{ "ST Micro m25p32"  , 0x20, 0x1620, 0x400000 , 1, {{ 0, 64  * 1024, 64  },} },
+	{ "ST Micro m25p64"  , 0x20, 0x1720, 0x800000 , 1, {{ 0, 64  * 1024, 128 },} },
+	{ "Spans S25FL128P"  , 0x01, 0x1820, 0x1000000 , 1, {{ 0, 64  * 1024, 256 },} },
+
+	/* ST Micro 256K Sectors */
+	{ "ST Micro m25p128" , 0x20, 0x1820, 0x1000000, 1, {{ 0, 256 * 1024, 64  },} },
+
+    /* ST Micro 64 K Sectors, 25MHz speed */
+	{ "ST Micro m45p20"  , 0x20, 0x1240, 0x040000 , 1, {{ 0, 64  * 1024, 4   },} },
+	{ "ST Micro m45p40"  , 0x20, 0x1340, 0x080000 , 1, {{ 0, 64  * 1024, 8   },} },
+	{ "ST Micro m45p80"  , 0x20, 0x1440, 0x100000 , 1, {{ 0, 64  * 1024, 16  },} },
+	{ "ST Micro m45p16"  , 0x20, 0x1540, 0x200000 , 1, {{ 0, 64  * 1024, 32  },} },
+	{ "ST Micro m45p32"  , 0x20, 0x1640, 0x400000 , 1, {{ 0, 64  * 1024, 64  },} },
+	{ "ST Micro m45p64"  , 0x20, 0x1740, 0x800000 , 1, {{ 0, 64  * 1024, 128 },} },
+
+};
+
+
+static
+int 
+m25pxx_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info)
+{
+	int retval;
+	retval = spi_generic_probe(bank,ctrl_drv,chip_info,"m25pxx",
+						m25pxx_data,ARRAY_SIZE(m25pxx_data));	
+
+	if (retval == -1)
+		return retval;
+
+	/* UnProctect all sectors */
+ 	/* SRWD=0 (Bit 7)  BP0,BP1,BP2 = 0 (Bit 2,3,4) */
+	if (spi_generic_write_status(bank,ctrl_drv,0x0) < 0)
+		printk("m25pxx: Unable to Unprotect all sectors\n");
+
+	return retval;
+}
+
+struct spi_chip_driver m25pxx_driver =
+{
+	.name 		= "m25pxx",
+	.module 	= THIS_MODULE,
+	.probe	 	= m25pxx_probe,
+	.erase_sector 	= spi_generic_erase,
+	.read_bytes	= spi_generic_read,
+	.write_bytes	= spi_generic_write,
+};
+
+
+
+int 
+m25pxx_init(void)
+{
+	init_MUTEX(&m25pxx_driver.lock);
+	register_spi_chip_driver(&m25pxx_driver);
+	return 0;
+}
+
+
+void 
+m25pxx_exit(void)
+{
+	init_MUTEX(&m25pxx_driver.lock);
+	unregister_spi_chip_driver(&m25pxx_driver);
+	return;
+}
+
+
+module_init(m25pxx_init);
+module_exit(m25pxx_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for ST M25Pxx flash chips");
+
+#endif
