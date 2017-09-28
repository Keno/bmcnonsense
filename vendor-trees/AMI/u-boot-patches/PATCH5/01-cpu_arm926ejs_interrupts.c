--- u-boot-1.1.4-cpu/cpu/arm926ejs/interrupts.c	2007-06-27 14:52:10.000000000 -0400
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/interrupts.c	2007-06-27 18:10:21.000000000 -0400
@@ -175,14 +175,22 @@
 	bad_mode ();
 }
 
+#if  defined(CONFIG_WPCM450)
+extern void HandleIntr(void);
+void do_irq (struct pt_regs *pt_regs)
+{
+	HandleIntr();
+}
+#else
 void do_irq (struct pt_regs *pt_regs)
 {
 	printf ("interrupt request\n");
 	show_regs (pt_regs);
 	bad_mode ();
 }
+#endif
 
-#ifdef CONFIG_INTEGRATOR
+#if defined(CONFIG_INTEGRATOR) || defined(CONFIG_WPCM450)
 
 	/* Timer functionality supplied by Integrator board (AP or CP) */
 
