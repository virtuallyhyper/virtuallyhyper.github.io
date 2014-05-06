---
author: jarret
comments: true
date: 2012-09-01 00:57:12+00:00
layout: post
slug: srm-5-x-custom-ssl-certificates-with-multiple-subject-alternative-names
title: SRM 5.x Custom SSL Certificates with Multiple Subject Alternative Names
wordpress_id: 3169
categories:
- SRM
- VMware
following_users:
- jarret
- kelatov
tags:
- Certifications
- openssl
- srm
- SSL
- subjectAltName
---

Recently a colleague asked me if they could have multiple Subject Alternative Names in the SRM SSL Certificate. My immediate response was yes as I had done it numberous times before, but I was informed that there were some problems with it. I dediced to create a few experiments to figure out if it would really work. If you are looking for the requirements for setting up SRM with custom certificates please see [this post](http://virtuallyhyper.com/2012/08/setting-up-srm-with-custom-certificates/).

If we have the wrong Subject Alternative Name we would expect to get the “The host name in the Subject Alternative Name of the provided certificate does not match the SRM host name.” error as was described in [this post](http://virtuallyhyper.com/2012/08/srm-fails-to-install-a-custom-certificate-with-he-host-name-in-the-subject-alternative-name-of-the-provided-certificate-does-not-match-the-srm-host-name/). In that post I described how to identify and resolve the issue. In this case I added multiple Subject Alternative Names to see what works and what does not.

The first thing I did was regenerate my certificate. This time I added in a non resolvable Subject Alternative Name after the others. Here is what the extensions looked like on the new certificate.


```

$ openssl x509 -in srm-prod.crt -text -noout |grep -A 1 "Subject Alternative Name"
            X509v3 Subject Alternative Name:
                DNS:SRM51-PROD.moopless.com, DNS:srm51-prod.moopless.com, DNS:thisisnot.a.realdns.name

```


In it I have 3 Subject Alternative Names: SRM51-PROD.moopless.com, srm51-prod.moopless.com, and thisisnot.a.realdns.name. It was working fine with the SRM51-PROD.moopless.com and srm51-prod.moopless.com. I then copied this certificate to the server and selected it during a "Modify install". When I hit next the installer failed out with the error we expected.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/subjectaltname-does-not-match.jpg)](http://virtuallyhyper.com/wp-content/uploads/2012/08/subjectaltname-does-not-match.jpg)

Looking at the installer logs at the [SRM Installation Logs](http://kb.vmware.com/kb/1021802) I saw the following.


```

Enter password for certificate file \\fileserver2\files\ssl\multiple\srm-prod.p12: Error [76]: The DNS of the certificate, does not match the SRM host. The expected DNS name is: thisisnot.a.realdns.name. The provided SRM host is: srm51-prod.moopless.com

```


The installer was comparing thisisnot.a.realdns.name with the "Local Host" in the extension. So this means that the installer is either checking all of the Subject Alternative Names in the certificate or just checking the last one.

I was curious to see if it would work when we had a DNS alias (CNAME) to the real host name. I created a CNAME, fake.moopless.com, which pointed to srm51-prod.moopless.com and generated the certificate again. Here is how the Subject Alternative Names looked like in the certificate.


```

$ openssl x509 -in srm-prod.crt -text -noout |grep -A 1 "Subject Alternative Name"
            X509v3 Subject Alternative Name:
                DNS:SRM51-PROD.moopless.com, DNS:srm51-prod.moopless.com, DNS:fake.moopless.com

```


I ran through the installer and found the same error. Below are the logs from the installer.


```

Enter password for certificate file \\fileserver2\files\ssl\multiple\srm-prod.p12: Error [76]: The DNS of the certificate, does not match the SRM host. The expected DNS name is: fake.moopless.com. The provided SRM host is: srm51-prod.moopless.com

```


I changed the DNS record to an A record pointing to the same IP and it failed with the same error. This means that this part of the installer does not check the DNS resolution and compare it to the "Local Host" name. I seems to be a just a string comparison.

The last test that I tried was moving the Subject Alternative Names around in the certificate.. This was to check if the installer checks all of the Subject Alternative Names or not. Here is what the certificate looks like.


```

$ openssl x509 -in srm-prod.crt -text -noout |grep -A 1 "Subject Alternative Name"
            X509v3 Subject Alternative Name:
                DNS:SRM51-PROD.moopless.com, DNS:fake.moopless.com, DNS:srm51-prod.moopless.com

```


I ran through the installer and it went through just fine. So we know that the SRM installer only compares the last Subject Alternative Name to the "Local Host" name. The service runs fine when there are multiple Subject Alternative Names if the last one is the name of the SRM server. Remember that it is case sensitive.
