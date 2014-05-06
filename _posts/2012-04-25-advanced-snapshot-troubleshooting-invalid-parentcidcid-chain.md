---
author: jarret
comments: true
date: 2012-04-25 01:47:30+00:00
layout: post
slug: advanced-snapshot-troubleshooting-invalid-parentcidcid-chain
title: 'Advanced Snapshot Troubleshooting: Invalid parentCID/CID chain'
wordpress_id: 871
categories:
- Storage
- VMware
following_users:
- jarret
- joechan
- kelatov
tags:
- CID
- clone
- command line
- parentCID
- snapshots
- troubleshooting
- vmkfstools
---

The most common snapshot descriptor issue that we run into is an invalid CID/parentCID chain. In [VMware Snapshot Troubleshooting](http://virtuallyhyper.com/index.php/2012/04/vmware-snapshot-troubleshooting/) we went over the snapshot basics. Here we will discuss the snapshot descriptor, the CID/parentCID chain, and how to fix these problems. [This VMware KB](http://kb.vmware.com/kb/1007969) talks in detail about the CID/parentCID issues and how to resolve it.


## Snapshot Descriptor


You may have noticed that every disk has both a.vmdk and a-flat.vmdk. The.vmdk is the descriptor that describes the-flat.vmdk. Snapshots also have a descriptor and a binary file named-######.vmdk and-######-delta.vmdk.

Lets take a look at a snapshot descriptor file.


```
# ls *.vmdk
sql-000001-delta.vmdk  sql-000001.vmdk        sql-flat.vmdk          sql.vmdk
# cat sql-000001.vmdk
# Disk DescriptorFile
version=1
encoding="UTF-8"
CID=ffffffee
parentCID=fffffffe
isNativeSnapshot="no"
createType="vmfsSparse"
parentFileNameHint="sql.vmdk"
# Extent description
RW 16777216 VMFSSPARSE "sql-000001-delta.vmdk"

# The Disk Data Base
#DDB

ddb.longContentID = "7dfbf361d1fbbf2082b192cffffffffe"

```


In [VMware Snapshot Troubleshooting](http://virtuallyhyper.com/index.php/2012/04/vmware-snapshot-troubleshooting/), we talked about the parentFileNameHint. Today we will be looking at the CID and parentCID

The CID is the Content ID for the virtual disks by the use of a unique 8 digit hex number. The CID is used to link the content to the parent disk. The CID is used by the host to confirm that the data is consistent in the parent disks.

The parentCID is the CID of the parent disk. The parentCID should be the same of the parent's CID, and ffffffff for the base disk. If the parentCID is not the same as the parent's CID, we cannot guarantee that the data is the same when the snapshot was taken. The host will throw an error about the parent disk being modified. Usually this error is "The parent virtual disk has been modified since the child was created"


## Confirming the CID/parentCID chain




### Using grep


There are many ways to confirm the CID/parentCID chain. One of these ways to check the chain is using grep on the command line. Using [grep](http://linux.die.net/man/1/grep) we can pull the CID and parentCID of each descriptor. If we manually look at each pair above, we can verify that the chain is consistent.


```
# grep CID sql-00000?.vmdk sql.vmdk
sql-000001.vmdk:CID=cffffffe
sql-000001.vmdk:parentCID=dffffffe
sql-000002.vmdk:CID=bffffffe
sql-000002.vmdk:parentCID=cffffffe
sql-000003.vmdk:CID=affffffe
sql-000003.vmdk:parentCID=bffffffe
sql-000004.vmdk:CID=fffffffe
sql-000004.vmdk:parentCID=affffffe
sql.vmdk:CID=dffffffe
sql.vmdk:parentCID=ffffffff

```



### Using vmkfstools


The easiest way is to the use vmkfstools command. With this command we can have it open the entire snapshot chain and confirm the CID/parentCIDs.

If we run the vmkfstools command on the last snapshot, it will open all of the snapshots in the chain up to the base disk. If there is a problem in the chain, it will tell us.


```
# vmkfstools -v 10 -q sql-000004.vmdk
DISKLIB-VMFS  : "./sql-000004-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000002-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000001-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-LINK  : DiskLinkIsAttachPossible: Content ID mismatch (parentCID fffffffe != ffffffde) /vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000002.vmdk /vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000001.vmdk.
DISKLIB-CHAIN : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000001.vmdk" : failed to open (The parent virtual disk has been modified since the child was created. The content ID of the parent virtual disk does not match the corresponding parent content ID in the child).
DISKLIB-VMFS  : "./sql-000004-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000002-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000001-delta.vmdk" : closed.
DISKLIB-LIB   : Failed to open 'sql-000004.vmdk' with flags 0x17 The parent virtual disk has been modified since the child was created. The content ID of the parent virtual disk does not match the corresponding parent content ID in the child (18).
Failed to open 'sql-000004.vmdk' : The parent virtual disk has been modified since the child was created. The content ID of the parent virtual disk does not match the corresponding parent content ID in the child (18).

```



## Fixing the chain


The vmkfstools command in "Using vmkfstools" failed to open sql-000001-delta.vmdk because the parentCID in sql-000002.vmdk (fffffffe) does not match the CID in sql-000001.vmdk (ffffffde). In order to fix this we can change the parentCID in sql-000002.vmdk. If we had confirmed the chain with grep, we would have seen this manually.

Below we are using sed to change the parentCID, but you can manually edit sql-000002.vmdk and change the parentCID to ffffffde using vi.


```
# sed -i 's/parentCID=[0-9a-f]*/parentCID=ffffffde/' sql-000002.vmdk
# grep parentCID sql-000002.vmdk
parentCID=ffffffde

```


Let's check vmkfstools again. This time is opens all of the snapshots and the base disk. The "is not an rdm" is expected.


```
# vmkfstools -v 10 -q sql-000004.vmdk
DISKLIB-VMFS  : "./sql-000004-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000002-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000001-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-flat.vmdk" : open successful (23) size = 8589934592, hd = 0. Type 3
sql-000004.vmdk is not an rdm
DISKLIB-VMFS  : "./sql-000004-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000002-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000001-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-flat.vmdk" : closed.
AIOMGR-S : stat o=5 r=15 w=0 i=0 br=245760 bw=0

```


After making the chain we should be able to power on the VM, or clone the latest snapshot.
