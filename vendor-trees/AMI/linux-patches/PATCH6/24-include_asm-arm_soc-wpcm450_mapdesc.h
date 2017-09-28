--- linux-2.6.15.4-cpu/include/asm-arm/soc-wpcm450/mapdesc.h	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wpcm/include/asm-arm/soc-wpcm450/mapdesc.h	2007-06-25 14:29:19.000000000 -0400
@@ -0,0 +1,8 @@
+#ifndef WPCM450_MAP_DESC
+
+#define WPCM450_MAP_DESC					\
+{WPCM_REGISTER_VA_BASE, __phys_to_pfn(WPCM_REGISTER_BASE), SZ_256M, MT_DEVICE }
+
+#endif
+
+
