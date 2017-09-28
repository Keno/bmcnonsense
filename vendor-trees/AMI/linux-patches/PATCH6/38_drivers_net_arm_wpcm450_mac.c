--- linux-pristine/drivers/net/arm/wpcm450_mac.c	2009-05-14 14:10:28.000000000 -0700
+++ linux-changed/drivers/net/arm/wpcm450_mac.c	2009-05-14 13:41:09.000000000 -0700
@@ -23,6 +23,7 @@
 extern void DetectPhy(int num);	// [SMC]
 extern int ResetPhyChip(int num);	// [SMC]
 
+int isMACInitialized = 0;
 #if defined(CFG_PROJ_NETWORK_ETH0)
 unsigned long wpcm_mac_io[MAX_WPCM_MAC]  =  { WPCM_MAC1_VA_BASE };
 unsigned int  wpcm_mac_irq[MAX_WPCM_MAC] =  { IRQ_MAC_1_RX };
@@ -32,6 +33,9 @@
 #elif defined(CFG_PROJ_NETWORK_BOTH)
 unsigned long wpcm_mac_io[MAX_WPCM_MAC]  =  { WPCM_MAC1_VA_BASE, WPCM_MAC2_VA_BASE };
 unsigned int  wpcm_mac_irq[MAX_WPCM_MAC] =  { IRQ_MAC_1_RX, IRQ_MAC_2_RX };
+#elif defined(CFG_PROJ_NETWORK_BOOTSELECT)
+unsigned long wpcm_mac_io[MAX_WPCM_MAC]  =  { 0 };
+unsigned int  wpcm_mac_irq[MAX_WPCM_MAC] =  { 0 };
 #endif
 
 /* Global variables  used for MAC driver */
@@ -40,15 +44,53 @@
                                 MIEN_EnTxBErr | MIEN_EnRxBErr | MIEN_EnTXABT | MIEN_EnRDU
 								| MIEN_EnTXEMP;
 
+int UseNCSI=0;
+static unsigned int 	NCSI_link = 1; //Initially make it up
+static unsigned int	NCSI_opmode = 1; //Initially make it 100Mbps
+static unsigned int 	NCSI_duplex = 1; //Initially make it Full Duplex
+
 static void init_rxtx_rings(struct net_device *dev);
 static int send_frame(struct net_device * ,unsigned char *,int);
 
 static int wpcm_open(struct net_device *dev);
 static int wpcm_stop(struct net_device *dev);
 
-// joe --- begin
-static unsigned char AlreadyInit=0;
-// joe --- end
+static int __init ncsiport(char *ncsistr)
+{
+	if( get_option(&ncsistr, &UseNCSI) == 0 )
+		UseNCSI = 0;
+
+	return 1;
+}
+
+__setup("usencsi=", ncsiport);
+
+unsigned long
+init_wpcm_mac_eth()
+{	
+
+	if(!isMACInitialized)
+	{
+
+		if( UseNCSI )
+		{
+			printk("I AM USING NCSI\n");
+			wpcm_mac_io[0] = WPCM_MAC2_VA_BASE;
+			wpcm_mac_irq[0] = IRQ_MAC_2_RX;
+		}
+		else
+		{
+			printk("I AM ************NOT********* USING NCSI\n");
+			wpcm_mac_io[0] = WPCM_MAC1_VA_BASE;
+			wpcm_mac_irq[0] = IRQ_MAC_1_RX;
+		}
+		
+
+		isMACInitialized = 1;
+	}
+
+	return 0;
+}
 
 void 
 wpcm_WriteCam(int which,int x, unsigned char *pval)
@@ -205,13 +247,12 @@
 
 	if (!IsPhyAccessLocked())
 	{
-//		Link = GetLinkStatus(which);
-		if( which == 1 )
+		if( UseNCSI )
 		{
 			// ETH1 is NCSI in simso so. link is always up.
-			Link = 1;
+			Link = NCSI_link;
 		}
-		else
+		else 
 		{
 			Link = GetLinkStatus(which);
 		}
@@ -234,20 +275,20 @@
 				ResetMAC(dev);
 			}
 		}
+		
 	}
-	
+		
     priv->timer.expires = jiffies +POLL_TIMEOUT*HZ;
     add_timer(&priv->timer);
 }
 #endif
 
