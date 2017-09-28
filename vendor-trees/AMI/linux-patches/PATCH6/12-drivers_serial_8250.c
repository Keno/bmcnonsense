--- linux-2.6.15.4-cpu/drivers/serial/8250.c	2006-02-10 02:22:48.000000000 -0500
+++ linux-2.6.15.4-wpcm/drivers/serial/8250.c	2007-06-26 17:27:36.000000000 -0400
@@ -1244,6 +1244,7 @@
 		up->ier |= UART_IER_THRI;
 		serial_out(up, UART_IER, up->ier);
 
+#ifndef CONFIG_WPCM450_SERIAL
 		if (up->bugs & UART_BUG_TXEN) {
 			unsigned char lsr, iir;
 			lsr = serial_in(up, UART_LSR);
@@ -1255,6 +1266,14 @@
 				(lsr & UART_LSR_TEMT && iir & UART_IIR_NO_INT))
 				transmit_chars(up);
 		}
+#else
+		/* Winbond UART does not set the UART_BUG_TXEN correctly. Every time
+		   serial8250_startup is called up->bugs is set differenly. Such 
+		   UARTs used to work till 2.6.12.6 (before the bugs concept is 
+           introduced to the serial driver). So now irespective of the setting
+		   of bugs, treat it is a UART_BUG_TXEN */
+		transmit_chars(up);
+#endif
 	}
 
 	/*
@@ -1548,8 +1557,21 @@
 		i->head = &up->list;
 		spin_unlock_irq(&i->lock);
 
+#ifdef CONFIG_WPCM450_SERIAL
+		/* 
+		If an Intr contoller support prioritize intrs, we may need this patch.
+	    Without SA_INTERRUPT,
+	    a higher priority intr can take control while we process serial intr.
+        This causes data corruption (I don't know why).
+        Hence we are using SA_INTERRUPT to disable intrs while in the serial ISR
+		*/
+		ret = request_irq(up->port.irq, serial8250_interrupt,
+				  irq_flags|IRQF_DISABLED, "serial", i);
+#else
 		ret = request_irq(up->port.irq, serial8250_interrupt,
 				  irq_flags, "serial", i);
+#endif
+
 		if (ret < 0)
 			serial_do_unlink(i, up);
 	}
@@ -2429,6 +2451,12 @@
 		up->port.regshift = old_serial_port[i].iomem_reg_shift;
 		if (share_irqs)
 			up->port.flags |= UPF_SHARE_IRQ;
+#ifdef CONFIG_WPCM450_SERIAL
+		up->port.type 	  = PORT_16550;
+		up->port.fifosize = uart_config[up->port.type].fifo_size;
+		up->capabilities = uart_config[up->port.type].flags;
+		up->tx_loadsz = uart_config[up->port.type].tx_loadsz;
+#endif
 	}
 }
 
