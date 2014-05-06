---
author: jarret
comments: true
date: 2012-08-06 16:37:00+00:00
layout: post
slug: using-a-custom-ca-with-vsphere-replication
title: Using Custom Root CA Certificates with VMware SRM 5.x vSphere Replication
wordpress_id: 1759
categories:
- SRM
- Storage
- VMware
ef_usergroup:
- JJK
following_users:
- jarret
- kelatov
tags:
- CA
- Certifications
- cli
- command line
- openssl
- srm
- VRMS
- VRS
- vSphere Replication
---

A friend of mine recently pointed out this [blog post](http://virtualizationinformation.com/srm-certificates-and-sphere-replication/) when talking about SRM with custom certificates. Oddly enough, I had a similar customer where we hacked away at the VRMS and VRS to get the Root CA into the appliances. Since I just wrote [this blog post](http://virtuallyhyper.com/2012/08/setting-up-srm-with-custom-certificates/) about getting SRM installed with a custom Root CA, I figured that I would write one about getting it set up with vSphere replication.

**Warning:** I have no idea if this is supported by VMware or not. As far as my personal testing, everything works fine.

Setting up Signed Certificates is normally an easy process if the Root CA is trusted in the VRMS and VRS keystore. The problem is there is not easy way to add a Root CA to the keystores on the VRMS and VRS. So if you have a custom Root CA, or one like [InCommon](http://www.incommon.org/cert/), which is not in the keystore already you have to hack the appliances.

First, you need to have SRM installed with custom certificates. If you are looking to install SRM with custom certificates, make sure to generate them correctly by following [this KB](http://kb.vmware.com/kb/1008390). I went over the procedure in [this post](http://virtuallyhyper.com/2012/08/setting-up-srm-with-custom-certificates/). Once you have SRM installed with custom certs and the sites paired, you can start with vSphere replication.


## Configuring the vSphere Replication Management Server (VRMS)




### Generating Signed Certificates for VRMS


First off let's generate a certificate for the VRMS servers. You will have to generate one for each site.

The documentation is lacking, but you can use the subjectAltName, or use the Common Name (CN) for the certificates. It just needs to match the FQDN. A [VMware KB](http://kb.vmware.com/kb/2013275) article says the following.


> A valid certificate is signed by a trusted Certificate Authority. VRMS trusts all CAs that the Java Virtual Machine trusts and all certificates stored in opt/vmware/hms/security/hms-truststore.jks file in the new VRMS appliance. The certificate must not be expired, and the certificate subject or alternative subject name must match the FQDN of the server.


But it also says


> The certificate used by each member of a replication server pair must include a Subject Alternative Name attribute. The attribute value is the fully-qualified domain name of the VRMS server host. This value is different for each member of the VRMS server pair. The fully qualified domain name is subject to a case-sensitive comparison. Always use lower case letters when specifying the name during vSphere Replication installation.


The current documentation is confusing and severely lacking, but here is what works.



	
  * The certificate must be a PKCS#12 format with a pfx extension.

	
  * You can have either the common name or the subjectAltName match the hostname set in the web interface (VAMI) Either should be sufficient, but having both allows for a custom CA.

	
  * The Root CA needs to be added to /opt/vmware/hms/security/hms-truststore.jks.

	
  * The certificate cannot have an MD5 signature.

	
  * The certificate should have 2048-bit keys.


Based on the list above, you can decide how you want to generate the certificate. I will list out the steps that I use below.

So let's generate the CSR. First we need to copy over the openssl.cnf locally and edit it for our local changes.


```

# mkdir srm-vrms-prod &amp;&amp; cd srm-vrms-prod
# cp /etc/ssl/openssl.cnf srm-vrms-prod.cnf

```


Now we need to add the subjectAltName to the srm-vrms-prod.cnf (local copy of the openss.cnf). Since I do not want to put this into the Subject, we need to enable the v3_req extensions. Edit the srm-vrms-prod.cnf and uncomment the following line.


```

req_extensions = v3_req

```


Now find the [ v3_req ] section. In this section we need to add the subjectAltName like the line below.


```

subjectAltName = DNS: srm-vrms-prod.virtuallyhyper.com

```


Save the srm-vrms-prod.cnf file and generate the CSR.


```

# openssl req -new -newkey rsa:2048 -sha1  -nodes -out srm-vrms-prod.csr -keyout srm-vrms-prod.key -config srm-vrms-prod.cnf
Generating a 2048 bit RSA private key
...............+++
................................+++
writing new private key to 'srm-vrms-prod.key'
-----
You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [US]:US
State or Province Name (full name) [Colorado]:Colorado
Locality Name (eg, city) [Boulder]:Boulder
Organization Name (eg, company) [Virutallyhyper]:Virtuallyhyper
Organizational Unit Name (eg, section) [Lab]:Lab
Common Name (eg, YOUR name) []:srm-vrms-prod.virtuallyhyper.com
Email Address [admin'@'virtuallyhyper.com]:admin'@'virtuallyhyper.com
Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:

```


Now we need to sign the certificate. You will likely have to give this to your security admin or request signing from the CA website. Here I am my own CA, so I can sign it locally.


```

# openssl ca -days 3650 -in srm-vrms-prod.csr -out srm-vrms-prod.crt

```


Now let's confirm that our CA did not strip off the subjectAltName extension.


```

# openssl x509 -in srm-vrms-prod.crt -text -noout |grep -A1 &quot;Subject Alternative Name&quot;
            X509v3 Subject Alternative Name:
                DNS:srm-vrms-prod.virtuallyhyper.com

```


Create the PFX file. Openssl will query you for a password, make sure to use something you will remember.


```

# openssl pkcs12 -export -in srm-vrms-prod.crt -inkey srm-vrms-prod.key -name &quot;srm-vrms-prod&quot; -out srm-vrms-prod.pfx

```


Now we have generated the certificate for the protected site. Please use the same proceedure to create the certificate for the recovery site.


### Installing the certificate into the VRMS


After getting the SRM servers paired using custom certificates you are ready to deploy the VRMS on both sites. It is best to deploy each VRMS on its respective vCenter to avoid any potential problems. Before deploying the VRMS, make sure you set the "vCenter Server Managed IP" in the Runtime Settings of vCenter Server Settings.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/vCenter-Managed-Ip-address.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/vCenter-Managed-Ip-address.png)

Now you can open up the SRM plug-in and click on vSphere replication on the bottom left of the page.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/SRM-select-vSphere-Replication.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/SRM-select-vSphere-Replication.png)

From here you will see the steps to getting vSphere replication going. The first step is to deploy the VRMS. When deploying the appliance you go through a wizard to configure the IP address and set the password.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Deploy-VRMS.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Deploy-VRMS.png)

I would advise deploying the appliances when connected to the respective SRM servers. Then we need to insert the root CA into the java keystore on the VRMS before we configure VRMS.

Once they are deployed on both sites, we will need to ssh into each one of the servers so that we can copy in the certificates. The user name will be root and the password is what you set when deploying the appliance. The problem is that root ssh is disabled by default. So we need to enable root ssh, or create another user on the appliance. Log into the console and run the following commands.


```

# sed -i 's/PermitRootLogon no/PermitRootLogon yes/' /etc/ssh/sshd_config
# /etc/init.d/sshd restart

```


Alternatively to the commands above, you can edit the /etc/ssh/sshd_config file and change "PermitRootLogon no" to "PermitRootLogon yes" and then restart the sshd service.

Now ssh into the VRMS using the root user name and the password that we used before. Let's create a certificate folder that we are going to put the root CA certificates into.


```

# mkdir /root/certs
# cd /root/certs

```


Now we need to copy over the certificate to the VRMS. If you are using windows you can use [winscp](http://winscp.net) to copy these, on linux you can scp them over.


```

# scp cacert.crt root'@'srm-vrms-prod.virtuallyhyper.com:/root/certs/
vSphere Replication Management Server
root at srm-vrms-prod.virtuallyhyper.com's password:
cacert.crt           100% |*******************************************************************************************|  1651       00:00

```


Now let's add the Root CA Cert to the java keystore for the VRMS. To do this we will use keytool. More syntax can be [found here](http://www.sslshopper.com/article-most-common-java-keytool-keystore-commands.html). The password for the keystore is "vmware".


```

# cd /opt/vmware/hms/security
# keytool -import -trustcacerts -alias root -file /root/certs/cacert.crt -keystore hms-truststore.jks
Enter keystore password:
Owner: EMAILADDRESS=admin'@'virtuallyhyper.com, CN=virtuallyhyper.com, OU=Lab, O=Virtuallyhyper, L=Boulder, ST=Colorado, C=US
Issuer: EMAILADDRESS=admin'@'virtuallyhyper.com, CN=virtuallyhyper.com, OU=Lab, O=Virtuallyhyper, L=Boulder, ST=Colorado, C=US
Serial number: 8c456ac6e8d7d055
Valid from: Sat Aug 04 17:09:39 UTC 2012 until: Tue Aug 02 17:09:39 UTC 2022
Certificate fingerprints:
	 MD5:  B6:59:FD:70:D9:1E:24:02:B0:26:69:5E:B7:F2:50:8D
	 SHA1: 0E:BC:68:96:F2:D7:F9:FD:7F:95:4C:31:BB:65:19:8B:B0:30:F9:48
	 Signature algorithm name: SHA1withRSA
	 Version: 3
Extensions:
#1: ObjectId: 2.5.29.14 Criticality=false
SubjectKeyIdentifier [
KeyIdentifier [
0000: F4 0A A8 D0 EC A1 9D AC   F8 0D C4 56 6B C1 79 1B  ...........Vk.y.
0010: 34 87 90 45                                        4..E
]
]
#2: ObjectId: 2.5.29.19 Criticality=false
BasicConstraints:[
  CA:true
  PathLen:2147483647&lt; ] #3: ObjectId: 2.5.29.35 Criticality=false AuthorityKeyIdentifier [ KeyIdentifier [ 0000: F4 0A A8 D0 EC A1 9D AC   F8 0D C4 56 6B C1 79 1B  ...........Vk.y. 0010: 34 87 90 45                                        4..E ] [EMAILADDRESS=admin'@'virtuallyhyper.com, CN=virtuallyhyper.com, OU=Lab, O=Virtuallyhyper, L=Boulder, ST=Colorado, C=US] SerialNumber: [    8c456ac6 e8d7d055] ] Trust this certificate? [no]:  yes Certificate was added to keystore 
```


Let's confirm that it actually took the certificate. Run the following command and enter the password ("vmware"). You will see the CA certificate that we added.


```
 # keytool -list -alias root -keystore hms-truststore.jks Enter keystore password: root, Aug 5, 2012, trustedCertEntry, Certificate fingerprint (MD5): B6:59:FD:70:D9:1E:24:02:B0:26:69:5E:B7:F2:50:8D 
```


So we have added the Root CA Certificate to the java keystore, but we also need to add it to OS keystore. The OS keystore is located at /etc/ssl/certs/. The format for the the certificates in this directory is that there is a symbolic link that links to the file. The symbolic link name is a hash of the certificate with .0 appended. Let's look at the existing Go Daddy certificate in this directory to get an idea of the format.


```
 # ls -l /etc/ssl/certs/219d9499.0 lrwxrwxrwx 1 root root 23 Feb 25 06:34 219d9499.0 -&gt; Go_Daddy_Class_2_CA.pem
# ls -l /etc/ssl/certs/Go_Daddy_Class_2_CA.pem
-rw-r--r-- 1 root root 1448 May  5  2010 Go_Daddy_Class_2_CA.pem

```


So what we need is the hash of our certificate to link to the .pem file. Since my certificate is in pem format, I can copy it over to this directory and change the extension.


```

# cd /etc/ssl/certs/
# cp /root/certs/cacert.crt /etc/ssl/certs/virtuallyhyper.pem
# chmod 644 /etc/ssl/certs/virtuallyhyper.pem

```


Now let's create the hash symlink to the pem file. Before we make the link, let's just confirm that we are not going to overwrite anything.


```

# openssl x509 -noout -hash -in /etc/ssl/certs/virtuallyhyper.pem
41c89313
# ls -l 41c89313*

```


If the last ls command did not return anything, let's create the symlink.


```

# ln -s /etc/ssl/certs/virtuallyhyper.pem `openssl x509 -noout -hash -in /etc/ssl/certs/virtuallyhyper.pem`.0
# ls -l 41c89313.0
lrwxrwxrwx 1 root root 27 Aug  5 01:06 41c8913.0 -&gt; /etc/ssl/certs/virtuallyhyper.pem

```


If you want the web interface (VAMI) to use the certificate you can follow the instructions below. This is optional, but you might as well. Copy over the CRT and KEY files for your certificate to the VRMS server. We will concatenate them and use them for the lighthttpd service that runs the VAMI.


```

scp srm-vrms-prod.key srm-vrms-prod.crt root'@'srm51-vrms-prod.virtuallyhyper.com:/root/certs/
vSphere Replication Management Server
root at srm51-vrms-prod.virtuallyhyper.com's password:
srm-vrms-prod.key      100% |*********************************************************************|  1675       00:00
srm-vrms-prod.crt      100% |*********************************************************************|  4959       00:00

```


Ssh over to the VRMS again. Now we will concatenate the certificates and then replace the existing self signed certificate.


```

# cd /root/certs/
# cat srm-vrms-prod.key srm-vrms-prod.crt &gt; srm-vrms-prod.pem
# mv /opt/vmware/etc/lighttpd/server.pem /opt/vmware/etc/lighttpd/server.pem.old
# cp /root/certs/srm-vrms-dr.pem /opt/vmware/etc/lighttpd/server.pem
# /etc/init.d/vami-lighttp restart

```


Let's disable root ssh for security reasons. Again you can manually edit the /etc/ssh/sshd_config file and change "PermitRootLogin" to no, or you can run the command below.


```

# sed -i 's/PermitRootLogon yes/PermitRootLogon no/' /etc/ssh/sshd_config

```


Now reboot the VRMS to make sure that all changes take effect. Make sure that you do the steps above on both sites.


### Configuring the VRMS


We can now configure the VRMS on both sites. In the SRM plug-in click on "Configure the VRM Server" to launch the web interface.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Configure-VRMS.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Configure-VRMS.png)

Log in to the web interface using the root username and the password you gave it when deploying. Click on the configuration tab. At the bottom you will have an option to upload a PFX certificate. Upload the one we generated previously. It will ask you for the password. Enter the one you used when generating the P12 file.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRMS-upload-certificate.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRMS-upload-certificate.png)

