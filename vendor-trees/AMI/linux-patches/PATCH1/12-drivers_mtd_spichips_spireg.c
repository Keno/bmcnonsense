--- linux/drivers/mtd/spichips/spireg.c	1969-12-31 19:00:00.000000000 -0500
+++ linux.mb/drivers/mtd/spichips/spireg.c	2007-07-09 15:40:40.000000000 -0400
@@ -0,0 +1,180 @@
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
+LIST_HEAD(spi_chip_drvs_list);
+DEFINE_SPINLOCK(spi_ctrl_drvs_lock);
+LIST_HEAD(spi_ctrl_drvs_list);
+
+void 
+register_spi_chip_driver(struct spi_chip_driver *drv)
+{
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
