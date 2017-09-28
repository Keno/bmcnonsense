--- uboot-pristine/drivers/spiflash/m25pxx.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/drivers/spiflash/m25pxx.c	2009-05-14 10:30:27.000000000 -0700
@@ -50,6 +50,8 @@
 	{ "ST Micro m45p32"  , 0x20, 0x1640, 0x400000 , 1, {{ 0, 64  * 1024, 64  },} },
 	{ "ST Micro m45p64"  , 0x20, 0x1740, 0x800000 , 1, {{ 0, 64  * 1024, 128 },} },
 
+	/* [SMC] added for Macronix 128 MBIT flash */
+	{ "Macrox" , 0xC2, 0x1820, 0x1000000 , 1, {{ 0, 64  * 1024, 256 },} }
 };
 
 
