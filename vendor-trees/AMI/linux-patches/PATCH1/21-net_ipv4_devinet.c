--- linux/net/ipv4/devinet.c	2006-02-10 02:22:48.000000000 -0500
+++ linux.tco/net/ipv4/devinet.c	2007-05-10 16:21:44.000000000 -0400
@@ -103,6 +103,41 @@
 static void devinet_sysctl_unregister(struct ipv4_devconf *p);
 #endif
 
+/* AMI Extension Begin */
+void (*ipv4_config_fn) (unsigned long) = NULL;
+char ipv4_config_if[32];
+
+int
+RegisterIPV4Config(char *name,void (*fn)(unsigned long))
+{
+	if (ipv4_config_fn != NULL)
+		return 1;
+	ipv4_config_fn = fn;
+	strcpy(ipv4_config_if,name);
+	return 0;
+}
+
+int
+UnRegisterIPV4Config(char *name)
+{
+	if (strcmp(name,ipv4_config_if) != 0)
+	{
+		printk("WARNING: UnRegisterIPV4Config : Removing unregistered function\n");
+		ipv4_config_fn = NULL;
+		return 1;
+	}	
+	ipv4_config_fn = NULL;
+	return 0;
+}
+
+
+
+EXPORT_SYMBOL(RegisterIPV4Config);
+EXPORT_SYMBOL(UnRegisterIPV4Config);
+
+/* AMI Extension End */
+
+
 /* Locks all the inet devices. */
 
 static struct in_ifaddr *inet_alloc_ifa(void)
@@ -757,6 +792,14 @@
 
 		ifa->ifa_address = ifa->ifa_local = sin->sin_addr.s_addr;
 
+/* AMI Extension Begin */
+		if (ipv4_config_fn)
+		{
+			if (strcmp(ipv4_config_if,ifr.ifr_name) == 0)
+				ipv4_config_fn(sin->sin_addr.s_addr);
+		}
+/* AMI Extension End */
+
 		if (!(dev->flags & IFF_POINTOPOINT)) {
 			ifa->ifa_prefixlen = inet_abc_len(ifa->ifa_address);
 			ifa->ifa_mask = inet_make_mask(ifa->ifa_prefixlen);
