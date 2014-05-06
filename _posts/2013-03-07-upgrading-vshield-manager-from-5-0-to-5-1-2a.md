---
author: jarret
comments: true
date: 2013-03-07 17:35:17+00:00
layout: post
slug: upgrading-vshield-manager-from-5-0-to-5-1-2a
title: Upgrading vShield Manager from 5.0 to 5.1.2a
wordpress_id: 7272
categories:
- Networking
- VMware
following_users:
- jarret
- kelatov
tags:
- '#vCNS'
- '#vShieldManager'
- '#vShieldUpgrade'
- VMware
---

Recently I had to upgrade a [vCloud Director (vCD)](https://www.vmware.com/products/vcloud-director/overview.html) instance from 1.5 to 5.1.1. One of the dependencies of the upgrade is vShield Manager, so I upgraded it.





## Requirements for Upgrading





It is a good idea to make sure that we have all of the requirements before doing the upgrade. From [this KB](http://kb.vmware.com/kb/2034699):





> 
  
> 
> **Software Requirements**
> 
> 
  
  
> 
> These are the minimum required versions of VMware products to be installed with vShield 5.1.1:
> 
> 
  
  
> 
> 
  
>   * VMware vCenter Server 5.0 or later 
> 
  
  
  
> 
> Note: For VXLAN virtual wires, you need vCenter Server 5.1 or later.
> 
> 
  
  
> 
> 
  
>   * VMware ESX 4.1 or later for each server
> 
  
  
  
> 
> Note: For VXLAN virtual wires, you need VMware ESXi 5.1 or later.
> 
> 
  
  
> 
> 
  
>   * VMware Tools
> 
  
  
  
> 
> Notes: For vShield Endpoint and vShield Data Security, you must upgrade your virtual machines to hardware version 7 or 8 and install VMware Tools 8.6.0 released with ESXi 5.0 Patch 3. You must install VMware Tools on virtual machines that are to be protected by vShield App.
> 
> 
  
  
> 
> 
  
>   * VMware vCloud Director 1.5 or later
> 
  






My vCenter is currently 5.1, ESXi hosts are at 5.0, and vCD is 1.5 so I meet the requirements for the upgrade.





## Upgrading vShield Manager





The first component that I need to upgrade is vShield Manager, also known as [vCloud Network and Security (vCNS)](http://www.vmware.com/products/datacenter-virtualization/vcloud-network-security/overview.html), to 5.1.2a. [This KB](http://kb.vmware.com/kb/2034699) goes over the whole process in detail.





### Pre-Upgrade the vShield Manager





Before doing the actual upgrade, we should make sure that we have enough free space on the vShield Manager to do the upgrade. To do this I logged into the vShield Manager appliance and ran **show filesystems**. From the output below I only have 2.0 GB free on my _/common_ partition. The minimum space for the upgrade is 2.5GB, so I will need to run the maintenance bundle.





![vShield Manager Space Utilization](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-Space.jpg)





In order to upgrade the appliance I need to upload the maintenance bundle to clean up some space. This can be downloaded from the [vCNS download page](https://my.vmware.com/web/vmware/info/slug/security_products/vmware_vcloud_networking_and_security/5_1). After I downloaded the maintenance bundle, I logged into the vShield Manager and clicked on the _Updates Tab_ and _Upload Settings_.





![vShield Manager Upload Settings](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-Upload-Settings.jpg)





Click on **Choose File** and select the maintenance bundle and click **Upload File**. This just returned with a blank page for me. I logged into the vShield Manager again and ran **show manager log** and at the bottom I found that it was throwing an invalid file error.





![vShield Manager Upgrade Invalid File Error](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-manager-Upgrade-Invalid-File.jpg)





I found [this blog post](http://thephuck.com/virtualization/vshieldmanager-upgrade-bundle-errors-with-invalid-file-misnamed-at-download-vmware-vsphere51/) that describes the problem. Apparently the appliance expects a _tar.gz_ extension where as the file downloaded from VMware has only a _gz_ extension. So I renamed the file from `VMware-vShield-Manager-upgrade-bundle-maintenance-5.1.2-997359.gz` to `VMware-vShield-Manager-upgrade-bundle-maintenance-5.1.2-997359.tar.gz` and tried the upload again. This time it brought be to a screen where I can choose to install it. Click on **Install**, then **Confirm Installation** to start the maintenance.





![vShield Manager Upgrade Install Maintenance bundle](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-Upgrade-Install-Maintenance-bundle.jpg)





We should check the space again to make sure we have enough. Log into the manager and run **show filesystems**. From below we can see that I now have 2.6G free.





![vShield Manager Space after Maintenance](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-Space-after-Maintenance.jpg)





### Upgrade vShield Manager





Now that the maintenance bundle has installed we can install the actual bundle upgrade. Make sure to rename the bundle from `VMware-vShield-Manager-upgrade-bundle-5.1.2-943471.gz` to `VMware-vShield-Manager-upgrade-bundle-5.1.2-943471.tar.gz`. Now upload the file and click **Install**.





![vShield Manager install upgrade](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-install-upgrade.jpg)





You will see the appliance Upgrade and then reboot. Once that is complete, log into the web interface and confirm that it is showing the new version on the _Updates Tab_.





![vShield Manager post upgrade](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-post-upgrade.jpg)





Since they changed the system requirements for vShield Manager from 5.0 to 5.1, we need to upgrade the hardware of the virtual machine. The problem is that it is really painful to do manually and not recommended, so what we need to do is take a backup from our newly upgraded vShield Manager and restore it to a new appliance with the correct hardware requirements. [This KB](http://kb.vmware.com/kb/1022135) gives us the process:





> 
  
> 
> 
  
>   1. Click **Settings & Reports** from the vShield inventory panel.
> 
  
>   2. Click the **Configuration** tab.
> 
  
>   3. Click **Backups**.
> 
  
>   4. Type the **Host IP Address** of the system where the backup will be saved.
> 
  
>   5. Type the **User Name** required to log in to the backup system.
> 
  
>   6. Type the **Password** associated with the user name for the backup system.
> 
  
>   7. In the **Backup Directory** field, type the absolute path where backups are to be stored.
> 
  
>   8. Type a text string in **Filename Prefix**. This text is prepended to the backup filename for easy recognition on the backup system. For example, if you type ppdb, the resulting backup is named as ppdbHH_MM_SS_DDMMYYYY.
> 
  
>   9. From the Transfer Protocol dropdown, choose either **SFTP or FTP**.
> 
  
>   10. To perform the backup, click **Backup**.
> 
  






After the backup has completed you will see it at the bottom of the page and on your FTP/SFTP Server.





![vShield Manager Backup](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-Backup.jpg)





Now we want to shut down the original appliance and rename it. We are going to deploy a new 5.1.2a appliance and restore the backup we just made to it. The instructions for deploying a new vShield Manager Appliance can be found in the [quick start guide](http://www.vmware.com/pdf/vshield_51_quickstart.pdf) or in [this blog article](http://netappsky.com/vmware/vmware-vshield-zones/).





After the applince has been deployed and powered on, login to the console with the username **admin** and the password **default**. In the console type **enable** and enter the password **default** again. Now that we are logged into the enabled console we can run **setup** to configure an IP address for the vShield manager. Give the new vShield Manager the same IP address as the one we are replacing.





![vShield Manager Setup](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-setup.jpg)





Once you have updated the IP address, log into the vShield Manager Web Client. We are going to restore the previous backup using a similar procedure as the backup.







  1. Click **Settings & Reports** from the vShield inventory panel.


  2. Click the **Configuration** tab.


  3. Click **Backups**.


  4. Type the **Host IP Address** of the system where the previous backup was saved.


  5. Type the **User Name** required to log in to the backup system.


  6. Type the **Password** associated with the user name for the backup system.


  7. In the **Backup Directory** field, type the absolute path where the backup was stored.


  8. Type a text string in **Filename Prefix**. Ensure to enter the same string as before


  9. From the Transfer Protocol dropdown, choose either **SFTP or FTP**.


  10. Click on **View Backups**.


  11. **Select** the backup we made before. 


  12. Click **Restore**.


  13. Click **OK* to restore the backup.





The next screen will show that the vShield Manager is restoring the backup. Wait until the operation completes.





![vShield Manager Restore Backup](http://virtuallyhyper.com/wp-content/uploads/2013/03/vShield-Manager-Restore-Backup.jpg)





Once the operation has completed, log into the web client again and confirm the settings. vShield Manager has been upgraded. If you have vShield App appliances, the bottom of [this KB](http://kb.vmware.com/kb/2034699) lists the instructions for upgrading vShield App and Edge appliances.



