---
author: jarret
comments: true
date: 2012-08-31 16:17:46+00:00
layout: post
slug: installing-openvz-on-centos-6-in-an-esxi-5-vm
title: Installing OpenVZ on CentOS 6 in an ESXi 5 VM
wordpress_id: 1763
categories:
- Home Lab
- VMware
following_users:
- jarret
- kelatov
tags:
- hypervisor
- nested
- openvz
- selinux
- sysctl
- vzctl
---

After [installing KVM](http://virtuallyhyper.com/2012/07/installing-kvm-as-a-virtual-machine-on-esxi5-with-bridged-networking/), I wanted to try out some other hypervisors that are common. I have owned multiple [OpenVZ](http://wiki.openvz.org/Main_Page) VPS's. In fact this site is currently hosted on one. OpenVZ was one of the easiest deployments yet. The technology is very cool, but more limited than the other hypervisors.  I would suggest reading the [quick start guide](http://wiki.openvz.org/Quick_installation). The information below is how I installed it.

The first thing we need to do is install CentOS 6 in a [hardware version 8 VM](http://www.vladan.fr/vmware-vsphere-5-virtual-machine-hardware-version-8/) on the ESXi host. Make sure that the VM is attached to a port group in [promiscuous mode](http://kb.vmware.com/kb/1004099).



## Configuring the ESXi 5 VM for OpenVZ


As per [this article](http://communities.vmware.com/docs/DOC-8970), we can set some options in the VMX to allow nested hypervisors. Append the following lines to the VMX file after the VM has been shut down.


```

cpuid.1.ecx=&quot;----:----:----:----:----:----:--h-:----&quot;
cpuid.80000001.ecx.amd=&quot;----:----:----:----:----:----:----:-h--&quot;
cpuid.8000000a.eax.amd=&quot;hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh&quot;
cpuid.8000000a.ebx.amd=&quot;hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh&quot;
cpuid.8000000a.edx.amd=&quot;hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh&quot;
monitor.virtual_mmu = &quot;hardware&quot;
monitor.virtual_exec = &quot;hardware&quot;

```


Now we need to reload the VMX file so the host acknowledges the new changes. We will do this with the [vim-cmd command](http://kb.vmware.com/kb/1026043). 


```

# vim-cmd vmsvc/getallvms |grep openvz1
254    openvz1                              [iscsi_dev] openvz1/openvz1.vmx                                                          rhel6_64Guest             vmx-08                                                                                                                                                        
# vim-cmd vmsvc/reload 254
# vim-cmd vmsvc/power.on 254
Powering on VM:

```




## Installing OpenVZ


Now we can log into the VM and start configuring OpenVZ. The first thing we should do is add the openVZ repo. 


```

# wget http://download.openvz.org/openvz.repo
# mv openvz.repo /etc/yum.repos.d/
# rpm --import http://download.openvz.org/RPM-GPG-Key-OpenVZ
# yum update

```


Now we can install the vzkernel and reboot into it.

```

# yum install vzkernel
# reboot

```


After the reboot we should confirm that we are running on the right kernel.

```

# uname -a
Linux openvz1 2.6.32-042stab057.1 #1 SMP Fri Jun 22 02:17:07 MSD 2012 x86_64 x86_64 x86_64 GNU/Linux

```


Now we install the OpenVZ tools.


```

# yum install vzctl vzquota -y

```


We need to change some kernel parameters for OpenVZ to work correctly. Open up the /etc/sysctl.conf and change/append the following lines.


```

net.ipv4.ip_forward = 1
net.ipv4.conf.default.proxy_arp = 0
net.ipv4.conf.all.rp_filter = 1
kernel.sysrq = 1
net.ipv4.conf.default.send_redirects = 1
net.ipv4.conf.all.send_redirects = 0
net.ipv4.icmp_echo_ignore_broadcasts=1
net.ipv4.conf.default.forwarding=1

```


We can reload the settings from /etc/sysctl.conf by running the following command.

```

# sysctl -p

```


In this instance I want to allow for [multiple subnets](http://www.robertshady.com/content/multiple-subnets-single-ethernet-interface-under-openvz), so we can change the following lines in the /etc/vz/vz.conf.


```

NEIGHBOUR_DEVS=all

```


Selinux is [incompatible with OpenVZ](http://wiki.centos.org/HowTos/Virtualization/OpenVZ) so we need to disable it. Edit the /etc/sysconfig/selinux and change the following line.


```

SELINUX=disabled

```


Now that OpenVZ is installed, we should reboot again. This will disable selinux and also confirm that everything comes up on boot.


```

# reboot

```




## Deploying VMs in OpenVZ


There are many ways of deploying VMs in OpenVZ. The easiest is to download a prebuilt template. Get a template and put it in the right location. Look [here](http://wiki.openvz.org/Download/template/precreated) for prebuilt templates.

In my case, I am just going to use a debian 6 template from the repository.


```

# cd /vz/templates/cache
# wget http://download.openvz.org/template/precreated/debian-6.0-x86.tar.gz

```


Now that we have the template we can deploy the VM. When we deploy the VM we will give it a unique id  (200 in this case). Check out [this article](http://wiki.openvz.org/Basic_operations_in_OpenVZ_environment) for the basic operations in OpenVZ.


```

# vzctl create 200 --ostemplate debian-6.0-x86 --config basic
Creating container private area (debian-6.0-x86)
Performing postcreate actions
CT configuration saved to /etc/vz/conf/200.conf
Container private area was created

```


I want this VM to be started when I boot the OpenVZ server.


```

# vzctl set 200 --onboot yes --save
CT configuration saved to /etc/vz/conf/200.conf

```


Now we can configure this VM's properties. It is as easy as running a few basic commands to set up the VM. With the commands we will set up the Hostname and ip address, etc. With this you can see how easy it would be to script deployments. 


```

# vzctl set 200 --hostname debian1.virtuallyhyper.com --save
CT configuration saved to /etc/vz/conf/200.conf
[root@openvz1 cache]# vzctl set 200 --ipadd 192.168.10.200 --save
CT configuration saved to /etc/vz/conf/200.conf
# vzctl set 200 --nameserver 192.168.5.10 --save
CT configuration saved to /etc/vz/conf/200.conf
# vzctl set 200 --searchdomain virtuallyhyper.com --ram 128M --diskspace 4G:5G --swap 128M --save
CT configuration saved to /etc/vz/conf/200.conf

```


Since everything is configured the way we want it, let's power on the VM.


```

# vzctl start 200
Starting container ...
Container is mounted
Adding IP address(es): 192.168.10.200
Setting CPU units: 1000
Container start in progress..

```


One of the cool features of OpenVZ is that we can execute commands inside the running VM from the hypervisor's console. In this case, let's change the root password.


```

# vzctl exec 200 passwd
Enter new UNIX password: abc123
Retype new UNIX password: abc123
passwd: password updated successfully

```


If running the commands was not enough, we could open a console session with the follwoing command. 


```

# vzctl enter 200

```





## Installing a Panel


One of the coolest things about Open Virtualization Software is the number of panels out there. The panel is the web interface that you use to manage and deploy VMs. It is not necessary to have one, but it is worth checking out.

[OpenVz Web Panel](https://code.google.com/p/ovz-web-panel/) is an open source panel. It has an incredibly easy install and is worth checking out. 

The OpenVz Web Panel comes with a easy installer script. (It will also install ruby on the system)


```

# wget -O - http://ovz-web-panel.googlecode.com/svn/installer/ai.sh | sh

```



Now browse to  and login in with the credentials admin/admin.
After logging in you can manage everything graphically. 

