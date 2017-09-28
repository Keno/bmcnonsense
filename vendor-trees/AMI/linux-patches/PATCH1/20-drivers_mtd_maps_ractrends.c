--- linux/drivers/mtd/maps/ractrends.c	1969-12-31 19:00:00.000000000 -0500
+++ linux.new/drivers/mtd/maps/ractrends.c	2008-01-15 14:50:00.000000000 -0500
@@ -0,0 +1,216 @@
+
+/*
+ * ractrends.c - MTD mapper for FMH Compatible images used in ractrends
+ * 			     series of boards.
+ * 			     Creates MTD Devices for each of the JFFS or JFFS2 modules
+ * 			     present in the FMH Image
+ * 			     
+ * Copyright 2003 American Megatrends Inc.
+ *
+ */
+
+#include <linux/autoconf.h>
+#include <linux/module.h>
+#include <linux/types.h>
+#include <linux/kernel.h>
+
+#include <linux/mtd/mtd.h>
+#include <linux/mtd/map.h>
+#include <linux/mtd/concat.h>
+#include <linux/mtd/partitions.h>
+
+#include <asm/io.h>
+#include "fmh.h"
+#include "../../../../projdef.h"
+
+
+
+
+static struct mtd_partition ractrends_flash_partitions[16];
+static char partition_name [16][9];
+
+#define FULL_FLASH_PARTITION_NAME	"fullpart"
+
+unsigned long
+GetFMHPartitions(unsigned long FlashStart, unsigned long FlashSize, unsigned long SectorSize,struct mtd_info *ractrends_mtd)
+{
+	unsigned long SectorCount = FlashSize/SectorSize;
+	unsigned long i;
+	unsigned long PartCount=0;
+	FMH *fmh;
+	MODULE_INFO *mod;
+	unsigned short ModuleType;
+
+	/* Create a MTD for full Flash */
+	strncpy(partition_name[PartCount],FULL_FLASH_PARTITION_NAME,8);
+	partition_name[PartCount][9] = 0;
+	ractrends_flash_partitions[PartCount].name   = partition_name[PartCount];
+	ractrends_flash_partitions[PartCount].offset = 0;
+	ractrends_flash_partitions[PartCount].size 	 = FlashSize;
+	ractrends_flash_partitions[PartCount].mask_flags = 0;
+	printk("MTD Partition %ld : %s @ 0x%x of Size 0x%x\n",PartCount,
+						ractrends_flash_partitions[PartCount].name,
+						ractrends_flash_partitions[PartCount].offset,
+						ractrends_flash_partitions[PartCount].size);
+	PartCount++;
+	
+	for (i=0;i<SectorCount;)
+	{
+	
+		fmh = ScanforFMH((unsigned char *)(FlashStart+(i*SectorSize)),SectorSize,ractrends_mtd);
+	
+	
+		if (fmh == NULL)
+		{
+			i++;
+			continue;	
+		}
+		mod = &(fmh->Module_Info);
+
+		/* Check if module type is JFFS or JFFS2 */
+		ModuleType = le16_to_host(mod->Module_Type);
+		if ((ModuleType != MODULE_JFFS) && (ModuleType != MODULE_JFFS2) && (ModuleType != MODULE_INITRD_CRAMFS))
+		{
+			i+=(le32_to_host(fmh->FMH_AllocatedSize)/SectorSize);
+			continue;
+		}
+
+		/* Copy the Name String */
+		strncpy(partition_name[PartCount],mod->Module_Name,8);
+		partition_name[PartCount][9] = 0;
+
+		/* Fill the parition information */
+		ractrends_flash_partitions[PartCount].name   = partition_name[PartCount];
+		ractrends_flash_partitions[PartCount].offset = (i*SectorSize)
+													+ le32_to_host(mod->Module_Location);
+		ractrends_flash_partitions[PartCount].size = le32_to_host(fmh->FMH_AllocatedSize)
+													- le32_to_host(mod->Module_Location);
+		ractrends_flash_partitions[PartCount].mask_flags = 0;
+		printk("MTD Partition %ld : %s @ 0x%x of Size 0x%x\n",PartCount,
+						ractrends_flash_partitions[PartCount].name,
+						ractrends_flash_partitions[PartCount].offset,
+						ractrends_flash_partitions[PartCount].size);
+
+		i+=(le32_to_host(fmh->FMH_AllocatedSize)/SectorSize);
+		PartCount++;
+	}
+
+	return PartCount;
+}
+
+
+#define MAX_BANKS CONFIG_MTD_RACTRENDS_MAX_BANKS
+struct map_info ractrends_flash_map[MAX_BANKS];
+static struct mtd_info *ractrends_mtd[MAX_BANKS];
+static struct mtd_info *concat_mtd = NULL;
+static unsigned long bankcount= 0;
+
+int __init 
+init_ractrends_flash(void)
+{
+	unsigned long FlashAddr = CFG_PROJ_FLASH_START;
+	unsigned long FlashSize = CFG_PROJ_FLASH_SIZE;
+	unsigned long PartitionCount;
+	unsigned long bank;
+	printk(KERN_NOTICE "Ractrends Flash mapping: 0x%lx at 0x%lx\n",
+					FlashSize, FlashAddr);
+
+	FlashSize = CFG_PROJ_FLASH_SIZE/MAX_BANKS;
+	for (bank = 0; bank < MAX_BANKS; bank++)
+		ractrends_mtd[bank] = 0;
+
+	for (bank = 0; bank < MAX_BANKS; bank++)
+	{
+		printk("Probing for Flash at Bank # %ld\n",bank);
+		ractrends_flash_map[bank].name = "Ractrends";
+		ractrends_flash_map[bank].bankwidth = 2;
+		ractrends_flash_map[bank].size = FlashSize;
+		ractrends_flash_map[bank].phys = FlashAddr + (bank * FlashSize);
+		ractrends_flash_map[bank].map_priv_1 = bank;
+		ractrends_flash_map[bank].virt = (void __iomem *)ioremap(
+			ractrends_flash_map[bank].phys, ractrends_flash_map[bank].size);
+		if (!ractrends_flash_map[bank].virt)
+		{
+			if (bank == 0)
+			{
+				printk("ERROR: init_ractrends_flash: failed to ioremap\n");
+				return -EIO;
+			}
+			break;
+		}
+		ractrends_mtd[bank] = NULL;
+		simple_map_init(&ractrends_flash_map[bank]);	
+
+#if defined CFG_PROJ_FLASH_TYPE_NOR
+		ractrends_mtd[bank] = do_map_probe("cfi_probe",&ractrends_flash_map[bank]);
+#else
+#if defined CFG_PROJ_FLASH_TYPE_SPI
+		ractrends_mtd[bank] = do_map_probe("spi_probe",&ractrends_flash_map[bank]);
+#else
+#error "CFG_PROJ_FLASH_TYPE_XXX not specified (or Unsupported)  in projdef.h"
+#endif
+#endif
+		if (!ractrends_mtd[bank]) 
+		{
+			if (bank == 0)
+			{
+				printk("ERROR: init_ractrends_flash: flash probe failed\n");
+				return -ENXIO;
+			}
+			break;
+		}
+		ractrends_mtd[bank]->owner = THIS_MODULE;
+
+	}
+	bankcount = bank;
+
+	if (bank > 1)	
+		concat_mtd = mtd_concat_create(ractrends_mtd,bank,"Concat Ractrends");
+	else	
+		concat_mtd = ractrends_mtd[0];
+	
+	if (!concat_mtd)
+	{
+
+		printk("ERROR: init_ractrends_flash: flash concat failed\n");
+		return -ENXIO;
+	}
+
+
+	PartitionCount = GetFMHPartitions(0,concat_mtd->size,
+				concat_mtd->erasesize,concat_mtd);
+	return add_mtd_partitions(concat_mtd,
+			ractrends_flash_partitions,PartitionCount);
+}
+
+static void __exit 
+cleanup_ractrends_flash(void)
+{
+	unsigned long bank;
+	if (bankcount > 1)
+	{
+		del_mtd_partitions(concat_mtd);
+		map_destroy(concat_mtd);
+		concat_mtd = NULL;
+
+	}
+
+	for (bank = 0; bank < bankcount; bank++)
+	{
+		if (ractrends_mtd[bank])
+		{
+			del_mtd_partitions(ractrends_mtd[bank]);
+			map_destroy(ractrends_mtd[bank]);
+			ractrends_mtd[bank] = NULL;
+			iounmap((void *)ractrends_flash_map[bank].virt);
+			ractrends_flash_map[bank].virt = 0;
+		}
+	}
+
+}
+
+module_init(init_ractrends_flash);
+module_exit(cleanup_ractrends_flash);
+
+MODULE_AUTHOR("Samvinesh Christopher. American Megatrends Inc.");
+MODULE_DESCRIPTION("MTD map driver for the Ractrends Series of Boards");
