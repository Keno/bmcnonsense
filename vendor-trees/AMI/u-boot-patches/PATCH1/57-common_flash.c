--- uboot/common/flash.c	2007-07-06 15:22:33.000000000 -0400
+++ uboot.mb/common/flash.c	2007-07-06 14:39:43.000000000 -0400
@@ -121,6 +121,48 @@
 	return (NULL);
 }
 
+int
+flash_read (ulong addr, ulong cnt,char* dest)
+{
+#ifdef CONFIG_SPD823TS
+	return (ERR_TIMOUT);	/* any other error codes are possible as well */
+#else
+
+	ulong         end        = addr + cnt - 1;
+	flash_info_t *info_first = addr2info (addr);
+	flash_info_t *info_last  = addr2info (end );
+	flash_info_t *info;
+	ulong len;
+
+	if (cnt == 0) {
+		return (ERR_OK);
+	}
+
+	if (!info_first || !info_last) {
+		return (ERR_INVAL);
+	}
+
+	while (cnt)
+	{
+		info = addr2info(addr);
+		/* Get remaining size in the bank */
+		len =  info->size - (addr - info->start[0]); 
+
+		/* Copy till the end of bank or less (cnt) */
+		len=(cnt > len)?len:cnt;
+
+		/* finally read data from flash */
+		read_buff(info, (uchar *)dest, addr, len);
+		dest+=len;
+		addr+=len;
+		cnt-=len;
+	}
+	
+	
+	return (ERR_OK);
+#endif /* CONFIG_SPD823TS */
+}
+
 /*-----------------------------------------------------------------------
  * Copy memory to flash.
  * Make sure all target addresses are within Flash bounds,
