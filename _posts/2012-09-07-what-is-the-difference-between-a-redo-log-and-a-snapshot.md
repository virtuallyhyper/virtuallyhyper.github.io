---
author: jarret
comments: true
date: 2012-09-07 04:44:04+00:00
layout: post
slug: what-is-the-difference-between-a-redo-log-and-a-snapshot
title: What is the difference between a Redo log and a Snapshot?
wordpress_id: 3428
categories:
- VMware
- vTip
following_users:
- jarret
- kelatov
tags:
- redo logs
- snapshots
- vmdk
- vmfsSparse
---

Most people confuse redo logs with snapshots, but there is a subtle difference in the terminology. If you look at my [previous post about snapshots](http://virtuallyhyper.com/2012/04/vmware-snapshot-troubleshooting/), it goes into how to identify and troubleshoot snapshots. 

You will see the two words used interchangeabley within vCenter and ESX hosts. Many of the error messages will refer to a snapshot by calling it a redo log. This is from legacy terminology. Back in the [ESX 2.x](http://www.rtfm-ed.co.uk/docs/vmwdocs/whitepaper-vmware-esx2.x-redo-demystified.pdf) days redo logs were [essentially snapshots](http://kb.vmware.com/kb/1004458). In the ESX 3.5+ days we call them snapshots. 

If you see errors in vCenter about redo logs, it is likely that you are having problems with snapshots. Some common examples are below.





  * [msg.hbacommon.corruptredo:The redolog of server1-000001.vmdk has been detected to be corrupt. The virtual machine needs to be powered off. If the problem still persists, you need to discard the redolog](http://kb.vmware.com/kb/1006585)


  * [There is no more space for the redo log of server1-000001.vmdk](http://kb.vmware.com/kb/1002103)



Redo logs are created when the VM is using [independent non-persistent disks](http://www.vspecialist.co.uk/non-persistent-disks-with-vms/). With an independent non-persistent disk the changes are not written to disk. So you can run a VM off of a disk, and every time you reset the VM it will revert to the previous state. The question is where are the changes written? We would expect the changes to be written to a snapshot, but this is not the case. A redo log is created to store the changes. 


```

~ # ls -lh *REDO*
-rw-------    1 root     root       336.0M Sep  7 06:36 android.vmdk-delta.REDO_TkLU1V
-rw-------    1 root     root          322 Sep  7 06:29 android.vmdk.REDO_TkLU1V

```


The format is the [same as a snapshot](http://kb.vmware.com/kb/1026353). Below is the descriptor of the redo log. 


```

~ # cat android.vmdk.REDO_TkLU1V 
# Disk DescriptorFile
version=1
encoding="UTF-8"
CID=9ab16ab9
parentCID=c3d4fd75
isNativeSnapshot="no"
createType="vmfsSparse"
parentFileNameHint="android.vmdk"
# Extent description
RW 41943040 VMFSSPARSE "android.vmdk-delta.REDO_TkLU1V"

# The Disk Data Base 
#DDB

ddb.longContentID = "e6d6c9b8c7164488a10a36b39ab16ab9"

```


The redo logs are delta disks just like the snapshots.


```

# ls -lh /dev/deltadisks/
-rw-------    1 root     root        20.0G Sep  7 07:12 1b85f783-android.vmdk-delta.REDO_TkLU1V
-rw-------    1 root     root        20.0G Sep  7 07:12 4c2fd4a3-sql-000002-delta.vmdk
-rw-------    1 root     root        30.0G Sep  7 07:12 699e82ec-sql_1-000002-delta.vmdk
-r--r--r--    1 root     root            0 Sep  7 07:12 control

```


So they are essentially the same. The real difference is that redo logs are deleted as a part of the VM being shut down. If a VM with non-persistent disks is powered on and there are redo logs, they will automatically be deleted. If the disks have been changed to persistent, the host will ask if you want to delete or commit the changes in the redo logs. 

[![](http://virtuallyhyper.com/wp-content/uploads/2012/09/redo-log-delete.png)](http://virtuallyhyper.com/wp-content/uploads/2012/09/redo-log-delete.png)