After the certificate has been uploaded, enter all of the information on the page. Make sure to use the FQDN of the VRMS  in the VRM Host section. This is the name or IP it will use to connect to SRM.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRMS-VRM-Host-name.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRMS-VRM-Host-name.png)

Also make sure to use FQDN [vCenter Address](http://kb.vmware.com/kb/2007463) (Shameless promotion of my KB. Please rate it well), as it will default to the IP address. Otherwise you will get a 404 error and will have to wipe the database.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRMS-vCenter-Server-Address.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRMS-vCenter-Server-Address.png)

After filling out the correct information click "Save and Restart Service". The VRMS will gather the vCenter and SRM certs. It will complain about the SRM cert. The error will be "The certificate was not issued for use with the given hostname: srm-dr.virtuallyhyper.com". This is because the common name does not match the hostname. This is expected behavior, so accept the certificate.

After it has completed, VRMS will have registered it's self with vCenter. If you get a certificate warning in vCenter, you have generated the certificate wrong for VRMS or miss typed the VRM host field. If this is the case examine the certificate to see what is wrong with it.

Now we can pair the VRMS. Click on the "Configure VRMS Connection" to pair the VRMS.
[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/SRM-VRMS-pairing.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/SRM-VRMS-pairing.png)


## Deploying and Configuring vSphere Replication Server (VRS)


Now that we have the VRMS set up with custom certificates and paired, we can deploy the VRS and set them up for custom certificates. The first thing we should do is deploy a VRS on both sites. Click on "Deploy a VR Server" to deploy a VRS on both sites.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRS-Deploy1.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRS-Deploy1.png)

