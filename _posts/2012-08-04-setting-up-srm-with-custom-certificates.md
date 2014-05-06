---
author: jarret
comments: true
date: 2012-08-04 20:50:23+00:00
layout: post
slug: setting-up-srm-with-custom-certificates
title: Installing SRM with custom certificates
wordpress_id: 1862
categories:
- SRM
- VMware
ef_usergroup:
- JJK
following_users:
- jarret
- kelatov
tags:
- CA
- Certifications
- how-to
- installation
- openssl
- Root CA
- srm
- subjectAltName
- vcenter
---

Often I will run into a customer that has custom certificates on their vCenter and are having problems setting up and installing SRM. The problem is that SRM and vCenter need to use the same trust method, so we need to have custom certs for SRM as well. Normally this is not a problem, but SRM is **very** picky on how the certs are configured.





In this post, we will go through the process of setting up custom certs for both vCenter and SRM.  In this series I am using sha1 certificates because Windows 2003 does not come with [sha2 until this hotfix](http://support.microsoft.com/kb/938397).





Below I use [openssl](http://www.openssl.org) on a Solaris machine, but Openssl is available across many operating systems. The windows [binaries are here](http://www.openssl.org/related/binaries.html). The openssl commands are operating system independent. Commands like cp and vi, should be replaced with GUI operations.





## Installing the Root CA into the Trusted Root Certification Authorities





This was one of the biggest frustrations for me. There are many errors that will come up saying that the certificate is not trusted, but when you look at the certificate it may say that it is. The key is to install the certificate to the Local Computer Trusted Root Certification Authorities. When installing a certificate by right clicking on the certificate, it will add it to the user's certificates, but not for the local computer.





First we need to get the CA's public certificate that we want to install in the Trusted Root CAs. First let's open up the certificate and ensure that it is not trusted already.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/CA-Root-not-trusted.jpg)](http://virtuallyhyper.com/wp-content/uploads/2012/08/CA-Root-not-trusted.jpg)





After opening the certificate we can see that the CA Root is not trusted. If you run the  "Install Certificate" it will install it for your user and not for the local computer, so go ahead and close this window.





Go to Start-> Run and run "mmc.exe". Once in the MMC, we need to add the Certificates snap in. Click on "File->Add/Remove Snap-in...".





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Console-Add-Snap-in.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Console-Add-Snap-in.png)





On the next screen we want to select Certificates on the Left and hit the add button. Make sure to select the Computer account.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Add-Computer-account-certificates.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Add-Computer-account-certificates.png)





When the Certificates Snap-In is added, we want to browse to the "Trusted Root Certification Authorities -> Certificates". In the end we want to see our Root CA Certificate listed here. In my case it is not here, that is why my certificate is not trusted. So we need to import it. Right click on "Certificates" and select "All Tasks-> Import".[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Import-Certificate-to-Trusted-root.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Import-Certificate-to-Trusted-root.png)





This will bring up the import certificate wizard. You should browse to the Root CA Certificate we opened initially. We want to ensure it is put into the Trusted Root Certification Authorities.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Import-Certificate-into-Trusted-CA.jpg)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Import-Certificate-into-Trusted-CA.jpg)





Once you have imported the CA Root Certificate we should see it listed in the Trusted Root Certification Authorities.  Below we can see that the virtuallyhyper.com certificate is in there.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Cert-in-trusted-root.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Cert-in-trusted-root.png)





Let's check out the certificate again to confirm that is is now trusted. Go back to where you have stored your Root CA CRT on the server. Double click on the Root CA CRT file to open it again.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Trusted-Root-CA-Cert.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Trusted-Root-CA-Cert.png)





Repeat this process for both vCenter servers and both SRM servers. If this is the Root CA for your domain, you may want to consider adding it to a GPO.





## Generating and Installing vCenter certificates





