--- linux/drivers/mtd/spichips/spiflash.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.mb/drivers/mtd/spichips/spiflash.h	2007-07-09 15:32:17.000000000 -0400
@@ -0,0 +1,164 @@
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
+#ifndef __LINUX_SPI_H__
+#define __LINUX_SPI_H__
+
+#include <linux/kernel.h>
+#include <linux/autoconf.h>
+#include <linux/module.h>
+#include <linux/types.h>
+#include <linux/kernel.h>
+#include <linux/sched.h>
+#include <linux/errno.h>
+#include <linux/init.h>
+#include <linux/slab.h>
+#include <linux/list.h>
+#include <linux/string.h>
+#include <linux/spinlock.h>
+#include <linux/device.h>
+#include <linux/interrupt.h>
+#include <linux/mtd/mtd.h>
+#include <linux/mtd/map.h>
+#include <linux/mtd/partitions.h>
+#include <linux/mtd/flashchip.h>
+#include <linux/mtd/compatmac.h>
+#include <linux/dma-mapping.h>
+#include <linux/wait.h>
+#include <linux/delay.h>
+#include <asm/delay.h>
+#include <asm/semaphore.h>
+#define CFG_FLASH_SPI_DRIVER 1   /* Need for compile the modules */
+/*---------------------------------------------------------------------------
+ The folllowing should match exactly with its uboot counterpart spiflash.h
+----------------------------------------------------------------------------*/
+
+#define DEVICE_TYPE_X8	(8 / 8)
+#define DEVICE_TYPE_X16	(16 / 8)
+#define DEVICE_TYPE_X32	(32 / 8)
+
+#define SPI_READ_PAGE_SIZE (4*1024)			/* 4K page size */
+#define SPI_WRITE_PAGE_SIZE  256
+
+#define MAX_ERASE_REGIONS    4	
+
+struct spi_flash_info 
+{
+	const char *name;
+	const __u8  mfr_id;
+	const __u16 dev_id;
+	const u_long size;
+	const int numeraseregions;
+	const struct mtd_erase_region_info regions[MAX_ERASE_REGIONS];
+};
+
+typedef enum spidir 
+{
+	SPI_NONE = 0,
+	SPI_READ = 1,
+	SPI_WRITE = 2
+} SPI_DIR;
+
+struct spi_ctrl_driver 
+{
+	struct module *module;
+	struct semaphore lock;
+	char *name;
+	struct list_head list;
+
+	/* Supports fast read at higher frequency */
+	int  fast_read;		
+
+	/* Max datasize to be used to read type functions in spi_transfer*/
+	int  max_read;		
+
+	/* spi_transfer can be used for all type of spi access */
+	int  (*spi_transfer)(int bank,unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen);
+
+	/* spi_burst_read is not NULL, if the ctrl supports read large data continuosly */
+	int  (*spi_burst_read)(int bank,unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long  datalen);
+	
+#ifdef __UBOOT__   
+	int  (*spi_init)(void);
+#endif
+};
+
+struct spi_chip_driver 
+{
+	struct module *module;
+	struct semaphore lock;
+	char *name;
+	struct list_head list;
+	int (*probe)(int bank,struct spi_ctrl_driver *ctlr_drv, struct spi_flash_info *chip_info);
+	void (*unlock_sector)   (struct map_info *map, unsigned long sect_addr,int unlock);
+	int  (*is_sector_locked)(struct map_info *map, unsigned long sect_addr);
+	int  (*erase_sector)	(struct map_info *map, unsigned long sect_addr);
+	int  (*read_bytes)      (struct map_info *map, loff_t addr, size_t bytes, unsigned char *buf);
+	int  (*write_bytes)     (struct map_info *map, loff_t addr, size_t bytes, 
+																	   const unsigned char *buf);
+	void (*sync)		(struct map_info *map);
+	int  (*suspend)		(struct map_info *map);
+	void (*resume)		(struct map_info *map);
+};
+
+struct spi_flash_private 
+{
+	int device_type;
+	int interleave;
+	int numchips;
+	unsigned long chipshift;
+	struct flchip chips;
+	struct spi_chip_driver *chip_drv;
+	struct spi_ctrl_driver *ctrl_drv;
+};
+
+/* SPI Core Functions to register,access chip and controller drivers */
+struct spi_chip_driver *get_spi_chip_driver_by_index (int index);
+struct spi_chip_driver *get_spi_chip_driver_by_name (const char *name);
+struct spi_ctrl_driver *get_spi_ctrl_driver_by_index (int index);
+struct spi_ctrl_driver *get_spi_ctrl_driver_by_name (const char *name);
+void register_spi_chip_driver(struct spi_chip_driver *drv);
+void unregister_spi_chip_driver(struct spi_chip_driver *drv);
+void register_spi_ctrl_driver(struct spi_ctrl_driver *drv);
+void unregister_spi_ctrl_driver(struct spi_ctrl_driver *drv);
+
+/* Functions registered to MTD  for accessing the chips */
+int  spi_flash_read(struct mtd_info *, loff_t, size_t, size_t *, u_char *);
+int  spi_flash_write(struct mtd_info *, loff_t, size_t, size_t *,const u_char *);
+int  spi_flash_erase(struct mtd_info *, struct erase_info *);
+void spi_flash_sync(struct mtd_info *);
+int  spi_flash_suspend(struct mtd_info *);
+void spi_flash_resume(struct mtd_info *);
+int  spi_flash_unlock(struct mtd_info *mtd, loff_t ofs, size_t len);
+int  spi_flash_lock(struct mtd_info *mtd, loff_t ofs, size_t len);
+
+/* Generic spi funtions which can be used by any spi device (if uses generic algo) */
+int spi_generic_write_enable(int bank,struct spi_ctrl_driver *ctrl_drv);
+int spi_generic_read_enable(int bank,struct spi_ctrl_driver *ctrl_drv);
+int spi_generic_read_status(int bank,struct spi_ctrl_driver *ctrl_drv,unsigned char *status);
+int spi_generic_write_status(int bank,struct spi_ctrl_driver *ctrl_drv, unsigned char status);
+int spi_generic_erase(struct map_info *map, unsigned long sect_addr);
+int spi_generic_read(struct map_info *map, loff_t addr, size_t bytes, unsigned char *buff);
+int spi_generic_write(struct map_info *map, loff_t addr, size_t bytes, const unsigned char *buff);
+int spi_generic_probe(int bank, struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info,
+			char *spi_name,struct spi_flash_info *spi_list, int spi_list_len);
+
+
+#endif
