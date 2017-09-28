--- linux.old/drivers/net/arm/wpcm450_mii.c	1969-12-31 19:00:00.000000000 -0500
+++ linux/drivers/net/arm/wpcm450_mii.c	2008-05-13 09:59:02.000000000 -0400
@@ -0,0 +1,298 @@
+/*
+ * linux/deriver/net/wpcm450_mac.c
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
+
+#include <asm/irq.h>
+#include <asm/arch/hardware.h>
+#include "../../../../projdef.h"
+#include "wpcm450_mac.h"
+#include "wpcm450_mii.h"
+
+static unsigned long PHYAD[MAX_WPCM_MAC];
+
+extern unsigned long wpcm_mac_io[MAX_WPCM_MAC]; 
+extern unsigned int  wpcm_mac_irq[MAX_WPCM_MAC];
+
+static unsigned long MiiStationRead(int num, unsigned long PhyInAddr, unsigned long PhyAddr);
+static int  MiiStationWrite(int num,unsigned long PhyInAddr,unsigned long PhyAddr,unsigned long PhyWrData);
+
+static int PhyAccessLock =0;
+
+static
+int
+LockPhyAccess(void)
+{
+	unsigned long flags;
+	local_irq_save(flags);
+	if (PhyAccessLock)
+	{
+		printk("wpcm450_mii: WARNING. Trying to lock already locked PHY acess\n");
+		local_irq_restore(flags);
+		return 1;
+	}
+	PhyAccessLock = 1;
+	local_irq_restore(flags);
+	return 0;
+}
+
+int
+IsPhyAccessLocked(void)
+{
+	unsigned long flags;
+	local_irq_save(flags);
+	if (PhyAccessLock)
+	{
+		local_irq_restore(flags);
+		return 1;
+	}
+	local_irq_restore(flags);
+	return 0;
+}
+
+static
+int
+UnLockPhyAccess(void)
+{
+	unsigned long flags;
+	local_irq_save(flags);
+	PhyAccessLock = 0;
+	local_irq_restore(flags);
+	return 0;
+}
+
+
+void 
+DetectPhy(int num)
+{
+	int i;
+	unsigned long RdValue = 0;
+ 	volatile int loop=1000*100;
+
+	LockPhyAccess();
+
+	/* Scan for PHY */
+	for (i = 0; i < 32; i++) // [SMC] orig: for (i=31 ; i>0 ; i--) 
+   	{
+ 		MiiStationWrite(num,PHY_CNTL_REG,i << 8,RESET_PHY);
+		RdValue = MiiStationRead(num,PHY_CNTL_REG, i<<8);
+		if ( (RdValue != 0xFFFF) && (RdValue & 0x3000) )
+			break;
+    }
+	if (i==32) //if (i==0) 
+    {
+		printk("eth%d: WARNING:PHY Auto scan failed !\n",num);
+		UnLockPhyAccess();
+		return;
+    }
+	printk("eth%d: Found PHY @ Address %d\n",num,i);
+	PHYAD[num] = i<<8;
+
+	/* Issue Reset */
+	MiiStationWrite(num,PHY_CNTL_REG,PHYAD[num],RESET_PHY);
+	while (loop--)
+	{
+		RdValue = MiiStationRead(num,PHY_CNTL_REG,PHYAD[num]);
+		if (!(RdValue & RESET_PHY))
+		{
+			UnLockPhyAccess();
+			return;
+		}
+	}
+	printk("eth%d: Reset Failed for PHY %d\n",num,i);
+	UnLockPhyAccess();
+	return;
+}
+
+int
+GetLinkStatus(int which)
+{
+    unsigned long RdValue;
+	LockPhyAccess();
+    RdValue = MiiStationRead(which, PHY_STATUS_REG, PHYAD[which]) ; /* Latch the current value */
+    RdValue = MiiStationRead(which, PHY_STATUS_REG, PHYAD[which]) ; /* Read the ourrent Value */	
+	
+	if((RdValue & PHY_REMOTE_FAULT)!=0)
+	{
+//		  printk(" ERROR: PHY Remote Fault\n");
+		  UnLockPhyAccess();
+		  return 0;
+	}
+	
+	if ((RdValue & (PHY_AN_COMPLETE | LINK_STATUS_VALID)) == 
+							(PHY_AN_COMPLETE | LINK_STATUS_VALID))
+	{
+		UnLockPhyAccess();
+		return 1;
+	}
+	UnLockPhyAccess();	
+	return 0;
+
+}
+
+int 
+Set100MBFullDuplex(int which)
+{
+	LockPhyAccess();
+	printk("100MB - Full Duplex\n");
+	wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_OPMOD|MCMDR_FDUP,which);
+	UnLockPhyAccess();
+	return 0;
+}
+
+int 
+ResetPhyChip(int num)
+{
+ 	unsigned long RdValue;
+ 	int which=num;
+ 	volatile unsigned long loop=1000*100;
+	LockPhyAccess();
+
+	printk("eth%d: Enable and Restart Auto Negotiation\n",num);
+	RdValue = MiiStationRead(which, PHY_CNTL_REG, PHYAD[num]);
+	if(MiiStationWrite(which, PHY_CNTL_REG, PHYAD[num], RdValue | ENABLE_AN | RESTART_AN)==1)
+ 	{
+		printk("eth%d: ERROR:Enable and Restart Auto Negotiation Failed\n",num);
+		UnLockPhyAccess();
+		return 1;
+ 	}	
+
+    printk("eth%d: Wait for auto-negotiation complete",num);
+ 	while (loop) 	/* wait for auto-negotiation complete */
+   	{
+		if (!(loop % (1000*10)))
+			printk(".");
+
+    	RdValue = MiiStationRead(which, PHY_STATUS_REG, PHYAD[num]) ;
+
+    	if((RdValue & PHY_REMOTE_FAULT)!=0)
+    	{
+			  printk(" ERROR: PHY Remote Fault\n");
+			  UnLockPhyAccess();
+    		  return 1;
+    	}
+
+    	if ((RdValue & (PHY_AN_COMPLETE | LINK_STATUS_VALID)) == 
+								(PHY_AN_COMPLETE | LINK_STATUS_VALID))
+    	{
+            printk(" Passed ");
+            break;
+      	}
+		loop--;
+    	if(loop==0)
+    	{
+    		printk(" Failed,PhyStatus=0x%lX\n", RdValue);
+			UnLockPhyAccess();
+      	 	return 1;
+    	}	 
+   	}
+
+    RdValue = MiiStationRead(which, PHY_CNTL_REG, PHYAD[num]);
+
+	// Is auto negotiation on
+	if ((RdValue & ENABLE_AN) != 0) 
+	{
+		RdValue = MiiStationRead(which, PHY_ANLPA_REG, PHYAD[num]);
+		if ((RdValue & PHY_ANLPAR_100) != 0) 
+		{
+			printk("100MB - ");
+			wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_OPMOD,which);
+		}else
+		{
+			printk("10MB - ");	
+			wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)&~MCMDR_OPMOD,which);		
+		}
+		if ((RdValue & (PHY_ANLPAR_10FD | PHY_ANLPAR_TXFD)) != 0) 
+		{
+			printk("Full Duplex\n");
+			wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_FDUP,which);
+		}else
+		{
+			printk("Half Duplex\n");
+			wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)&~MCMDR_FDUP,which);
+		}
+	}else
+	{
+ 		if ((RdValue & DR_100MB) == 0) 	/* 10MB */
+		{
+			printk("10MB - ");	
+			wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)&~MCMDR_OPMOD,which);
+		}
+		else 
+		{
+			printk("100MB - ");
+			wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_OPMOD,which);
+		}
+		if ((RdValue & PHY_FULLDUPLEX) != 0) 	/* Full Duplex */
+		{
+			printk("Full Duplex\n");
+			wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)|MCMDR_FDUP,which);	
+		}
+		else 
+		{ 
+			printk("Half Duplex\n");
+			wpcm_WriteReg(MCMDR,wpcm_ReadReg(MCMDR,which)&~MCMDR_FDUP,which);
+		}
+	}
+
+	UnLockPhyAccess();
+	return 0;
+}
+
+static
+int  
+MiiStationWrite(int num,unsigned long PhyInAddr,unsigned long PhyAddr,unsigned long PhyWrData)
+{
+ 	volatile int i = 1000;
+ 	int which=num;
+    volatile long loop=1000*100;
+
+   	wpcm_WriteReg(MIID,PhyWrData,which);
+   	wpcm_WriteReg(MIIDA,PhyInAddr|PhyAddr|PHYBUSY|PHYWR|MDCCR,which);
+   	while(i--);
+   	while((wpcm_ReadReg(MIIDA,which) &PHYBUSY))
+   	{
+   		loop--;
+   		if(loop==0)
+		{
+	//		printk("ERROR: eth%d: Mii Write (0x%lx:0x%lx) Failed\n",num,PhyInAddr,PhyAddr);
+   			return 1;
+		}
+   	}
+  	return 0;
+}
+
+
+static
+unsigned long 
+MiiStationRead(int num, unsigned long PhyInAddr, unsigned long PhyAddr)
+{
+ 	volatile int i = 1000;
+ 	int which=num;
+ 	volatile unsigned long loop=1000*100;
+ 	unsigned long PhyRdData ;
+
+    wpcm_WriteReg(MIIDA, PhyInAddr | PhyAddr | PHYBUSY | MDCCR,which);
+   	while(i--);
+    while( (wpcm_ReadReg(MIIDA,which)& PHYBUSY) ) 
+    {
+    	loop--;
+    	if(loop==0)
+		{
+	//		printk("ERROR: eth%d: Mii Read (0x%lx:0x%lx) Failed\n",num,PhyInAddr,PhyAddr);
+     		return (unsigned long)0;
+		}
+    }
+    
+    PhyRdData = wpcm_ReadReg(MIID,which) ; 
+ 	return PhyRdData ;
+}
+
