--- u-boot-1.1.4-cpu/cpu/arm926ejs/WPCM450/hermon_intr.c	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/WPCM450/hermon_intr.c	2007-06-27 18:35:09.000000000 -0400
@@ -0,0 +1,170 @@
+/* WPCM450  Intr  Routines for u-boot  */
+
+#include <common.h>
+#include <command.h>
+#include <linux/types.h>
+#include "soc_hw.h"
+
+typedef unsigned long UINT32;
+
+extern int timer_init(void);
+
+#define MAX_INTR (32)
+
+struct irq_action
+{
+	interrupt_handler_t *handler;
+	void *arg;
+	int count;
+	int spurious;
+};
+
+static struct irq_action vectors[MAX_INTR];
+
+void
+SpuriousHandler(void *IntNum)
+{
+	if ((int)IntNum < MAX_INTR)
+		printf("Spurious IRQ %d received \n",(UINT32)IntNum);
+	return;
+}
+
+int
+interrupt_init (void)
+{
+	UINT32 i;
+
+	/* Initialize to Dummy Handler */
+	for (i=0;i<MAX_INTR;i++)
+	{
+		vectors[i].handler = SpuriousHandler;
+		vectors[i].arg     = (void*)i;
+		vectors[i].count   = 0;
+		vectors[i].spurious = 0;
+		/* Set the default Trig Mode and Trig Level here for each*/
+	}
+
+	/* Clear All Current and Pending IRQ */
+
+	/* Disable All IRQ */
+	*(UINT32*)(AIC_IMR) = 0 ;
+
+	/* Enable IRQ in PSR */
+	enable_interrupts();
+
+	/* Enable Timer */
+	timer_init();
+	return 0;
+}
+
+
+void
+HandleIntr(void)
+{
+	UINT32 IntNum;
+
+//	disable_interrupts();
+
+	/* Get the Interrupts */
+	IntNum = *(UINT32*)(AIC_IPER) ;
+	IntNum = *(UINT32*)(AIC_ISNR) ;
+	
+
+	/* Spurious Interrupt on IntNum */
+	if (vectors[IntNum].handler == SpuriousHandler)
+	{
+		vectors[IntNum].spurious++;
+		(*vectors[IntNum].handler)(vectors[IntNum].arg);
+	}
+	else
+	/* Good Interrupt on IntNum */
+	{
+		vectors[IntNum].count++;
+		(*vectors[IntNum].handler)(vectors[IntNum].arg);
+	}
+
+	/* Clear the Interrupt */
+
+//	enable_interrupts();
+	return;
+}
+
+
+void
+irq_install_handler(int IntNum, interrupt_handler_t *handler, void *arg)
+{
+
+	if (IntNum >=MAX_INTR)
+	{
+		printf("ERROR: Unsupported INTR Number %d\n",IntNum);
+		return;
+	}
+
+	if (handler == NULL)
+	{
+		printf("WARNING: NULL Handler. Freeing Interrupt %d\n",IntNum);
+		irq_free_handler(IntNum);
+		return;
+	}
+
+	if (vectors[IntNum].handler != SpuriousHandler)
+	{
+		if (vectors[IntNum].handler == handler)
+		{
+			printf("WARNING: Same vector is installed for INT %d\n",IntNum);
+			return;
+		}
+		printf("Replacing vector for INT %d\n",IntNum);
+	}
+
+	disable_interrupts();
+	vectors[IntNum].handler = handler;
+	vectors[IntNum].arg = arg;
+	enable_interrupts();
+
+	/* Enable the specfic IRQ */
+	*(UINT32*)(AIC_IMR) |= (1 << IntNum) ;
+	*(UINT32*)(AIC_MECR) |= (1 << IntNum) ;
+	
+
+	return;
+}
+
+
+void
+irq_free_handler(int IntNum)
+{
+	/* Disable the specfic IRQ */
+	*(UINT32*)(AIC_MDCR) |= (1 << IntNum) ;
+
+	disable_interrupts();
+	vectors[IntNum].handler = SpuriousHandler;
+	vectors[IntNum].arg = (void *)IntNum;
+	enable_interrupts();
+	return;
+
+}
+
+#if (CONFIG_COMMANDS & CFG_CMD_IRQ)
+
+int
+do_irqinfo(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
+{
+	int vec;
+
+	printf ("\nInterrupt-Information:\n");
+	printf ("Num  Routine   Arg       Count  Spurious\n");
+
+	for (vec=0; vec<MAX_INTR; vec++)
+	{
+		printf ("%02d   %08lx  %08lx      %d         %d\n",
+				vec,
+				(ulong)vectors[vec].handler,
+				(ulong)vectors[vec].arg,
+				vectors[vec].count,
+				vectors[vec].spurious);
+	}
+	return 0;
+}
+
+#endif  /* CONFIG_COMMANDS & CFG_CMD_IRQ */
