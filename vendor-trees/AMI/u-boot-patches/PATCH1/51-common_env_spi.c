--- uboot/common/env_spi.c	1969-12-31 19:00:00.000000000 -0500
+++ uboot.spi/common/env_spi.c	2007-05-15 12:44:32.000000000 -0400
@@ -0,0 +1,216 @@
+/* LowLevel function for SPI environment support
+ * Author : American Megatrends Inc
+ *
+ * This program is free software; you can redistribute it and/or
+ * modify it under the terms of the GNU General Public License as
+ * published by the Free Software Foundation; either version 2 of
+ * the License, or (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ *
+ * You should have received a copy of the GNU General Public License
+ * along with this program; if not, write to the Free Software
+ * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
+ * MA 02111-1307 USA
+ *
+ */
+#include <common.h>
+
+#if defined(CFG_ENV_IS_IN_SPI) /* Environment is in SPI */
+
+#include <command.h>
+#include <environment.h>
+#include <linux/stddef.h>
+extern	void spi_silent_init(void);
+
+#if ((CONFIG_COMMANDS&(CFG_CMD_ENV|CFG_CMD_FLASH)) == (CFG_CMD_ENV|CFG_CMD_FLASH))
+#define CMD_SAVEENV
+#elif defined(CFG_ENV_ADDR_REDUND)
+#error Cannot use CFG_ENV_ADDR_REDUND without CFG_CMD_ENV & CFG_CMD_FLASH
+#endif
+
+#if defined(CFG_ENV_SIZE_REDUND) && (CFG_ENV_SIZE_REDUND < CFG_ENV_SIZE)
+#error CFG_ENV_SIZE_REDUND should not be less then CFG_ENV_SIZE
+#endif
+
+#ifdef CONFIG_INFERNO
+# ifdef CFG_ENV_ADDR_REDUND
+#error CFG_ENV_ADDR_REDUND is not implemented for CONFIG_INFERNO
+# endif
+#endif
+
+char * env_name_spec = "spi";
+
+#ifdef ENV_IS_EMBEDDED
+
+extern uchar environment[];
+env_t *env_ptr = (env_t *)(&environment[0]);
+
+#ifdef CMD_SAVEENV
+/* static env_t *flash_addr = (env_t *)(&environment[0]);-broken on ARM-wd-*/
+static env_t *flash_addr = (env_t *)CFG_ENV_ADDR;
+#endif
+
+#else /* ! ENV_IS_EMBEDDED */
+
+env_t *env_ptr = (env_t *)CFG_ENV_ADDR;
+#ifdef CMD_SAVEENV
+static env_t *flash_addr = (env_t *)CFG_ENV_ADDR;
+
+#endif
+
+#endif /* ENV_IS_EMBEDDED */
+
+#ifdef CFG_ENV_ADDR_REDUND
+static env_t *flash_addr_new = (env_t *)CFG_ENV_ADDR_REDUND;
+
+/* CFG_ENV_ADDR is supposed to be on sector boundary */
+static ulong end_addr = CFG_ENV_ADDR + CFG_ENV_SECT_SIZE - 1;
+static ulong end_addr_new = CFG_ENV_ADDR_REDUND + CFG_ENV_SECT_SIZE - 1;
+
+#define ACTIVE_FLAG   1
+#define OBSOLETE_FLAG 0
+#endif /* CFG_ENV_ADDR_REDUND */
+
+extern uchar default_environment[];
+extern int default_environment_size;
+
+#define ACTIVE_FLAG   1
+#define OBSOLETE_FLAG 0
+
+
+
+
+
+extern uchar default_environment[];
+/* extern int default_environment_size; */
+
+
+uchar env_get_char_spec (int index)
+{
+	uchar c;
+	flash_read (CFG_ENV_ADDR+index+offsetof(env_t,data),1,&c);
+	return (c);
+}
+
+void env_relocate_spec (void)
+{
+	flash_read (CFG_ENV_ADDR,CFG_ENV_SIZE,(uchar *)env_ptr);
+}
+
+#ifdef CMD_SAVEENV
+
+int saveenv(void)
+{
+	int	len, rc;
+	ulong	end_addr;
+	ulong	flash_sect_addr;
+#if defined(CFG_ENV_SECT_SIZE) && (CFG_ENV_SECT_SIZE > CFG_ENV_SIZE)
+	ulong	flash_offset;
+	uchar	env_buffer[CFG_ENV_SECT_SIZE];
+#else
+	uchar *env_buffer = (uchar *)env_ptr;
+#endif	/* CFG_ENV_SECT_SIZE */
+	int rcode = 0;
+
+#if defined(CFG_ENV_SECT_SIZE) && (CFG_ENV_SECT_SIZE > CFG_ENV_SIZE)
+
+	flash_offset    = ((ulong)flash_addr) & (CFG_ENV_SECT_SIZE-1);
+	flash_sect_addr = ((ulong)flash_addr) & ~(CFG_ENV_SECT_SIZE-1);
+
+	debug ( "copy old content: "
+		"sect_addr: %08lX  env_addr: %08lX  offset: %08lX\n",
+		flash_sect_addr, (ulong)flash_addr, flash_offset);
+
+	/* copy old contents to temporary buffer */
+//	memcpy (env_buffer, (void *)flash_sect_addr, CFG_ENV_SECT_SIZE);
+	flash_read(flash_sect_addr,CFG_ENV_SECT_SIZE,env_buffer);
+
+	/* copy current environment to temporary buffer */
+	memcpy ((uchar *)((unsigned long)env_buffer + flash_offset),
+		env_ptr,
+		CFG_ENV_SIZE);
+
+	len	 = CFG_ENV_SECT_SIZE;
+#else
+	flash_sect_addr = (ulong)flash_addr;
+	len	 = CFG_ENV_SIZE;
+#endif	/* CFG_ENV_SECT_SIZE */
+
+#ifndef CONFIG_INFERNO
+	end_addr = flash_sect_addr + len - 1;
+#else
+	/* this is the last sector, and the size is hardcoded here */
+	/* otherwise we will get stack problems on loading 128 KB environment */
+	end_addr = flash_sect_addr + 0x20000 - 1;
+#endif
+
+	printf ("Protect off %08lX ... %08lX\n",
+		(ulong)flash_sect_addr, end_addr);
+
+	if (flash_sect_protect (0, flash_sect_addr, end_addr))
+		return 1;
+
+	puts ("Erasing Flash...");
+	if (flash_sect_erase (flash_sect_addr, end_addr))
+		return 1;
+
+	puts ("Writing to Flash... ");
+	rc = flash_write((char *)env_buffer, flash_sect_addr, len);
+	if (rc != 0) {
+		flash_perror (rc);
+		rcode = 1;
+	} else {
+		puts ("done\n");
+	}
+
+	/* try to re-protect */
+	(void) flash_sect_protect (1, flash_sect_addr, end_addr);
+	return rcode;
+}
+
+#endif /* CMD_SAVEENV */
+/************************************************************************
+ * Initialize Environment use
+ *
+ * We are still running from ROM, so data use is limited
+ * Use a (moderately small) buffer on the stack
+ */
+int env_init(void)
+{
+	DECLARE_GLOBAL_DATA_PTR;
+
+	ulong crc, len, new;
+	unsigned off;
+	uchar buf[64];
+	if (gd->env_valid == 0){
+		spi_silent_init();
+
+		/* read old CRC */
+		flash_read (CFG_ENV_ADDR+offsetof(env_t,crc),sizeof(ulong),(char*)&crc);
+		new = 0;
+		len = ENV_SIZE;
+		off = offsetof(env_t,data);
+		while (len > 0) {
+			int n = (len > sizeof(buf)) ? sizeof(buf) : len;
+			flash_read (CFG_ENV_ADDR+off,n , buf);
+			new = crc32 (new, buf, n);
+			len -= n;
+			off += n;
+		}
+		if (crc == new) {
+			gd->env_addr  = offsetof(env_t,data);
+			gd->env_valid = 1;
+		} else {
+			gd->env_addr  = (ulong)&default_environment[0];
+			gd->env_valid = 0;
+		}
+	}
+
+ 	return (0);
+}
+
+#endif /* CFG_ENV_IS_IN_SPI */
