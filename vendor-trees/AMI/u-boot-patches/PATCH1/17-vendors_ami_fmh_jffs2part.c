--- uboot/vendors/ami/fmh/jffs2part.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.mb/vendors/ami/fmh/jffs2part.c	2007-07-06 13:43:46.000000000 -0400
@@ -0,0 +1,93 @@
+# include <common.h>
+# include <config.h>
+
+#if defined CFG_JFFS_CUSTOM_PART
+# include <jffs2/jffs2.h>
+# include <flash.h>
+# include "fmh.h"
+
+extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
+
+#define MAX_PART	8 
+static struct part_info part[MAX_PART];
+static int 	  part_count = -1;
+
+void  
+ScanFMHforJFFS2(void)
+{	
+	flash_info_t *flinfo;
+	unsigned long SectorSize;
+	unsigned long SectorCount;	/* Uniform Size Sector Count */	
+	int i,bank;
+	FMH	*fmh;
+	MODULE_INFO *mod;
+	
+	for (bank=0;bank<CFG_MAX_FLASH_BANKS;bank++)
+	{
+		flinfo = &flash_info[bank];
+		SectorCount = flinfo->sector_count;
+		if (flinfo->size == 0)
+			break;
+
+		/* Normally Flash those have a boot sectors have one top/bottom
+	 	* sector divided into 8 smaller sectors, which means the flash 
+	 	* will have 7 more sectors leading to a odd number of sectors.
+	 	* Use this to find the actual number of uniform size sectors 
+		 */
+		if (SectorCount % 2)	
+		{
+			printf("This flash has non uniform sectors\n");
+			SectorCount = SectorCount - 8 + 1;
+		}
+	
+		SectorSize = flinfo->size/SectorCount;
+		for (i=0;i<SectorCount;)
+		{
+			fmh = ScanforFMH((unsigned char *)flinfo->start[i],SectorSize);
+			if (fmh == NULL)
+			{
+				i++;
+				continue;
+			}
+
+			/* If Module type is JFFS2, then save partition information */
+			mod = &(fmh->Module_Info);
+			if (le16_to_host(mod->Module_Type) == MODULE_JFFS2)
+			{
+				part[part_count].offset	= (flinfo->start[i] + 
+							le32_to_host(mod->Module_Location));
+				part[part_count].size		= le32_to_host(mod->Module_Size);
+				part_count++;
+				if (part_count == MAX_PART)
+						return;
+			}
+
+			/* Skip the Sectors occupied by the Module */		
+			i+=(le32_to_host(fmh->FMH_AllocatedSize)/SectorSize);
+		}
+	}
+
+	return;
+}	
+
+
+struct part_info*
+jffs2_part_info(int part_num)
+{
+	/* If First call, then scan the FMH and get JFFS2 Sections */
+	if (part_count == -1)
+	{	
+		part_count = 0;
+		memset(&part[0], 0, sizeof(struct part_info)*MAX_PART);
+		ScanFMHforJFFS2();
+	}
+
+	
+	/* Check if Unsupported partition number */
+	if (part_num >= part_count)
+		return NULL;
+
+	return &part[part_num];
+}
+
+#endif
