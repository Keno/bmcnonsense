--- linux/include/asm-arm/soc-wpcm450/hwmap.h	1969-12-31 19:00:00.000000000 -0500
+++ linux.mb/include/asm-arm/soc-wpcm450/hwmap.h	2007-07-06 17:33:43.000000000 -0400
@@ -0,0 +1,169 @@
+/*
+ *  This file contains the WPCM450 SOC Register Base, IRQ and DMA  mappings
+ *
+ *  Copyright (C) 2005 American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation; either version 2 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
+ */
+
+#ifndef     _HWREG_WPCM450_H
+#define     _HWMAP_WPCM450_H
+
+
+/********************************************************************
+   Internal Register Mapping  = 0xB0000000 to 0xCFFFFFFF
+*********************************************************************/
+
+#define WPCM_REGISTER_BASE				0xB0000000
+#define WPCM_REGISTER_SIZE				0x20000000
+
+/*--------------------------- Individual Devices ------------------------- */
+#define WPCM_GCR_BASE					0xB0000000
+#define WPCM_CLK_BASE					0xB0000200
+#define WPCM_MC_BASE					0xB0001000
+#define WPCM_MAC1_BASE					0xB0002000
+#define WPCM_MAC2_BASE					0xB0003000
+#define WPCM_USB2_BASE					0xB0005000
+#define WPCM_USB1_BASE					0xB0006000
+#define WPCM_VCD_BASE					0xB0010000
+#define WPCM_UART0_BASE					0xB8000000
+#define WPCM_UART1_BASE					0xB8000100
+#define WPCM_PECI_BASE					0xB8000200
+#define WPCM_GFXI_BASE					0xB8000300
+#define WPCM_TIMER_BASE					0xB8001000
+#define WPCM_AIC_BASE					0xB8002000
+#define WPCM_GPIO_BASE					0xB8003000
+#define WPCM_TACH0_BASE					0xB8004000
+#define WPCM_TACH1_BASE					0xB8005000
+#define WPCM_I2C_0_BASE					0xB8006000
+#define WPCM_I2C_1_BASE					0xB8006100
+#define WPCM_I2C_2_BASE					0xB8006200
+#define WPCM_I2C_3_BASE					0xB8006300
+#define WPCM_I2C_4_BASE					0xB8006400
+#define WPCM_I2C_5_BASE					0xB8006500
+#define WPCM_PWM_BASE					0xB8007000
+#define WPCM_KCS_BASE					0xB8008000
+#define WPCM_ADC_BASE					0xB8009000
+#define WPCM_AES_BASE					0xB800B000
+
+// If these are needed map individually as we don't have much space
+//#define WPCM_SPI_0_BASE					0xC0000000
+//#define WPCM_SPI_1_BASE					0xC1000000
+//#define WPCM_SPI_2_BASE					0xC2000000
+//#define WPCM_SPI_3_BASE					0xC3000000
+//#define WPCM_XBUS_BASE					0xC4000000
+//#define WPCM_AHB3_RAM_BASE				0xC6000000
+
+#define WPCM_FIU_BASE					0xC8000000
+#define WPCM_BOOT_SPI_BASE				WPCM_FIU_BASE
+#define WPCM_SHM_BASE					0xC8001000
+
+
+
+/*--------------- Virtual address of the IO Registers Region  ------------*/
+#define WPCM_REGISTER_VA_BASE				IO_ADDRESS(WPCM_REGISTER_BASE)
+
+#define WPCM_GCR_VA_BASE					IO_ADDRESS(WPCM_GCR_BASE)
+#define WPCM_CLK_VA_BASE					IO_ADDRESS(WPCM_CLK_BASE)
+#define WPCM_MC_VA_BASE						IO_ADDRESS(WPCM_MC_BASE)
+#define WPCM_MAC1_VA_BASE					IO_ADDRESS(WPCM_MAC1_BASE)
+#define WPCM_MAC2_VA_BASE					IO_ADDRESS(WPCM_MAC2_BASE)
+#define WPCM_USB2_VA_BASE					IO_ADDRESS(WPCM_USB2_BASE)
+#define WPCM_USB1_VA_BASE					IO_ADDRESS(WPCM_USB1_BASE)
+#define WPCM_VCD_VA_BASE					IO_ADDRESS(WPCM_VCD_BASE)
+#define WPCM_UART0_VA_BASE					IO_ADDRESS(WPCM_UART0_BASE)
+#define WPCM_UART1_VA_BASE					IO_ADDRESS(WPCM_UART1_BASE)
+#define WPCM_PECI_VA_BASE					IO_ADDRESS(WPCM_PECI_BASE)
+#define WPCM_GFXI_VA_BASE					IO_ADDRESS(WPCM_GFXI_BASE)
+#define WPCM_TIMER_VA_BASE					IO_ADDRESS(WPCM_TIMER_BASE)
+#define WPCM_AIC_VA_BASE					IO_ADDRESS(WPCM_AIC_BASE)
+#define WPCM_GPIO_VA_BASE					IO_ADDRESS(WPCM_GPIO_BASE)
+#define WPCM_TACH0_VA_BASE					IO_ADDRESS(WPCM_TACH0_BASE)
+#define WPCM_TACH1_VA_BASE					IO_ADDRESS(WPCM_TACH1_BASE)
+#define WPCM_I2C_0_VA_BASE					IO_ADDRESS(WPCM_I2C_0_BASE)
+#define WPCM_I2C_1_VA_BASE					IO_ADDRESS(WPCM_I2C_1_BASE)
+#define WPCM_I2C_2_VA_BASE					IO_ADDRESS(WPCM_I2C_2_BASE)
+#define WPCM_I2C_3_VA_BASE					IO_ADDRESS(WPCM_I2C_3_BASE)
+#define WPCM_I2C_4_VA_BASE					IO_ADDRESS(WPCM_I2C_4_BASE)
+#define WPCM_I2C_5_VA_BASE					IO_ADDRESS(WPCM_I2C_5_BASE)
+#define WPCM_PWM_VA_BASE					IO_ADDRESS(WPCM_PWM_BASE)
+#define WPCM_KCS_VA_BASE					IO_ADDRESS(WPCM_KCS_BASE)
+#define WPCM_ADC_VA_BASE					IO_ADDRESS(WPCM_ADC_BASE)
+#define WPCM_AES_VA_BASE					IO_ADDRESS(WPCM_AES_BASE)
+//#define WPCM_SPI_0_VA_BASE					IO_ADDRESS(WPCM_SPI_0_BASE)
+//#define WPCM_SPI_1_VA_BASE					IO_ADDRESS(WPCM_SPI_1_BASE)
+//#define WPCM_SPI_2_VA_BASE					IO_ADDRESS(WPCM_SPI_2_BASE)
+//#define WPCM_SPI_3_VA_BASE					IO_ADDRESS(WPCM_SPI_3_BASE)
+//#define WPCM_XBUS_VA_BASE					IO_ADDRESS(WPCM_XBUS_BASE)
+//#define WPCM_AHB3_RAM_VA_BASE					IO_ADDRESS(WPCM_AHB3_RAM_BASE)
+//#define WPCM_FIU_VA_BASE					IO_ADDRESS(WPCM_FIU_BASE)
+//#define WPCM_SHM_VA_BASE					IO_ADDRESS(WPCM_SHM_BASE)
+
+
+/*****************************************************************
+			  IRQ Assignment
+*****************************************************************/
+#define NR_IRQS							32
+#define MAXIRQNUM                       31
+
+#define IRQ_RESERVED_0					0
+#define IRQ_WDT	  		    			1
+#define IRQ_GPIOE_0       	            2
+#define IRQ_GPIOE_1                     3
+#define IRQ_GPIOE_2                     4
+#define IRQ_GPIOE_3            			5
+#define IRQ_PECI						6
+#define IRQ_UART0                      7
+#define IRQ_UART1						8
+#define IRQ_KCS 						9
+#define IRQ_FIU                         10
+#define IRQ_SHM                 	    11
+#define IRQ_TIMER_0						12
+#define IRQ_TIMER_1                     13
+#define IRQ_TIMER_2_3_4	                14
+#define IRQ_MAC_1_RX                    15
+#define IRQ_MAC_1_TX					16
+#define IRQ_MAC_2_RX					17
+#define IRQ_MAC_2_TX					18
+#define IRQ_RESERVED_19                 19
+#define IRQ_USB_2                   	20
+#define IRQ_USB_1						21
+#define IRQ_VCD                 		22
+#define IRQ_I2C_3                   	23
+#define IRQ_TACH_0                    	24
+#define IRQ_TACH_1						25
+#define IRQ_I2C_0_1_2                   26
+#define IRQ_I2C_4						27
+#define IRQ_PWM							28
+#define IRQ_I2C_5                     	29
+#define IRQ_RESERVED_30                	30
+#define IRQ_ADC							31
+
+/*****************************************************************
+				    APB DMA channel assignment
+*****************************************************************/
+/* TODO */
+
+
+/*****************************   MAC 100 Base Address *********************/
+#define MAX_WPCM_ETH 	 2	
+#define WPCM_MAC100_BASE_0  	WPCM_MAC1_VA_BASE
+#define WPCM_MAC100_BASE_1  	WPCM_MAC2_VA_BASE
+#define WPCM_MAC100_BASE_2  	0
+#define WPCM_MAC100_BASE_3  	0
+
+
+
+#endif