At this point we have a VRS deployed on both sites. If we have setup our certificates correctly we can register the VRS with out having to put custom certificates on the VRS. If you have problems registering the VRS, then you will need to add the custom certificate to the VRS.

The VRS requires a CRT and a Key instead of a PCKS#12. We will need to generate both certificates for all of the VRS we have. You can have more than one on each site, and it is suggested to have multiple VRS depending on the size of your environment. We generate these in the same manner as we did with the VRMS certificates. The one difference is that we are going to add the IP in as a subjectAltName. This is because the VRS has a problem where the hostname [is always localhost.localdom](http://kb.vmware.com/kb/2012535).

Make a new directory and copy over the openssl.cnf.


```

# mkdir srm-vrs-prod &amp;&amp; cd srm-vrs-prod
# cp /etc/ssl/openssl.cnf srm-vrs-prod.cnf

```


Like we did with the VRMS we will make the changes again to enable extensions and add in the subjectAltName. Notice that this time we have included the IP and the DNS name.

Uncomment the line below.


```

req_extensions = v3_req

```


Add the subjectAltName to the [ v3_req ] section.


```

subjectAltName = DNS:srm-vrs-prod.virtuallyhyper.com,IP:192.168.10.86

```


Now let's generate the CSR using srm-vrs-prod.cnf and then sign it.


