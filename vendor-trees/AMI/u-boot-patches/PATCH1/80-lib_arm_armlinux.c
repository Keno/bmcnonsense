--- uboot/lib_arm/armlinux.c	2009-03-09 16:10:44.000000000 +0530
+++ uboot/lib_arm/armlinux.c	2009-03-11 12:37:05.000000000 +0530
@@ -126,4 +126,5 @@
 #ifdef CFG_YAFU_SUPPORT 
 			       printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+                setenv("mode", "1");
 				fwupdate();
 #endif
@@ -142,4 +143,5 @@
 #ifdef CFG_YAFU_SUPPORT 
 			       printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+                setenv("mode", "1");
 				fwupdate();
 #endif
@@ -170,4 +172,5 @@
 #ifdef CFG_YAFU_SUPPORT
 				printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+                setenv("mode", "1");
 				fwupdate();
 #endif
@@ -187,4 +190,5 @@
 #ifdef CFG_YAFU_SUPPORT
 				printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+                setenv("mode", "1");
 				fwupdate();
 #endif			