There are many set of instructions to follow when generating and installing custom certs for vCenter. [KB 1003921](http://kb.vmware.com/kb/1003921) describes the basic process. I will not go in depth here, but I will list my steps for generating the certificates. If you are looking for some more detail please see [this blog post](http://geeksilver.wordpress.com/2011/05/13/how-to-use-ca-certificate-to-replace-vmware-certificate-on-esxi-4-and-vcenter/) or [this one](http://www.wooditwork.com/2011/11/30/vsphere-5-certificates-3-replacing-the-default-vcenter-5-server-certificate-2-2/).  See [this blog post](http://longwhiteclouds.com/2012/02/13/vcenter-server-virtual-appliance-changing-ssl-certs-made-easy/) if you are using the Linux vCenter appliance.





First we need to generate the CSR. We can do this with openssl. I changed my openssl.cnf, so I do not have to type in the some values, so you should enter your information correctly.






```






# mkdir vcenter-prod && cd vcenter-prod





# openssl req -new -newkey rsa:2048 -sha1  -nodes -out rui.csr -keyout rui.key





You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,





## If you enter '.', the field will be left blank.





Country Name (2 letter code) [US]: US
State or Province Name (full name) [Colorado]: Colorado
Locality Name (eg, city) [Boulder]: Boulder
Organization Name (eg, company) [Virtuallyhyper]: Virtuallyhyper
Organizational Unit Name (eg, section) [Lab]: Lab
Common Name (eg, YOUR name) []:srm-prod.virtuallyhyper.com
Email Address [admin'@'virtuallyhyper.com]:admin'@'virtuallyhyper.com





Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:

```






Now we need to sign the CSR. You will do with with your root CA, so the steps will likely be different.






```






# openssl ca -days 3650 -in rui.csr -out rui.crt






```






Next we have to merge the signed certificate and the private key into the PFX file. The PFX will require a password, which will be set to "testpassword" in the command. vCenter will expect this to be "testpassword", so please do not change it.






```






# openssl pkcs12 -export -in rui.crt -inkey rui.key -name "rui" -passout pass:testpassword -out rui.pfx






```






Now that we have generated and signed our certificate we need to install it for vCenter. You need to copy the rui.pfx, rui.key and the rui.crt to the SSL directory for vCenter. Make sure to back up the original ones first. The SSL folder is located in the application directory for vCenter.






```

Windows 2003 C:\Documents and Settings\All Users\Application Data\VMware\VMware VirtualCenter\SSL
Windows 2008/R2 C:\ProgramData\VMware\VMware VirtualCenter\SSL

```






Now we should stop the vCenter service and [update the database password](http://kb.vmware.com/kb/1003070). Make sure to run the Command Prompt as an Administrator.






```

C:\Documents and Settings\jarret> net stop vpxd
C:\Documents and Settings\jarret>"c:\Program Files\VMware\Infrastructure\Virtual
Center Server\vpxd.exe" -p
C:\Documents and Settings\jarret> net start vpxd
C:\Documents and Settings\jarret> net start vctomcat

```






Now you can test vCenter to confirm that everything is working and the new certificate is installed. Make sure to generate and install the certificate on both the protected and the recovery sites. Go ahead and reconnect all of the hosts.





## Generating and installing SRM certs





There is an excellent write up about this that can be found [here](http://communities.vmware.com/docs/DOC-11411). The requirements are listed in [this KB article](http://kb.vmware.com/kb/1008390). The problem is that the details are easy to miss and it can be a huge headache. The biggest thing to remember is that the **Subject for both certificates need to be the same**. That means that the **CN,O,OU,C,S and L need to be the same for the certificates for both SRM servers**.





### Generating the Protected Site SRM Certificate





First lets generate the CSR for the protected site SRM server. I did not set up a separate SRM server in this lab, so I am installing SRM on to the vCenter server. We still need to generate a new certificate for SRM on this server.





We need to make some changes to the openssl.cnf. First lets copy this to our current directory so we can modify some properties for this certificate. If you are using windows, there will be a local copy of openssl.cfg. Copy that to another file name (I.e srm-prod.cnf) and make the changes there.






```






# mkdir srm-prod && cd srm-prod





# cp /etc/ssl/openssl.cnf srm-prod.cnf






```






Open up the srm-dr.cnf and make sure to add a subjectAltName. It should look like the one below. Make sure that it is correct case, as it is case sensitive. We also need to add clientAuth to all for client authentication. This is a requirement for trusted certs with SRM. Make sure not to put the subjectAltName into the subject section ([ req_distinguished_name ]) as the subjects need to be the same for both certs. To get by this we can add it as an extension of the certificate.





Find the line below and uncomment it. It should be found in the [ req ] section






```

req_extensions = v3_req

```






The go down to the [ v3_ca ] section and put in the following lines.






```

subjectAltName=DNS:srm-prod.virtuallyhyper.com
extendedKeyUsage = serverAuth, clientAuth

```






Now we can generate the CSR. Notice how the CN is "srm". The Subject (all of the fields below) needs to be exactly the same across both SRM certificates. The subjectAltName above is what will define the hostname for SRM. Check out this post about using [multiple subjectAltNames](http://virtuallyhyper.com/2012/08/srm-5-x-custom-ssl-certificates-with-multiple-subject-alternative-names/).






```






# openssl req -new -newkey rsa:2048 -sha1  -nodes -out srm-prod.csr -keyout srm-prod.key -config srm-prod.cnf





You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,





## If you enter '.', the field will be left blank.





Country Name (2 letter code) [US]: US
State or Province Name (full name) [Colorado]: Colorado
Locality Name (eg, city) [Boulder]: Boulder
Organization Name (eg, company) [Virtuallyhyper]: Virtuallyhyper
Organizational Unit Name (eg, section) [Lab]: Lab
Common Name (eg, YOUR name) []:srm
Email Address [admin'@'virtuallyhyper.com]: admin'@'virtuallyhyper.com





Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:

```






Check to ensure that the subjectAltName and client authentication is in the CSR. You can open the certificate in Windows and visually check it.






```






# openssl req -text -noout  -in srm-prod.csr |egrep -A 1 "Subject Alternative Name|Extended Key Usage"




    
X509v3 Subject Alternative Name:
DNS:srm-prod.virtuallyhyper.com
X509v3 Extended Key Usage:
TLS Web Server Authentication, TLS Web Client Authentication
    
```











Now sign your certificate with your Root CA. This step will be different in your environment. You will likely have to sent the CSR to the security administrator, or insert it into a central Root CA website.






```






# openssl ca -days 3650 -in srm-prod.csr -out srm-prod.crt






```






Let's make sure that the CA did not strip off our extensions. Again you can check this visually by opening the certificate in Windows.






```






# openssl x509 -in srm-prod.crt -text -noout |egrep -A 1 "Subject Alternative Name|Extended Key Usage"




    
X509v3 Subject Alternative Name:
DNS:srm-prod.virtuallyhyper.com
X509v3 Extended Key Usage:
TLS Web Server Authentication, TLS Web Client Authentication

```











Next we need to create the p12.  ( SRM needs a p12 and vCenter needs a PFX)  When running the command below it will ask you for a password. This will need to be entered when importing the SRM certificate when adding the p12.






```






# openssl pkcs12 -export -in srm-prod.crt -inkey srm-prod.key -out srm-prod.p12






```






### Generating the Recovery Site SRM Certificate





No we have to generate the recovery site SRM certificate. Let's copy over the openssl.cnf into our new folder.






```






# mkdir srm-dr && cd srm-dr





# cp /etc/ssl/openssl.cnf srm-dr.cnf






```






Make the modifications to srm-dr.cnf again.






```

req_extensions = v3_req





...





subjectAltName=DNS:srm-dr.virtuallyhyper.com
extendedKeyUsage = serverAuth, clientAuth

```






Generate the CSR and sign it. Remember this is with the same values as above.






```






# openssl req -new -newkey rsa:2048 -sha1  -nodes -out srm-dr.csr -keyout srm-dr.key -config srm-dr.cnf





You are about to be asked to enter information that will be incorporated
into your certificate request.
What you are about to enter is what is called a Distinguished Name or a DN.
There are quite a few fields but you can leave some blank
For some fields there will be a default value,





## If you enter '.', the field will be left blank.





Country Name (2 letter code) [US]: US
State or Province Name (full name) [Colorado]: Colorado
Locality Name (eg, city) [Boulder]: Boulder
Organization Name (eg, company) [Virtuallyhyper]: Virtuallyhyper
Organizational Unit Name (eg, section) [Lab]: Lab
Common Name (eg, YOUR name) []:srm
Email Address [admin'@'virtuallyhyper.com]:admin'@'virtuallyhyper.com





Please enter the following 'extra' attributes
to be sent with your certificate request
A challenge password []:
An optional company name []:





# openssl ca -days 3650 -in srm-dr.csr -out srm-dr.crt





# openssl pkcs12 -export -in srm-dr.crt -inkey srm-dr.key -out srm-dr.p12






```






Let's check to ensure that the CA did not strip off our extensions.






```






# openssl x509 -in srm-prod.crt -text -noout |egrep -A 1 "Subject Alternative Name|Extended Key Usage"




    
X509v3 Subject Alternative Name:
DNS:srm-dr.virtuallyhyper.com
X509v3 Extended Key Usage:
TLS Web Server Authentication, TLS Web Client Authentication

```











### Installing SRM with the certificates





Now what we have generated the certificates we need to install SRM with them. Open up the SRM installer as an administrator. Make sure to use FQDN for all of the answers. If you already have SRM installed you can add them by doing a "Modify" install.





Make sure to enter the FQDN of the vCenter server and an administrator account on the vCenter Server. It does not have to the local administrator account.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/SRM-install-Step-1-vCenter.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/SRM-install-Step-1-vCenter.png)





A few more pages in, we will see a page asking if you want to use the self generated certificates, or a p12 certificate. Select the p12 certificate option. On the next page you will select the p12 certificate we generated above. Enter the password you used when packing it into a p12.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/SRM-install-Step-3-select-p12-certificate.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/SRM-install-Step-3-select-p12-certificate.png)





When you hit next, it will evaluate the certificate. If you get the error below, you did not add the Root CA Certificate to the Trusted Root Certification Authorities. Please go back to the "Installing the Root CA into the Trusted Root Certification Authorities" section.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Failed-to-validate-certificate-Trusted-root.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Failed-to-validate-certificate-Trusted-root.png)





The next page will be where we enter the local site name. We need to make sure that what we enter here is the same as we put in the subjectAltName in the certificate. The "Local Host" will default to the IP address, so make sure to correct it to the FQDN.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/enter-the-fqdn.jpg)](http://virtuallyhyper.com/wp-content/uploads/2012/08/enter-the-fqdn.jpg)





If the Local Host name does not exactly match the one in the subjectAltName we will get the error below. Please check out [this post](http://virtuallyhyper.com/2012/08/srm-fails-to-install-a-custom-certificate-with-he-host-name-in-the-subject-alternative-name-of-the-provided-certificate-does-not-match-the-srm-host-name) on how to troubleshoot this error.





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/Failed-to-validate-certificate-FQDN.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/Failed-to-validate-certificate-FQDN.png)





After this you will have to set up the database and finish the installer. Please repeat this step for the recovery site SRM server to have SRM installed on both sites. The installers can run in parallel to save time.





If the installers have completed you can now pair the sites and configure SRM.



