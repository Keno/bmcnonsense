--- linux-2.6.15.4-wpcm/arch/arm/mach-hermon/Makefile.boot	1969-12-31 19:00:00.000000000 -0500
+++ linux-2.6.15.4-wb/arch/arm/mach-hermon/Makefile.boot	2007-06-18 22:52:41.000000000 -0400
@@ -0,0 +1,4 @@
+   zreladdr-y	:= 0x00008000
+params_phys-y	:= 0x00000100
+initrd_phys-y	:= 0x00900000
+
