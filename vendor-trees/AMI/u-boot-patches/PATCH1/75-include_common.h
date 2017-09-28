--- uboot/include/common.h	2005-12-16 11:39:27.000000000 -0500
+++ uboot.new/include/common.h	2007-11-20 16:17:15.000000000 -0500
@@ -211,6 +211,7 @@
 char	*getenv	     (char *);
 int	getenv_r     (char *name, char *buf, unsigned len);
 int	saveenv	     (void);
+int Getenvvar(unsigned char *count,unsigned char *VarName,int *BootVarlen);
 #ifdef CONFIG_PPC		/* ARM version to be fixed! */
 void inline setenv   (char *, char *);
 #else
