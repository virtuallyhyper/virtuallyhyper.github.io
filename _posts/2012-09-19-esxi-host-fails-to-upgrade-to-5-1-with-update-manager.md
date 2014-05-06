---
author: jarret
comments: true
date: 2012-09-19 16:25:48+00:00
layout: post
slug: esxi-host-fails-to-upgrade-to-5-1-with-update-manager
title: ESXi host fails to upgrade to 5.1 with Update Manager
wordpress_id: 3532
categories:
- Home Lab
- VMware
following_users:
- jarret
- kelatov
tags:
- .fseventsd
- .Spotlight-V100
- '5.1'
- altbootbank
- bootbank
- dpm
- OSX
- upgrade
- VUM
---

Today I was running through some upgrades in my lab to ESXi 5.1 with update manager. One small issue was that after adding a base line to the host and hitting remediate, the host upgrade would fail with the following message.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/09/esxi5.1-upgrade-fails.png)](http://virtuallyhyper.com/wp-content/uploads/2012/09/esxi5.1-upgrade-fails.png)

The error message "The operation is not supported on the selected inventory objects. Check the events for the objects selected for the operation." can mean different things. One of them is because the [admission control is preventing the host from entering maintenance mode](http://kb.vmware.com/kb/1024331).

Looking at the events tab I saw that DPM was causing the operation to fail.
[![](http://virtuallyhyper.com/wp-content/uploads/2012/09/esx5.1-upgrade-DPM.png)](http://virtuallyhyper.com/wp-content/uploads/2012/09/esx5.1-upgrade-DPM.png)

To fix this I could disable DPM in the cluster manually, but VUM gives me the option to temporarily disable it while it does the remediation. During the remediation wizard, I had to check the option below.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/09/esx5.1-upgrade-disable-dpm.png)](http://virtuallyhyper.com/wp-content/uploads/2012/09/esx5.1-upgrade-disable-dpm.png)

After I did this, I was able to start the upgrade, but it failed with "Software of system configuration of host esx4.moopless.com is incompatible. Check the scan results for details". This is because the host has not been rebooted after the vCenter was upgraded. When the vCenter was upgraded, new host agents would have been installed when the hosts were reconnected. These agents require a reboot before any other patches can be applied to the host.

So I rebooted the host and kicked off another remediation that failed saying that it could not run the update script on the host.

[![](http://virtuallyhyper.com/wp-content/uploads/2012/09/esx5.1-upgrade-run-script.png)](http://virtuallyhyper.com/wp-content/uploads/2012/09/esx5.1-upgrade-run-script.png)

So I went searching for the logs. In the vmware-vum-server-log4cpp.log on the [VUM server](http://kb.vmware.com/kb/1003693) and found the following.


```

[2012-09-10 15:14:46:519 'NfcClientWrapper' 2840 INFO]  [nfcClientWrapper, 155] Copying file: local = C:/WINDOWS/TEMP/upgradekejxxzsf.tmp/metadata.zip, remote = /tmp/vuaScript-oDoyLu/metadata.zip
[2012-09-10 15:14:46:519 'NfcClientWrapper' 2840 INFO]  [nfcClientWrapper, 155] Copying file: local = C:/WINDOWS/TEMP/upgradekejxxzsf.tmp/precheck.py, remote = /tmp/vuaScript-oDoyLu/precheck.py
[2012-09-10 15:14:46:660 'NfcClientWrapper' 2840 INFO]  [nfcClientWrapper, 155] Copying file: local = C:/WINDOWS/TEMP/upgradekejxxzsf.tmp/esximage.zip, remote = /tmp/vuaScript-oDoyLu/esximage.zip
[2012-09-10 15:14:53:706 'HU-Upgrader' 2840 ERROR]  [upgraderImpl, 421] Script execution failed on host: 192.168.5.33)

```


The logs above are not very informative, so I went to the host to see why it failed copying the 'esximage.zip' over to the host. In the vpxa.log I found the following.


```

2012-09-11T05:02:32.447Z [FFCF5B90 verbose 'Default' opID=task-internal-233-fb0a87f0] [VpxaDatastoreContext] No conversion for localpath /tmp/vuaScript-oDoyLu/esximage.zip
2012-09-11T05:02:33.337Z [FFCF5B90 warning 'Libs' opID=task-internal-233-fb0a87f0] [NFC ERROR] NfcFile_ContinueReceive: reached EOF

```


It looks like it was failing to copy over the files to the bootbanks. I kicked off another remediation and took a look at the system while it was upgrading. The host creates a new ramdisk to stage the changes. It is called the 'upgradescratch'.


```

# vdu -h |tail -n 1
                ramdisk upgradescratch:  300M		(  103 inodes)

```


I ran a df to see how much space was used in the ram disk.


```

# df -h
Filesystem    Size   Used Available Use% Mounted on
...
vfat        285.9M  37.8M    248.1M  13% /vmfs/volumes/c968084a-b7429e1a-770c-aa784ee74000

```


So we know that it was copying over the files. I ran it again and found that it was indeed increasing the used space.


```

# df -h
Filesystem    Size   Used Available Use% Mounted on
...
vfat        285.9M 240.0M     45.8M  84% /vmfs/volumes/c968084a-b7429e1a-770c-aa784ee74000

```


Since update manager was copying over the files and it was failing when trying to run the script, I went ahead and took a look at the bootbanks. There are 2 bootbanks in ESXi; /bootbank and /altbootbank. [This document](http://www.vmware.com/files/pdf/ESXi_architecture.pdf) describes boot banks as the following.


> The ESXi system has two independent banks of memory, each
of which stores a full system image, as a fail-safe for applying
updates. When you upgrade the system, the new version is
loaded into the inactive bank of memory, and the system is
set to use the updated bank when it reboots. If any problem
is detected during the boot process, the system automatically
boots from the previously used bank of memory. You can also
intervene manually at boot time to choose which image to use
for that boot, so you can back out of an update if necessary


In the /bootbank I did an ls -la and found the following files.


```

# ls -la |head -n 5
drwxr-xr-x    1 root     root                512 Sep 18 05:33 ..
drwxr-xr-x    1 root     root                  8 Jan  1  1970 .Spotlight-V100
drwxr-xr-x    1 root     root                  8 Jan  1  1970 .Trashes
-rwx------    1 root     root               4096 Aug 22 21:33 ._.Trashes
drwxr-xr-x    1 root     root                  8 Jan  1  1970 .fseventsd

```


From this [blog post](http://coolestguyplanettech.com/downtown/spotlight-out-control-mac-os-x-lion-107), you can see that I once [attached this usb boot drive to my Mac](http://www.ghacks.net/2009/08/19/spotlight-v100-and-trashes-folders-on-usb-flash-drives/) and the Mac added the .Spotlight-V100, .Trashes, ._.Trashes, and .fseventsd files when it mounted the volume. The truth is that I used OSX to [dd install](http://vmwire.com/2011/09/11/creating-vsphere-5-esxi-embedded-usb-stick/) ESXi 5.0. Once the DD had finished it auto mounted the partitions.

Since these files will never be used in ESXi, I decided to remove them.


```

 # rm -rf .fseventsd/ ._.Trashes .Trashes/ .Spotlight-V100/

```


Since any filesystem that was mounted by OSX would contain these, I decided to clean up the /altbootbank as well. Here are the files in the /altbootbank.


```

 # ls -la
drwxr-xr-x    1 root     root                512 Sep 18 05:36 ..
drwxr-xr-x    1 root     root                  8 Jan  1  1970 .Spotlight-V100
drwxr-xr-x    1 root     root                  8 Jan  1  1970 .fseventsd
-rwx------    1 root     root                 96 Jun 29 13:49 boot.cfg
-rwx------    1 root     root                197 Aug 23 11:53 imgdb.tgz
-rwx------    1 root     root                 20 Sep 14 08:50 useropts.gz

```


I removed the .Spotlight-V100 and .fseventsd from the altbootbank


```

# rm -rf .Spotlight-V100/ .fseventsd/

```


I did another remediation of the upgrade and it went through just fine. So these OSX specific files were disrupting the upgrade.
