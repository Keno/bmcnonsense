--- uboot-pristine/board/HERMON/clock_init.c	2009-05-14 10:29:19.000000000 -0700
+++ uboot-changed/board/HERMON/clock_init.c	2009-05-14 10:30:26.000000000 -0700
@@ -70,6 +70,18 @@
   volatile UINT32 temp ;
 //	UINT32 PLLCON180 = 0x002C2005;
 
+// joe --- begin
+  UINT32 INTCR2	= (WPCM_GCR_VA_BASE + 0x0060);
+
+  /************************************************************************/
+  /* If WDR set, then clear it                 */
+  /************************************************************************/
+  if(IS_BIT_SET(*(volatile unsigned long*)INTCR2, 31))
+  {
+	CLEAR_BIT(*(volatile unsigned long *)INTCR2, 31);	
+  }
+// joe --- end
+
   /************************************************************************/
   /* If watchdog reset was done. Don't do any clock init                  */
   /************************************************************************/
