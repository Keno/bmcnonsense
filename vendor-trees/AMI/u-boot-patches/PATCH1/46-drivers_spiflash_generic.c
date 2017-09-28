--- uboot/drivers/spiflash/generic.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.new/drivers/spiflash/generic.c	2007-07-16 01:08:01.000000000 -0400
@@ -0,0 +1,391 @@
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
+
+/* Flash opcodes. */
+#define	OPCODE_WREN		0x06	/* Write enable */
+#define	OPCODE_WRDI		0x04	/* Write disable*/
+#define	OPCODE_RDID		0x9F	/* Read JEDEC ID */
+#define	OPCODE_RDSR		0x05	/* Read status register */
+#define OPCODE_WRSR		0x01	/* Write status register */
+#define	OPCODE_READ		0x03	/* Read data bytes */
+#define	OPCODE_FAST_READ	0x0B	/* Read Fast read */
+#define	OPCODE_PP		0x02	/* Page program */
+#define	OPCODE_SE		0xD8	/* Sector erase */
+#define OPCODE_DP		0xB9	/* Deep Power Down */
+#define	OPCODE_RES		0xAB	/* Read Electronic Signature */
+
+/* Status Register bits. */
+#define	SR_WIP			0x01	/* Write in progress */
+#define	SR_WEL			0x02	/* Write enable latch */
+#define	SR_BP0			0x04	/* Block protect 0 */
+#define	SR_BP1			0x08	/* Block protect 1 */
+#define	SR_BP2			0x10	/* Block protect 2 */
+#define	SR_SRWD			0x80	/* SR write protect */
+
+#define PROGRAM_PAGE_SIZE	256	/* Max Program Size */
+
+static 
+int inline 
+spi_error(int retval)
+{
+	printk("SPI Chip %s (%d) : Error (%d)\n",__FILE__,__LINE__,retval);
+	return retval;
+}
+
+
+int 
+spi_generic_read_status(int bank, struct spi_ctrl_driver *ctrl_drv,unsigned char *status)
+{
+	int  retval;
+	u8 code = OPCODE_RDSR;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,status, 1);
+
+	if (retval < 0) 
+		return spi_error(retval);
+
+	return 0;
+}
+
+int 
+spi_generic_write_status(int bank,struct spi_ctrl_driver *ctrl_drv, unsigned char status)
+{
+	int retval;
+	u8 code = OPCODE_WRSR;
+
+	/* Send write enable */
+	spi_generic_write_enable(bank,ctrl_drv);
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_WRITE,&status, 1);
+	if (retval < 0) 
+		return spi_error(retval);
+
+	return 0;
+}
+
+
+int 
+spi_generic_write_enable(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	u8 code = OPCODE_WREN;
+	int retval;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0);
+	if (retval < 0) 
+		return spi_error(retval);
+	return 0;
+}
+
+int 
+spi_generic_write_disable(int bank, struct spi_ctrl_driver *ctrl_drv)
+{
+	u8 code = OPCODE_WRDI;
+	int retval;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_NONE, NULL, 0);
+	if (retval < 0) 
+		return spi_error(retval);
+	return 0;
+}
+
+/* Define max times to check status register before we give up. */
+#define	MAX_READY_WAIT_COUNT	4000000
+
+static int 
+wait_till_ready(int bank,struct spi_ctrl_driver *ctrl_drv)
+{
+	unsigned long  count;
+	unsigned char sr;
+
+	for (count = 0; count < MAX_READY_WAIT_COUNT; count++) 
+	{
+		if (spi_generic_read_status(bank,ctrl_drv,&sr) < 0)
+		{
+			printk("Error reading SPI Status Register\n");
+			break;
+		}
+		else 
+		{
+			if (!(sr & SR_WIP))
+				return 0;
+		}
+	}
+
+	printk("spi_generic: Waiting for Ready Failed\n");
+	return 1;
+}
+
+
+
+
+int
+spi_generic_erase(struct map_info *map, unsigned long sect_addr)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;
+	int retval;
+	unsigned char command[4];
+
+	down(&priv->chip_drv->lock);
+
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		up(&priv->chip_drv->lock);
+		return -1;
+	}
+
+	/* Send write enable */
+	spi_generic_write_enable(bank,ctrl_drv);
+
+	/* Set up command buffer. */
+	command[0] = OPCODE_SE;
+	command[1] = sect_addr >> 16;
+	command[2] = sect_addr >> 8;
+	command[3] = sect_addr;
+
+	/* Issue Controller Transfer Routine */
+	retval = ctrl_drv->spi_transfer(bank,command, 4 ,SPI_NONE, NULL, 0);
+	if (retval < 0) 
+	{
+		up(&priv->chip_drv->lock);
+		return spi_error(retval);
+	}
+
+	up(&priv->chip_drv->lock);
+	return retval;
+}
+
+
+int  
+spi_generic_read(struct map_info *map, loff_t addr, size_t bytes, unsigned char *buff)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;
+	int retval;
+	size_t transfer;
+	unsigned char command[5];
+	int  (*readfn)(int bank,unsigned char *,int , SPI_DIR, unsigned char *, unsigned long);
+
+	/* Some time zero bytes length are sent */
+	if (bytes==0)
+		return 0;	
+
+	down(&priv->chip_drv->lock);
+
+	/* Wait until finished previous command. */
+	if (wait_till_ready(bank,ctrl_drv))
+	{
+		up(&priv->chip_drv->lock);
+		return -1;
+	}
+
+	if (ctrl_drv->spi_burst_read)
+		readfn = ctrl_drv->spi_burst_read;
+	else	
+		readfn = ctrl_drv->spi_transfer;
+
+	transfer=bytes;
+	while (bytes)
+	{
+		if (ctrl_drv->spi_burst_read)
+			transfer=bytes;
+		else
+		{
+			transfer=ctrl_drv->max_read;
+			if (transfer > bytes)
+				transfer = bytes;
+		}
+
+		if (!ctrl_drv->fast_read)
+		{
+			/* Set up command buffer. */	/* Normal Read */
+			command[0] = OPCODE_READ;
+			command[1] = addr >> 16;
+			command[2] = addr >> 8;
+			command[3] = addr;
+
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, 4 ,SPI_READ, buff, (unsigned long)transfer);
+		}
+		else
+		{
+			/* Set up command buffer. */   /* Fast Read */
+			command[0] = OPCODE_FAST_READ;
+			command[1] = addr >> 16;
+			command[2] = addr >> 8;
+			command[3] = addr;
+			command[4] = 0;			/* dummy data */
+
+			/* Issue Controller Transfer Routine */
+			retval = (*readfn)(bank,command, 5 ,SPI_READ, buff, (unsigned long)transfer);
+		}
+
+		if (retval < 0) 
+		{
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+
+		bytes-=transfer;
+		addr+=transfer;
+		buff+=transfer;
+	}
+
+	up(&priv->chip_drv->lock);
+	return 0;
+}
+
+
+int  
+spi_generic_write(struct map_info *map, loff_t addr, size_t bytes, const unsigned char *buff)
+{
+	struct spi_flash_private *priv=map->fldrv_priv;
+	int bank = map->map_priv_1;
+	struct spi_ctrl_driver *ctrl_drv = priv->ctrl_drv;
+	int retval;
+	unsigned char command[4];
+	size_t transfer;
+
+	/* Some time zero bytes length are sent */
+	if (bytes==0)
+		return 0;	
+
+	down(&priv->chip_drv->lock);
+
+	while (bytes)
+	{
+		/* Wait until finished previous command. */
+		if (wait_till_ready(bank,ctrl_drv))
+		{
+			up(&priv->chip_drv->lock);
+			return -1;
+		}
+
+		/* Send write enable */
+		spi_generic_write_enable(bank,ctrl_drv);
+
+		transfer = PROGRAM_PAGE_SIZE;
+		if (bytes <  transfer)
+			transfer = bytes;
+	
+		/* Set up command buffer. */  
+		command[0] = OPCODE_PP;
+		command[1] = addr >> 16;
+		command[2] = addr >> 8;
+		command[3] = addr;
+
+
+		/* Issue Controller Transfer Routine */
+		retval = ctrl_drv->spi_transfer(bank,command,4 ,SPI_WRITE, 
+						(unsigned char *)buff, transfer);
+		if (retval < 0) 
+		{
+			up(&priv->chip_drv->lock);
+			return spi_error(retval);
+		}
+		addr+=(transfer-retval);
+		buff+=(transfer-retval);
+		bytes-=(transfer-retval);
+	}
+
+	up(&priv->chip_drv->lock);
+	return 0;
+}
+
+/***********************************************************************************/
+extern int spi_verbose;
+int 
+spi_generic_probe(int bank,struct spi_ctrl_driver *ctrl_drv, struct spi_flash_info *chip_info,
+			char *spi_name,struct spi_flash_info *spi_list, int spi_list_len)
+{
+	int  retval;
+	u32 val;
+	int i;
+	u8 code = OPCODE_RDID;
+
+	if (spi_verbose == 2) 
+		printk("SPI: probing for %s devices ...\n",spi_name);
+
+	/* Send write enable */
+	retval =spi_generic_write_enable(bank,ctrl_drv);
+	if (retval < 0) 
+	 	return -1;
+
+	/* Issue Controller Transfer Routine */
+	val = 0;
+	retval = ctrl_drv->spi_transfer(bank,&code, 1,SPI_READ,(unsigned char *)&val, 3);
+	val &= 0x00FFFFFF;
+
+	if (retval < 0) 
+	{
+		spi_error(retval);
+		return -1;
+	}
+
+	/* Match the ID against the table entries */
+	for (i = 0; i < spi_list_len; i++) 
+	{
+		if ((spi_list[i].mfr_id == ((val)& 0xFF)) &&
+		    (spi_list[i].dev_id == ((val >> 8)& 0xFFFF)))
+			break;
+	}
+
+	if (i == spi_list_len) 
+	{
+//		if (spi_verbose == 2)
+//			printk("%s : Unrecognized ID (0x%x) got \n",spi_name,val);
+		return -1;
+	}
+	memcpy(chip_info,&spi_list[i],sizeof(struct spi_flash_info));
+
+	if (spi_verbose > 0)
+		printk(KERN_INFO"Found SPI Chip %s \n",spi_list[i].name);
+
+	return 0;
+
+}
+
+EXPORT_SYMBOL(spi_generic_probe);
+EXPORT_SYMBOL(spi_generic_erase);
+EXPORT_SYMBOL(spi_generic_read);
+EXPORT_SYMBOL(spi_generic_write);
+EXPORT_SYMBOL(spi_generic_write_disable);
+EXPORT_SYMBOL(spi_generic_write_enable);
+EXPORT_SYMBOL(spi_generic_read_status);
+EXPORT_SYMBOL(spi_generic_write_status);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("MTD SPI driver for Generic SPI flash chips");
+
+#endif
