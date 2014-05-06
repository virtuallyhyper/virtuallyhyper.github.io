---
author: karim
comments: true
date: 2013-04-08 19:22:50+00:00
layout: post
slug: extending-the-system-drive-on-windows-2008-running-as-a-kvm-vm
title: Extending the System Drive on Windows 2008 Running Under KVM
wordpress_id: 8057
categories:
- Home Lab
- OS
ef_usergroup:
- JJK
following_users:
- jarret
- joechan
- kelatov
tags:
- diskmgmt.msc
- qemu-img resize
- WIN2k8
---

While I was writing [this](http://virtuallyhyper.com/2013/04/deploying-a-test-windows-environment-in-a-kvm-infrastucture) post I first allocated 15GB for my Windows install. I quickly realized that after a plethora of updates that is not enough space. Luckily it's pretty easy to remedy that with a two step process.





### 1. Increase the Underlying Hard Disk IMG file Used in KVM





Shutdown the VM and then using **virsh** figure out the full path of the IMG file used by the desired VM. Here is a list of all the VMs that are powered off:




    

```
    [virtuser@kvm ~]$ virsh -c qemu:///system list --inactive
     Id Name                 State
    ----------------------------------
      - VM1                  shut off
      - VM2                  shut off
      - kelatov_Win2k8       shut off
      - kelatov_win7_2       shut off
    
```






Let's see we wanted to resize the Hard Disk of the "kelatov-win7-2" VM. So go ahead and find out where the IMG file is for that VM:




    

```
    [virtuser@kvm ~]$ virsh -c qemu:///system domblklist kelatov_win7_2
    Target     Source
    ------------------------------------------------
    hda        /images/kelatov_win7_2.img
    hdc        -  
    
```






Now let's make sure the size of the file is 15GB like we expect:




    

```
    [virtuser@kvm ~]$ ls -lh /images/kelatov_win7_2.img
    -rw-------. 1 root root 16G Jun 21  2012 /images/kelatov_win7_2.img
    
```






That looks correct. Now let's go ahead and add 5GB to the IMG file:




    

```
    [virtuser@kvm images]$ sudo qemu-img resize kelatov_win7_2.img +5G
    Image resized.
    
```






Let's make sure the new size shows up:




    

```
    [virtuser@kvm ~]$ ls -lh /images/kelatov_win7_2.img
    -rw-------. 1 root root 21G Jun 21  2012 /images/kelatov_win7_2.img
    
```






### 2. Extend the Volume Inside the Windows 2008 OS





Power on the VM and from within Windows run:




    

```
    diskmgmt.msc
    
```






And you will see the following:





[![windows_disk_mgmt](http://virtuallyhyper.com/wp-content/uploads/2013/04/windows_disk_mgmt.png)](http://virtuallyhyper.com/wp-content/uploads/2013/04/windows_disk_mgmt.png)





Then right click on the **C:** partition and then select "Extend Volume":





[![right_click_c-part](http://virtuallyhyper.com/wp-content/uploads/2013/04/right_click_c-part.png)](http://virtuallyhyper.com/wp-content/uploads/2013/04/right_click_c-part.png)





Then the "Extend Volume Wizard" will startup:





[![extend_volume_wizard](http://virtuallyhyper.com/wp-content/uploads/2013/04/extend_volume_wizard.png)](http://virtuallyhyper.com/wp-content/uploads/2013/04/extend_volume_wizard.png)





Click "Next" and it will automatically select the available space on that disk:





[![select_disks_extend_volume](http://virtuallyhyper.com/wp-content/uploads/2013/04/select_disks_extend_volume.png)](http://virtuallyhyper.com/wp-content/uploads/2013/04/select_disks_extend_volume.png)





In our case we have 5GB available which as I mentioned is automatically selected. At this point just click "Next" and then "Finish". After the process is finished, you will see the **C:** partition take up the rest of the drive:





[![c_part_extended](http://virtuallyhyper.com/wp-content/uploads/2013/04/c_part_extended.png)](http://virtuallyhyper.com/wp-content/uploads/2013/04/c_part_extended.png)





Now we can apply more updates :)