-extern int Set100MBFullDuplex(int which);	// [SMC]
-
 static int   
 wpcm_open(struct net_device *dev)
 {
   	struct wpcm_mac_local * priv;
   	int    which ;
+	unsigned int curval = 0;
 
   	priv=(struct wpcm_mac_local *)dev->priv;
   	which= priv->which; 
@@ -287,16 +328,26 @@
 	/* Enable MDC clock generation */	
   	wpcm_WriteReg(MCMDR,MCMDR_EnMDC,which);
   	
-	if( which == 1 )
+	if( UseNCSI ) //Link, Speed parameters comes from ncsi driver
 	{
-		// ETH1 is NCSI port. So, we always say Full Duplex and 100Mbps
-		printk("%s: 100MB - ",dev->name);
-		wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_OPMOD,which);
-		printk("Full Duplex\n");
-		wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_FDUP,which);
-
-		//Set100MBFullDuplex(which);   // [SMC] added    		
-		priv->LinkDown=0;
+		if(NCSI_link)
+		{
+			printk("Open link:%d Opmode:%d duplex:%d\n ",NCSI_link, NCSI_opmode, NCSI_duplex);
+		
+			curval = wpcm_ReadReg(MCMDR,which);	
+       			curval = curval & (~MCMDR_OPMOD);
+			wpcm_WriteReg(MCMDR,curval | ((NCSI_opmode)<< 20),which);
+       		
+			curval = wpcm_ReadReg(MCMDR,which);	
+       			curval = curval & (~MCMDR_FDUP);
+			wpcm_WriteReg(MCMDR,curval | ((NCSI_duplex) << 18),which);
+			priv->LinkDown=0;
+		}
+		else
+		{
+			printk ("NCSILink down :%d\n", NCSI_link);
+			priv->LinkDown = 1;
+		}
 	}
 	else
 	{
@@ -304,7 +355,7 @@
 		/* This also sets Duplex,Speed to MCMDR register */
 	    DetectPhy(which);
 	    if(ResetPhyChip(which)==1)
-		priv->LinkDown=1;
+			priv->LinkDown=1;
 	    else
     		priv->LinkDown=0;
 	}
@@ -534,6 +585,18 @@
 	wpcm_WriteReg(CAMCMR,rx_mode,which);
 }
 
+static int init_MCMDR(unsigned char which)
+{
+	printk("Reset MCMDR\n");
+	wpcm_WriteReg(MCMDR,MCMDR_SWR,which);
+	while (wpcm_ReadReg(MCMDR,which) & MCMDR_SWR)
+		printk(".");
+	printk("\n");
+
+	/* Enable MDC clock generation */	
+  	wpcm_WriteReg(MCMDR,MCMDR_EnMDC | MCMDR_FDUP,which);
+}	
+
 static int 
 wpcm_do_ioctl(struct net_device *dev,struct ifreq *ifr,int cmd)
 {
@@ -549,6 +612,7 @@
 
 		// get eth0 link status
 		case  SIOCDEVPRIVATE:
+			init_MCMDR(0);
 			DetectPhy(0);
 			ResetPhyChip(0);
 			ifr->ifr_ifru.ifru_ivalue=GetLinkStatus(0);
@@ -750,3 +814,45 @@
 	add_timer(&lp->timer);
 #endif
 	return 0;
+	
+}
+
+int wpcm_setncsistatus(struct net_device *dev, unsigned int* linkstatus, unsigned int* opmode, unsigned int* duplex)
+{
+	struct wpcm_mac_local * priv=(struct wpcm_mac_local *)dev->priv;
+        int    which=priv->which ;
+	unsigned int curval = 0;
+	
+       	
+	//Set global variables
+//	NCSI_opmode = *opmode;
+	NCSI_opmode = 1;
+//	NCSI_duplex = *duplex;
+	NCSI_duplex = 1;
+	NCSI_link = *linkstatus;
+	printk("Link:%d Opmode:%d duplex:%d\n ",NCSI_link, NCSI_opmode, NCSI_duplex);
+	
+
+	if (*linkstatus == 1)
+	{	
+		curval = wpcm_ReadReg(MCMDR,which);	
+       		curval = curval & (~MCMDR_OPMOD);
+		wpcm_WriteReg(MCMDR,curval | ((NCSI_opmode)<< 20),which);
+       		
+		curval = wpcm_ReadReg(MCMDR,which);	
+       		curval = curval & (~MCMDR_FDUP);
+		wpcm_WriteReg(MCMDR,curval | ((NCSI_duplex) << 18),which);
+		priv->LinkDown=0;
+	}
+	else
+	{
+		priv->LinkDown=1;
+	}
+	
+	return 0;	
+}
+
+
+EXPORT_SYMBOL (UseNCSI);
+EXPORT_SYMBOL (wpcm_setncsistatus);
+
