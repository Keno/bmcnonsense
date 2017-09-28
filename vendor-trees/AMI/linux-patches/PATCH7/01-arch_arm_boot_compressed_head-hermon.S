--- linux-2.6.15.4-wpcm/arch/arm/boot/compressed/head-hermon.S	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wb/arch/arm/boot/compressed/head-hermon.S	2007-06-18 22:53:13.000000000 -0400
@@ -0,0 +1,5 @@
+#include <asm/mach-types.h>
+
+		.section	".start", "ax"
+		mov     r7,     #(MACH_TYPE_HERMON & 0xff)
+	        orr     r7, r7, #(MACH_TYPE_HERMON & 0xff00)
