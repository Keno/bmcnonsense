--- uboot/common/cmd_nvedit.c	2007-11-20 16:36:23.000000000 -0500
+++ uboot.new/common/cmd_nvedit.c	2007-11-20 16:17:42.000000000 -0500
@@ -404,6 +404,43 @@
 }
 
 /************************************************************************
+*Gets all the environment variable names
+*/
+int Getenvvar(unsigned char *count,unsigned char *VarName,int *BootVarlen) 
+{
+     int i=0,Counter=0;
+     unsigned char *Nextenv;
+     uchar *env,*nxt = NULL;
+
+     uchar *env_data = env_get_addr(0);
+     Nextenv = VarName;
+     for (env=env_data; *env; env=nxt+1)
+     {
+		i=0;
+		for (nxt=env; *nxt; ++nxt)
+		{			
+			if(*nxt != '\0')
+			{
+			      if(i==0)
+			      {
+			          *count += 1;
+				   while((*Nextenv++ = *nxt++) != '=' ) 
+				   {
+			    		Counter ++;
+				   };
+				   Nextenv --;
+		   		   *Nextenv++ = '\0';
+	    	   		   i =1;
+			      }
+			}
+              }
+      }
+     *BootVarlen = Counter + *count;
+     return 0;
+}
+
+
+/************************************************************************
  * Prompt for environment variable
  */
 
