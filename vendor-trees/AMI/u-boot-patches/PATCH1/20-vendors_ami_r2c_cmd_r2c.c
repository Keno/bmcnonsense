--- uboot/vendors/ami/r2c/cmd_r2c.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.nonet/vendors/ami/r2c/cmd_r2c.c	2008-04-01 08:35:11.000000000 -0400
@@ -0,0 +1,42 @@
+# include <common.h>
+# include <config.h>
+# include <command.h>
+# include <net.h>
+# include "cmd_r2c.h"
+# include "externs.h"
+
+#ifdef CFG_YAFU_SUPPORT
+extern int fwupdate(void);
+#endif
+
+/* U-boot's cmd function enter remote recovery console session */
+int  
+do_r2c (cmd_tbl_t *cmdtp, int flag , int argc, char *argv[])
+{
+	return rconsole();
+}
+
+
+/* Actual function implementing r2c protocol*/
+/* Never Returns. Can reset card either using r2c 
+ * reset command or ctrl-c on the terminal console*/
+int  
+rconsole(void)
+{
+#ifdef CFG_YAFU_SUPPORT
+	setenv("mode", "1"); 
+	fwupdate();
+
+#else
+#if (CONFIG_COMMANDS & CFG_CMD_NET)
+	while (1)
+	{
+		printf("Entering AMI r2c console\n");
+		CallFromNetLoop = 1;
+		NetLoop(R2C);
+	}
+#endif
+#endif
+	return 0;
+}	
+
