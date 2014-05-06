---
author: jarret
comments: true
date: 2012-09-01 15:25:24+00:00
layout: post
slug: pxe-booting-a-diskless-ubuntu-server
title: PXE Booting a Diskless Ubuntu Server
wordpress_id: 3104
categories:
- Home Lab
- OS
following_users:
- jarret
- kelatov
tags:
- dhcpd
- pxe
- rdiff-backup
- tftp
- xbmc
---

Today I came home to find that my [XBMC](http://xbmc.org) frontend would not boot. It turns out that the hard drive on this machine had died. Due to my [recent luck with hard drives](http://virtuallyhyper.com/2012/08/restoring-from-backup-in-my-home-lab/) I was not surprised. 

I take nightly backups of all my servers, so I could restore from backup. Since this was a client device to my media server, I decided to scrap the hard drive and just make this a PXE boot client. To set this up I needed to set up a DHCP server, TFTP server, and an NFS server. My media server already had all of this set up, but I went through and reconfigured all of the services.

The first thing I did was copy over the files from last night's backup. For backups I use [rdiff-backup](http://www.nongnu.org/rdiff-backup/) which is old but very stable. It uses libsrync to sync the files and compresses the previous changes. 

First I [restored the current backup](http://www.nongnu.org/rdiff-backup/examples.html) to a new NFS share. 


```

# rdiff-backup --restore-as-of now /data/backups/rdiff/frontend/. /data/pxe/frontend/.

```


Once the files were restored, I set up the NFS server. Since this is a Solaris server running ZFS I use the [command below](https://blogs.oracle.com/lubos/entry/how_to_share_zfs_over). In Linux, you would use [exportfs](http://www.centos.org/docs/5/html/Deployment_Guide-en-US/s1-nfs-server-export.html).


```

# zfs set sharenfs=root'@'10.0.1.80,rw='@'10.0.1.80 data/pxe/frontend
# zfs get sharenfs data/pxe/frontend
NAME               PROPERTY  VALUE                              SOURCE
data/pxe/frontend  sharenfs  rw='@'10.0.1.80,root='@'10.0.1.80      local

```


Next I need to set up a DHCP server. On my Nexenta server I use [isc-dhcp-server](http://docs.oracle.com/cd/E23824_01/html/821-1453/dhcp-overview-50.html). In Linux you could use [dhcp3-server](http://www.howtoforge.com/dhcp_server_linux_debian_sarge). Once it is installed, I needed to edit the [/etc/isc-dhcp/dhcpd.conf](http://linux.die.net/man/5/dhcpd.conf) file.

I added a host section for this host.


```

host frontend {
  hardware ethernet 00:01:2E:27:00:5E;
  fixed-address 10.0.1.80;
  option subnet-mask 255.255.255.0;
  option broadcast-address 10.0.1.255;
  option routers 10.0.1.1;
  filename "/pxelinux.0";
}

```


If you had multiple servers, you could use the pxeclient configuration to assign IPs only to clients that PXE boot. This is the configuration I use to PXE install different servers.


```

class "pxe-boot" {
match if substring ( option vendor-class-identifier, 0, 9) = "PXEClient";
}
shared-network Internal {
subnet 10.0.1.0  netmask 255.255.255.0 {
        pool {
                allow members of "pxe-boot";
                range 10.0.1.250 10.0.1.254;
                option subnet-mask 255.255.255.0;
                option broadcast-address 10.0.1.255;
                option routers 10.0.1.1;
                next-server 10.0.1.83;
                filename "pxelinux.0";
         }
}
}

```


After making the changes to dhcpd.conf we need to start the service. 


```

# svcadm enable isc-dhcp-server

```


Now it is on to setting up the TFTP server. I use [TFTP-HPA](http://chschneider.eu/linux/server/tftpd-hpa.shtml). The first thing to do is make all of the directories that we would need for this configuration.


```

# mkdir -p /var/lib/tftpboot/{pxelinux.cfg,frontend}

```


So we have dhcpd installed and set up, tftp installed and setup as well, and lastly we had all the necessary files for our server. We now need to complete it by regenerating the initrd and configuring the boot configuration. Since my frontend was down, I cannot regenerate the initrd on it. I had a couple options in this situation. The first option is to boot a live CD, install the kernel and regenerate the initrd. The second option is to boot the client by PXE using the iso with [memdisk](http://www.syslinux.org/wiki/index.php/MEMDISK#ISO_images).

I went with a third option this time. In my ESX lab I deployed a new server with the same version of Ubuntu 11.10 x32. I installed the same kernel as the front end was using.


```

# apt-get install linux-image-3.0.0-17-generic

```


Next I need to regenerate the initrd. Before we generate it we want to change it to boot NFS instead of the local disk. Let's change the default boot configuration.


```

$ sed -i 's/BOOT=local/BOOT=nfs/'  /etc/initramfs-tools/initramfs.conf

```


Now we can [regenerate the initrd](http://manpages.ubuntu.com/manpages/intrepid/man8/initramfs-tools.8.html). 


```

$ /usr/sbin/mkinitramfs -o /boot/initrd.img-3.0.0-17-generic-netboot

```


Now copy the initrd and vmlinuz over to the tftpboot directory.


```

$ scp /boot/initrd.img-3.0.0-17-generic-netboot /boot/vmlinuz-3.0.0-17-generic fileserver2:/var/lib/tftpboot/frontend/

```


Since Solaris does not have pxelinux.0, we can pull it from the temporary ubuntu host and copy it over the tftpboot folder. 


```

$ apt-get install syslinux
$ scp /usr/lib/syslinux/pxelinux.0 fileserver2:/var/lib/tftpboot/

```


Now we can go back to the server and finish up the PXE configuration. Since we only want this client to run this configuration, we can create the configuration file with the same name as the MAC. Open up /var/lib/tftpboot/pxelinux.cfg/01-00-01-2e-27-00-5e and put in the contents below. More information in [this article](https://help.ubuntu.com/community/DisklessUbuntuHowto#Static_IP).


```

LABEL linux
KERNEL frontend/vmlinuz-3.0.0-17-generic
APPEND root=/dev/nfs initrd=frontend/initrd.img-3.0.0-17-generic nfsroot=10.0.1.83:/data/pxe/frontend ip=dhcp rw 

```


When the client boots the dhcp server tells the client to use pxelinux.0, which defaults to using the files in /pxelinux.0 in [this order](http://www.syslinux.org/wiki/index.php/PXELINUX#How_do_I_Configure_PXELINUX.3F). The configuration file will tell the server to boot using our modified initrd using the NFS mount for /.

The problem is that our original files from the NFS mount are still configured for a physical machine. We need to modify the fstab to remove the physical devices. Open up the /data/pxe/frontend/etc/fstab file and remove the physical device line and add a line for the NFS root. My fstab looks like this:


```

proc            /proc           proc    nodev,noexec,nosuid 0       0
/dev/nfs	/		nfs	defaults	1	1
none            /tmp            tmpfs   defaults        0       0
none            /var/run        tmpfs   defaults        0       0
none            /var/lock       tmpfs   defaults        0       0
none            /var/tmp        tmpfs   defaults        0       0

```


We also need to change the network configuration so that the client does not try to reconfigure the network and drop connection to the root filesystem. Open up the /data/pxe/frontend/etc/network/interfaces file and change the configuration for eth0.


```

auto lo
iface lo inet loopback

iface eth0 inet manual

```



Thats it. It is time to boot the machine. The client was then booted and my front end came up with out any problems. 
