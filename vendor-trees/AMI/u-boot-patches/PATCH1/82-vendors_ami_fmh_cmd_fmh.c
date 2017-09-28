--- uboot-pristine/vendors/ami/fmh/cmd_fmh.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/vendors/ami/fmh/cmd_fmh.c	2009-05-14 10:30:24.000000000 -0700
@@ -11,6 +11,7 @@
 # define  KERNEL_LOADADDR	0xC00000
 # define  KERNEL_FILE		"/boot/uImage"
 
+extern int UseNCSI;
 
 /* Note: Currently FMH support is done only for the first bank */
 extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS];
@@ -231,6 +232,7 @@
 	unsigned long initrd_address = 0xFFFFFFFF;
 	char INITRDAddrStr[] = "0xFFFFFFFF";
 	char baudrate_str[16];
+	char usencsistr[32];
 
 	sprintf(baudrate_str,"%ld",CONFIG_BAUDRATE);
 
@@ -485,6 +487,8 @@
 #ifdef CONFIG_BIGPHYSAREA
 		strcat(bootargs," bigphysarea=" CONFIG_BIGPHYSAREA);
 #endif		
+		sprintf(usencsistr, " usencsi=%d", UseNCSI);
+		strcat(bootargs, usencsistr);
 #ifdef CONFIG_EXTRA_BOOTARGS 
 		strcat(bootargs," " CONFIG_EXTRA_BOOTARGS);
 #endif
