--- linux-2.6.15.4-wpcm/arch/arm/kernel/head.S	2007-06-20 11:37:52.000000000 -0400
+++ linux-2.6.15.4-wb/arch/arm/kernel/head.S	2007-06-25 14:09:28.000000000 -0400
@@ -148,6 +148,18 @@
 #endif /* defined(CONFIG_SMP) */
 
 
+#ifdef CONFIG_ARCH_HERMON
+/*
+ * This provides a C-API version of the printascii
+ */
+ENTRY(EarlyPrintk_Asm)
+	stmfd	sp!, {r0 - r12, lr}
+	bl	EarlyPrintk
+	ldmfd	sp!, {r0 - r12, pc}
+
+#endif
+
+
 
 /*
  * Setup common bits before finally enabling the MMU.  Essentially
