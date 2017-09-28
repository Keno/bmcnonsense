--- uboot/vendors/ami/r2c/cmd_r2c.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.console/vendors/ami/r2c/cmd_r2c.h	2006-10-04 11:41:08.119459723 -0400
@@ -0,0 +1,17 @@
+#ifndef __AMI_CMD_R2C_H__
+#define __AMI_CMD_R2C_H__
+
+/* U-boot's cmd function enter remote recovery console session */
+extern int  do_r2c (cmd_tbl_t *, int, int, char *[]);
+
+U_BOOT_CMD(				
+	r2c,	1,	0,	do_r2c,				
+	"r2c     - Enter AMI's Remote Recovery Console Session\n",	
+	"    - Enter AMI's Remote Recovery Console Session\n"		
+);
+
+
+/* Actual function implementing r2c protocol*/
+extern int  rconsole(void);	
+
+#endif	
