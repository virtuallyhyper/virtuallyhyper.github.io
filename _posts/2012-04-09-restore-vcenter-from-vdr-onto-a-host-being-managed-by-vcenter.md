---
author: joe
comments: true
date: 2012-04-09 17:24:46+00:00
layout: post
slug: restore-vcenter-from-vdr-onto-a-host-being-managed-by-vcenter
title: Restore vCenter from VDR onto a Host Being Managed by vCenter
wordpress_id: 631
categories:
- VMware
following_users:
- jarret
- joechan
- kelatov
tags:
- restore
- vcenter
- vdr
- virtualcenter
- vmware data recovery
- vpxa
---

I recently had an issue where I was unable to restore vCenter (running in a virtual machine) to a host that vCenter was managing (quite the catch-22 situation).

Here is the error we were getting:


```

Access to resource settings on the host is restricted to the server 'x.x.x.x' which is managing it.

```


So we need to trick the hosts into thinking they aren't being managed by vCenter. This may or may not be supported officially by VMware, but it worked for me.

- backup vpxa.cfg

**NOTE**: Check your version of ESX/ESXi to determine where the vpxa.cfg file is actually located. The location of the vpxa configuration directory changed from /etc/**opt**/vmware/vpxa to /etc/vmware/vpxa in ESXi 5. Here is a quick reference.

ESX and ESXi 4.x and prior:
/etc/opt/vmware/vpxa/vpxa.cfg

ESXi 5:
/etc/vmware/vpxa/vpxa.cfg


```

~ # cp /etc/vmware/vpxa/vpxa.cfg /etc/vmware/vpxa/vpxa.cfg.bak
~ # ls /etc/vmware/vpxa
dasConfig.xml  vpxa.cfg       vpxa.cfg.bak

```


- edit vpxa.cfg with the command-line text editor, "vi" (how to use vi is outside the scope of this post, but there are plenty of resources online if you would like to learn more about it):


```

~ # vi /etc/vmware/vpxa/vpxa.cfg

```


- and replace


```

  <vpxa>
    <bundleVersion>1000000</bundleVersion>
    <datastorePrincipal>root</datastorePrincipal>
    <hostIp>192.168.1.200</hostIp>
    <hostKey>52137432-cb9e-05d5-3bbf-001ffd2baa57</hostKey>
    <hostPort>443</hostPort>
    <licenseExpiryNotificationThreshold>15</licenseExpiryNotificationThreshold>
    <memoryCheckerTimeInSecs>30</memoryCheckerTimeInSecs>
    <serverIp>192.168.1.2</serverIp>
    <serverPort>902</serverPort>
  </vpxa>

```


with:


```

  <vpxa>
  </vpxa>

```


essentially removing everything between:


```
<vpxa>
```


and


```
</vpxa>
```


- restart the management agents (other ways to do this in ESX and ESXi are shown in [VMware KB 1003490 - Restarting the Management agents on an ESX or ESXi Server](http://kb.vmware.com/kb/1003490), but what I show below is for an ESXi host:


```

~ # /sbin/services.sh restart

```


- Proceed with restore of vCenter server from VDR to the host that was managed by that very vCenter

**NOTE:** Prior to booting up vCenter again after the restore, I highly recommend you restore vpxa.cfg:


```

~ # cp /etc/vmware/vpxa/vpxa.cfg.bak /etc/vmware/vpxa/vpxa.cfg

```

