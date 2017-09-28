--- u-boot-1.1.4-cpu/include/wpcm450_hw.h	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/include/wpcm450_hw.h	2007-06-27 17:59:40.000000000 -0400
@@ -0,0 +1,15 @@
+#ifndef _WPCM450_HW_H_
+#define _WPCM450_HW_H_
+
+#include <config.h>
+
+/* U-Boot does not use MMU. So no mapping */
+#define IO_ADDRESS(x)	(x)
+#define MEM_ADDRESS(x)  (x)
+
+#include <wpcm450/hwmap.h>
+#include <wpcm450/hwreg.h>
+#include <wpcm450/hwdef.h>
+#include <wpcm450/serreg.h>
+#include <wpcm450/macreg.h>
+#endif
