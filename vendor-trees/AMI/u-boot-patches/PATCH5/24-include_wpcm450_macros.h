--- u-boot-1.1.4-cpu/include/wpcm450/macros.h	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-1.1.4-wpcm/include/wpcm450/macros.h	2007-06-28 01:38:41.000000000 -0400
@@ -0,0 +1,71 @@
+#ifndef __MACROS_H__
+#define __MACROS_H__
+
+typedef unsigned char  UINT8;
+typedef unsigned long  UINT32;
+typedef unsigned short UINT16;
+
+typedef unsigned char UCHAR;
+typedef unsigned int UINT;
+typedef unsigned long ULONG;
+
+typedef int INT;
+typedef char CHAR;
+typedef void VOID;
+typedef unsigned char BOOL;
+
+#define VPlong(x)   			(*(volatile unsigned int *)(x))
+#define VPshort(x) 			(*(volatile unsigned short *)(x))
+#define VPchar(x)  			(*(volatile unsigned char *)(x))
+
+#define    BIT(X)          (1 << (X))
+/*------------------------------------------------------------------------------
+ * BIT operation macros
+ *----------------------------------------------------------------------------*/
+
+#define SET_BIT(reg, bit)       ((reg)|=(0x1<<(bit)))
+#define READ_BIT(reg, bit)      ((reg)&(0x1<<(bit)))
+#define CLEAR_BIT(reg, bit)     ((reg)&=(~(0x1<<(bit))))
+#define IS_BIT_SET(reg, bit)    (((reg)&(0x1<<(bit)))!=0)
+
+/*------------------------------------------------------------------------------
+ * FIELD operation macros
+ *----------------------------------------------------------------------------*/
+
+// Get contents of "field" from "reg"
+#define GET_FIELD(reg, field)   \
+    GET_FIELD_SP(field##_S,field##_P,reg)
+    
+// Set contents of "field" in "reg" to value"
+#define SET_FIELD(reg, field, value)    \
+    SET_FIELD_SP(field##_S,field##_P,reg, value)
+    
+// Auxiliary macro: Get contents of field using size and position
+#define GET_FIELD_SP(size, position, reg)    ((reg>>position)&((1<<size)-1))
+
+// Auxiliary macro: Set contents of field using fields size and position
+#define SET_FIELD_SP(size, position, reg ,value)    \
+    (reg = (reg &(~((((UINT32)1<<size)-1)<<position)))|((UINT32)value<<position))
+
+/*------------------------------------------------------------------------------
+ * MASK operation macros
+ *----------------------------------------------------------------------------*/
+
+#define SET_MASK(reg, bit_mask)       ((reg)|=(bit_mask))
+#define READ_MASK(reg, bit_mask)      ((reg)&(bit_mask))
+#define CLEAR_MASK(reg, bit_mask)     ((reg)&=(~(bit_mask)))
+#define IS_MASK_SET(reg, bit_mask)    (((reg)&(bit_mask))!=0)
+
+#define GET_MSB(x)  ((x&0xFFFF)>>8)
+#define GET_LSB(x)  ((x&0x00FF))
+
+/*------------------------------------------------------------------------------
+ * Bits Masks
+ *----------------------------------------------------------------------------*/ 
+#define BITS_7_0        0xFF
+#define BITS_15_8       0xFF00
+#define BITS_23_16      0xFF0000
+
+#endif 
+
+
