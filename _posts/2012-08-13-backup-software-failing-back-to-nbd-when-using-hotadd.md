---
author: jarret
comments: true
date: 2012-08-13 20:30:20+00:00
layout: post
slug: backup-software-failing-back-to-nbd-when-using-hotadd
title: Backup Software Fails Back to NBD When Using HotAdd
wordpress_id: 1338
categories:
- Storage
- VMware
ef_usergroup:
- JJK
following_users:
- jarret
- kelatov
tags:
- Backups
- Hot-add
- NBD
---

During a backup window, random Virtual Machines (VMs) would fail back to NBD mode instead of using HotAdd. This occurred on 60% of the VMs every night, but the VMs that experienced this were different every night.





There are three different virtual disk [transport modes](http://pubs.vmware.com/vsphere-50/index.jsp?topic=/com.vmware.vddk.pg.doc_50/vddkDataStruct.5.5.html): NBD, SAN, and Hotadd. SAN and HotAdd are typically faster and less resource intensive. If these modes fail, the backup should fail back to NBD.





Now what is HotAdd? The [VMware VDDK documentation](http://pubs.vmware.com/vsphere-50/index.jsp?topic=/com.vmware.vddk.pg.doc_50/vddkDataStruct.5.5.html) describes HotAdd.





> 
  
> 
> If the application runs in a virtual machine, it can create a linked-clone virtual machine from the backup snapshot and read the linked clone’s virtual disks for backup. This involves a SCSI HotAdd on the host where the application is running – disks associated with the linked clone are HotAdded on the virtual machine. The backup process for HotAdd is to take a snapshot of the VM, attach the base disk in [independent nonpersistant](http://www.vmware.com/support/ws3/doc/ws32_disks2.html) mode, backup from the attached drive, detach the disk, and then commit the snapshot. The disk is attached to the backup proxy, so the backup software can read the data locally. Independent nonpersistant disks are used so that the base disk is not modified.
> 
> 






To get a better picture of what is occurring we can look at an image from the [VMware VDDK documentation](http://pubs.vmware.com/vsphere-50/index.jsp?topic=/com.vmware.vddk.pg.doc_50/vddkDataStruct.5.5.html). The ESX host on the right has a backup proxy and a VM that it is backing up. The disk is attached to both the backup proxy and the VM. The image below is copied from the previous link.





[![Hot-Add](http://virtuallyhyper.com/wp-content/uploads/2012/07/vddkDataStruct.5.5.3.png)](http://virtuallyhyper.com/wp-content/uploads/2012/07/vddkDataStruct.5.5.3.png)





So the first thing to check for is licensing. If the hosts are unable to utilize HotAdd mode, they may not be licensed for HotAdd. The [VDDK release notes](http://www.vmware.com/support/developer/vddk/VDDK-500-ReleaseNotes.html) describe HotAdd licensing.





> 
  
> 
> In vSphere 5.0, the SCSI HotAdd feature is enabled only for vSphere editions Enterprise and higher, which have Hot Add licensing enabled. No separate Hot Add license is available for purchase as an add-on. In vSphere 4.1, Hot Add capability was also allowed in Advanced edition. Therefore, customers with vSphere Essentials or Standard edition who use backup products (including VMware Data Recovery) are not able to perform proxy-based backup, which relies on SCSI HotAdd. Those customers must use alternate transport modes.
> 
> 






In our case we were on version 4.1 and were using Advanced edition, so the licensing was not a factor.





Another cause can be file size limitations. When a snapshot of a VM is taken, the snapshot files will be put in the directory of the VM by default. If the VM has large disks and a small block size for the configuration files, we would hit a problem where the snapshot file is larger than the maximum size of the datastore containing the configuration files. KBs [1012384](http://kb.vmware.com/kb/1012384) and [1031038](http://kb.vmware.com/kb/1031038) address this issue in more depth. To rule out size issues, we picked one of the VMs that was failing and took a snapshot. The snapshot completed successfully.





The NDB backups were intermittent and inconsistent, so we knew that it was likely an issue with the appliance itself, but user permissions can also be a common issue. If you see that Hot-Add is failing, you can check the user permissions for the backup user. Log into the vSphere Client with the backup user's credentials and manually perform a hot-add operation. To do this, select 2 VMs. One (VMa) will be the VM we backup and the other (VMb) will be the backup proxy. Follow the steps below to test a manual hot-add process (without taking an actual backup):







  1. Take a quiesced snapshot of VMa, making sure the memory option is not checked.


  2. Edit Settings on VMb.





    1. Click "Add an existing hardisk"


    2. Select the base disk for VMa.


    3. Add it as a independent non-persistent disk.




  3. Edit Settings on VMb.





    1. Remove the disk from VMa.


    2. Leave the default remove option selected.




  4. Delete all snapshots on VMa






If there are any issues doing the operations above, then they should be investigated. If any of the options are greyed out, it is likely a permission issue or a configuration issue with the VM. Some fixes for the above scenario are listed in the VMware KB entitled [Hot plug operations are disabled when using VMDirectPath I/O](http://kb.vmware.com/kb/1010789)





In our specific case we noticed that 60% into the backup process, it would revert to NBD backups. This could mean that the VM or the Host is overloaded. We looked at the virtual SCSI controllers (SCSI 0, SCSI 1, SCSI2, SCSI 3) on the backup proxy and found that there was only SCSI 0 (one SCSI Controller). Each controller can have up to 15 disks attached. When a SCSI controller is filled up, no more disks can be added to the backup proxy VM. When we looked at the proxy backup VM during the snapshot window, we saw that all 15 slots were filled up. To alleviate this, we added 3 more SCSI Controllers (SCSI 1-3).





If you only have a single disk on the backup proxy, you can add small "placeholder" disks to add additional SCSI controllers. To add another controller, you need to add a new disk, or move an existing disk to another SCSI slot. To move a disk to another SCSI Controller, change the controller to SCSI 1:* (i.e SCSI 1:0) for that specific virtual disk, at that point ESX will automatically add a new controller.



