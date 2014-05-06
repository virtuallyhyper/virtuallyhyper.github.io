---
author: jarret
comments: true
date: 2012-04-25 01:39:14+00:00
layout: post
slug: advanced-snapshot-troubleshooting-incorrect-vmdk-geometry
title: 'Advanced Snapshot Troubleshooting: Incorrect VMDK Geometry'
wordpress_id: 1129
categories:
- Storage
- VMware
following_users:
- jarret
- joechan
- kelatov
tags:
- clone
- command line
- failed to open parent
- snapshots
- troubleshooting
- vmdk
- vmkfstools
---

In [VMware Snapshot Troubleshooting](http://virtuallyhyper.com/2012/04/vmware-snapshot-troubleshooting/) we discussed basic snapshot operations. In the "Invalid parentCID/CID Chain" article, we discussed how to identify invalid snapshot chains. In this article we will discuss disk geometry and how to fix problems with geometry.


## Understanding disk geometry




### What disk geometry values are in the descriptor file?


Every disk has two parts, the descriptor file and the binary file. The descriptor is a text file that describes the properties of the binary files. In the previous articles we discussed parentNameFileHint, CID, parentCID and today we will go into the geometry of the disk. For a much more indepth explanation please see [this document](http://www.vmware.com/support/developer/vddk/vmdk_50_technote.pdf).

Below is the descriptor for the base disk of a VM. The file name is sql.vmdk.


```
# Disk DescriptorFile
version=1
encoding="UTF-8"
CID=5352cace
parentCID=ffffffff
isNativeSnapshot="no"
createType="vmfs"

# Extent description
RW 268435456 VMFS "sql-flat.vmdk"

# The Disk Data Base
#DDB

ddb.adapterType = "lsilogic"
ddb.geometry.sectors = "63"
ddb.geometry.heads = "255"
ddb.geometry.cylinders = "16709"
ddb.uuid = "60 00 C2 97 ee 00 4e ed-cb 09 ab 10 91 3f 7f 99"
ddb.longContentID = "abc01f80a6019b4b2fa062d35352cace"
ddb.toolsVersion = "8295"
ddb.virtualHWVersion = "7"
ddb.deletable = "true"

```


The highlighted lines above are for the RW, ddb.geometry.sectors, ddb.geometry.heads, and the ddb.geometry.cylinders. Together these values describe the "physical" properties of the virtual disk.

The ddb.geometry.sectors is the number of sectors for the virtual disk.
The ddb.geometry.heads is the number of heads for the virtual disk.
The ddb.geometry.cylinders is the number of cylinders in the virtual disk.
The RW value describes the size of the disk in sectors. RW is actually the disk access, so this would be Read/Write access.

The sectors and heads are consistent for all SCSI/SAS virtual disks, including both lsilogic and buslogic. The sectors contain 512 bytes. If the IDE disk, the heads and sectors are different. For IDE disks please refer to [this KB](http://kb.vmware.com/kb/1016192).


### Calculating Disk Geometry


to change it. If you do not want to change it, the geometry for IDE is below.


```
ddb.adapterType = "ide"
ddb.geometry.sectors = "63"
ddb.geometry.heads = "16"
```


SCSI disks use the following geometry values.


```
ddb.geometry.sectors = "63"
ddb.geometry.heads = "255"
```


Calculating the disk geometry is a simple to do given the byte size of the disk. We take the byte size of the flat file and then divide it out to get the RW value. From the RW value we can calculate the number of cylinders using the sectors and heads. The equations are below.

NOTE: The [floor operation](http://en.wikipedia.org/wiki/Floor_and_ceiling_functions) means to round down to the nearest integer.


```
RW = Byte Size / (bytes in a sector)
cylinders = floor (RW / Sectors / Heads)
```


So lets take an example of a flat file and calculate the geometry from it.


```
# ls sql-flat.vmdk
-rw------- 1 root root 137438953472 Mar 16 12:41 sql-flat.vmdk

```


Since the flat file is 137438953472 bytes, we can calculate the RW to be 268435456.


```
RW = 137438953472 / 512 = 268435456
```


With a RW size of 268435456, we can calculate the number of cylinders. We will use the static values of sectors and heads to do the calculation.


```
cylinders = floor (268435456 / 255 / 63 = 16709.334329287)
cylinders = 16709
```


So we have the following values that we can check in our descriptor. If we look at the descriptor, we can see that the geometry is correct.


```

RW 268435456 VMFS "sql-flat.vmdk"
ddb.geometry.sectors = "63"
ddb.geometry.heads = "255"
ddb.geometry.cylinders = "16709"
```


NOTE: The RW field in the descriptor is a comnbination of the RW label, number of sectors, disk type, and the binary file name.


## Troubleshooting geometry issues




### Snapshot Geometry


Since the snapshot is a delta file, it should have the same geometry as the base disk in the chain. So it is easy to know what the geometry values should be. We do not include cylinders, heads or sectors in the snapshot descriptors, so we only have to check the RW values in the snapshot chain.

Below is an example of a snapshot descriptor. In it we can see the RW line is a bit different than the base disk. It is missing the cylinders, heads, and sectors. The disk type is now VMFSSPARESE, which is the disk type for a snapshot file.


```
# Disk DescriptorFile
version=1
encoding="UTF-8"
CID=fffffffe
parentCID=fffffffe
isNativeSnapshot="no"
createType="vmfsSparse"
parentFileNameHint="sql-000004.vmdk"
# Extent description
RW 16777216 VMFSSPARSE "sql-000005-delta.vmdk"
```



### Manually checking the RW values in the chain


We can manually check the RW values with grep to confirm that they are all the same for the chain. Below are a few different examples on how to do this.

The first example is using grep to manually confirm that the RW values are the same. The grep command pulls the RW line out of each descriptor in the chain. We can verify them visually to see that sql-000003.vmdk has the wrong RW value.


```
# grep RW sql-00000?.vmdk sql.vmdk
sql-000001.vmdk:RW 16777216 VMFSSPARSE "sql-000001-delta.vmdk"
sql-000002.vmdk:RW 16777216 VMFSSPARSE "sql-000002-delta.vmdk"
sql-000003.vmdk:RW 125167908 VMFSSPARSE "sql-000003-delta.vmdk"
sql-000004.vmdk:RW 16777216 VMFSSPARSE "sql-000004-delta.vmdk"
sql-000005.vmdk:RW 16777216 VMFSSPARSE "sql-000005-delta.vmdk"
sql.vmdk:RW 16777216 VMFS "sql-flat.vmdk"

```


The next example works better when there are many snapshot files and we want to confirm that they are all the same. In this example we can see that there is one that is different, so we have to search for the file that is different.

The output of the first command shows 1 snapshot has a RW value of 125167908. The second output line shows that we have 5 descriptors that have the RW value of 16777216.


```
# egrep -h -o "RW [0-9]+" sql-0000??.vmdk sql.vmdk | sort |uniq -c
1 RW 125167908
5 RW 16777216

```


Since we have one that was different, we have to identify it. So we grep for the file with the unique RW value.


```
# grep 125167908 sql-0000??.vmdk sql.vmdk
sql-000003.vmdk:RW 125167908 VMFSSPARSE "sql-000003-delta.vmdk"
```


We can see that sql-000003.vmdk is the snapshot with the wrong RW value.


### Checking the RW values in the chain with vmkfstools


As with previous posts, we will use the vmkfstools command to check the snapshot chain. If the RW values are different between a snapshot and it's parent, vmkfstools will tell us.


```
# vmkfstools -v 10 -q sql-000005.vmdk
DISKLIB-VMFS  : "./sql-000005-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000004-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-LINK  : DiskLinkIsAttachPossible: the capacity of each link is different (125167908 != 16777216).
DISKLIB-CHAIN : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003.vmdk" : failed to open (The capacity of the parent virtual disk and the capacity of the child disk are different).
DISKLIB-VMFS  : "./sql-000005-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000004-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003-delta.vmdk" : closed.
DISKLIB-LIB   : Failed to open 'sql-000005.vmdk' with flags 0x17 The capacity of the parent virtual disk and the capacity of the child disk are different (67).
Failed to open 'sql-000005.vmdk' : The capacity of the parent virtual disk and the capacity of the child disk are different (67).


```


In the example above we can see that the RW values are not consistent across the snapshots. The highlighted line shows us what values do not match. Since it was opening sql-000003.vmdk, we know this is the disk that has the wrong RW value.

Let's confirm that this is the one with the wrong RW value. First let's see what the snapshot has.


```
 # grep RW sql-000003.vmdk
RW 125167908 VMFSSPARSE "sql-000003-delta.vmdk"

```


Now lets compare that to the base disk.


```
# grep RW sql.vmdk
RW 16777216 VMFS "sql-flat.vmdk"
```


vmkfstools told us that it was 16777216 was not equal to 125167908, and we can see that here. The snapshot has the wrong RW value.

In order to fix this, we can change the RW value in sql-000003.vmdk to 16777216 and try again. In the example below we are using sed to make the changes for us, vi can be used to manually make the changes.


```
# sed -i 's/RW 125167908/RW 16777216/' sql-000003.vmdk
```


Let's confirm sql-000003.vmdk has the updated RW value.


```
 # grep RW sql-000003.vmdk
RW 16777216 VMFSSPARSE "sql-000003-delta.vmdk"
```


So the RW values should be correct now, so let's confirm with vmkfstools again.


```
# vmkfstools -v 10 -q sql-000005.vmdk
DISKLIB-VMFS  : "./sql-000005-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000004-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000002-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000001-delta.vmdk" : open successful (23) size = 20480, hd = 0. Type 8
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-flat.vmdk" : open successful (23) size = 8589934592, hd = 0. Type 3
sql-000005.vmdk is not an rdm
DISKLIB-VMFS  : "./sql-000005-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000004-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000003-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000002-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-000001-delta.vmdk" : closed.
DISKLIB-VMFS  : "/vmfs/volumes/4e4043f8-160ce551-98d6-00221591be89/sql/sql-flat.vmdk" : closed.
AIOMGR-S : stat o=6 r=18 w=0 i=0 br=294912 bw=0

```


This time the descriptors are correct. Now we can clone the disk or try to commit the snapshots. See [VMware Snapshot Troubleshooting](http://virtuallyhyper.com/2012/04/vmware-snapshot-troubleshooting/) for information on cloning or committing.
