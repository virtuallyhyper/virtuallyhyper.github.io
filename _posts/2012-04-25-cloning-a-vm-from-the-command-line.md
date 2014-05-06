---
author: jarret
comments: true
date: 2012-04-25 01:34:45+00:00
layout: post
slug: cloning-a-vm-from-the-command-line
title: Cloning a VM from the Command Line
wordpress_id: 1242
categories:
- VMware
- vTip
following_users:
- jarret
- joechan
- kelatov
tags:
- clone
- command line
- register vm
- snapshots
- vmdk
- vmkfstools
- vtip
---

Creating a new VM and attaching a disk to it can easily be done through the vSphere Client, but it can be more challenging through the command line. In order to clone a VM from the command line, we need to copy and update the VMX, clone the VMDK and register the VM through the command line.

If you are interested in managing the host with out using the vSphere Client, [vm-help has a series](http://www.vm-help.com/esx40i/manage_without_VI_client_1.php) of posts about managing the host from the command line.

First we start with creating a new directory for the cloned VM. I chose the name sql.cloned for this example and made the directory using the [absolute path](http://en.wikipedia.org/wiki/Path_(computing)).


```
# mkdir /vmfs/volumes/iscsi_dev/sql.cloned
```


Next, we have to clone the VMDK. This VM is running on snapshots, so we clone the latest snapshot seen in the VMX.


```

 # grep vmdk sql.vmx
scsi0:0.fileName = "sql-000005.vmdk"

# vmkfstools -d thin -i sql-000005.vmdk /vmfs/volumes/iscsi_dev/sql.cloned/sql.cloned.vmdk
Destination disk format: VMFS thin-provisioned
Cloning disk 'sql-000005.vmdk'...
Clone: 100% done. 
```


Now that the disk is cloned over to the new VM's directory, let's copy the VMX over.


```

# cp sql.vmx /vmfs/volumes/iscsi_dev/sql.cloned/sql.cloned.vmx

```


Since we have the disks and the VMX cloned over to the new datastore, let's change to that directory.


```
# cd /vmfs/volumes/iscsi_dev/sql.cloned/
# ls
sql.cloned-flat.vmdk  sql.cloned.vmdk       sql.cloned.vmx

```


We can see that sql.cloned has a disk and a vmx. We need to update the VMX to point to the new disk. Let's see what the VMX currently has for references to the old name.


```
# grep sql sql.cloned.vmx
nvram = "sql.nvram"
displayName = "sql"
extendedConfigFile = "sql.vmxf"
scsi0:0.fileName = "sql-000005.vmdk"

```


In the output above, there are a few references to the previous name. The first is the [nvram](http://www.vmware.com/support/ws5/doc/ws_learning_files_in_a_vm.html) file. We did not copy this over, but we still can. The nvram is the configuration file for the BIOS of the VM, so if you have custom BIOS settings, copy the nvram over to the new directory.

The displayName will also need to be updated to reflect the new name. This is the name for the VM that id displayed on the host.

The [extendedConfigFile](http://www.vmware.com/support/ws5/doc/ws_learning_files_in_a_vm.html) (.vmxf) is for the configuration of a team. Most times it is unneeded and so we did not copy it over.

The last variable that needs to be changed is the scsi0:0.fileName, which is the pointer to the disk. We will update this along with the display name with the sed commands below. This can be manually changed with vi as well, but it is easier to illustrate the changes with sed.


```
# sed -i 's/displayName = "sql"/displayName = "sql.cloned"/' sql.cloned.vmx

# sed -i 's/scsi0:0.fileName = "sql-000005.vmdk"/scsi0:0.fileName = "sql.cloned.vmdk"/' sql.cloned.vmx

# sed -i 's/nvram = "sql.nvram"/nvram = "sql.cloned.nvram"/' sql.cloned.vmx

```


Let's check to see if the changes actually made it into the VMX.


```
# grep sql sql.cloned.vmx
nvram = "sql.cloned.nvram"
displayName = "sql.cloned"
extendedConfigFile = "sql.vmxf"
scsi0:0.fileName = "sql.cloned.vmdk"

```


So now that we have updated the references to the files that we have in the sql.cloned directory, let's register the VM on the host. vim-cmd is only on ESXi, if you are on ESX you can try vmware-vim-cmd as described in [this KB](http://kb.vmware.com/kb/1006160).


```
# vim-cmd solo/registervm /vmfs/volumes/iscsi_dev/sql.cloned/sql.cloned.vmx
222

```


vim-cmd was able to register the VM to the host and returned the ID of the virtual machine, 222, to us. Now we can try to power the VM on. In the GUI you would see it pop up asking if we moved or copied the virtual machine. On the command line we need to check the messages and answer it. Since the VM will not power on until the question is answered, we will background the command so that we do not lock up the ssh session.


```
# vim-cmd vmsvc/power.on 222 &
 Powering on VM:
```


**NOTE:** The "&" above backgrounds the command so that we can interact with the console while the command is running in the background. The man page of "sh" describes the background command as below.


> If a command is terminated by the control operator &, the shell exe-
cutes the command in the background in a subshell. The shell does not
wait for the command to finish, and the return status is 0.


We can now run the message command on the VM to see if it has been sitting there waiting for the question to be answered.


```
# vim-cmd vmsvc/message 222
Virtual machine message 0:
msg.uuid.altered:This virtual machine may have been moved or copied.

In order to configure certain management and networking features VMware ESX needs to know which.

Did you move this virtual machine, or did you copy it?
If you don't know, answer "I copied it".
0. Cancel (Cancel)
1. I _moved it (I _moved it)
2. I _copied it (I _copied it) [default]

```


We know that we copied it, so we answer question 0 with option 2.


```

# vim-cmd vmsvc/message 222 0 2
```


The VM powers on and we now have manually cloned the virtual machine from the command line.
