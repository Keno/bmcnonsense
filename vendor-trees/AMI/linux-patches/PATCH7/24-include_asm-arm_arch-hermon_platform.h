--- linux-2.6.15.4-wpcm/include/asm-arm/arch-hermon/platform.h	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wb/include/asm-arm/arch-hermon/platform.h	2007-06-18 23:15:48.000000000 -0400
@@ -0,0 +1,49 @@
+/*
+ *  linux/include/asm-arm/arch-hermon/platform.h
+ *
+ *  This file contains the WPCM450 SOC specific values
+ *
+ *  Copyright (C) 1999 ARM Limited.
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
+ */
+
+#ifndef     _PLATFORM_HERMON_H_
+#define     _PLATFORM_HERMON_H_
+
+#include <asm/arch/hardware.h>
+
+#include <asm/soc-wpcm450/hwmap.h>
+#include <asm/soc-wpcm450/hwreg.h>
+#include <asm/soc-wpcm450/hwdef.h>
+
+#include <asm/soc-wpcm450/mapdesc.h>
+
+#include <asm/soc-wpcm450/macreg.h>
+#include <asm/soc-wpcm450/serreg.h>
+
+/*****************************************************************
+						    Flash
+*****************************************************************/
+#define WPCM_FLASH_BASE                  0x40000000
+#define WPCM_FLASH_SZ                    0x00800000		/* 8 MB */
+#define WPCM_FLASH_VA_BASE               MEM_ADDRESS(WPCM_FLASH_BASE)
+
+/* Ractrends MTD Map driver require the following defines */
+#define CPE_FLASH_BASE					 WPCM_FLASH_BASE
+#define CPE_FLASH_SZ					 WPCM_FLASH_SZ
+
+#endif
