--- uboot-pristine/board/HERMON/u-boot.lds	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/board/HERMON/u-boot.lds	2009-05-14 10:30:26.000000000 -0700
@@ -39,13 +39,13 @@
 	  lib_generic/libgeneric.a (.text)
 	}
 
-	. = 0x0770FF80;
+	. = 0x06F0FF80;
 	.fmh 	:
 	{
 		board/HERMON/fmh.o (.bss)
 	} 
 
-	. = 0x07710000;
+	. = 0x06F10000;
 	.text2 		:
 	{
 	  *(.text)
