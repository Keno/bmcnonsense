--- uboot.org/common/env_common.c	2007-10-31 13:00:18.000000000 -0400
+++ uboot/common/env_common.c	2007-11-06 08:59:55.000000000 -0500
@@ -57,6 +57,8 @@
 static uchar env_get_char_init (int index);
 uchar (*env_get_char)(int) = env_get_char_init;
 
+int gDefaultEnvironment = 0;
+
 /************************************************************************
  * Default settings to be used when no valid environment is found
  */
@@ -206,6 +208,7 @@
 {
 	DECLARE_GLOBAL_DATA_PTR;
 
+	gDefaultEnvironment = 0;
 	DEBUGF ("%s[%d] offset = 0x%lx\n", __FUNCTION__,__LINE__,
 		gd->reloc_off);
 
@@ -238,6 +241,7 @@
 		puts ("Using default environment\n\n");
 #else
 		puts ("*** Warning - bad CRC, using default environment\n\n");
+		gDefaultEnvironment = 1;
 		SHOW_BOOT_PROGRESS (-1);
 #endif
 
