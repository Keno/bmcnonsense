--- linux/drivers/mtd/spichips/hermon_spi.c	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/drivers/mtd/spichips/hermon_spi.c	2007-07-15 23:19:27.000000000 -0400
@@ -0,0 +1,494 @@
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
+#include "../../../../projdef.h"
+#ifdef __UBOOT__	
+#include "soc_hw.h"
+#endif
+
+#define SPI_INPUT_CLOCK 100		/* Input Frequency MHZ */
+#define MAX_CLOCK	25		/* Max SPI Frequency MHZ supported by Controller */
+#define MAX_READ	4	
+#define MAX_CHIPS	4
+
+#define EXTENDED_WRITE  1
+
+typedef struct 
+{
+	volatile unsigned char FIU_CFG;	
+	volatile unsigned char BURST_CFG;	
+	volatile unsigned char RESP_CFG;		
+	volatile unsigned char CFBB_PROT;
+
+	volatile unsigned short FWIN1_LOW;
+	volatile unsigned short FWIN1_HIGH;
+	volatile unsigned short FWIN2_LOW;
+	volatile unsigned short FWIN2_HIGH;
+	volatile unsigned short FWIN3_LOW;
+	volatile unsigned short FWIN3_HIGH;	
+
+	volatile unsigned char PROT_LOCK;
+	volatile unsigned char PROT_CLEAR;
+	volatile unsigned char Rsvd[2];
+	volatile unsigned char SPI_FL_CFG;
+	volatile unsigned char Rsvd1;
+	volatile unsigned char UMA_CODE;
+	volatile unsigned char UMA_AB0;			/* lsb address */
+	volatile unsigned char UMA_AB1;
+	volatile unsigned char UMA_AB2;
+	volatile unsigned char UMA_DB0; // cannot make union - not on word address 
+	volatile unsigned char UMA_DB1;
+	volatile unsigned char UMA_DB2;
+	volatile unsigned char UMA_DB3;
+	volatile unsigned char UMA_CTS;
+	volatile unsigned char UMA_ECTS;
+} BOOT_SPI_REGS;
+
+#ifdef __UBOOT__	
+volatile BOOT_SPI_REGS *Regs = (volatile BOOT_SPI_REGS *)(WPCM_BOOT_SPI_BASE);
+#else
+volatile BOOT_SPI_REGS *Regs = NULL; /* will be io-remapped during init */
+#endif
+
+static
+int  
+hermon_read_xfer(int cs, unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen) ;
+
+#if (EXTENDED_WRITE == 1)
+static
+int  
+hermon_write_extended_xfer(int cs, unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen) ;
+#endif
+
+static
+int  
+hermon_write_single_xfer(int cs, unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen) ;
+
+static
+void
+InitializeCtrl(void)
+{
+
+	*(volatile unsigned long *)(GCR_MFSEL1) &= 0xFFFFFFDF ; // make gpio into SPI Chip Select 3
+	*(volatile unsigned long *)(GCR_MFSEL1) |= 0x010 ; 	// make gpio into SPI Chip Select 2
+	*(volatile unsigned long *)(GCR_MFSEL1) |= 0x08 ; 	// make gpio into SPI Chip Select 1
+
+	// set total flash size in number of 512K byte blocks
+	Regs->FIU_CFG = (CFG_PROJ_FLASH_SIZE) / 524288  ;
+	Regs->SPI_FL_CFG = (CFG_PROJ_FLASH_BANK_SIZE) / 524288 ;
+		
+	/* Set access limit registers */
+	/* Open up entire 64MB space */
+	Regs->FWIN1_LOW = 0 ;
+	Regs->FWIN1_HIGH = 0x4000 ; 
+	Regs->FWIN2_LOW = 0 ;
+	Regs->FWIN2_HIGH = 0 ;
+	Regs->FWIN3_LOW = 0 ;
+	Regs->FWIN3_HIGH = 0 ;
+
+	return;
+}
+	
+
+
+static
+int
+ValidateInputs(unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, int datalen)
+{
+	if ((cmdlen != 1) && (cmdlen != 4) && (cmdlen !=5))
+	{
+		printk("hermon_spi : Invalid Command Len %d\n",cmdlen);
+		return -1;
+	}
+	if (cmd == NULL)
+	{
+		printk("hermon_spi : Invalid : Null Command\n");
+		return -1;
+	}
+	if (dir != SPI_NONE)
+	{
+		if ( (data == NULL) || (datalen == 0))
+		{
+			printk("hermon_spi : Invalid data or datalen \n");
+			return -1;
+		}
+	}
+
+	if ((dir == SPI_READ) && (datalen > MAX_READ))
+	{
+		printk("hermon_spi : Invalid : Reads cannot exceed %d bytes\n",MAX_READ);
+		return -1;
+	}
+	
+	return 0;
+
+}
+
+
+
+static
+int  
+hermon_read_xfer(int cs, unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen)
+{
+	unsigned char uma_opcode ;
+	unsigned long src ;	
+	unsigned long transfer;
+
+	/* The command has to execute at least once for command that don't need data */
+	while(1) 
+	{
+		/* Fill opcode and address */
+		Regs->UMA_CODE = cmd[0];
+		Regs->UMA_AB0 = cmd[3];
+		Regs->UMA_AB1 = cmd[2];	
+		Regs->UMA_AB2 = cmd[1];
+
+		src = cmd[1] << 16 | cmd[2] << 8 | cmd[3];
+		
+		/* Based on len, select Data Field size */
+		switch(datalen) 
+		{
+			case 0:
+				uma_opcode = 0x0 ;
+				break ;
+			case 1:
+				uma_opcode = 0x1 ;
+				break ;
+			case 2:
+				uma_opcode = 0x2 ;
+				break ;
+			case 3:
+				uma_opcode = 0x3 ;
+				break ;
+			case 4:
+				uma_opcode = 0x4;
+				break;
+			default:
+				printk("hermon_read_xfer: ERROR Cannot Handle datalen > 4\n");
+				return -1;
+		}
+		transfer = uma_opcode;
+		
+		/* Set if Address Field to be used or not */
+		if (cmdlen != 1)
+			uma_opcode |= 0x08;		
+	
+		/* Select which bank */	
+		uma_opcode |= ((cs & 3) << 5);
+
+		/* Issue Command Trigger */
+		Regs->UMA_CTS = (uma_opcode | 0x80); 
+
+		/* wait for transaction to complete */
+		while(Regs->UMA_CTS & 0x80) ; 
+
+		/* Read data received */
+		switch(transfer)
+		{
+			case 1:
+				*data = Regs->UMA_DB0 ;
+				data++ ;
+				break ;
+			case 2:
+				*data = Regs->UMA_DB0 ;
+				data++ ;
+				*data = Regs->UMA_DB1 ;
+				data++ ;
+				break ;
+			case 3:
+				*data = Regs->UMA_DB0 ;
+				data++ ;
+				*data = Regs->UMA_DB1 ;
+				data++ ;
+				*data = Regs->UMA_DB2 ;
+				data++ ;
+				break ;
+			case 4: 
+				*data = Regs->UMA_DB0 ;
+				data++ ;
+				*data = Regs->UMA_DB1 ;
+				data++ ;
+				*data = Regs->UMA_DB2 ;
+				data++ ;
+				*data = Regs->UMA_DB3 ;
+				data++ ;
+				break ;
+		}
+	
+		datalen -= transfer ;
+		if (datalen == 0)
+			break;
+
+		/* Increment Source address */
+		src = cmd[1] << 16 | cmd[2] << 8 | cmd[3];
+		src += transfer ;
+		cmd[1] = src >> 16;
+		cmd[2] = src >> 8;
+		cmd[3] = src;
+	}  
+	return 0 ;
+}
+
+
+#if (EXTENDED_WRITE == 1)
+static
+int  
+hermon_write_extended_xfer(int cs,unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen)
+{
+	unsigned char uma_code ;
+
+
+	/* If data len < 8 do normal byte writes */
+	if (datalen < 8)
+		return hermon_write_single_xfer(cs,cmd,cmdlen,dir,data,datalen);
+	
+
+	/* ------------------------Do extended transfers --------------------*/
+	switch(cs & 3) 
+	{
+		case 0:
+			Regs->UMA_ECTS = 0xE ; // software assert cs0
+			break;
+		case 1:
+			Regs->UMA_ECTS = 0xD ; // software assert cs1
+			break;
+		case 2:
+			Regs->UMA_ECTS = 0xB ; // software assert cs2
+			break;
+		case 3:
+			Regs->UMA_ECTS = 0x7 ; // software assert cs3
+			break;
+	}
+
+	/* first is normal write uma transaction with no data xfer */
+	Regs->UMA_CODE = cmd[0];
+	Regs->UMA_AB0 = cmd[3];
+	Regs->UMA_AB1 = cmd[2];	
+	Regs->UMA_AB2 = cmd[1];
+	if (cmdlen == 1)
+		uma_code = 0x90 | ((cs &3) << 5);
+	else
+		uma_code = 0x98 | ((cs &3) << 5);
+	Regs->UMA_CTS = uma_code;
+	/* wait for transaction to complete */
+	while(Regs->UMA_CTS & 0x80) ; 
+
+	/* Xfer 8 bytes at a time */
+	/* The spi controller thinks it is issuing a normal cmd, address, and data
+	 sequence, but since Chip Select is stuck low, they are just data bytes */
+	while(datalen >= 8) 
+	{
+		Regs->UMA_CODE = *data ;
+		Regs->UMA_AB2 = *(data + 1) ;
+		Regs->UMA_AB1 = *(data + 2) ;
+		Regs->UMA_AB0 = *(data + 3) ;
+		Regs->UMA_DB0 = *(data + 4) ;
+		Regs->UMA_DB1 = *(data + 5) ;
+		Regs->UMA_DB2 = *(data + 6) ;	
+		Regs->UMA_DB3 = *(data + 7) ;
+
+		Regs->UMA_CTS = 0x9C | (cs & 3) << 5; // spi cmd, 3 address bytes and 4 data bytes
+
+		/* wait for transaction to complete */
+		while(Regs->UMA_CTS & 0x80); 
+
+		data+=8 ;
+		datalen-=8 ;
+	}
+	/* Deassert CS to stop extended transfers */
+	Regs->UMA_ECTS = 0xF ; 
+
+	return datalen; 
+}
+#endif
+
+static
+int  
+hermon_write_single_xfer(int cs,unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen)
+{
+	unsigned char uma_opcode ;
+	unsigned long transfer;	
+
+	transfer = (datalen > 4)?4:datalen;
+
+	Regs->UMA_CODE = cmd[0];
+	Regs->UMA_AB0 = cmd[3];
+	Regs->UMA_AB1 = cmd[2];	
+	Regs->UMA_AB2 = cmd[1];
+
+	if (cmdlen == 1)
+		uma_opcode = 0x90 | ((cs &3) << 5);
+	else
+		uma_opcode = 0x98 | ((cs &3) << 5);
+
+	uma_opcode |= (transfer & 0x7);
+
+	switch (transfer)
+	{
+		case 4:
+		  Regs->UMA_DB3 = *(data+3);
+		case 3:
+		  Regs->UMA_DB2 = *(data+2);
+		case 2:
+		  Regs->UMA_DB1 = *(data+1);
+		case 1:
+		  Regs->UMA_DB0 = *(data+0);
+	}
+
+	Regs->UMA_CTS = uma_opcode ;
+
+	while(Regs->UMA_CTS & 0x80) ; // wait for transaction to complete
+
+	return datalen-transfer;
+}
+
+static
+int  
+hermon_spi_burst_read(int cs, unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen)
+{
+	unsigned long transfer;
+	unsigned long src;
+	if (cs >= MAX_CHIPS)
+		return -1;
+
+ 	src = cmd[1] << 16 | cmd[2] << 8 | cmd[3];
+	while (datalen)
+	{
+		cmd[1] = src >> 16;
+		cmd[2] = src >> 8;
+		cmd[3] = src;
+		transfer = (datalen > MAX_READ)?MAX_READ:datalen;
+		if (hermon_read_xfer(cs,cmd, cmdlen, dir,data,transfer) == -1)
+			return -1; 
+		datalen-=transfer;
+		data+=transfer;
+		src+=transfer;
+	}
+	return 0;
+}
+
+		
+static
+int  
+hermon_spi_transfer(int cs,unsigned char *cmd,int cmdlen, SPI_DIR dir, 
+				unsigned char *data, unsigned long datalen)
+{
+	if (cs >= MAX_CHIPS)
+		return -1;
+
+	if (ValidateInputs(cmd,cmdlen,dir,data,datalen) == -1)
+		return -1;
+
+	if (dir == SPI_READ) {
+		return hermon_read_xfer(cs,cmd, cmdlen, dir, 
+				data, datalen) ;
+	}
+	if (dir == SPI_WRITE) {
+#if (EXTENDED_WRITE == 1)
+		return hermon_write_extended_xfer(cs, cmd,cmdlen, dir, 
+				data, datalen) ;
+#else
+		return hermon_write_single_xfer(cs, cmd,cmdlen, dir, 
+				data, datalen) ;
+#endif
+	}
+	if (dir == SPI_NONE) {
+		return hermon_read_xfer(cs,cmd, cmdlen, dir,NULL,0);
+	}
+
+	printk("hermon_spi_transfer: ERROR: Unknown direction\n");
+	return -1;
+
+}
+
+#ifdef __UBOOT__	
+extern void ll_serial_init(void);
+static int 
+hermon_spi_init(void)
+{
+	//ll_serial_init();
+	InitializeCtrl();	
+	
+	return 0;
+}
+#endif
+
+#ifndef __UBOOT__	/* linux */
+struct spi_ctrl_driver hermon_spi_driver =
+#else
+/* 
+   Uboot supports only one spi controller and has to be defined 
+   in the structure "SPI_CTRL_DRIVER"
+*/
+struct spi_ctrl_driver SPI_CTRL_DRIVER =
+#endif
+{
+	.name 		= "hermon_spi",
+	.module 	= THIS_MODULE,
+	.max_read	= MAX_READ,
+	.fast_read	= 0,
+	.spi_transfer	= hermon_spi_transfer,
+	.spi_burst_read	= hermon_spi_burst_read,
+#ifdef __UBOOT__	
+	.spi_init	= hermon_spi_init,
+#endif
+};
+
+
+#ifndef __UBOOT__	/* linux */
+static int 
+hermon_spi_init(void)
+{
+	Regs = ioremap_nocache(WPCM_BOOT_SPI_BASE,SZ_4K);
+	if (Regs == NULL)
+	{
+		printk("ERROR: Unable to ioremap WPCM450 SPI Flash Registers\n");
+		return -1;
+	}
+	InitializeCtrl();	
+	init_MUTEX(&hermon_spi_driver.lock);
+	register_spi_ctrl_driver(&hermon_spi_driver);
+	return 0;
+}
+
+
+static void 
+hermon_spi_exit(void)
+{
+	unregister_spi_ctrl_driver(&hermon_spi_driver);
+	iounmap((void*)Regs);
+	return;
+}
+
+module_init(hermon_spi_init);
+module_exit(hermon_spi_exit);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("WPCM450 SOC SPI Controller driver");
+
+#endif
