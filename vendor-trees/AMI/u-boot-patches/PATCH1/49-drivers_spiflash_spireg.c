--- u-boot-1.1.4.ami/drivers/spiflash/spireg.c	1969-12-31 19:00:00.000000000 -0500
+++ u-boot-vsc/drivers/spiflash/spireg.c	2008-08-21 13:46:14.347936734 -0400
@@ -0,0 +1,194 @@
+/*
+ * Copyright (C) 2007 American Megatrends Inc
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
+ * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
+ */
+
+#ifdef __UBOOT__	
+#include <common.h>
+#endif
+#include "spiflash.h"
+#ifdef	CFG_FLASH_SPI_DRIVER
+
+DEFINE_SPINLOCK(spi_chip_drvs_lock);
+DEFINE_SPINLOCK(spi_ctrl_drvs_lock);
+
+#ifdef __UBOOT__  /*MIPS*/
+struct list_head spi_chip_drvs_list = { 0, 0};
+struct list_head spi_ctrl_drvs_list = { 0, 0};
+#else
+LIST_HEAD(spi_chip_drvs_list);
+LIST_HEAD(spi_ctrl_drvs_list);
+#endif
+
+void 
+register_spi_chip_driver(struct spi_chip_driver *drv)
+{
+#ifdef __UBOOT__  /*MIPS*/
+	if ((spi_chip_drvs_list.prev == 0) && (spi_chip_drvs_list.next == 0))
+		INIT_LIST_HEAD(&spi_chip_drvs_list);
+#endif
+	spin_lock(&spi_chip_drvs_lock);
+	list_add(&drv->list, &spi_chip_drvs_list);
+	spin_unlock(&spi_chip_drvs_lock);
+}
+
+void 
+unregister_spi_chip_driver(struct spi_chip_driver *drv)
+{
+	spin_lock(&spi_chip_drvs_lock);
+	list_del(&drv->list);
+	spin_unlock(&spi_chip_drvs_lock);
+}
+
+void 
+register_spi_ctrl_driver(struct spi_ctrl_driver *drv)
+{
+#ifdef __UBOOT__  /*MIPS*/
+	if ((spi_ctrl_drvs_list.prev == 0) && (spi_ctrl_drvs_list.next == 0))
+		INIT_LIST_HEAD(&spi_ctrl_drvs_list);
+#endif
+	spin_lock(&spi_ctrl_drvs_lock);
+	list_add(&drv->list, &spi_ctrl_drvs_list);
+	spin_unlock(&spi_ctrl_drvs_lock);
+}
+
+void 
+unregister_spi_ctrl_driver(struct spi_ctrl_driver *drv)
+{
+	spin_lock(&spi_ctrl_drvs_lock);
+	list_del(&drv->list);
+	spin_unlock(&spi_ctrl_drvs_lock);
+}
+
+struct spi_chip_driver *
+get_spi_chip_driver_by_name (const char *name)
+{
+	struct list_head *pos;
+	struct spi_chip_driver *ret = NULL, *this;
+
+	spin_lock(&spi_chip_drvs_lock);
+
+	list_for_each(pos, &spi_chip_drvs_list) {
+		this = list_entry(pos, typeof(*this), list);
+
+		if (!strcmp(this->name, name)) {
+			ret = this;
+			break;
+		}
+	}
+	if (ret && !try_module_get(ret->module))
+		ret = NULL;
+
+	spin_unlock(&spi_chip_drvs_lock);
+
+	return ret;
+}
+
+struct spi_chip_driver *
+get_spi_chip_driver_by_index (int index)
+{
+	struct list_head *pos;
+	struct spi_chip_driver *ret = NULL, *this;
+	int got;
+
+	spin_lock(&spi_chip_drvs_lock);
+	
+	got=0;
+	list_for_each(pos, &spi_chip_drvs_list) {
+		this = list_entry(pos, typeof(*this), list);
+
+		if (index == got) {
+			ret = this;
+			break;
+		}
+		got++;
+	}
+	if (ret && !try_module_get(ret->module))
+		ret = NULL;
+
+	spin_unlock(&spi_chip_drvs_lock);
+
+	return ret;
+}
+
+
+struct spi_ctrl_driver *
+get_spi_ctrl_driver_by_name (const char *name)
+{
+	struct list_head *pos;
+	struct spi_ctrl_driver *ret = NULL, *this;
+
+	spin_lock(&spi_ctrl_drvs_lock);
+
+	list_for_each(pos, &spi_ctrl_drvs_list) {
+		this = list_entry(pos, typeof(*this), list);
+
+		if (!strcmp(this->name, name)) {
+			ret = this;
+			break;
+		}
+	}
+	if (ret && !try_module_get(ret->module))
+		ret = NULL;
+
+	spin_unlock(&spi_ctrl_drvs_lock);
+
+	return ret;
+}
+
+struct spi_ctrl_driver *
+get_spi_ctrl_driver_by_index (int index)
+{
+	struct list_head *pos;
+	struct spi_ctrl_driver *ret = NULL, *this;
+	int got;
+
+	spin_lock(&spi_ctrl_drvs_lock);
+
+	got=0;
+	list_for_each(pos, &spi_ctrl_drvs_list) {
+		this = list_entry(pos, typeof(*this), list);
+
+		if (index == got) {
+			ret = this;
+			break;
+		}
+		got++;
+	}
+	if (ret && !try_module_get(ret->module))
+		ret = NULL;
+
+	spin_unlock(&spi_ctrl_drvs_lock);
+
+	return ret;
+}
+
+
+
+EXPORT_SYMBOL(get_spi_chip_driver_by_name);
+EXPORT_SYMBOL(get_spi_chip_driver_by_index);
+EXPORT_SYMBOL(get_spi_ctrl_driver_by_name);
+EXPORT_SYMBOL(get_spi_ctrl_driver_by_index);
+EXPORT_SYMBOL(register_spi_ctrl_driver);
+EXPORT_SYMBOL(unregister_spi_ctrl_driver);
+EXPORT_SYMBOL(register_spi_chip_driver);
+EXPORT_SYMBOL(unregister_spi_chip_driver);
+
+MODULE_LICENSE("GPL");
+MODULE_AUTHOR("American Megatrends Inc");
+MODULE_DESCRIPTION("Core routines for (un)registering  spi chip and controller drivers");
+
+#endif
