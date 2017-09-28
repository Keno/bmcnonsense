--- u-boot-1.1.4-cpu/cpu/arm926ejs/WPCM450/hermon_mac.c	1969-12-31 17:00:00.000000000 -0700
+++ u-boot-1.1.4-wpcm/cpu/arm926ejs/WPCM450/hermon_mac.c	2008-04-18 18:25:55.000000000 -0500
@@ -0,0 +1,900 @@
+/* Tulip based Ethernet MAC 10/100 Controller for U-boot (1.1.4)*/
+
+#include <config.h>
+#include <common.h>
+#include <malloc.h>
+#include <command.h>
+#include <miiphy.h>
+#include "soc_hw.h"
+#include "hermon_mac.h"
+#include "net.h"
+
+#define DEFAULT_CSR11  (CSR11_TT_VAL(0)| CSR11_NTP_VAL(0) | CSR11_RT_VAL(0) | CSR11_NRP_VAL(0))
+#define DEFAULT_CSR6   (CSR6_TR_VAL(0))
+#define DEFAULT_CSR0   (CSR0_BAR_RXH | CSR0_DSL_VAL(0) | CSR0_PBL_VAL(0) | CSR0_TAP_VAL(0))
+
+extern void DetectPhyAddr(char *devname);
+extern unsigned char GetConfigPhyAddr(char *devname);
+extern int wpcm_miiphy_write (char *devname, unsigned char addr, unsigned char reg, unsigned short value);
+extern int wpcm_miiphy_read (char *devname, unsigned char addr, unsigned char reg,  unsigned short *value);
+
+extern int GetLinkStatus(char *devname);
+extern int GetPhySpeed(char *devname);
+extern int GetPhyDuplex(char *devname);
+
+#define inl(addr) 			(*((volatile u32 *)(addr)))
+#define inw(addr)			(*((volatile u16 *)(addr)))
+#define outl(value, addr)  	(*((volatile u32 *)(addr)) = value)
+#define outb(value, addr)	(*((volatile u8 *)(addr)) = value)
+
+#define virt2phys(x)   (x)
+
+//#define MAC_DEBUG
+
+#define ETH_ZLEN 60
+
+extern int eth_init(bd_t *bd);
+extern void eth_halt(void);
+extern int eth_rx(void);
+extern int eth_send(volatile void *packet, int length);
+
+
+/* Notes:
+      Functions beginning with wpcm_mac100_ interfaces with hardware
+	  Functions beginning with wpcm_eth_ interfaces with U-boot
+*/
+
+static  int  wpcm_mac100_open(struct eth_device *dev);
+static void  hermon_mac100_close(struct eth_device *dev);
+static void  hermon_mac100_reset(struct eth_device *dev);
+static int 	 wpcm_mac100_alloc_buffers(ETH_PRIV_DATA *lp);
+static void  hermon_mac100_enable( struct eth_device *dev );
+static int   wpcm_mac100_recv(struct eth_device *dev);
+static  int  hermon_mac100_send_packet(struct eth_device *dev, void *packet, int length);
+
+static int 	 wpcm_eth_init(struct eth_device *dev,bd_t *bd);
+static int   wpcm_eth_rx(struct eth_device *dev);
+static void  wpcm_eth_halt(struct eth_device *dev);
+static int   wpcm_eth_send(struct eth_device *dev, volatile void *packet, int length);
+
+#ifdef  MAC_DEBUG
+void
+wpcm_mac100_dump(char *Mesg,struct eth_device *dev)
+{
+	unsigned int ioaddr;
+	ETH_PRIV_DATA *lp;
+	int i;
+
+	lp = (ETH_PRIV_DATA *)dev->priv;
+	ioaddr = lp->hw_addr;
+
+	printf("-----------------------------------------------------\n");
+	printf("         HERMON MAC Dump : %s\n",Mesg);
+	printf("-----------------------------------------------------\n");
+
+	printf("CAMCMR = 0x%08X\n",inl(ioaddr+CAMCMR)& 0xFF);
+	printf("CAMEN = 0x%08X\n",inl(ioaddr+CAMEN));
+	printf("CAM0M = 0x%08X\n",inl(ioaddr+CAM0M));
+	printf("CAM0L = 0x%08X\n",inl(ioaddr+CAM0L));
+	printf("TXDLSA = 0x%08X\n",inl(ioaddr+TXDLSA));
+	printf("RXDLSA = 0x%08X\n",inl(ioaddr+RXDLSA));
+	printf("MCMDR = 0x%08X\n",inl(ioaddr+MCMDR));
+	printf("MIID = 0x%08X\n",inl(ioaddr+MIID));
+	printf("FFTCR = 0x%08X\n",inl(ioaddr+FFTCR));
+	printf("TSDR = 0x%08X\n",inl(ioaddr+TSDR));
+	printf("RSDR = 0x%08X\n",inl(ioaddr+RSDR));
+	printf("DMARFC = 0x%08X\n",inl(ioaddr+DMARFC));
+	printf("MIEN = 0x%08X\n",inl(ioaddr+MIEN));
+	printf("MISTA = 0x%08X\n",inl(ioaddr+MISTA));
+	printf("MGSTA = 0x%08X\n",inl(ioaddr+MGSTA));
+	printf("MPCNT = 0x%08X\n",inl(ioaddr+MPCNT));
+	printf("MRPC = 0x%08X\n",inl(ioaddr+MRPC));
+	printf("MRPCC = 0x%08X\n",inl(ioaddr+MRPCC));
+	printf("MREPC = 0x%08X\n",inl(ioaddr+MREPC));
+	printf("DMARFS = 0x%08X\n",inl(ioaddr+DMARFS));
+	printf("CTXDSA = 0x%08X\n",inl(ioaddr+CTXDSA));
+	printf("CTXBSA = 0x%08X\n",inl(ioaddr+CTXBSA));
+	printf("CRXDSA = 0x%08X\n",inl(ioaddr+CRXDSA));
+	printf("CRXBSA = 0x%08X\n",inl(ioaddr+CRXBSA));
+
+	for(i=0;i<RXDES_NUM;i++)
+	{
+		printf("RX Desc[%d].SL  = 0x%08x\n",i, lp->rx_descs[i].SL);
+		printf("RX Desc[%d].Buffer  = 0x%08x\n",i, lp->rx_descs[i].Buffer);
+		printf("RX Desc[%d].next = 0x%08x\n",i, lp->rx_descs[i].next);
+	}
+
+	for(i=0;i<TXDES_NUM;i++)
+	{
+		printf("TX Desc[%d].mode  = 0x%08x\n",i, lp->tx_descs[i].mode);
+		printf("TX Desc[%d].Buffer  = 0x%08x\n",i, lp->tx_descs[i].Buffer);
+		printf("TX Desc[%d].SL = 0x%08x\n",i, lp->tx_descs[i].SL);
+		printf("TX Desc[%d].next = 0x%08x\n",i, lp->tx_descs[i].next);
+	}
+	return;
+}
+#else
+#define wpcm_mac100_dump(dev)
+#endif
+
+static
+int
+wpcm_mac100_open(struct eth_device *dev)
+{
+	unsigned int ioaddr;
+	ETH_PRIV_DATA *hw;
+
+#ifdef  MAC_DEBUG
+	printf("MAC:(%s):Open\n",dev->name);
+#endif
+
+	hw = (ETH_PRIV_DATA *)dev->priv;
+	ioaddr = hw->hw_addr;
+
+	/* Rset the hardware */
+	hermon_mac100_reset( dev );
+//	wpcm_mac100_dump("After Reset",dev);
+
+	/* Enable device for communication */
+	hermon_mac100_enable( dev );
+
+//	wpcm_mac100_dump("After Open",dev);
+	return 0;
+}
+
+
+static
+void
+hermon_reset_rings(struct eth_device *dev)
+{
+	ETH_PRIV_DATA *lp;
+	int i;
+
+#ifdef  MAC_DEBUG
+	printf("HERMON:(%s):Reset Rings\n",dev->name);
+#endif
+
+	lp = (ETH_PRIV_DATA *)dev->priv;
+
+	lp->rx_idx = 0;
+	lp->tx_idx = 0;
+
+	for(i=0;i<RXDES_NUM;i++)
+	{
+		lp->rx_descs[i].SL = RDESC0_DMA_OWN;
+		lp->rx_descs[i].Buffer = (u32)&(lp->rx_buf_dma[i*RX_BUF_SIZE]);
+//		lp->rx_descs[i].Length = RX_BUF_SIZE | RDESC1_RCH ;
+		if ((i+1) != RXDES_NUM)
+			lp->rx_descs[i].next = (u32)&(lp->rx_descs_dma[i+1]);
+		else
+		{
+			lp->rx_descs[i].next = (u32)&(lp->rx_descs_dma[0]);
+	//		lp->rx_descs[i].Length |= RDESC1_RER ;
+		}
+	}
+
+	for(i=0;i<TXDES_NUM;i++)
+	{
+		lp->tx_descs[i].SL = 0;
+		lp->tx_descs[i].mode = 0 ;
+		lp->tx_descs[i].Buffer = (u32)&(lp->tx_buf_dma[i*TX_BUF_SIZE]);
+		if ((i+1) != TXDES_NUM)
+			lp->tx_descs[i].next = (u32)&(lp->tx_descs_dma[i+1]);
+		else
+		{
+			lp->tx_descs[i].next = (u32)&(lp->tx_descs_dma[0]);
+//			lp->tx_descs[i].Length |= TDESC1_TER | TDESC1_TCH ;
+		}
+	}
+
+
+	return;
+}
+
+static
+void
+hermon_mac100_close(struct eth_device *dev)
+{
+	unsigned int ioaddr;
+	ETH_PRIV_DATA *hw;
+	u32 val;
+
+#ifdef  MAC_DEBUG
+	printf("MAC:(%s):Close\n",dev->name);
+#endif
+
+	hw = (ETH_PRIV_DATA *)dev->priv;
+	ioaddr = hw->hw_addr;
+
+	outl(0,hw->hw_addr+MCMDR);
+
+	/* Clear pending interrupts */
+	val = inl(ioaddr + MISTA);
+	outl(val, ioaddr + MISTA);
+	return;
+}
+
+
+static
+void
+hermon_mac100_reset(struct eth_device *dev)
+{
+
+	unsigned int ioaddr;
+	u32 val;
+	ETH_PRIV_DATA *hw;
+
+#ifdef  MAC_DEBUG
+	printf("HERMONMAC:(%s):Reset\n",dev->name);
+#endif
+
+	hw = (ETH_PRIV_DATA *)dev->priv;
+	ioaddr = hw->hw_addr;
+
+	// software reset
+	outl(0x01000000, ioaddr + MCMDR);
+
+	outl(0x10000,hw->hw_addr+FFTCR);
+	val = inl(hw->hw_addr+MCMDR);
+	val &= ~(MCMDR_TXON | MCMDR_RXON);
+	outl(val,hw->hw_addr+MCMDR);
+	outl(0x100300,hw->hw_addr+FFTCR);
+
+	/* Clear pending interrupts */
+	val = inl(ioaddr + MISTA);
+	outl(val, ioaddr + MISTA);
+// ADDITIONAL WORK IS DONE IN uCLinux DRIVER
+
+	return;
+}
+
+
+static
+int
+wpcm_mac100_alloc_buffers(ETH_PRIV_DATA *lp)
+{
+#ifdef  MAC_DEBUG
+	printf("MAC:Alloc Buffers\n");
+#endif
+	/*---------------------------------- Receive Descriptors ----------------------------------*/
+	/* Allocate Memory for Descriptors */
+	lp->rx_descs = malloc( sizeof(RECV_DESC)*(RXDES_NUM+1));
+	if (lp->rx_descs == NULL)
+	{
+		printf("ERROR: Receive Ring Descriptor allocation error\n");
+		return -1;
+	}
+
+	/* Align to the Dword Boundary - NOTE: Dword is 8 bytes here */
+	lp->rx_descs =  (RECV_DESC *)((int)(((char *)lp->rx_descs)+sizeof(RECV_DESC)-1)&0xfffffff0);
+
+	/* Get Physical Address for DMA */
+	lp->rx_descs_dma = virt2phys(lp->rx_descs);
+
+	/* Clear out Descriptors */
+	memset((void *)lp->rx_descs, 0, sizeof(RECV_DESC)*RXDES_NUM);
+
+
+	/* Allocate Memory for Buffers */
+	lp->rx_buf = malloc(RX_BUF_SIZE*(RXDES_NUM+1));
+	if (lp->rx_buf == NULL)
+	{
+		printf("ERROR:Receive Ring Buffer allocation error\n");
+		return -1;
+	}
+
+	/* Align to the Dword Boundary - NOTE: Dword is 8 bytes here */
+	lp->rx_buf =  (char *)((int)((((char *)lp->rx_buf)+7))&0xfffffff0);
+
+	/* Get Physical Address for DMA */
+	lp->rx_buf_dma = virt2phys(lp->rx_buf);
+
+	/*---------------------------------- Xmit Descriptors ----------------------------------*/
+
+	/* Allocate Memory for Descriptors */
+	lp->tx_descs = malloc( sizeof(XMIT_DESC)*(TXDES_NUM+1));
+	if (lp->tx_descs == NULL)
+	{
+		printf("ERROR:Transmit Ring Descriptors allocation error\n");
+		return -1;
+	}
+
+	/* Align to the Dword Boundary - NOTE: Dword is 8 bytes here */
+	lp->tx_descs =  (XMIT_DESC *)((int)(((char *)lp->tx_descs)+sizeof(XMIT_DESC)-1)&0xfffffff0);
+
+	/* Get Physical Address for DMA */
+	lp->tx_descs_dma = virt2phys(lp->tx_descs);
+
+	/* Clear out Descriptors */
+	memset((void *)lp->tx_descs, 0, sizeof(XMIT_DESC)*TXDES_NUM);
+
+	/* Allocate Memory for Buffers */
+	lp->tx_buf = malloc( TX_BUF_SIZE*(TXDES_NUM+1));
+	if (lp->tx_buf == NULL)
+	{
+		printf("ERROR:Transmit Ring Buffer allocation error\n");
+		return -1;
+	}
+
+	/* Align to the Dword Boundary - NOTE: Dword is 8 bytes here */
+	lp->tx_buf =  (char *)((int)((((char *)lp->tx_buf)+7))&0xfffffff0);
+
+	/* Get Physical Address for DMA */
+	lp->tx_buf_dma = virt2phys(lp->tx_buf);
+
+	/*---------------------------------- Setup Frame ----------------------------------*/
+	/* Allocate Memory for Descriptors */
+	lp->setup_descs = malloc(sizeof(XMIT_DESC) *2 );
+	if (lp->setup_descs == NULL)
+	{
+		printf("ERROR:Setup Descriptors allocation error\n");
+		return -1;
+	}
+
+	/* Align to the Dword Boundary - NOTE: Dword is 8 bytes here */
+	lp->setup_descs =  (XMIT_DESC *)((int)(((char *)lp->setup_descs)+sizeof(XMIT_DESC)-1)&0xfffffff0);
+
+	/* Get Physical Address for DMA */
+	lp->setup_descs_dma = virt2phys(lp->setup_descs);
+
+	/* Clear out Descriptors */
+	memset((void *)lp->setup_descs, 0, sizeof(XMIT_DESC));
+
+	/* Allocate Memory for Buffers */
+	lp->setup_frame = malloc(SETUP_FRAME_SIZE+7);
+	if (lp->setup_frame == NULL)
+	{
+		printf("ERROR:Setup Ring  Buffer allocation error\n");
+		return -1;
+	}
+
+	/* Align to the Dword Boundary - NOTE: Dword is 8 bytes here */
+	lp->setup_frame =  (char *)((int)((((char *)lp->setup_frame)+7))&0xfffffff0);
+
+	/* Get Physical Address for DMA */
+	lp->setup_frame_dma = virt2phys(lp->setup_frame);
+
+
+
+#ifdef  MAC_DEBUG
+	printf("lp->rx_descs    = %x, lp->rx_descs_dma    = %x\n", lp->rx_descs, lp->rx_descs_dma);
+	printf("lp->rx_buf      = %x, lp->rx_buf_dma      = %x\n", lp->rx_buf, lp->rx_buf_dma);
+	printf("lp->tx_descs    = %x, lp->tx_descs_dma    = %x\n", lp->tx_descs, lp->tx_descs_dma);
+	printf("lp->tx_buf      = %x, lp->tx_buf_dma      = %x\n", lp->tx_buf, lp->tx_buf_dma);
+	printf("lp->setup_descs = %x, lp->setup_descs_dma = %x\n", lp->setup_descs, lp->setup_descs_dma);
+	printf("lp->setup_frame = %x, lp->setup_frame_dma = %x\n", lp->setup_frame,lp->setup_frame_dma);
+#endif
+
+	return 0;
+}
+
+static void write_cam (struct eth_device *dev, char *mac_addr)
+{
+	u32 lsw, msw ;
+	unsigned int ioaddr ;
+	ETH_PRIV_DATA *lp;
+
+#ifdef  MAC_DEBUG
+	printf("HERMON:(%s):Set Mac Address\n",dev->name);
+#endif
+	lp = (ETH_PRIV_DATA *)dev->priv;
+	ioaddr = lp->hw_addr;
+	
+	// is this forward or reverse ??
+        msw =   (mac_addr[0] << 24) |
+                (mac_addr[1] << 16) |
+                (mac_addr[2] << 8) |
+                mac_addr[3];
+                                                              
+        lsw = (mac_addr[4] << 24) |
+           (mac_addr[5] << 16);
+
+	outl(lsw,ioaddr + CAM0_L);
+	outl(msw,ioaddr + CAM0_M);
+	
+	lsw = inl(ioaddr + CAMEN) ;
+	outl(lsw | 1, ioaddr + CAMEN) ;
+//	outl(CAMCMR_ECMP|CAMCMR_ABP|CAMCMR_AMP, ioaddr + CAMCMR);
+	outl(CAMCMR_ECMP|CAMCMR_ABP, ioaddr + CAMCMR);
+//	outl(1<<19, ioaddr + MCMDR) ;
+
+}
+
+static
+void
+hermon_mac100_enable( struct eth_device *dev )
+{
+	unsigned int ioaddr;
+	ETH_PRIV_DATA *lp;
+//	unsigned long csr6;
+	u32 val;
+	int value;
+
+#ifdef  MAC_DEBUG
+	printf("HERMON MAC:(%s):Enable\n",dev->name);
+#endif
+
+	lp = (ETH_PRIV_DATA *)dev->priv;
+	ioaddr = lp->hw_addr;
+
+	// software reset
+	outl(0x01000000, ioaddr + MCMDR);
+	/* Init Ring */
+	hermon_reset_rings(dev);
+
+// SET GPIO TO ENABLE RMII 1 and 2 INTERFACE
+	outl((inl(GCR_MFSEL1)| 0x0001F000),GCR_MFSEL1);
+
+	/* set a reserved bit in Fifo Threshold Control */	
+	outl(0x10000,ioaddr + FFTCR);
+	outl(0x100300,ioaddr + FFTCR);
+
+	/* Set Descriptor Pointers */
+	outl((u32)lp->rx_descs_dma,ioaddr + RXDLSA);
+	outl((u32)lp->tx_descs_dma,ioaddr + TXDLSA);
+
+	/* Set Max RX Frame */
+	outl(RX_BUF_SIZE,ioaddr + DMARFC);
+
+	/* Set MAC Address */
+	write_cam (dev, dev->enetaddr);
+
+	if( strcmp( dev->name, "wpcm_eth1") == 0 )
+	{
+		val = inl(ioaddr + MCMDR) ;
+		outl(val | MCMDR_OPMOD, ioaddr+MCMDR);
+		printf("Link: 100Mbps  ");
+			
+		val = inl(ioaddr + MCMDR) ;
+		outl(val | MCMDR_FDUP, ioaddr+MCMDR);
+		printf("Full Duplex\n");
+	}
+	else if (GetLinkStatus(dev->name))
+ 	{
+ 		value = GetPhySpeed(dev->name);
+ 		if (value == _10BASET) 
+		{
+			val = inl(ioaddr + MCMDR) ;
+			outl(val & ~MCMDR_OPMOD, ioaddr+MCMDR);
+			printf("Link: 10Mbps  ");
+		}else  // _100BASET
+		{
+			val = inl(ioaddr + MCMDR) ;
+			outl(val | MCMDR_OPMOD, ioaddr+MCMDR);
+			printf("Link: 100Mbps  ");
+		}
+
+		value = GetPhyDuplex(dev->name);
+		if(value == FULL) {
+			val = inl(ioaddr + MCMDR) ;
+			outl(val | MCMDR_FDUP, ioaddr+MCMDR);
+			printf("Full Duplex\n");
+		}
+		else {
+			val = inl(ioaddr + MCMDR) ;
+			outl(val & ~MCMDR_FDUP, ioaddr+MCMDR);
+			printf("Half Duplex\n");
+		}
+	}
+	else { // cable unplugged?
+		// force to full duplex and 100Mb
+		val = inl(ioaddr + MCMDR) ;
+		outl(val | MCMDR_OPMOD | MCMDR_FDUP, ioaddr+MCMDR);
+		printf("Link Status Not Valid\n");
+	}
+
+	/* Set Default Command Register Bits */
+	val = inl(ioaddr + MCMDR) ;
+	outl(val | MCMDR_EnMDC | MCMDR_ACP,ioaddr + MCMDR); 
+
+	/* Enable Receiver and Transmitter */
+	val = inl(ioaddr + MCMDR) ;
+	outl((val | MCMDR_RXON | MCMDR_TXON) ,ioaddr + MCMDR); 
+
+	/* Start Receive State Machine */
+	outl(0 ,ioaddr + RSDR); 
+
+	/* Clear pending interrupts */
+	val = inl(ioaddr + MISTA);
+	outl(val, ioaddr + MISTA);
+
+	return;
+}
+
+
+
+
+static
+int
+hermon_mac100_send_packet(struct eth_device *dev,  void *packet, int length)
+{
+	volatile XMIT_DESC *cur_desc;
+	unsigned int ioaddr;
+	ETH_PRIV_DATA *lp;
+
+#ifdef  MAC_DEBUG
+//	printf("MAC:(%s):Send\n",dev->name);
+#endif
+
+	lp = (ETH_PRIV_DATA *)dev->priv;
+	ioaddr =lp->hw_addr;
+
+	cur_desc = &lp->tx_descs[lp->tx_idx];
+	while (cur_desc->mode & TDESC0_OWN)
+	{
+#ifdef  MAC_DEBUG
+		printf("MAC:(%s):Send : Transmit Busy\n",dev->name);
+#endif
+		udelay(10);
+   	}
+
+	/* Compute length to be transfered */
+	length = ETH_ZLEN < length ? length : ETH_ZLEN;
+	length = length > TX_BUF_SIZE ? TX_BUF_SIZE : length;
+
+	/* Copy to Xmit buffer */
+	memcpy((char *)cur_desc->Buffer, packet, length);
+
+	/* Set necessary bits and Length of data */
+	
+	cur_desc->SL = length & 0xFFFF ;
+//	cur_desc->mode = (PaddingMode | CRCMode | MACTxIntEn);
+	cur_desc->mode = (PaddingMode | CRCMode);
+
+//	cur_desc->Length = TDESC1_TCH | TDESC1_FS | TDESC1_LS |length;
+//	cur_desc->Length =  TDESC1_FS | TDESC1_LS |length;
+
+	/* If last descriptor, set end of chain */
+//	if (lp->tx_idx  == (TXDES_NUM -1))
+//		cur_desc->Length |= TDESC1_TER;
+
+	/* Hand it over to MAC ownership */
+	cur_desc->mode |= TDESC0_OWN;
+
+	/* Move curdesc ptr to next desc */
+	lp->tx_idx = (lp->tx_idx+1)%TXDES_NUM;
+
+	/* Start Xmit */
+	outl(0,ioaddr + TSDR);	/* Issue Xmit Demand */
+	return length;
+}
+
+
+static
+int
+wpcm_mac100_recv(struct eth_device *dev)
+{
+	unsigned int ioaddr;
+	int 	packet_length;
+	volatile RECV_DESC *cur_desc;
+	int 	cpy_length;
+	int		start_idx,org_start_idx;
+	int		seg_length;
+	ETH_PRIV_DATA *lp;
+	int 	FrameFS = 0;
+	uchar *data;
+	u32 status=0;
+
+#ifdef  MAC_DEBUG
+//	printf("MAC:(%s):Recv\n",dev->name);
+#endif
+
+	lp= (ETH_PRIV_DATA *)dev->priv;
+	ioaddr = lp->hw_addr;
+
+	packet_length = 0;
+	start_idx = lp->rx_idx;
+
+	while (1)
+	{
+		/* Get the current descriptor*/
+		cur_desc = &lp->rx_descs[lp->rx_idx];
+
+		/* Still owned by MAC. No data yet */
+		if (cur_desc->SL & RDESC0_DMA_OWN)
+		{
+			/* If we already got partial frame, wait for the full frame*/
+			if (FrameFS)
+				continue;
+			goto ReleaseDesc;
+		}
+
+		/* Got a descriptor. Increment our pointer with MAC's pointer */
+		lp->rx_idx = (lp->rx_idx+1)%RXDES_NUM;
+
+		/* Check if error is set */
+		status = inl(ioaddr + MISTA); // RXBus ERR and descriptor unavailable
+		outl(status,ioaddr + MISTA) ; // clear any bits
+
+//		if (cur_desc->Status & RDESC0_ES)
+		if ((cur_desc->SL & 0x006A0000) | (status & (MISTA_RDU |MISTA_RxBErr)))
+		{
+			if (cur_desc->SL & 0x006A0000)
+			{
+				printf("MAC:(%s) : Recv Error : 0x%x\n",dev->name,cur_desc->SL);
+				goto ReleaseDesc;
+			}
+		}
+
+
+
+		/* Check if we got a first descriptor */
+		if (FrameFS == 0)
+		{
+#if 0
+			if (!cur_desc->Status & RDESC0_FD)
+			{
+				printf("ASRMAC(%s): Recv Error : First Descriptor of Frame not got\n",dev->name);
+				goto ReleaseDesc;
+			}
+			else
+#endif
+				FrameFS = 1;
+		}
+
+		/* If we get a last descriptor of the frame, get the frame length */
+		if (cur_desc->SL & RDESC0_RXGD)
+		{
+//			packet_length = RDESC0_FL_GET(cur_desc->Status);
+			packet_length = cur_desc->SL & 0xFFFF ;
+#ifdef  MAC_DEBUG
+//			printf("MAC:(%s):Recevied Frame of size 0x%x\n",dev->name,packet_length);
+#endif
+//			printf("R\n");
+			break;
+		}
+#ifdef  MAC_DEBUG
+//		printf("MAC:(%s):Waiting for full frame \n",dev->name,packet_length);
+#endif
+	}
+
+	/* Check if valid packet */
+	if (packet_length == 0)
+		goto ReleaseDesc;
+
+	/* Copy the data to U-Boot buffer */
+	data = (uchar *)NetRxPackets[0];
+	cpy_length = 0;
+	org_start_idx = start_idx;
+	for (; start_idx!=lp->rx_idx; start_idx=(start_idx+1)%RXDES_NUM)
+	{
+		seg_length = min(packet_length - cpy_length, RX_BUF_SIZE);
+		memcpy(data+cpy_length, (char *)lp->rx_descs[start_idx].Buffer,seg_length);
+#ifdef  MAC_DEBUG
+//		printf("MAC:(%s):Copying Partial Frame of size 0x%x\n",dev->name,seg_length);
+#endif
+		cpy_length += seg_length;
+	}
+	start_idx = org_start_idx;
+/*
+	int z ;
+	for(z=0;z<12;z++) {
+		printf("%x ",data[z]);
+	}
+	printf("\n");
+*/
+	NetReceive(NetRxPackets[0], packet_length);		/* Send to U-Boot layer */
+	
+
+
+ReleaseDesc:
+	/* Release the descriptors back to MAC */
+	for (; start_idx!=lp->rx_idx; start_idx=(start_idx+1)%RXDES_NUM)
+			lp->rx_descs[start_idx].SL = RDESC0_DMA_OWN;
+
+	/* Check if Recv is suspended : Due to unavailable desc */
+	if(status & MISTA_RDU)
+	{
+		outl(0,ioaddr + RSDR);			/* Issue Recv Poll Demand */
+	}
+
+	return packet_length;
+}
+
+/*----------------------------------------------------------------------*/
+/*    				 U-Boot Interface Functions							*/
+/*----------------------------------------------------------------------*/
+static int eth_init_called[MAX_WPCM_ETH];
+static
+int
+wpcm_eth_init(struct eth_device *dev,bd_t *bd)
+{
+	ETH_PRIV_DATA *hw;
+
+#ifdef  MAC_DEBUG
+	printf("MAC:(%s):Init \n",dev->name);
+#endif
+	hw = (ETH_PRIV_DATA *)dev->priv;
+
+	/* Allocate the required decsriptors and buffers */
+	if (!(eth_init_called[hw->devnum]))
+	{
+		if (wpcm_mac100_alloc_buffers(hw) == -1)
+			return -1;
+	}
+
+	wpcm_mac100_open(dev);
+	eth_init_called[hw->devnum] = 1;
+	return 1;
+}
+
+
+static
+void
+wpcm_eth_halt(struct eth_device *dev)
+{
+	hermon_mac100_close(dev);
+}
+
+static
+int
+wpcm_eth_rx(struct eth_device *dev)
+{
+	return wpcm_mac100_recv(dev);
+}
+
+static
+int
+wpcm_eth_send(struct eth_device *dev, volatile void *packet, int length)
+{
+	return hermon_mac100_send_packet(dev,(void *)packet,length);
+}
+
+
+static int eth_initialized = 0;
+int
+wpcm_eth_initialize(bd_t *bis)
+{
+	struct eth_device *dev;
+	int eth_num;
+	ETH_PRIV_DATA *hw;
+	u32 val;
+	unsigned int ioaddr;
+
+#ifdef  MAC_DEBUG
+	printf("MAC: Initialize Called\n");
+#endif
+	if (eth_initialized != 0)
+	{
+		printf("wpcm_eth_initialize: WARNING: Already Initialized\n");
+		return -1;
+	}
+
+	for(eth_num=0;eth_num < MAX_WPCM_ETH;eth_num++)
+	{
+
+		/* Allocate Device Structure */
+		dev = (struct eth_device *) malloc(sizeof(struct eth_device));
+		if (dev == NULL)
+		{
+			printf("wpcm_eth_initialize: Cannot allocate eth_device %d\n",eth_num);
+			return 1;
+		}
+		memset(dev,0,sizeof(*dev));
+
+		/* Allocate Our Private Strcuture */
+		hw = (ETH_PRIV_DATA *) malloc(sizeof (ETH_PRIV_DATA));
+		if (hw == NULL)
+		{
+			printf("wpcm_eth_initialize: Cannot allocate private data %d\n",eth_num);
+			return -1;
+		}
+		memset(hw,0,sizeof(*hw));
+
+
+		switch (eth_num)
+		{
+
+#if defined (CONFIG_NET_MULTI)
+
+#ifdef CONFIG_HAS_ETH1
+			case 1:
+				hw->hw_addr = ioaddr = WPCM_MAC2_BASE;
+				memcpy(dev->enetaddr,bis->bi_enet1addr,6);
+				break;
+#endif
+#endif
+			case 0:
+				hw->hw_addr = ioaddr = WPCM_MAC1_BASE;
+				memcpy(dev->enetaddr,bis->bi_enetaddr,6);
+				break;
+			default:
+				printf("Invalid MAC %d\n",eth_num);
+				return -1 ;
+				break;
+		}
+
+	hw->devnum = eth_num;
+	eth_init_called[hw->devnum] = 0;
+
+	sprintf (dev->name, "wpcm_eth%d", eth_num);
+      	dev->priv = (void *) hw;
+      	dev->init = wpcm_eth_init;
+        dev->halt = wpcm_eth_halt;
+        dev->send = wpcm_eth_send;
+        dev->recv = wpcm_eth_rx;
+
+//	// start the management data clock
+//	outl(MCMDR_EnMDC,ioaddr + MCMDR); 
+
+		/* Install IRQ Handlers here if needed */
+
+	/* Clear pending interrupts */
+	val = inl(ioaddr + MISTA);
+	outl(val, ioaddr + MISTA);
+#ifdef  MAC_DEBUG
+	printf("MAC: Registering %s\n",dev->name);
+#endif
+		eth_register(dev);
+
+#if defined (CONFIG_MII) || (CONFIG_COMMANDS & CFG_CMD_MII)
+		/* register mii command access routines */
+		miiphy_register(dev->name,wpcm_miiphy_read,wpcm_miiphy_write);
+#endif
+		if (eth_num == 0)	// [SMC]
+      DetectPhyAddr(dev->name);
+	}		/* for ethnum */
+
+	eth_initialized = 1;
+	return 1;
+}
+
+
+/* --------------------------------------------------------------------------------
+ *  If CONFIG_NET_MULTI is not defined => Only one ethernet device, define
+ *  eth_xxx functions here (eth.c will compile its eth_xxx functions)
+ *---------------------------------------------------------------------------------
+*/
+#if !defined(CONFIG_NET_MULTI)
+
+struct eth_device *eth_dev = NULL;
+
+struct
+eth_device *
+eth_get_dev(void)
+{
+	return eth_dev;
+}
+
+int
+eth_register(struct eth_device *dev)
+{
+	eth_dev = dev;
+	return 0;
+}
+
+void
+eth_halt(void)
+{
+	if (eth_dev)
+		wpcm_eth_halt(eth_dev);
+	return;
+}
+
+int
+eth_init(bd_t *bis)
+{
+		if (!eth_initialized)
+		{
+			if (wpcm_eth_initialize(bis) == -1)
+				return -1;
+		}
+		if (eth_dev)
+			return wpcm_eth_init(eth_dev,bis);
+		else
+			return -1;
+}
+
+int
+eth_send(volatile void *packet,int length)
+{
+		return wpcm_eth_send(eth_dev,packet,length);
+}
+
+int
+eth_rx(void)
+{
+		return  wpcm_eth_rx(eth_dev);
+}
+
+#endif    /* !CONFIG_NET_MULTI */
