--- linux.old/drivers/net/arm/wpcm450_intr.c	1969-12-31 19:00:00.000000000 -0500
+++ linux/drivers/net/arm/wpcm450_intr.c	2008-05-13 09:58:35.000000000 -0400
@@ -0,0 +1,190 @@
+/* 
+ * Ethernet driver ( 2.6.x ) for winbond WPCM450 
+*/
+#include <linux/autoconf.h>
+#include <linux/module.h>
+#include <linux/init.h>
+
+#include <linux/interrupt.h>
+#include <linux/skbuff.h>
+#include <linux/netdevice.h>
+#include <linux/etherdevice.h>
+#include <linux/pci.h>
+
+#include <asm/irq.h>
+#include <asm/io.h>
+#include <asm/hardware.h>
+#include <asm/arch/hardware.h>
+
+#include "../../../../projdef.h"
+#include "wpcm450_mac.h"
+
+extern unsigned long wpcm_mac_io[MAX_WPCM_MAC]; 
+extern unsigned int  wpcm_mac_irq[MAX_WPCM_MAC];
+
+static
+void
+ProcessTxIntr(struct net_device *dev,unsigned long status)
+{
+    struct wpcm_mac_local *priv = (struct wpcm_mac_local *)dev->priv;
+    int which=priv->which;
+
+    unsigned long cur_ptr;
+    TXBD  *txbd;
+
+
+	/* Get the Desc currently used by TxDMA */
+	cur_ptr=wpcm_ReadReg(CTXDSA,which);
+	if (cur_ptr == 0)
+	{
+		printk("ERROR: TxIntr: Should not Happen (CTXDSA == 0)\n");
+		return;
+	}
+
+	/* Process all completed TX packets  */
+   	while ((unsigned long)&priv->tx_desc_dma[priv->cur_tx_entry] != cur_ptr)
+    {
+    	txbd =(TXBD *)&(priv->tx_desc[priv->cur_tx_entry]);
+    	priv->cur_tx_entry = (priv->cur_tx_entry+1)%(TX_DESC_SIZE);
+    	
+	    if(txbd->SL &TXDS_TXCP)
+	    	priv->stats.tx_packets++;
+	    else
+		   priv->stats.tx_errors++;    
+        
+        txbd->SL=0; 
+        txbd->mode=0;
+        
+
+		if (atomic_read(&priv->skb_count) > 0)
+		{
+			/* Decrement pending Skb connt */   
+        	atomic_dec(&priv->skb_count);
+       
+			/* Free the oldest SKB */
+        	dev_kfree_skb_any(priv->skb[priv->skb_oldest]);
+
+        	/*prepare SKB pointer for next tx_interrupt*/
+        	SKB_DO_STEP(priv->skb_oldest);
+		}
+		else
+			printk("ERROR: TxIntr: skb_count == 0\n");
+
+      	if (netif_queue_stopped(dev))	
+		{
+		//	printk("%s: Tx Descriptors Available. Resuming queue\n",dev->name);
+			netif_wake_queue(dev);
+		}
+	}
+	
+	if(status & MISTA_TXEMP)
+	{
+		printk("%s Transmit Undeflow (MISTA TXEMP). Fifo Thd = 0x%08x\n",dev->name,
+ 	 			wpcm_ReadReg(FIFOTHD,which));
+	}
+
+	if(status & MISTA_EXDEF)
+		printk("%s Defer Exceed Intr (MISTA EXDEF)\n",dev->name);
+
+    if(status & MISTA_TxBErr)
+	{
+		printk("%s Xmit Bus Error Intr (MISTA TxBErr)\n",dev->name);
+       	ResetMAC(dev);	
+	}
+
+    if(!(status & MISTA_TDU))
+    {
+    	if (netif_queue_stopped(dev))	
+		{	
+			netif_wake_queue(dev);
+        	printk("%s: TX Desc freed. netif queue woken up\n",dev->name);
+		}
+    }
+  
+    return;
+}
+
+static
+void
+ProcessRxIntr(struct net_device *dev,unsigned long status)
+{
+    struct wpcm_mac_local *priv = (struct wpcm_mac_local *)dev->priv;
+    int which=priv->which;
+
+    //printk("RxIntr!!!!! 0x%08lx\n",status);
+
+	/* If no more receive desc available or Bus error */
+    if(status & (MISTA_RDU|MISTA_RxBErr))
+    {
+		/* Process all Rx Descriptors */
+    	priv->is_rx_all=RX_DESC_SIZE;
+       	netdev_rx(dev);    
+       	priv->is_rx_all=0;
+	
+		/* Reset MAC if bus error */
+  		if(status & MISTA_RxBErr)
+  		{
+			printk("%s Recv Bus Error Intr (MISTA RxBErr)\n",dev->name);
+        	ResetMAC(dev);
+       	}
+		
+		/* Wakeup Rx State machine from halt state */
+       	wpcm_WriteReg(RSDR ,0,which);
+		return;
+		
+   	}
+
+	/* Normal Receive */
+    netdev_rx(dev);
+    return;
+}
+
+static
+irqreturn_t 
+common_interrupt(int irq, void *dev_id, char pri_intr)
+{
+    struct net_device *dev = (struct net_device *)dev_id;
+    struct wpcm_mac_local *priv = (struct wpcm_mac_local *)dev->priv;
+    unsigned long status;  
+    int which=priv->which;
+
+//    spin_lock(&priv->lock);
+
+    status=wpcm_ReadReg(MISTA,which);   
+    wpcm_WriteReg(MISTA,status,which);  
+	
+    if (pri_intr == 'T')
+	{
+    	if (status &  MISTA_TXINTR)
+			ProcessTxIntr(dev,status);
+    	if (status &  MISTA_RXINTR)
+			ProcessRxIntr(dev,status);
+	}
+    if (pri_intr == 'R')
+	{
+    	if (status &  MISTA_RXINTR)
+			ProcessRxIntr(dev,status);
+    	if (status &  MISTA_TXINTR)
+			ProcessTxIntr(dev,status);
+	}
+
+  //  spin_unlock(&priv->lock);
+
+	return IRQ_HANDLED;
+}
+
+
+
+irqreturn_t 
+_tx_interrupt(int irq, void *dev_id)
+{
+	return common_interrupt(irq,dev_id,'T');
+}
+
+irqreturn_t 
+_rx_interrupt(int irq, void *dev_id)
+{
+	return common_interrupt(irq,dev_id,'R');
+}
+
+
