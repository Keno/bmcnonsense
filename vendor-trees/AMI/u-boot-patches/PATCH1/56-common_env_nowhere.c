--- u-boot-1.1.4-cpu/common/env_nowhere.c	2007-06-27 14:52:02.000000000 -0400
+++ u-boot-1.1.4-wpcm/common/env_nowhere.c	2007-06-27 14:39:36.000000000 -0400
@@ -64,4 +64,8 @@
 	return (0);
 }
 
+int saveenv(void)
+{
+	return 0;
+}
 #endif /* CFG_ENV_IS_NOWHERE) */
