---
author: joe
comments: true
date: 2012-03-10 02:52:39+00:00
layout: post
slug: determine-vcenter-ip-from-esx-host-via-cli
title: Determine vCenter IP from ESX host via CLI
wordpress_id: 16
categories:
- VMware
- vTip
tags:
- cli
---

ESXi 5

```
~ # grep serverIp /etc/vmware/vpxa/vpxa.cfg
    10.131.11.183
```

ESX and ESXi 4.x and prior

```
~ # grep serverIp /etc/opt/vmware/vpxa/vpxa.cfg
    10.131.11.183
```

Note: The location of the vpxa configuration directory changed from /etc/**opt**/vmware/vpxa to /etc/vmware/vpxa in ESXi 5.
