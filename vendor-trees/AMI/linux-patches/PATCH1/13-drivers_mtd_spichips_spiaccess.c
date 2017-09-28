--- linux/drivers/mtd/spichips/spiaccess.c	1969-12-31 19:00:00.000000000 -0500
+++ linux.spi/drivers/mtd/spichips/spiaccess.c	2007-05-16 09:28:35.000000000 -0400
@@ -0,0 +1,433 @@
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
+#include "spiflash.h"
+
+int spi_verbose = 2;
+
+
+/*----------------------------------------------------------------------------------*/
+/*           Low level functions which finally talk to the chip driver              */
+/*----------------------------------------------------------------------------------*/
+static void inline
+chip_unlock_sector(struct map_info *map, unsigned long sect_addr,int unlock)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->unlock_sector)
+		return;
+	(*priv->chip_drv->unlock_sector)(map,sect_addr,unlock);
+	return;
+}
+
+static int inline
+chip_is_sector_locked(struct map_info *map, unsigned long sect_addr)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->is_sector_locked)
+		return 0;
+	return (*priv->chip_drv->is_sector_locked)(map,sect_addr);
+}
+
+static int inline
+chip_read_bytes(struct map_info *map, loff_t addr, size_t bytes, unsigned char *buf)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->read_bytes)
+		return -1;
+	return (*priv->chip_drv->read_bytes)(map,addr,bytes,buf);
+}
+
+static int inline
+chip_write_bytes(struct map_info *map, loff_t addr, size_t bytes, const unsigned char *buf)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->write_bytes)
+		return -1;
+	return (*priv->chip_drv->write_bytes)(map,addr,bytes,buf);
+}
+
+static int inline
+chip_erase_sector(struct map_info *map, unsigned long sect_addr)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+
+	if (!priv->chip_drv->erase_sector)
+		return -1;
+	return (*priv->chip_drv->erase_sector)(map,sect_addr);
+}
+
+
+void 
+spi_flash_sync(struct mtd_info *mtd)
+{
+	struct map_info *map = mtd->priv;
+	struct spi_flash_private *priv = map->fldrv_priv;
+
+	if (!priv->chip_drv->sync)
+		return;
+	(*priv->chip_drv->sync)(map);
+	return;
+}
+
+int 
+spi_flash_suspend(struct mtd_info *mtd)
+{
+	struct map_info *map = mtd->priv;
+	struct spi_flash_private *priv = map->fldrv_priv;
+
+	if (!priv->chip_drv->suspend)
+		return -EINVAL;
+	return (*priv->chip_drv->suspend)(map);
+}
+
+void
+spi_flash_resume(struct mtd_info *mtd)
+{
+	struct map_info *map = mtd->priv;
+	struct spi_flash_private *priv = map->fldrv_priv;
+
+	if (!priv->chip_drv->resume)
+		return;
+	(*priv->chip_drv->resume)(map);
+	return;
+}
+
+
+/*----------------------------------------------------------------------------------*/
+/*  Intermediate Functions which interfaces mtd functionss to chip driver fucntions */
+/*----------------------------------------------------------------------------------*/
+static int 
+spi_flash_do_unlock(struct mtd_info *mtd, loff_t ofs, size_t len,
+			       int is_unlock)
+{
+	struct map_info *map;
+	struct mtd_erase_region_info *merip;
+	int eraseoffset, erasesize, eraseblocks;
+	int i;
+	int retval = 0;
+	int lock_status;
+
+	map = mtd->priv;
+
+	/* Pass the whole chip through sector by sector and check for each
+	   sector if the sector and the given interval overlap */
+	for(i = 0; i < mtd->numeraseregions; i++) 
+	{
+		merip = &mtd->eraseregions[i];
+
+		eraseoffset = merip->offset;
+		erasesize = merip->erasesize;
+		eraseblocks = merip->numblocks;
+
+		if (ofs > eraseoffset + erasesize)
+			continue;
+
+		while (eraseblocks > 0) 
+		{
+			if (ofs < eraseoffset + erasesize && ofs + len > eraseoffset) 
+			{
+				chip_unlock_sector(map, eraseoffset, is_unlock);
+
+				lock_status = chip_is_sector_locked(map, eraseoffset);
+
+				if (is_unlock && lock_status) 
+				{
+					printk("Cannot unlock sector at address %x length %xx\n",
+					       eraseoffset, merip->erasesize);
+					retval = -1;
+				} 
+				else if (!is_unlock && !lock_status) 
+				{
+					printk("Cannot lock sector at address %x length %x\n",
+					       eraseoffset, merip->erasesize);
+					retval = -1;
+				}
+			}
+			eraseoffset += erasesize;
+			eraseblocks --;
+		}
+	}
+	return retval;
+}
+
+static int 
+read_one_chip(struct map_info *map, struct flchip *chip,
+			       loff_t adr, size_t len, u_char *buf)
+{
+	uint32_t	i;
+	size_t bytes;
+
+	chip->state = FL_READY;
+
+	bytes = SPI_READ_PAGE_SIZE;
+	i = 0;
+	while (len >= bytes) 
+	{
+		if (0 != chip_read_bytes(map,adr,bytes,&buf [i])) 
+		{
+			printk (KERN_ERR "spi_read failed in read_one_chip function\n");
+			return -1;
+		}
+		len -= bytes;
+		i   += bytes;
+		adr += bytes;
+	}	
+
+	if (0 != len) 
+	{
+		if (0 != chip_read_bytes(map, adr, len, &buf [i])) 
+		{
+			printk (KERN_ERR "spi_read failed in read_one_chip function\n");
+			return -1;
+		}
+	}
+
+	return 0;
+}
+
+static int
+write_one_chip(struct map_info *map, struct flchip *chip,
+	                        loff_t addr, size_t len, const u_char * buf)
+{
+	uint32_t page_size, page_offset;
+	uint32_t i;
+
+	chip->state = FL_WRITING;
+
+	/* what page do we start with? */
+	page_offset = addr % SPI_WRITE_PAGE_SIZE;
+
+	/* do all the bytes fit onto one page? */
+	if (page_offset + len <= SPI_WRITE_PAGE_SIZE) 
+	{
+		if (0 != chip_write_bytes (map, addr, len, buf)) 
+		{
+			printk (KERN_ERR "spi writing in the spi_write_bytes function failed\n");
+			return -1;
+		}
+	} 
+	else 
+	{
+		/* the size of data remaining on the first page */
+		page_size = SPI_WRITE_PAGE_SIZE - page_offset;
+		if (0 != chip_write_bytes (map, addr, page_size, buf))
+		{
+			printk (KERN_ERR "spi writing in the spi_write_bytes function failed\n");
+			return -1;
+		}
+		/* write everything in PAGESIZE chunks */
+		for (i = page_size; i < len; i += page_size) 
+		{
+			page_size = len - i;
+			if (page_size > SPI_WRITE_PAGE_SIZE)
+				page_size = SPI_WRITE_PAGE_SIZE;
+			if (0 != chip_write_bytes (map, (addr + i), page_size, (buf + i))) 
+			{
+				printk (KERN_ERR "spi writing in the spi_write_bytes function failed\n");
+				return -1;
+			}
+		}
+	}
+
+	chip->state = FL_READY;
+
+	return 0;
+
+}
+
+static  int 
+erase_one_block(struct map_info *map, struct flchip *chip,ulong addr)
+{
+	int	retval = 0;
+	
+	chip->state = FL_ERASING;
+
+	retval = chip_erase_sector (map,addr);
+
+	chip->state = FL_READY;
+
+	return (retval);
+}
+/*----------------------------------------------------------------------------------*/
+/*  		                       MTD Functions 					                */
+/*----------------------------------------------------------------------------------*/
+int 
+spi_flash_unlock(struct mtd_info *mtd, loff_t ofs, size_t len)
+{
+	return spi_flash_do_unlock(mtd, ofs, len, 1);
+}
+
+int 
+spi_flash_lock(struct mtd_info *mtd, loff_t ofs, size_t len)
+{
+	return spi_flash_do_unlock(mtd, ofs, len, 0);
+}
+
+int
+spi_flash_read(struct mtd_info *mtd, loff_t from, size_t len,
+                                          size_t *retlen, u_char *buf)
+{
+	struct map_info *map = mtd->priv;
+	struct spi_flash_private *private = map->fldrv_priv;
+	unsigned long ofs;
+	int ret = 0;
+
+	if ((from + len) > mtd->size) 
+	{
+		printk(KERN_WARNING "%s: read request past end of device "
+		       "(0x%lx)\n", map->name, (unsigned long)from + len);
+		return -EINVAL;
+	}
+
+	ofs = from;
+
+	*retlen = 0;
+	ret = read_one_chip(map, &private->chips,ofs,len,buf);
+
+	if (0 == ret) 
+		*retlen = len;
+	else 
+		ret = -EINVAL;
+
+	return ret;
+	
+}
+
+int 
+spi_flash_write(struct mtd_info *mtd, loff_t to , size_t len,
+                                       size_t *retlen, const u_char *buf)
+{
+
+	struct map_info *map = mtd->priv;
+	struct spi_flash_private *private = map->fldrv_priv;
+
+	if (to > mtd->size) 
+	{
+		printk (KERN_ERR "write address > size of spi flash\n");
+		return -EINVAL;
+	}
+
+	if ((len + to) > mtd->size) 
+	{
+		printk (KERN_ERR "write address + size > size of spi flash\n");
+		return -EINVAL;
+	}
+
+	*retlen = 0;
+	if (!len) 
+		return 0;
+
+	if (0 == write_one_chip(map, &private->chips, to, len, buf)) 
+	{
+		*retlen = len;
+		return 0;			
+	}
+
+	return -EINVAL;
+}
+
+
+int 
+spi_flash_erase(struct mtd_info *mtd, struct erase_info *instr)
+{
+	struct map_info *map = mtd->priv;
+	struct spi_flash_private *private = map->fldrv_priv;
+	unsigned long adr, len;
+	int ret = 0;
+	int i;
+	int first;
+	struct mtd_erase_region_info *regions = mtd->eraseregions;
+
+	if (instr->addr > mtd->size) 
+		return -EINVAL;
+
+	if ((instr->len + instr->addr) > mtd->size) 
+		return -EINVAL;
+
+	/* Check that both start and end of the requested erase are
+	 * aligned with the erasesize at the appropriate addresses.
+	 */
+
+	i = 0;
+
+        /* Skip all erase regions which are ended before the start of
+           the requested erase. Actually, to save on the calculations,
+           we skip to the first erase region which starts after the
+           start of the requested erase, and then go back one.
+        */
+        while ((i < mtd->numeraseregions) &&
+	       (instr->addr >= regions[i].offset)) 
+	{
+               i++;
+	}
+        i--;
+
+	/* OK, now i is pointing at the erase region in which this
+	 * erase request starts. Check the start of the requested
+	 * erase range is aligned with the erase size which is in
+	 * effect here.
+	 */
+	if (instr->addr & (regions[i].erasesize-1)) 
+		return -EINVAL;
+
+	/* Remember the erase region we start on. */
+	first = i;
+
+	/* Next, check that the end of the requested erase is aligned
+	 * with the erase region at that address.
+	 */
+
+	while ((i < mtd->numeraseregions) &&
+	       ((instr->addr + instr->len) >= regions[i].offset)) 
+	{
+                i++;
+	}
+
+	/* As before, drop back one to point at the region in which
+	 * the address actually falls.
+	 */
+	i--;
+
+	if ((instr->addr + instr->len) & (regions[i].erasesize-1)) 
+                return -EINVAL;
+	
+
+	adr = instr->addr;
+	len = instr->len;
+	i = first;
+
+	while (len) 
+	{
+		ret = erase_one_block(map, &private->chips, adr);
+
+		if (ret) 
+			return ret;
+
+		adr += regions[i].erasesize;
+		len -= regions[i].erasesize;
+	}
+
+	instr->state = MTD_ERASE_DONE;
+	mtd_erase_callback(instr);
+
+	return 0;
+}