```

# openssl req -new -newkey rsa:2048 -sha1  -nodes -out srm-vrs-prod.csr -keyout srm-vrs-prod.key -config srm-vrs-prod.cnf
Generating a 2048 bit RSA private key
..............................................+++
..+++
writing new private key to 'srm-vrs-prod.key'
-----
You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,
If you enter '.', the field will be left blank.
-----
Country Name (2 letter code) [US]:US
State or Province Name (full name) [Colorado]:Colorado
Locality Name (eg, city) [Boulder]:Boulder
Organization Name (eg, company) [Virtuallyhyper]:Virtuallyhyper
Organizational Unit Name (eg, section) [Lab]:Lab
Common Name (eg, YOUR name) []:srm-vrs-prod.virtuallyhyper.com
Email Address [admin'@'virtuallyhyper.com]:admin'@'virtuallyhyper.com

Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:
# openssl ca -days 3650 -in srm-vrs-prod.csr -out srm-vrs-prod.crt

```


Now we have to install the CA into VRS the same way we did on the VRMS. We will inset the Root CA Certificate into /etc/ssl/certs and make the symbolic link.

The nice thing about VRS is that root SSH is enabled by default. The first thing we need to do is ssh to the VRS and make a directory to copy over the certificates. The default root password is "vmware"


