--- linux.old/drivers/net/arm/wpcm450_mac.h	1969-12-31 19:00:00.000000000 -0500
+++ linux/drivers/net/arm/wpcm450_mac.h	2008-05-13 10:19:51.000000000 -0400
@@ -0,0 +1,306 @@
+/*
+ * Ethernet driver for winbond WPCM450 SOC
+*/
+
+#ifndef _WPCM450_MAC_H_
+#define _WPCM450_MAC_H_
+
+#if defined(CFG_PROJ_NETWORK_ETH0) || defined(CFG_PROJ_NETWORK_ETH1)
+#define MAX_WPCM_MAC  	 1
+#elif defined(CFG_PROJ_NETWORK_BOTH)
+#define MAX_WPCM_MAC  	 2
+#endif
+
+#define MAC_OFFSET       0
+/*-------------------------------- Register Locations -------------------------*/
+//CAM Registers
+#define  CAMCMR			(MAC_OFFSET)    //CAM Command Regiser
+#define  CAMEN			(MAC_OFFSET+0x4)//CAM ennable regiser
+#define  CAM0M			(MAC_OFFSET+0x8)//CAM1 Most significant Word register
+#define  CAM0L			(MAC_OFFSET+0xc)//CAM1 Least Significant Word Register
+#define  CAM_ENTRY_SIZE	0x8     		//CAM  entry size
+#define  CAM_ENTRIES	0x16    		//CAM  entries
+
+//MAC Regiseters
+#define MIEN			(MAC_OFFSET+0xac) //MAC Interrupt Enable Register
+#define MCMDR			(MAC_OFFSET+0x90) //MAC Command Regiser
+#define MIID			(MAC_OFFSET+0x94) //MII Management Data Register
+#define MIIDA			(MAC_OFFSET+0x98) //MII Management Data Control and Address Register
+#define MPCNT			(MAC_OFFSET+0xb8) //Missed Packet Counter Register
+
+//DMA Registers
+#define TXDLSA			(MAC_OFFSET+0x88) //Transmit Descriptor Link List Start Address Regiser 
+#define RXDLSA			(MAC_OFFSET+0x8c) //Receive Descriptor LInk List Start Addresss Register
+#define DMARFC			(MAC_OFFSET+0xa8) //DMA Receive Frame Control Register
+#define TSDR			(MAC_OFFSET+0xa0) //Transmit Start Demand Register
+#define RSDR			(MAC_OFFSET+0xa4) //Recevie Start Demand Register
+#define FIFOTHD			(MAC_OFFSET+0x9c) //FIFO Threshold Adjustment Register
+
+//EMC Status Register
+#define MISTA			(MAC_OFFSET+0xb0) //MAC Interrupter Status Register
+#define MGSTA			(MAC_OFFSET+0xb4) //MAC General Status Register
+#define MRPC			(MAC_OFFSET+0xbc)  //MAC Receive Pauese counter register
+#define MRPCC			(MAC_OFFSET+0xc0) //MAC Receive Pauese Current Count Regiser
+#define MREPC			(MAC_OFFSET+0xc4)  //MAC Remote pause count retister
+
+//DMA Registers
+#define DMARFS			(MAC_OFFSET+0xc8) //DMA Receive Frame Status Register
+#define CTXDSA			(MAC_OFFSET+0xcc) //Current Transmit Descriptor Start Addresss Register
+#define CTXBSA			(MAC_OFFSET+0xd0) //Current Transmit Buffer Start Address Regiser
+#define CRXDSA			(MAC_OFFSET+0xd4) //Current Receive Descriptor start Address regiser
+#define CRXBSA			(MAC_OFFSET+0xd8) //Current Receive Buffer Start Address Regiser
+
+
+/*------------------------------------Descriptors -------------------------------*/
+typedef struct 
+{
+	volatile unsigned long	SL;
+	volatile unsigned long	buffer;
+	volatile unsigned long	reserved;
+	volatile unsigned long	next;
+}RXBD;
+
+
+
+//ownership bit
+#define	RX_OWNERSHIP_CPU	(0x0<<30)
+#define	RX_OWNERSHIP_DMA	(0x2<<30)
+
+// RX Frame Descriptor's Owner bit
+#define RXfOwnership_DMA 0x80000000  // 10 = DMA
+#define RXfOwnership_CPU 0x3fffffff  // 00 = CPU
+
+//received descriptor status
+#define RXDS_RXINTR     (1<<16) //set if reception of packet caused an interrupt condition
+#define RXDS_CRCE		(1<<17) // set if crc error 
+#define RXDS_PTLE		(1<<19) //set if received frame longer than 1518 bytes
+#define RXDS_RXGD		(1<<20) // receiving good packet 
+#define RXDS_ALIE		(1<<21) //Alignment Error
+#define RXDS_RP			(1<<22) //runt packet	
+
+typedef struct 
+{
+	volatile unsigned long mode;
+	volatile unsigned long buffer;
+	volatile unsigned long SL;
+	volatile unsigned long next;
+}TXBD;
+
+// TX Frame Descriptor's Owner bit
+#define TXfOwnership_DMA 0x80000000  // 1 = DMA
+#define TXfOwnership_CPU 0x7fffffff  // 0 = CPU
+
+//Tx ownership bit
+#define TX_OWNERSHIP_CPU  (0x0<<31) 
+#define TX_OWNERSHIP_DMA  (0x1<<31)
+
+// Tx Frame Descriptor's Control bits
+#define MACTxIntEn    0x04
+#define CRCMode       0x02
+#define NoCRCMode     0x00
+#define PaddingMode   0x01
+#define NoPaddingMode 0x00
+
+//tx mode 
+#define  TX_MODE_PAD	0x1	  	 //pad
+#define  TX_MODE_CRC	(0x1<<1)  //crc mode 
+#define  TX_MODE_IE		(0x1<<2)  //interrupt enable
+
+//Tx status
+#define TXDS_TXINTR 	(1<<16)		//Interruput on Transmit
+#define TXDS_DEF		(1<<17)		//Transmit defered
+#define TXDS_TXCP		(1<<19)		//Transmit Completion
+#define TXDS_EXDEF		(1<<20)		//exceed deferal
+#define TXDS_NCS		(1<<21)		//No Carrier Sense Error
+#define TXDS_TXABT		(1<<22)		//transimtting aborted
+#define TXDS_LC			(1<<23)		//late collision
+#define TXDS_TXHA		(1<<24)		//transmitting halted
+#define TXDS_PAU		(1<<25)		//Paused
+#define TXDS_SQE		(1<<26)		//SQE error
+#define TXDS_CCNT		(0xf<<28)	//transmit collision count
+
+/*----------------------- Register Bit Definitions -----------------------*/
+
+//cam command regiser
+#define CAMCMR_AUP		(0x1<<0) //Accept unicast packet
+#define CAMCMR_AMP		(0x1<<1) //Accpet multicast packet
+#define CAMCMR_ABP		(0x1<<2) //Accept broadcast packet 
+#define CAMCMR_CCAM		(0x1<<3) //complement CAM
+#define CAMCMR_ECMP		(0x1<<4) //Enable CAM compare
+
+
+//MAC Interrupt Enable Register
+#define MIEN_EnRXINTR	(1<<0)
+#define MIEN_EnCRCE		(1<<1)
+#define MIEN_EnRXOV		(1<<2)
+#define MIEN_EnPTLE		(1<<3)
+#define MIEN_EnRXGD		(1<<4)
+#define MIEN_EnALIE		(1<<5)
+#define MIEN_EnRP		(1<<6)
+#define MIEN_EnMMP		(1<<7)
+#define MIEN_EnDFO		(1<<8)
+#define MIEN_EnDEN		(1<<9)
+#define MIEN_EnRDU		(1<<10)
+#define MIEN_EnRxBErr	(1<<11)
+#define MIEN_EnCFR		(1<<14)
+#define MIEN_EnTXINTR	(1<<16)
+#define MIEN_EnTXEMP	(1<<17)
+#define MIEN_EnTXCP		(1<<18)
+#define MIEN_EnEXDEF	(1<<19)
+#define MIEN_EnNCS		(1<<20)
+#define MIEN_EnTXABT	(1<<21)
+#define MIEN_EnLC		(1<<22)
+#define MIEN_EnTDU		(1<<23)
+#define MIEN_EnTxBErr	(1<<24)
+
+//MAC Command Regiser
+#define MCMDR_RXON		(1<<0)
+#define MCMDR_ALP		(1<<1)
+#define MCMDR_ARP		(1<<2)
+#define MCMDR_ACP		(1<<3)
+#define MCMDR_AEP		(1<<4)
+#define MCMDR_SPCRC		(1<<5)
+#define MCMDR_TXON		(1<<8)
+#define MCMDR_NDEF		(1<<9)
+#define MCMDR_SDPZ		(1<<16)
+#define MCMDR_EnSQE		(1<<17)
+#define MCMDR_FDUP		(1<<18)
+#define MCMDR_EnMDC		(1<<19)
+#define MCMDR_OPMOD		(1<<20)
+#define MCMDR_LBK		(1<<21)
+#define MCMDR_SWR		(1<<24)
+
+
+//MAC MII Management Data Control and Address Register
+#define  MIIDA_PHYRAD	(0x1F<<0)
+#define  MIIDA_PHYAD	(0x1F<<8)
+#define  MIIDA_WR		(1<<16)
+#define  MIIDA_BUSY		(1<<17)
+#define  MIIDA_PreSP	(1<<18)
+#define  MIIDA_MDCON	(1<<19)
+#define  MIIDA_MDCCR	(3<<20)
+
+#define  PHYBUSY  MIIDA_BUSY
+#define  PHYWR    MIIDA_WR
+#define  MDCCR   (1 << 20)
+
+
+//FIFO Threshold Adjustment Register 
+#define  FIFOTHD_RxTHD	 (3<<0)
+#define  FIFOTHD_TxTHD	 (3<<8)
+#define  FIFOTHD_Blength (3<<20)
+
+//MAC Interrupt Status Register
+#define MISTA_RXINTR	(1<<0)
+#define MISTA_CRCE		(1<<1)
+#define MISTA_RXOV		(1<<2)
+#define MISTA_PTLE		(1<<3)
+#define MISTA_RXGD		(1<<4)
+#define MISTA_ALIE		(1<<5)
+#define MISTA_RP		(1<<6)
+#define MISTA_MMP		(1<<7)
+#define MISTA_DFOI		(1<<8)
+#define MISTA_DENI		(1<<9)
+#define MISTA_RDU		(1<<10)
+#define MISTA_RxBErr		(1<<11)
+#define MISTA_CFR		(1<<14)
+#define MISTA_TXINTR	(1<<16)
+#define MISTA_TXEMP		(1<<17)
+#define MISTA_TXCP		(1<<18)
+#define MISTA_EXDEF		(1<<19)
+#define MISTA_NCS		(1<<20)
+#define MISTA_TXABT		(1<<21)
+#define MISTA_LC		(1<<22)
+#define MISTA_TDU		(1<<23)
+#define MISTA_TxBErr	(1<<24)
+
+//MAC General Status Register
+#define MGSTA_CFR		(1<<0)
+#define MGSTA_RXHA		(1<<1)
+#define MGSTA_RFFull	(1<<2) 
+#define MGSTA_CCNT		(0xf<<4)
+#define MGSTA_DEF		(1<<8)
+#define MGSTA_PAU		(1<<9)
+#define MGSTA_SQE		(1<<10)
+#define MGSTA_TXHA		(1<<11)
+
+
+#define wpcm_WriteReg(reg,val,which) \
+  (*((volatile unsigned int *)(wpcm_mac_io[(which)]+(reg)))=(val))
+
+#define wpcm_ReadReg(reg,which)      \
+ (*((volatile unsigned int *)(wpcm_mac_io[(which)]+(reg))))
+
+#define wpcm_WriteCam0(which,x,lsw,msw) \
+		wpcm_WriteReg(CAM0L+(x)*CAM_ENTRY_SIZE,lsw,which);\
+		wpcm_WriteReg(CAM0M+(x)*CAM_ENTRY_SIZE,msw,which);\
+
+/* Driver related stuff */
+#define POLL_LINK_STATUS	
+#define POLL_TIMEOUT    	(10)
+
+#define RX_DESC_SIZE 		(32)
+#define TX_DESC_SIZE 		(16)
+#define PACKET_BUFFER_SIZE 	(1600)
+
+#define TX_TIMEOUT  		(2 * HZ)	/* 2 Seconds */
+
+#define SKB_NUMBER 		(32)
+
+#define PACKET_SIZE   		1560
+#define SKB_DO_STEP(x)     	x = ((x+1 == SKB_NUMBER)? 0 : x+1)        
+
+struct  wpcm_mac_local
+{
+	struct net_device_stats stats;	
+//	spinlock_t 		lock;
+	unsigned long 	which;
+
+	volatile unsigned long is_rx_all;
+
+	unsigned long rx_packets;
+	unsigned long rx_bytes;
+
+	struct sk_buff* skb[SKB_NUMBER];
+ 	int skb_oldest;
+	int skb_newest;
+	atomic_t  skb_count;
+
+#ifdef POLL_LINK_STATUS
+	struct 	timer_list timer;
+#endif
+	char	LinkDown;
+
+	unsigned long mcmdr;
+
+	volatile unsigned long cur_tx_entry;
+
+	volatile unsigned long tx_index; 
+	volatile unsigned long rx_index;
+
+	RXBD *rx_desc;
+	TXBD *tx_desc;
+	char *rx_buf;
+	char *tx_buf;
+
+	RXBD *rx_desc_dma;
+	TXBD *tx_desc_dma;
+	char *rx_buf_dma;
+	char *tx_buf_dma;
+};
+ 
+
+irqreturn_t _rx_interrupt(int irq, void *dev_id);
+irqreturn_t _tx_interrupt(int irq, void *dev_id);
+
+void netdev_rx(struct net_device *dev);
+void ResetMAC(struct net_device * dev);
+int do_wpcm_mac_probe(struct net_device *dev);
+void DumpRegisters(int which);
+
+void DetectPhy(int num);
+int ResetPhyChip(int num);
+int GetLinkStatus(int num);
+int IsPhyAccessLocked(void);
+		
+#endif 
