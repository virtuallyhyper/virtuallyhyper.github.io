---
author: jarret
comments: true
date: 2012-08-13 19:20:53+00:00
layout: post
slug: checking-if-a-nfs-datastore-is-mounted-read-only
title: Checking if an NFS datastore is Mounted Read-Only on ESX(i) Host
wordpress_id: 2210
categories:
- VMware
- vTip
following_users:
- jarret
- kelatov
tags:
- esxcfg-nas
- esxcli
- nfs
- read-only
---

Recently I saw a customer that had trouble vMotioning VMs on NFS datastores. This was occurring on multiple hosts across multiple datastores. It turns out that the customer had mounted some datastores as Read-only on a few hosts. Unfortunately its not straight forward to check for Read-only status on the command line, so I put it up here.

The first thing to try is to create a new file on the datastore


```

# touch /vmfs/volumes/ISOs/deleteme
touch: /vmfs/volumes/ISOs/deleteme: Read-only file system

```


Creating a new file failed with a Read-only file system error. This could mean that it was mounted as read only or the export does not include write permissions. I went to check the mount using esxcfg-nas command. Unfortunately it does not tell us much.


```

# esxcfg-nas -l
ISOs is /data/files/ISOs from fileserver2 mounted available

```


We can check more mount properties by using esxcli (5.x)


```

 esxcli storage nfs list
Volume Name  Host         Share             Accessible  Mounted  Hardware Acceleration
-----------  -----------  ----------------  ----------  -------  ---------------------
ISOs         fileserver2  /data/files/ISOs        true     true  Not Supported

```


Still it did not tell us if it was mounted Read-only. Let's check the mount properties in the esx.conf


```

# cat /etc/vmware/esx.conf |grep ISOs
/nas/ISOs/enabled = "true"
/nas/ISOs/host = "fileserver2"
/nas/ISOs/readOnly = "true"
/nas/ISOs/share = "/data/files/ISOs"

```


Finally we see that it is mounted Read-only. So it makes sense that we could not vMotion a VM to it.

Another way to check this is using vsish.


```

# vsish -e cat /vmkModules/nfsclient/mnt/ISOs/properties |grep readOnly
   readOnly:1

```

