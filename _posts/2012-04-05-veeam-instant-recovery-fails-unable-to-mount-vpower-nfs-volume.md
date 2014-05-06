---
author: joe
comments: true
date: 2012-04-05 18:00:00+00:00
layout: post
slug: veeam-instant-recovery-fails-unable-to-mount-vpower-nfs-volume
title: 'VEEAM Instant Recovery Fails: Unable to mount vPower NFS volume'
wordpress_id: 603
categories:
- Storage
- VMware
following_users:
- joechan
- kelatov
tags:
- nfs
- veeam
- vpower nfs
---

I had a customer recently who was trying to perform a VEEAM Instant Recovery operation and it was failing with the following error:


```

Failed to publish VM &quot;test vm&quot;
Unable to mount vPower NFS volume
(VEEAMBOX:/VeeamBackup_VEEAMBOX). veeambox.domain.local
An error occurred during host configuration.

```


Looking at the vmkernel log on the ESX(i) host, we saw:


```

2012-04-03T17:24:25.522Z cpu21:3239)NFS: 190: NFS mount &lt;VEEAMBOX_IP_ADDRESS&gt;:/VeeamBackup_VEEAMBOX failed: The mount request was denied by the NFS server. Check that the export exists and that the client is permitted to mount it.

```


It turns out he was running Microsoft Services for NFS in on his VEEAM server, and when VEEAM was telling the host to mount from the VEEAM server, it would try to mount from the MS NFS services. You could have also run:


```

netstat -nba

```


to determine which process was listening on the NFS ports, and per [VEEAM KB 1055](http://www.veeam.com/kb_articles.html/KB1055), if everything was running properly, you should expect to see something like:


```

  Proto  Local Address          Foreign Address        State
  TCP    0.0.0.0:111            0.0.0.0:0              LISTENING
 [VeeamNFSSvc.exe]
  TCP    0.0.0.0:135            0.0.0.0:0              LISTENING
  RpcSs
  TCP    0.0.0.0:1058           0.0.0.0:0              LISTENING
 [VeeamNFSSvc.exe]
  TCP    0.0.0.0:2049           0.0.0.0:0              LISTENING
 [VeeamNFSSvc.exe]

```


We stopped the Microsoft NFS service and restarted the VEEAM vPower NFS Service, and was able to perform the Instant Recovery operation successfully.