```

# mkdir /root/certs

```


scp the certificates we made and the Root CA certificate over to the VRS.


```

# scp srm-vrs-prod.key srm-vrs-prod.crt cacert.crt root'@'srm-vrs-prod.virtuallyhyper.com:/root/certs/

```


Now we can install the Root CA Certificate into /etc/ssl/certs/


```

# cd /etc/ssl/certs/
# cp /root/certs/cacert.crt virtuallyhyper.pem
# ls -l `openssl x509 -noout -hash -in /etc/ssl/certs/virtuallyhyper.pem`*
# ln -s /etc/ssl/certs/virtuallyhyper.pem `openssl x509 -noout -hash -in /etc/ssl/certs/virtuallyhyper.pem`.0

```


Now we can replace the web interface (VAMI) certificates with the ones we generated. This is optional.


```

# cd /root/certs/
# cat srm-vrs-prod.key srm-vrs-prod.crt &gt; srm-vrs-prod.pem
# mv /opt/vmware/etc/lighttpd/server.pem /opt/vmware/etc/lighttpd/server.pem.old
# cp /root/certs/srm-vrs-prod.pem /opt/vmware/etc/lighttpd/server.pem
# /etc/init.d/vami-lighttp restart

```


Now log into the web interface for the VRS. You can do this by clicking on the "Configure VR Server" link in the SRM plug-in. You can also get there by going to https://srm-vrs-prod.virtuallyhyper.com:5480 (sub in your FQDN). The username is root and the password "vmware". Click on the "VR Server" tab. In here you will see a section where you can upload the certificates we generated.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRS-Certificate-upload.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/VRS-Certificate-upload.png)

Once the certificates are uploaded, go back to the SRM plug-in and register the VRS. Everything should be configured to use the custom certificates.
