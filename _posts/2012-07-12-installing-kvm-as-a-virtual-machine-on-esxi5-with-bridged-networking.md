---
author: jarret
comments: true
date: 2012-07-12 14:44:33+00:00
layout: post
slug: installing-kvm-as-a-virtual-machine-on-esxi5-with-bridged-networking
title: Installing KVM as a Virtual Machine on ESXi 5 with Bridged Networking
wordpress_id: 1757
categories:
- Home Lab
- VMware
following_users:
- jarret
- kelatov
tags:
- bridged networking
- CentOs
- command line
- hypervisor
- iptables
- KVM
- libvirt
- linux
---

I had never used KVM, but after purchasing a KVM vps to host a few sites I wanted to install it for myself. Since I do not have much extra hardware laying around, I decided to set up KVM in a virtual machine. The performance was not great, but it worked well.

Before beginning, we should [set up the host](http://communities.vmware.com/docs/DOC-8970) to ensure that we can run nested VMs. First set the following option in the /etc/vmware/config


```

vhv.allow = TRUE

```


Then I needed to set up a VM and enable hardware virtualization inside the VM. I created a new Centos 6.3 VM with hardware version 8. Make sure that the portgroup is set for promiscuous mode.

Shut down the VM and add the following lines to the vmx. The first 2 lines set the MMU to be hardware. The next 5 set the CPU flags and the last is an optimization to stop hangs in kvm. See [this article](http://communities.vmware.com/docs/DOC-8970) for more details.


```

monitor.virtual_mmu = "hardware"
monitor.virtual_exec = "hardware"
cpuid.1.ecx="----:----:----:----:----:----:--h-:----"
cpuid.80000001.ecx.amd="----:----:----:----:----:----:----:-h--"
cpuid.8000000a.eax.amd="hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh"
cpuid.8000000a.ebx.amd="hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh"
cpuid.8000000a.edx.amd="hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:hhhh"
vcpu.hotadd = FALSE

```


After making the changes to the VMX, we need to reload the VM on the host.


```

# vim-cmd vmsvc/getallvms |grep kvm1
252
# vim-cmd vmsvc/reload 252
# vim-cmd vmsvc/power.on 252

```


Once booted install kvm and libvirt.


```

# yum install -y kvm libvirt

```


Restart the libvirt service and verify that kvm module is loaded.


```

# service libvirtd restart
# lsmod |grep kvm
kvm_amd                41551  0
kvm                   314739  1 kvm_amd

```


Next I set up KVM to be accessible for a non root user (Optional)


```

# usermod -a -G kvm  jarret

```


To avoid a reboot, we can chown /dev/kvm. First let's see who it is owned by. (Udev will set this automatically on a reboot)


```

# ls -l /dev/kvm
crw-rw-rw-. 1 root root 10, 232 Jul 12 06:08 /dev/kvm
# chown root:kvm /dev/kvm
# chmod 0660 /dev/kvm
# ls -l /dev/kvm
crw-rw-rw-. 1 root kvm 10, 232 Jul 12 06:08 /dev/kvm

```


Check udev rules for this device.


```

# cat /etc/udev/rules.d/80-kvm.rules
KERNEL=="kvm", GROUP="kvm", MODE="0666"

```


Add a group for libvirt


```

# groupadd libvirt
# usermod -a -G libvirt jarret

```


Allow users of libvirt Access edit /etc/polkit-1/localauthority/50-local.d/50-libvirt-remote-access.pkla and add the following lines.


```

[libvirt Management Access]
# For allowing access to specific user only:
#Identity=unix-user:jarret
# For allowing access to a group (like this guide):
Identity=unix-group:libvirt
Action=org.libvirt.unix.manage
ResultAny=yes
ResultInactive=yes
ResultActive=yes

```


Restart libvirtd


```

# service libvirtd restart

```


Log out and back in and confirm the groups.


```

# id
uid=500(jarret) gid=500(jarret) groups=500(jarret),36(kvm),501(libvirt)

```


Confirm that we can connect using the jarret user


```

# virsh -c qemu:///system sysinfo |head -n 1


```


Set the permissions on the images folder.


```

# chown :libvirt /var/lib/libvirt/images/
# chmod g+rws /var/lib/libvirt/images/

```


If you want to use VNC (you do) and not have to X forward it over ssh, you can set up VNC to listen on 0.0.0.0 instead of 127.0.0.1. Edit /etc/libvirt/qemu.conf and uncomment the following line.


```

vnc_listen = "0.0.0.0"

```


Set up a bridged network. I have 2 adapters in this VM, so we will use eth1.

Create /etc/sysconfig/network-scripts/ifcfg-br0 and add the following.


```

DEVICE="br0"
TYPE=Bridge
DELAY=0
ONBOOT="yes"
BOOTPROTO=static
IPADDR=192.168.10.41
NETMASK=255.255.255.0
NETWORK=192.168.10.0
GATEWAY=192.168.10.1
DNS1=192.168.5.10
PEERDNS="yes"
NM_CONTROLLED=no

```


Edit /etc/sysconfig/network-scripts/ifcfg-eth1 to set up the bridge.


```

DEVICE=eth1
ONBOOT=yes
BRIDGE=br0
NM_CONTROLLED=no
HWADDR=00:50:56:B0:07:A2

```


Restart networking


```

# service network restart

```


Show the bridges.


```

# brctl show
bridge name     bridge id               STP enabled     interfaces
br0             8000.005056b007a2       no              eth1
virbr0          8000.5254003ed252       yes             virbr0-nic

```


Set up iptables to allow traffic to forward across the bridge


```

# iptables -I FORWARD -m physdev --physdev-is-bridged -j ACCEPT
# service iptables save
# service iptables restart

```


Check the iptables rules


```

# iptables -L
Chain INPUT (policy ACCEPT)
target     prot opt source               destination
ACCEPT     udp  --  anywhere             anywhere            udp dpt:domain
ACCEPT     tcp  --  anywhere             anywhere            tcp dpt:domain
ACCEPT     udp  --  anywhere             anywhere            udp dpt:bootps
ACCEPT     tcp  --  anywhere             anywhere            tcp dpt:bootps
ACCEPT     all  --  anywhere             anywhere            state RELATED,ESTABLISHED
ACCEPT     icmp --  anywhere             anywhere
ACCEPT     all  --  anywhere             anywhere
ACCEPT     tcp  --  anywhere             anywhere            state NEW tcp dpt:ssh
REJECT     all  --  anywhere             anywhere            reject-with icmp-host-prohibited

Chain FORWARD (policy ACCEPT)
target     prot opt source               destination
ACCEPT     all  --  anywhere             anywhere            PHYSDEV match --physdev-is-bridged
ACCEPT     all  --  anywhere             192.168.122.0/24    state RELATED,ESTABLISHED
ACCEPT     all  --  192.168.122.0/24     anywhere
ACCEPT     all  --  anywhere             anywhere
REJECT     all  --  anywhere             anywhere            reject-with icmp-port-unreachable
REJECT     all  --  anywhere             anywhere            reject-with icmp-port-unreachable
REJECT     all  --  anywhere             anywhere            reject-with icmp-host-prohibited

Chain OUTPUT (policy ACCEPT)
target     prot opt source               destination

```


The bridged networking is setup and kvm is installed and configured for a non-root user to use it.

Before doing anything else, let's reboot and confirm that all settings come up correctly. (This will also allow for the changes to qemu.conf to take effect)

Now that KVM is setup, let's install a VM. First, let's put an ISO for the install in the /var/lib/libvirt/images folder.


```

# scp CentOS-6.0-x86_64-minimal.iso jarret@kvm2:/var/lib/libvirt/images/

```


Since we set the setgid bit above, the group will stay libvirt unter this folder.


```

# ls -l /var/lib/libvirt/images/
total 302772
-rwxrwxr-x. 1 jarret libvirt 310032384 Jul 12 07:05 CentOS-6.0-x86_64-minimal.iso

```


Now we need a place to store VMs. If you already have some space, you can skip this. I added a second disk of 50G, formatted it with ext4 and mounted it as /vms. We want to get the permissions right again


```

# chmod 1775 /vms
# chown root:libvirt /vms
# chmod g+s /vms

```


There are many ways of installing a VM. You could use [virt-manager](http://virt-manager.org/) for a GUI, but let's do this on the command line using our non-root user. There are MANY ways to do this, but I prefer virt-install, so let's install it.


```

# yum install libvirt-python python-virtinst

```


Create a VM. Check [this post](http://acidborg.wordpress.com/2010/02/18/how-to-create-virtual-machines-using-kvm-kernel-based-virtual-machine/), [this post](http://www.cyberciti.biz/faq/kvm-virt-install-install-freebsd-centos-guest/) and [this post](http://www.techotopia.com/index.php/Installing_a_KVM_Guest_OS_from_the_Command-line_(virt-install)) for more details on creating a VM.


```

# virt-install --name=centos1 --arch=x86_64 --vcpus=1 --ram=512 --os-type=linux --os-variant=rhel6 --hvm --connect=qemu:///system --network bridge:br0 --cdrom=/var/lib/libvirt/images/CentOS-6.0-x86_64-minimal.iso --disk path=/vms/centos1.img,size=20 --accelerate --vnc --noautoconsole --keymap=es
Starting install...
Creating storage file centos1.img                                                                |  20 GB     00:00
Creating domain...                                                                               |    0 B     00:00
Domain installation still in progress. You can reconnect to
the console to complete the installation process.

```


Check on the status of the VM


```

# virsh -c qemu:///system list
 Id    Name                           State
----------------------------------------------------
 1     centos1                        running

```


Typing "-c qemu:///system" will get annoying, so I found [this post](http://berrange.com/posts/2011/10/28/using-uri-aliases-with-libvirt-0-9-7-forthcoming-release/) about adding some aliases (these are alot more powerful when remote) Edit ~/.libvirt/libvirt.conf and add the following lines.


```

uri_aliases = [
  "qemu=qemu:///system",
]

```


Now let's test it out.


```

# virsh -c qemu list

d    Name                           State
----------------------------------------------------
 1     centos1                        running

```


So we have this VM running, let's check for VNC connectivity.


```

Check what IP vnc is listening on
# netstat -an |egrep "590?"
tcp        0      0 0.0.0.0:5900                0.0.0.0:*                   LISTEN

```


See what display centos1 is on. (it will correlate with the port above. I.e :0 should run on 5900, :1 5901, etc)


```

# virsh -c qemu vncdisplay centos1
:0

```


NOTE: If you have firewall rules you will have to allow tcp connections on the port for each connection. Most people prefer to X forward over ssh to avoid these complications and security implications.


```

# iptables -A INPUT -m state --state NEW -m tcp -p tcp --dport 5900  -j ACCEPT

```


If you are interest below are some links for further reading.



	
  * http://acidborg.wordpress.com/2010/02/18/how-to-create-virtual-machines-using-kvm-kernel-based-virtual-machine/

	
  * https://help.ubuntu.com/community/KVM/Managing

	
  * http://www.howtoforge.com/how-to-install-kvm-and-libvirt-on-centos-6.2-with-bridged-networking

	
  * http://communities.vmware.com/docs/DOC-8970

	
  * http://www.cyberciti.biz/faq/rhel-linux-kvm-virtualization-bridged-networking-with-libvirt/

	
  * http://wiki.centos.org/HowTos/KVM

	
  * http://docs.redhat.com/docs/en-US/Red_Hat_Enterprise_Linux/5/html/Virtualization/sect-Virtualization-Network_Configuration-Bridged_networking_with_libvirt.html


