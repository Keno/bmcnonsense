--- u-boot-1.1.4.org/lib_arm/armlinux.c	2008-02-21 09:33:37.000000000 -0500
+++ u-boot-1.1.4/lib_arm/armlinux.c	2008-02-21 09:37:00.000000000 -0500
@@ -73,6 +73,9 @@
 
 extern image_header_t header;	/* from cmd_bootm.c */
 
+#ifdef CFG_YAFU_SUPPORT
+extern int fwupdate(void);
+#endif
 
 void do_bootm_linux (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[],
 		     ulong addr, ulong *len_ptr, int verify)
@@ -115,6 +118,10 @@
 		if (ntohl (hdr->ih_magic) != IH_MAGIC) {
 			printf ("Bad Magic Number\n");
 			SHOW_BOOT_PROGRESS (-10);
+#ifdef CFG_YAFU_SUPPORT 
+			       printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+				fwupdate();
+#endif
 			do_reset (cmdtp, flag, argc, argv);
 		}
 
@@ -127,6 +134,10 @@
 		if (crc32 (0, (char *) data, len) != checksum) {
 			printf ("Bad Header Checksum\n");
 			SHOW_BOOT_PROGRESS (-11);
+#ifdef CFG_YAFU_SUPPORT 
+			       printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+				fwupdate();
+#endif
 			do_reset (cmdtp, flag, argc, argv);
 		}
 
@@ -151,6 +162,10 @@
 			csum = crc32 (0, (char *) data, len);
 			if (csum != ntohl (hdr->ih_dcrc)) {
 				printf ("Bad Data CRC\n");
+#ifdef CFG_YAFU_SUPPORT
+				printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+				fwupdate();
+#endif
 				SHOW_BOOT_PROGRESS (-12);
 				do_reset (cmdtp, flag, argc, argv);
 			}
@@ -164,6 +179,10 @@
 		    (hdr->ih_type != IH_TYPE_RAMDISK)) {
 			printf ("No Linux ARM Ramdisk Image\n");
 			SHOW_BOOT_PROGRESS (-13);
+#ifdef CFG_YAFU_SUPPORT
+				printf ("Jumping to Firmware Update Mode... waits for 2 minutes for somebody to start firmware upgrade process\n");
+				fwupdate();
+#endif			
 			do_reset (cmdtp, flag, argc, argv);
 		}
 
