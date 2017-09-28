--- linux.old/drivers/net/arm/wpcm450_mac.c	1969-12-31 19:00:00.000000000 -0500
+++ linux/drivers/net/arm/wpcm450_mac.c	2008-05-13 10:20:15.000000000 -0400
@@ -0,0 +1,752 @@
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
+#include <linux/delay.h>
+
+#include <asm/irq.h>
+#include <asm/io.h>
+#include <asm/hardware.h>
+#include <asm/arch/hardware.h>
+
+#include "../../../../projdef.h"
+#include "wpcm450_mac.h"
+
+extern void DetectPhy(int num);	// [SMC]
+extern int ResetPhyChip(int num);	// [SMC]
+
+#if defined(CFG_PROJ_NETWORK_ETH0)
+unsigned long wpcm_mac_io[MAX_WPCM_MAC]  =  { WPCM_MAC1_VA_BASE };
+unsigned int  wpcm_mac_irq[MAX_WPCM_MAC] =  { IRQ_MAC_1_RX };
+#elif defined(CFG_PROJ_NETWORK_ETH1)
+unsigned long wpcm_mac_io[MAX_WPCM_MAC]  =  { WPCM_MAC2_VA_BASE };
+unsigned int  wpcm_mac_irq[MAX_WPCM_MAC] =  { IRQ_MAC_2_RX };
+#elif defined(CFG_PROJ_NETWORK_BOTH)
+unsigned long wpcm_mac_io[MAX_WPCM_MAC]  =  { WPCM_MAC1_VA_BASE, WPCM_MAC2_VA_BASE };
+unsigned int  wpcm_mac_irq[MAX_WPCM_MAC] =  { IRQ_MAC_1_RX, IRQ_MAC_2_RX };
+#endif
+
+/* Global variables  used for MAC driver */
+static unsigned long  	gMCMDR = MCMDR_EnMDC | MCMDR_ACP;
+static unsigned long  	gMIEN = MIEN_EnTXINTR | MIEN_EnRXINTR | MIEN_EnRXGD | MIEN_EnTXCP |
+                                MIEN_EnTxBErr | MIEN_EnRxBErr | MIEN_EnTXABT | MIEN_EnRDU
+								| MIEN_EnTXEMP;
+
+static void init_rxtx_rings(struct net_device *dev);
+static int send_frame(struct net_device * ,unsigned char *,int);
+
+static int wpcm_open(struct net_device *dev);
+static int wpcm_stop(struct net_device *dev);
+
+// joe --- begin
+static unsigned char AlreadyInit=0;
+// joe --- end
+
+void 
+wpcm_WriteCam(int which,int x, unsigned char *pval)
+{
+	unsigned int msw,lsw;
+	
+ 	msw = (pval[0] << 24) | (pval[1] << 16) |
+        		(pval[2] << 8) | (pval[3]);
+ 	lsw = (pval[4] << 24) | (pval[5] << 16);
+
+ 	wpcm_WriteCam0(which,x,lsw,msw);
+	return;
+}
+#if 0
+void 
+ResetMAC(struct net_device * dev)
+{
+	struct wpcm_mac_local * priv=(struct wpcm_mac_local *)dev->priv;
+	int    which=priv->which ;
+	unsigned long val=wpcm_ReadReg(MCMDR,which);
+	
+	spin_lock(&priv->lock);
+
+	/* Stop Xmit and Recv */	
+	wpcm_WriteReg(FIFOTHD,0x10000,which); 
+	wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)&~(MCMDR_TXON|MCMDR_RXON),which);
+
+	/* Reset everything */
+	printk("%s: Resetting ...",dev->name);
+	wpcm_WriteReg(MCMDR,MCMDR_SWR,which);
+	while (wpcm_ReadReg(MCMDR,which) & MCMDR_SWR)
+		printk(".");
+	printk("\n");
+
+	/* Program Fifo Threasholds */
+	wpcm_WriteReg(FIFOTHD,0x100300,which);
+
+	if(!netif_queue_stopped(dev))
+	{
+		netif_stop_queue(dev);
+	}
+
+	/* Intitialize Rings and Internal data */
+   	init_rxtx_rings(dev);
+   	priv->cur_tx_entry=0;
+	priv->tx_index=0;
+	priv->rx_index=0;
+	
+	/* Write descriptors address to registers */
+	wpcm_WriteReg(RXDLSA,(unsigned long)priv->rx_desc_dma,which);
+	wpcm_WriteReg(TXDLSA,(unsigned long)priv->tx_desc_dma,which);
+
+	/* Max packet that can be received */
+	wpcm_WriteReg(DMARFC,PACKET_SIZE,which);
+  
+	/* Write our MAC Address and Enable broadcast,multicast and our packets */	
+	wpcm_WriteCam(priv->which,0,dev->dev_addr);
+	wpcm_WriteReg(CAMEN,wpcm_ReadReg(CAMEN,which) | 1,which);
+  	wpcm_WriteReg(CAMCMR,CAMCMR_ECMP|CAMCMR_ABP|CAMCMR_AMP,which);
+
+    /* Configure the MAC control registers. */
+    wpcm_WriteReg(MIEN,gMIEN,which);    	
+
+	/* ReEnable Recv and Xmit flags */
+    wpcm_WriteReg(MCMDR,MCMDR_TXON|MCMDR_RXON|val,which);
+	
+	/* Tell RX and TX state machines to exit halt state */
+    wpcm_WriteReg(TSDR ,0,which);
+    wpcm_WriteReg(RSDR ,0,which);
+  
+	/* Clear Any pending Interrupts */
+    wpcm_WriteReg(MISTA,wpcm_ReadReg(MISTA,which),which); 
+
+    dev->trans_start = jiffies;
+	if(netif_queue_stopped(dev))
+	{
+		netif_wake_queue(dev);
+	}
+	
+	spin_unlock(&priv->lock);
+	return;
+}
+#else
+void 
+ResetMAC(struct net_device * dev)
+{
+	wpcm_stop(dev);
+	udelay(1000);
+	wpcm_open(dev);
+}
+#endif
+
+static int 
+wpcm_set_mac_address(struct net_device *dev, void *addr)
+{ 
+
+	if(netif_running(dev))
+	{
+		printk("ERROR: Cannot Change MAC Address when Interface is up\n");
+		return -EBUSY;
+	}
+
+	memcpy(dev->dev_addr,addr,ETH_ALEN);
+		
+	printk("%s : MAC Address %x:%x:%x:%x:%x:%x\n",dev->name,
+			dev->dev_addr[0],
+			dev->dev_addr[1],
+			dev->dev_addr[2],
+			dev->dev_addr[3],
+			dev->dev_addr[4],
+			dev->dev_addr[5]);
+	
+	return 0;
+}
+
+static void 
+init_rxtx_rings(struct net_device *dev)
+{
+	int i;
+	struct wpcm_mac_local * wpcm_mac_local=dev->priv;
+	
+	
+	for ( i = 0 ; i < TX_DESC_SIZE ; i++ )
+	{
+	   	wpcm_mac_local->tx_desc[i].SL=0;
+	   	wpcm_mac_local->tx_desc[i].mode=0;
+		wpcm_mac_local->tx_desc[i].buffer=
+				(unsigned long)wpcm_mac_local->tx_buf_dma + (PACKET_BUFFER_SIZE*i);
+		wpcm_mac_local->tx_desc[i].next=
+				(unsigned long)wpcm_mac_local->tx_desc_dma + (sizeof(TXBD)*(i+1));
+	}
+	wpcm_mac_local->tx_desc[i-1].next=(unsigned long)wpcm_mac_local->tx_desc_dma;
+	
+	for( i =0 ; i < RX_DESC_SIZE ; i++)
+	{	 	   
+		wpcm_mac_local->rx_desc[i].SL=RXfOwnership_DMA;
+		wpcm_mac_local->rx_desc[i].buffer=
+				(unsigned long)wpcm_mac_local->rx_buf_dma + (PACKET_BUFFER_SIZE*i);
+		wpcm_mac_local->rx_desc[i].next=
+				(unsigned long)wpcm_mac_local->rx_desc_dma + (sizeof(RXBD)*(i+1));
+	}	
+	wpcm_mac_local->rx_desc[i-1].next=(unsigned long)wpcm_mac_local->rx_desc_dma;
+    	
+}
+
+#ifdef POLL_LINK_STATUS
+static void 
+wpcm_autodetect(unsigned long arg)
+{
+	struct net_device * dev =(struct net_device *)arg;
+    struct wpcm_mac_local *  priv=(struct wpcm_mac_local *)dev->priv;
+    int which=priv->which;
+	int Link;
+
+	if (!IsPhyAccessLocked())
+	{
+//		Link = GetLinkStatus(which);
+		if( which == 1 )
+		{
+			// ETH1 is NCSI in simso so. link is always up.
+			Link = 1;
+		}
+		else
+		{
+			Link = GetLinkStatus(which);
+		}
+
+		/* If Link is down */
+		if (!Link)
+		{
+			if(!priv->LinkDown)
+			{
+				printk("%s: Link Down\n",dev->name);
+				priv->LinkDown=1;
+			}
+		}
+		else
+		{
+			if(priv->LinkDown)
+			{
+				printk("%s: Link Up\n",dev->name);
+				priv->LinkDown=0;
+				ResetMAC(dev);
+			}
+		}
+	}
+	
+    priv->timer.expires = jiffies +POLL_TIMEOUT*HZ;
+    add_timer(&priv->timer);
+}
+#endif
+
+extern int Set100MBFullDuplex(int which);	// [SMC]
+
+static int   
+wpcm_open(struct net_device *dev)
+{
+  	struct wpcm_mac_local * priv;
+  	int    which ;
+
+  	priv=(struct wpcm_mac_local *)dev->priv;
+  	which= priv->which; 
+  	
+	/* Reset everything */
+	printk("%s: Resetting ...",dev->name);
+	wpcm_WriteReg(MCMDR,MCMDR_SWR,which);
+	while (wpcm_ReadReg(MCMDR,which) & MCMDR_SWR)
+		printk(".");
+	printk("\n");
+
+	/* Initilialize Ring and internal data */  	
+  	init_rxtx_rings(dev);
+ 	priv->cur_tx_entry=0;
+    priv->tx_index=0;
+	priv->rx_index=0;
+
+    priv->skb_oldest=0;
+    priv->skb_newest=0;
+    atomic_set(&priv->skb_count,0);
+
+	/* Program Fifo Threasholds */
+  	wpcm_WriteReg(FIFOTHD,0x200300,which); 
+
+	/* Write descriptors address to registers */
+  	wpcm_WriteReg(RXDLSA,(unsigned long)priv->rx_desc_dma,which);
+  	wpcm_WriteReg(TXDLSA,(unsigned long)priv->tx_desc_dma,which);
+
+	/* Max packet that can be received */
+  	wpcm_WriteReg(DMARFC,PACKET_SIZE,which);
+  	
+	/* Write our MAC Address and Enable broadcast,multicast and out packets */	
+  	wpcm_WriteCam(priv->which,0,dev->dev_addr);
+	wpcm_WriteReg(CAMEN,wpcm_ReadReg(CAMEN,which) | 1,which);
+  	wpcm_WriteReg(CAMCMR,CAMCMR_ECMP|CAMCMR_ABP|CAMCMR_AMP,which);
+  
+	/* Enable MDC clock generation */	
+  	wpcm_WriteReg(MCMDR,MCMDR_EnMDC,which);
+  	
+	if( which == 1 )
+	{
+		// ETH1 is NCSI port. So, we always say Full Duplex and 100Mbps
+		printk("%s: 100MB - ",dev->name);
+		wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_OPMOD,which);
+		printk("Full Duplex\n");
+		wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_FDUP,which);
+
+		//Set100MBFullDuplex(which);   // [SMC] added    		
+		priv->LinkDown=0;
+	}
+	else
+	{
+		/* Detect Phy and enable Auto negotiation */
+		/* This also sets Duplex,Speed to MCMDR register */
+	    DetectPhy(which);
+	    if(ResetPhyChip(which)==1)
+		priv->LinkDown=1;
+	    else
+    		priv->LinkDown=0;
+	}
+
+  	
+    /* Configure the MAC control registers. */
+    wpcm_WriteReg(MIEN,gMIEN,which);
+    wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|gMCMDR,which);
+
+	/* Enable Reception and Xmission*/
+    wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_RXON|MCMDR_TXON,which);
+   	
+    priv->mcmdr=wpcm_ReadReg(MCMDR,which);
+    priv->rx_packets=0;
+	priv->rx_bytes=0;
+
+	dev->trans_start = jiffies;
+	netif_start_queue(dev);
+	
+	/* Tell RX state machine to exit Halt State */
+    wpcm_WriteReg(RSDR ,0,which);
+
+    return 0;
+}
+ 
+
+static int   
+wpcm_stop(struct net_device *dev)
+{
+	struct wpcm_mac_local *priv=(struct wpcm_mac_local *)dev->priv;
+	int which=priv->which;
+
+
+	/* Stop everyting */	
+	netif_stop_queue(dev);
+	wpcm_WriteReg(MCMDR,0,which);
+
+	/* Free any pending SKB */
+	while (atomic_read(&priv->skb_count) > 0)
+	{
+		/* Decrement pending Skb connt */   
+       		atomic_dec(&priv->skb_count);
+       
+		/* Free the oldest SKB */
+        	dev_kfree_skb_any(priv->skb[priv->skb_oldest]);
+
+        	/*prepare SKB pointer for next tx_interrupt*/
+        	SKB_DO_STEP(priv->skb_oldest);
+	}
+
+     //   if (netif_queue_stopped(dev))	
+     //		netif_wake_queue(dev);
+
+	return 0;
+}
+
+ 
+static struct net_device_stats * 
+wpcm_get_stats(struct net_device *dev)    
+{
+	struct wpcm_mac_local *priv = (struct wpcm_mac_local *)dev->priv;
+	
+	return &priv->stats;
+}
+
+static void 
+wpcm_timeout(struct net_device *dev)
+{
+	printk("%s: Timeout\n",dev->name);
+	ResetMAC(dev);
+
+}
+
+static int 
+wpcm_start_xmit(struct sk_buff *skb, struct net_device *dev)
+{
+	struct wpcm_mac_local *priv = (struct wpcm_mac_local *)dev->priv;
+    unsigned long status;
+
+    if (atomic_read(&priv->skb_count) == SKB_NUMBER)
+    {
+       	printk("ERROR: wpcm_start_xmit failed - SKB slot unavailable\n");        
+       	return -1;	
+    }
+
+    priv->skb[priv->skb_newest] = skb;
+
+	/* atomic counter increment */
+	atomic_inc(&priv->skb_count);
+
+    if(!(status = send_frame(dev,skb->data,skb->len)))   
+    {
+       	//Prepare SKB pointer for next transmit
+       	SKB_DO_STEP(priv->skb_newest);
+        
+       	return 0;
+    }
+	atomic_dec(&priv->skb_count);
+    printk("ERROR: wpcm_start_xmit - send_frame() failed\n");        
+    return -1;	
+}
+
+
+
+/* We have a good packet(s), get it/them out of the buffers. */
+void 
+netdev_rx(struct net_device *dev)
+{
+	struct wpcm_mac_local * priv = (struct wpcm_mac_local *)dev->priv;
+	RXBD *rxbd;
+	unsigned long length;
+	unsigned long status;
+	int flag=0;
+
+	rxbd=(RXBD *)&priv->rx_desc[priv->rx_index];
+
+    do
+    {
+		if(priv->is_rx_all>0)
+		{
+			flag=1;
+    		--priv->is_rx_all;
+    	}
+    	else 
+		{
+			if(flag==1)
+    		{
+    				flag=0;
+    				break;
+    		}
+        	else 
+			{
+				if (wpcm_ReadReg(CRXDSA,priv->which)== (unsigned long)&priv->rx_desc_dma[priv->rx_index])
+    	   				break;
+			}
+    	}
+        
+    	length = rxbd->SL & 0xFFFF;
+    	status = rxbd->SL & 0x3FFF0000;
+
+		if (status & 0x80000000)
+			printk("WARNING: Read traverse Empty\n");
+		
+    
+		/* Status is Receive Good */	
+    	if(status & RXDS_RXGD)     
+    	{
+			unsigned char  * data;
+			struct sk_buff * skb;
+
+			/* Get the data */
+			data = (unsigned char*)&priv->rx_buf[priv->rx_index*PACKET_BUFFER_SIZE];
+
+			/* Get a Free SKB Buffer */
+			skb = dev_alloc_skb(length+2);  
+			if(!skb) 
+			{
+				printk("ERROR: %s Don't have free skb to receive. Dropping packet\n",dev->name);
+				priv->stats.rx_dropped++;
+				return;
+			}
+
+			/* Copy the data to skb and enqueue for upper layer */
+			skb->dev = dev;
+			skb_reserve(skb, 2);   //For IP Align 4-byte
+			skb_put(skb, length);
+			skb_copy_to_linear_data(skb, data, length);  
+			skb->protocol = eth_type_trans(skb, dev); 
+			priv->stats.rx_packets++;
+			priv->stats.rx_bytes += length;
+			netif_rx(skb);    
+		}
+		else	/* Some error */
+		{
+			priv->stats.rx_errors++;
+			if(status & RXDS_RP )
+			{
+				printk("ERROR: %s Receive Runt Packet \n",dev->name);
+				priv->stats.rx_length_errors++;
+			}
+			if(status & RXDS_CRCE )
+			{
+				printk("ERROR: %s Receive CRC Error Packet \n",dev->name);
+				priv->stats.rx_crc_errors ++;
+			}
+			if(status & RXDS_ALIE )
+			{
+				printk("ERROR: %s Receive Alignment Error Packet \n",dev->name);
+				priv->stats.rx_frame_errors++;
+			}
+			
+			if(status &  RXDS_PTLE)
+			{
+				printk("ERROR: %s Receive Too Big Packet \n",dev->name);
+				priv->stats.rx_over_errors++;
+			}
+		}
+
+		/* Release ownership to MAC */
+       	rxbd->SL =RXfOwnership_DMA;
+		rxbd->reserved = 0;
+    	priv->rx_index = (priv->rx_index+1)%(RX_DESC_SIZE);
+		rxbd=(RXBD *)&priv->rx_desc[priv->rx_index];
+		dev->last_rx = jiffies;
+	} while(1);
+	
+	priv->is_rx_all = 0;
+}
+
+static void 
+wpcm_set_multicast_list(struct net_device *dev)
+{
+	struct wpcm_mac_local *priv = (struct wpcm_mac_local *)dev->priv;
+	unsigned long rx_mode;
+	int which=priv->which;
+	
+	if(dev->flags&IFF_PROMISC)
+		rx_mode = CAMCMR_AUP|CAMCMR_AMP|CAMCMR_ABP|CAMCMR_ECMP;
+	else 
+	{
+		if((dev->flags&IFF_ALLMULTI)||dev->mc_list)
+			rx_mode=CAMCMR_AMP|CAMCMR_ABP|CAMCMR_ECMP;
+	 	else 
+			rx_mode = CAMCMR_ECMP|CAMCMR_ABP;
+	}
+	
+	wpcm_WriteReg(CAMCMR,rx_mode,which);
+}
+
+static int 
+wpcm_do_ioctl(struct net_device *dev,struct ifreq *ifr,int cmd)
+{
+	
+	switch(cmd)
+	{
+		case  SIOCSIFHWADDR:
+			if(dev->flags&IFF_PROMISC)
+				return -1;
+			memcpy(dev->dev_addr,ifr->ifr_hwaddr.sa_data,ETH_ALEN);
+    		wpcm_set_mac_address(dev,dev->dev_addr);  
+		    break; 
+
+		// get eth0 link status
+		case  SIOCDEVPRIVATE:
+			DetectPhy(0);
+			ResetPhyChip(0);
+			ifr->ifr_ifru.ifru_ivalue=GetLinkStatus(0);
+			break;
+
+	    default:
+			return -EOPNOTSUPP;
+	}
+	return 0;
+}
+#if 0
+void
+DumpRegisters(int which)
+{
+ 	printk("		** WPCM EMC Register %d **\n",which);
+ 	printk("CAMCMR:%x ",wpcm_ReadReg(CAMCMR,which)); 
+ 	printk("CAMEN:%x ",wpcm_ReadReg(CAMEN,which)); 
+ 	printk("MIEN: %x ",wpcm_ReadReg(MIEN,which));
+ 	printk("MCMDR: %x ",wpcm_ReadReg(MCMDR,which));
+ 	printk("MISTA: %x ",wpcm_ReadReg(MISTA,which)); 
+ 	printk("TXDLSA:%x ", wpcm_ReadReg(TXDLSA,which));	
+ 	printk("RXDLSA:%x \n", wpcm_ReadReg(RXDLSA,which));	
+ 	printk("DMARFC:%x ", wpcm_ReadReg(DMARFC,which));	
+ 	printk("TSDR:%x ", wpcm_ReadReg(TSDR,which));	
+ 	printk("RSDR:%x ", wpcm_ReadReg(RSDR,which));	
+ 	printk("FIFOTHD:%x ", wpcm_ReadReg(FIFOTHD,which));
+ 	printk("MISTA:%x ", wpcm_ReadReg(MISTA,which));
+ 	printk("MGSTA:%x ", wpcm_ReadReg(MGSTA,which));
+ 	printk("CTXDSA:%x \n",wpcm_ReadReg(CTXDSA,which)); 
+ 	printk("CTXBSA:%x ",wpcm_ReadReg(CTXBSA,which)); 
+ 	printk("CRXDSA:%x ", wpcm_ReadReg(CRXDSA,which));
+ 	printk("CRXBSA:%x ", wpcm_ReadReg(CRXBSA,which));
+}
+#endif
+
+int 
+send_frame(struct net_device *dev ,unsigned char *data,int length)
+{
+    struct wpcm_mac_local * priv= dev->priv;
+    int which;
+    TXBD *txbd,*next_txbd;
+    int val;
+    which=priv->which;
+
+    /* Get the next free TX desc */
+    txbd=&priv->tx_desc[priv->tx_index];
+
+    if(txbd->mode & TXfOwnership_DMA) 
+    {
+		printk("ERROR: %s : send_frame() called when no TX desc is free\n",dev->name);
+		return 1;
+    }
+    
+    if(length > 1514) 
+    {
+    	printk("WARNING: %s Send Data %d Bytes. Truncating to 1514\n",dev->name,length);
+    	length=1514;
+    }
+
+    /* Fill the descriptor and buffer */
+    txbd->SL = length & 0xFFFF;   
+    memcpy(&priv->tx_buf[priv->tx_index*PACKET_BUFFER_SIZE],data,length);
+    txbd->mode = (PaddingMode | CRCMode | MACTxIntEn);
+    dev->trans_start=jiffies;
+
+    /* If the next desc has data pending => all desc is full */	
+    priv->tx_index = (priv->tx_index+1)%(TX_DESC_SIZE);
+    next_txbd=&priv->tx_desc[priv->tx_index];
+    if(next_txbd->mode & TXfOwnership_DMA) 
+    {
+//		printk("%s: Tx Descriptors Unavailable. Pausing queue\n",dev->name);
+        netif_stop_queue(dev);
+    }
+
+    /* Enable TX and wakeup Tx State machine  from halt state */
+    txbd->mode |= TXfOwnership_DMA;	
+    val=wpcm_ReadReg(MCMDR,which);
+    if(!(val& MCMDR_TXON))
+          wpcm_WriteReg(MCMDR,val|MCMDR_TXON,which);
+    wpcm_WriteReg(TSDR ,0,which);
+
+
+    return 0;
+}
+
+
+int
+do_wpcm_mac_probe(struct net_device *dev)
+{
+	struct wpcm_mac_local *lp;
+	int retval;
+
+	lp = dev->priv;
+
+	/* Allocate Ring Buffers and descriptors */
+	lp->rx_desc = (RXBD *)dma_alloc_coherent(NULL,sizeof(RXBD)*RX_DESC_SIZE,
+			(dma_addr_t*)&lp->rx_desc_dma,GFP_KERNEL|GFP_DMA);
+	if (lp->rx_desc == NULL)
+	{
+		printk("ERROR (%s) Descriptor Allocation failure \n",dev->name);
+		return -ENOMEM;
+	}
+	lp->tx_desc = (TXBD *)dma_alloc_coherent(NULL,sizeof(TXBD)*TX_DESC_SIZE,
+			(dma_addr_t*)&lp->tx_desc_dma,GFP_KERNEL|GFP_DMA);
+	if (lp->tx_desc == NULL)
+	{
+		printk("ERROR (%s) Descriptor Allocation failure \n",dev->name);
+		return -ENOMEM;
+	}
+	lp->rx_buf   = (char *)dma_alloc_coherent(NULL,PACKET_BUFFER_SIZE *RX_DESC_SIZE,
+			(dma_addr_t*)&lp->rx_buf_dma,GFP_KERNEL|GFP_DMA);
+	if (lp->rx_buf == NULL)
+	{
+		printk("ERROR (%s) Descriptor Allocation failure \n",dev->name);
+		return -ENOMEM;
+	}
+	lp->tx_buf   = (char *)dma_alloc_coherent(NULL,PACKET_BUFFER_SIZE *TX_DESC_SIZE,
+			(dma_addr_t*)&lp->tx_buf_dma,GFP_KERNEL|GFP_DMA);
+	if (lp->tx_buf == NULL)
+	{
+		printk("ERROR (%s) Descriptor Allocation failure \n",dev->name);
+		return -ENOMEM;
+	}
+
+	/* Initialize local variables*/
+//	spin_lock_init(&lp->lock);
+
+#ifdef POLL_LINK_STATUS
+	init_timer(&lp->timer);
+#endif
+ 	lp->cur_tx_entry=0;
+	lp->tx_index=0;
+	lp->rx_index=0;
+
+	/* assign net_device methods */
+	dev->open 				= wpcm_open;
+	dev->stop 				= wpcm_stop;
+	dev->do_ioctl 			= wpcm_do_ioctl;
+	dev->get_stats 			= wpcm_get_stats;
+	dev->tx_timeout 		= wpcm_timeout;
+	dev->hard_start_xmit 	= wpcm_start_xmit;
+	dev->set_multicast_list	= wpcm_set_multicast_list;
+	dev->set_mac_address	= wpcm_set_mac_address;
+	dev->tx_queue_len 		= 16;
+	dev->dma 				= 0;
+	dev->watchdog_timeo 	= TX_TIMEOUT;
+
+	/* Program  MAC address */
+	wpcm_set_mac_address(dev,dev->dev_addr);      
+
+	ether_setup(dev);
+
+	/* Grab the resources */
+    if (!request_region(dev->base_addr, 0x100, dev->name))
+    {
+    	printk("ERROR:WPCM_MAC(%s): Address 0x%08lx in use \n",
+							dev->name,dev->base_addr);
+        return -EBUSY;
+    }
+	
+	retval =request_irq(dev->irq,&_rx_interrupt,IRQF_DISABLED,dev->name,dev);
+	if (retval)
+	{
+        printk("ERROR:WPCM_MAC(%s): Unable to assign RX IRQ %d\n",
+							dev->name,dev->irq);
+		release_region(dev->base_addr, 0x100);
+		return retval;
+	}
+
+	retval =request_irq(dev->irq+1,&_tx_interrupt,IRQF_DISABLED,dev->name,dev);
+	if (retval)
+	{
+   		printk("ERROR:WPCM_MAC(%s): Unable to assign TX IRQ %d\n",
+							dev->name,dev->irq+1);
+		free_irq(dev->irq,dev);
+		release_region(dev->base_addr, 0x100);
+		return retval;
+	}
+
+	retval = register_netdev(dev);
+	if (retval) 
+	{
+		printk("ERROR:WPCM_MAC(%s): Unable to register netdev\n",dev->name);
+		free_irq((dev->irq)+1,dev);
+		free_irq(dev->irq,dev);
+		release_region(dev->base_addr, 0x100);
+		return retval;
+	}
+
+	printk("%s: at Base: 0x%08lx RX_IRQ: %d  TX_IRQ: %d\n",dev->name,dev->base_addr,
+				dev->irq,dev->irq+1);
+
+#ifdef POLL_LINK_STATUS
+
+	/* Start Link Monitor */	
+	lp->timer.expires =jiffies+POLL_TIMEOUT*HZ;
+	lp->timer.data = (unsigned long) dev;
+	lp->timer.function = wpcm_autodetect;	/* timer handler */
+	add_timer(&lp->timer);
+#endif
+	return 0;
+	
+}
+
+
