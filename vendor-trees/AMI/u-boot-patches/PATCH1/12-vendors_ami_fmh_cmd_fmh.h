--- uboot/vendors/ami/fmh/cmd_fmh.h	1969-12-31 19:00:00.000000000 -0500
+++ uboot.console/vendors/ami/fmh/cmd_fmh.h	2006-10-04 11:41:08.102459609 -0400
@@ -0,0 +1,20 @@
+#ifndef __AMI_CMD_FMH_H__
+#define __AMI_CMD_FMH_H__
+
+/* For deciding on mac address */
+#define MAC_ADDR_LEN  18 		 /* no : in between */
+#define BLINK_DELAY_COUNT 200000 /* 200 msec */
+
+/* U-boot's cmd function to list and bootFMH */
+extern int  do_fmh(cmd_tbl_t *, int, int, char *[]);
+extern int  do_bootfmh(cmd_tbl_t *, int, int, char *[]);
+
+/* Actual function implementing listing of FMH*/
+extern int  ListFMH(void);	
+extern int  BootFMH(unsigned short PathID);
+extern int processMac( char *mac);
+extern int getMacKb(unsigned char *mac );
+extern int gets(char * buff,unsigned int size );
+extern int validateMac( unsigned char *s);
+
+#endif	
