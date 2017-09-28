--- uboot-pristine/lib_arm/armlinux.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/lib_arm/armlinux.c	2009-05-14 10:30:23.000000000 -0700
@@ -125,7 +125,7 @@
 			SHOW_BOOT_PROGRESS (-10);
 #ifdef CFG_YAFU_SUPPORT 
 			       printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
-                setenv("mode", "1");
+				setenv("mode", "1");
 				fwupdate();
 #endif
 			do_reset (cmdtp, flag, argc, argv);
@@ -142,7 +142,7 @@
 			SHOW_BOOT_PROGRESS (-11);
 #ifdef CFG_YAFU_SUPPORT 
 			       printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
-                setenv("mode", "1");
+				setenv("mode", "1");
 				fwupdate();
 #endif
 			do_reset (cmdtp, flag, argc, argv);
@@ -171,7 +171,7 @@
 				printf ("Bad Data CRC\n");
 #ifdef CFG_YAFU_SUPPORT
 				printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
-                setenv("mode", "1");
+				setenv("mode", "1");
 				fwupdate();
 #endif
 				SHOW_BOOT_PROGRESS (-12);
@@ -189,7 +189,7 @@
 			SHOW_BOOT_PROGRESS (-13);
 #ifdef CFG_YAFU_SUPPORT
 				printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
-                setenv("mode", "1");
+				setenv("mode", "1");
 				fwupdate();
 #endif			
 			do_reset (cmdtp, flag, argc, argv);
