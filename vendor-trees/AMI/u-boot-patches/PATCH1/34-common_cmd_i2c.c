--- u-boot-1.1.4/common/cmd_i2c.c	2005-12-16 11:39:27.000000000 -0500
+++ uboot.work/common/cmd_i2c.c	2007-02-26 10:35:58.000000000 -0500
@@ -930,4 +930,28 @@
 	"      (valid chip values 50..57)\n"
 );
 #endif
+
+static unsigned char  active_i2c_bus = 0;
+int do_i2c_bus  ( cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
+{
+	if (argc > 2) 
+	{
+		printf ("Usage:\n%s\n", cmdtp->usage);
+		return 1;
+	}
+
+	if (argc == 1)
+		printf("Selected i2c Bus = %d\n",active_i2c_bus);
+
+	active_i2c_bus = simple_strtoul(argv[1],NULL,16);
+	return 0;
+}
+
+U_BOOT_CMD(
+	i2cbus,	2,	1,	do_i2c_bus,
+	"i2cbus  - set/show the currently selected i2c bus\n",
+	"BusNum\n"
+	"    - set/show the currenly selected i2c bus \n"
+);
+
 #endif	/* CFG_CMD_I2C */
