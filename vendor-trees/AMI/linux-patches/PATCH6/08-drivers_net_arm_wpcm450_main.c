--- linux.old/drivers/net/arm/wpcm450_main.c	1969-12-31 19:00:00.000000000 -0500
+++ linux/drivers/net/arm/wpcm450_main.c	2008-05-13 09:58:48.000000000 -0400
@@ -0,0 +1,157 @@
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
+extern unsigned long enetaddr[4][6];
+
+extern unsigned long wpcm_mac_io[MAX_WPCM_MAC]; 
+extern unsigned int  wpcm_mac_irq[MAX_WPCM_MAC];
+
+void
+FindMACAddress(int unit, char *mac_addr)
+{
+	/*Passed to us from bootloader*/
+	memcpy(mac_addr,enetaddr[unit],6);
+	return;
+}
+
+/*----------------------------------------------------------------
+   If this driver is linked with kernel (i.e) not a module, then
+   this probe function is called from Space.c
+ ---------------------------------------------------------------*/
+#ifndef MODULE
+
+struct net_device * __init wpcm450_mac_probe(int unit)
+{
+	struct net_device *dev;
+	int err;
+	struct wpcm_mac_local *lp;
+
+	if (unit >= MAX_WPCM_MAC)
+		return ERR_PTR(-ENODEV);
+
+	/* Set MFSEL1 (Multi functions) to enable MAC Lines */
+	*((unsigned long *)(WPCM_GCR_VA_BASE + 0x0C)) |= 0x0001F000;
+
+	dev = alloc_etherdev(sizeof(struct wpcm_mac_local));
+	if (!dev)
+		return ERR_PTR(-ENODEV);
+	memset(dev->priv,0,sizeof(struct wpcm_mac_local));
+	lp = dev->priv;
+	lp->which=unit;
+
+	//SET_MODULE_OWNER(dev);
+
+	sprintf(dev->name, "eth%d", unit);
+	netdev_boot_setup_check(dev);
+
+	dev->base_addr = wpcm_mac_io[unit];
+	dev->irq = wpcm_mac_irq[unit];
+	dev->dma = 0;
+	FindMACAddress(unit,dev->dev_addr);
+
+	err = do_wpcm_mac_probe(dev);
+	if (err)
+		goto out;
+	return dev;
+out:
+	free_netdev(dev);
+	return ERR_PTR(err);
+}
+#endif
+
+
+/*----------------------------------------------------------------
+   If this driver is loadad as module, the module intialization
+   function is called first which registers the devices
+-------------------------------------------------------------*/
+#ifdef MODULE
+
+
+static struct net_device dev_wpcm_mac[MAX_WPMC_MAC];
+static int wpmc_mac_count;
+
+
+/*------------------------------------------------------------
+ . Module initialization function
+ .-------------------------------------------------------------*/
+int
+init_module(void)
+{
+
+	int i;
+	struct net_device *dev;
+
+	/* Set MFSEL1 (Multi functions) to enable MAC Lines */
+	*((unsigned long *)(WPCM_GCR_VA_BASE + 0x0C)) |= 0x0001F000;
+
+   	wpcm_mac_count = 0;
+	for (i = 0; i < MAX_WPCM_MAC ; i++)
+	{
+		dev = alloc_etherdev(sizeof(struct wpcm_mac_local));
+		if (!dev)
+			break;
+		memset(dev->priv,0,sizeof(struct wpcm_mac_local));
+		dev->priv->which=i;
+
+		sprintf(dev->name, "eth%d", i);
+		dev->base_addr =wpcm_mac_io[i];
+		dev->irq = wpcm_mac_irq[i];
+		dev->dma = 0;
+		FindMACAddress(i,dev->dev_addr);
+		if (do_wpcm_mac_probe(dev) == 0)
+		{
+			dev_wpcm_mac[wpcm_mac_count++] = dev;
+			continue;
+		}
+		free_netdev(dev);
+		break;
+	}
+
+	return wpcm_mac_count? 0 : -ENODEV;
+}
+
+
+/*------------------------------------------------------------
+ . Cleanup when module is removed with rmmod
+ .-------------------------------------------------------------*/
+void
+cleanup_module(void)
+{
+	int i;
+
+	for (i=0; i<wpcm_mac_count; i++)
+	{
+		struct net_device *dev = dev_wpcm_mac[i];
+		unregister_netdev(dev);
+		free_irq(dev->irq,dev);
+		free_irq(dev->irq+1,dev);
+		release_region(dev->base_addr, 0x100);
+		free_netdev(dev);
+	}
+
+	return;
+}
+
+
+#endif /* MODULE */
+
+
+
