---
title: 'VMware Workstation 9 on Fedora 17 Causes Kernel Error with Message "EIP is at HostIF_SafeRDMSR+0x11/0x20 [vmmon]"'
author: Karim Elatov
layout: post
permalink: /2012/09/vmware-workstation-9-on-fedora-17-cause-kernel-panic-with-eip-is-at-hostif_saferdmsr0x110x20-vmmon/
dsq_thread_id:
  - 1404673417
categories:
  - Home Lab
  - OS
  - VMware
tags:
  - fedora 17
  - vmware workstation
---
I recently updated my VMware Workstation from version 8 to version 9. The install went well and the compile of the modules didn't fail either. But as soon as I would try to power on a VM, I would get this message:


	Aug 23 10:32:27 klaptop kernel: [ 92.964074] general protection fault: 0000 [#1] SMP
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] Modules linked in: vmnet(O) ppdev parport_pc parport fuse vsock(O) vmci(O) vmmon(O) nf_conntrack_ipv4 nf_defrag_ipv4 xt_state nf_conntrack binfmt_misc snd_hda_codec_hdmi snd_hda_codec_idt snd_hda_intel arc4 snd_hda_codec coretemp uvcvideo videobuf2_vmalloc videobuf2_memops videobuf2_core iwlwifi snd_hwdep kvm_intel snd_pcm videodev udlfb kvm media snd_page_alloc mac80211 snd_timer snd cfg80211 i2c_i801 e1000e lpc_ich mfd_core microcode mei rfkill dell_wmi sparse_keymap dell_laptop dcdbas soundcore nfsd nfs_acl auth_rpcgss lockd sunrpc uinput sdhci_pci sdhci mmc_core firewire_ohci firewire_core crc_itu_t ata_generic pata_acpi wmi i915 video i2c_algo_bit drm_kms_helper drm i2c_core
	Aug 23 10:32:27 klaptop kernel: [ 92.965006]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] Pid: 2761, comm: vmware-vmx Tainted: G O 3.5.2-1.fc17.i686.PAE #1 Dell Inc. Latitude E4300 /0D201R
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] EIP: 0060:[] EFLAGS: 00213246 CPU: 0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] EIP is at HostIF_SafeRDMSR+0x11/0x20 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] EAX: 00000000 EBX: 00000001 ECX: 0000048c EDX: 00000000
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] ESI: f0278450 EDI: f0278440 EBP: eed45c6c ESP: eed45c68
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] DS: 007b ES: 007b FS: 00d8 GS: 00e0 SS: 0068
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] CR0: 80050033 CR2: 086ca9b0 CR3: 3679a000 CR4: 000407f0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] DR0: 00000000 DR1: 00000000 DR2: 00000000 DR3: 00000000
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] DR6: ffff0ff0 DR7: 00000400
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] Process vmware-vmx (pid: 2761, ti=eed44000 task=ef4d25b0 task.ti=eed44000)
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] Stack:
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] 00000000 eed45c8c f91f45ce f91f2608 f0278440 0000048c f91f4560 eed45ca4
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] 09f92468 eed45c9c f91f3529 f0278440 f0278440 eed45cb0 f91f6375 00000001
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] f0278440 ffffffea eed45f14 f91f107f f2a4ca80 f39c1d10 f5651640 f7004600
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] Call Trace:
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] Vmx86GetMSR+0x6e/0xc0 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? HostIF_AllocKernelMem+0x18/0x50 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? Vmx86GetUnavailPerfCtrsOnCPU+0x1b0/0x1b0 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] HostIF_CallOnEachCPU+0x19/0x40 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] Vmx86_GetAllMSRs+0x25/0x50 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] LinuxDriver_Ioctl+0x32f/0xd00 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? __mem_cgroup_commit_charge+0x144/0x2b0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? radix_tree_lookup_slot+0xd/0x10
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? find_get_page+0x1e/0xe0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? find_lock_page+0x35/0x90
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? shmem_getpage_gfp+0x8d/0x6f0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? kmap_atomic_prot+0xfe/0x160
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? unlock_page+0x4e/0x70
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? shmem_write_end+0x56/0xe0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? iov_iter_copy_from_user_atomic+0x55/0x80
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? shmem_write_begin+0x40/0x50
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? generic_file_buffered_write+0x17d/0x220
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? radix_tree_lookup_slot+0xd/0x10
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? find_get_page+0x1e/0xe0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? filemap_fault+0xef/0x3b0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? mem_cgroup_update_page_stat+0x1a/0x50
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? unlock_page+0x4e/0x70
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? __do_fault+0x420/0x5a0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? __lock_page_or_retry+0xa0/0xa0
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? handle_pte_fault+0x90/0xb00
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? handle_mm_fault+0x1dd/0x280
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? LinuxDriver_Ioctl+0xd00/0xd00 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] LinuxDriver_UnlockedIoctl+0x19/0x20 [vmmon]
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] do_vfs_ioctl+0x7a/0x580
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? vmalloc_fault+0x181/0x181
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? do_page_fault+0x1b7/0x450
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? vfs_write+0xe8/0x160
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? wait_on_retry_sync_kiocb+0x50/0x50
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] sys_ioctl+0x68/0x80
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] sysenter_do_call+0x12/0x28
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] [] ? __schedule+0x730/0x770
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] Code: 77 ef 89 d8 e8 51 8c 34 c7 90 eb a5 8b 43 08 e8 d6 d4 24 c7 eb d3 8d 74 26 00 55 89 e5 56 66 66 66 66 90 89 c1 31 c0 89 d6 89 c2 32 31 c9 89 06 89 c8 89 56 04 5e 5d c3 90 55 89 e5 57 56 53
	Aug 23 10:32:27 klaptop kernel: [ 92.965006] EIP: [] HostIF_SafeRDMSR+0x11/0x20 [vmmon] SS:ESP 0068:eed45c68
	Aug 23 10:32:27 klaptop kernel: [ 93.259084] ---[ end trace 41e66149eea61e4b ]---


Here are the version of my OS and Workstation:



	$ uname -a
	Linux klaptop.vmware.com 3.5.2-3.fc17.i686.PAE #1 SMP Tue Aug 21 19:27:17 UTC 2012 i686 i686 i386 GNU/Linux

	$ lsb_release -a
	LSB Version: :core-4.1-ia32:core-4.1-noarch:cxx-4.1-ia32:cxx-4.1-noarch:desktop-4.1-ia32:desktop-4.1-noarch:languages-4.1-ia32:languages-4.1-noarch:printing-4.1-ia32:printing-4.1-noarch
	Distributor ID: Fedora
	Description: Fedora release 17 (Beefy Miracle)
	Release: 17
	Codename: Beefy Miracle

	$ vmware-installer -l
	Product Name Product Version
	==================== ====================
	vmware-workstation 9.0.0.812388


While searching on the internet, I ran into VMware Community Thread [400616](http://communities.vmware.com/thread/400616), from that thread:

> I have figured out one of reasons.
> If you crash Linux kernel (3.5 or later) with a HostIF_SafeRDMSR call, this is caused by exception table format. Linux 3.5 (more specifically, Linux kernel commit 706276543b699d80f546e45f8b12574e7b18d952) have changed the format of exception table.
>
> VMware's vmmon.ko (vmware-vmx/lib/modules/source/vmmon-only/linux/hostif.c) generates exception table using the way described in kernel documentation ($KERNEL/Documentation/x86/exception-tables.txt) but this is quite old and completely useless beginning with Linux kernel 3.5.
>
> Fortunately, there is _ASM_EXTABLE macro (in $KERNEL/arch/x86/include/asm/asm.h) to generate portable exception table. So I have created a patch to make exception table portable and make vmmon.ko to support Linux 3.5. I hope this will help.

From that thread I found VMware Community [patch](http://communities.vmware.com/message/2103048) for this issue. I downloaded the patch:


	$ ls *patch*
	vmware9_kernel35_patch.tar.bz2


I then extracted the patch:


	$ tar xjfv vmware9_kernel35_patch.tar.bz2
	vmware9_kernel3.5_patch/
	vmware9_kernel3.5_patch/patch-modules_3.5.0.sh
	vmware9_kernel3.5_patch/vmware3.5.patch


I then went inside the patch directory and applied the patch:


	$ cd vmware9_kernel3.5_patch/
	$ sudo ./patch-modules_3.5.0.sh
	patching file vmmon-only/linux/hostif.c
	Stopping vmware (via systemctl):
	Using 2.6.x kernel build system.
	make: Entering directory '/tmp/modconfig-j0gI7d/vmmon-only'
	/usr/bin/make -C /lib/modules/3.5.2-3.fc17.i686.PAE/build/include/.. SUBDIRS=$PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= modules
	make[1]: Entering directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/linux/driver.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/linux/hostif.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/linux/driverLog.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/common/memtrack.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/common/apic.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/common/hashFunc.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/common/vmx86.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/common/cpuid.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/common/task.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/common/comport.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/common/phystrack.o
	CC [M] /tmp/modconfig-j0gI7d/vmmon-only/vmcore/moduleloop.o
	LD [M] /tmp/modconfig-j0gI7d/vmmon-only/vmmon.o
	Building modules, stage 2.
	MODPOST 1 modules
	CC /tmp/modconfig-j0gI7d/vmmon-only/vmmon.mod.o
	LD [M] /tmp/modconfig-j0gI7d/vmmon-only/vmmon.ko
	make[1]: Leaving directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	/usr/bin/make -C $PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= postbuild
	make[1]: Entering directory '/tmp/modconfig-j0gI7d/vmmon-only'
	make[1]: 'postbuild' is up to date.
	make[1]: Leaving directory '/tmp/modconfig-j0gI7d/vmmon-only'
	cp -f vmmon.ko ./../vmmon.o
	make: Leaving directory '/tmp/modconfig-j0gI7d/vmmon-only'
	Using 2.6.x kernel build system.
	make: Entering directory '/tmp/modconfig-j0gI7d/vmnet-only'
	/usr/bin/make -C /lib/modules/3.5.2-3.fc17.i686.PAE/build/include/.. SUBDIRS=$PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= modules
	make[1]: Entering directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/hub.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/driver.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/userif.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/netif.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/bridge.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/filter.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/procfs.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/smac_compat.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/smac.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/vnetEvent.o
	CC [M] /tmp/modconfig-j0gI7d/vmnet-only/vnetUserListener.o
	LD [M] /tmp/modconfig-j0gI7d/vmnet-only/vmnet.o
	Building modules, stage 2.
	MODPOST 1 modules
	CC /tmp/modconfig-j0gI7d/vmnet-only/vmnet.mod.o
	LD [M] /tmp/modconfig-j0gI7d/vmnet-only/vmnet.ko
	make[1]: Leaving directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	/usr/bin/make -C $PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= postbuild
	make[1]: Entering directory '/tmp/modconfig-j0gI7d/vmnet-only'
	make[1]: 'postbuild' is up to date.
	make[1]: Leaving directory '/tmp/modconfig-j0gI7d/vmnet-only'
	cp -f vmnet.ko ./../vmnet.o
	make: Leaving directory '/tmp/modconfig-j0gI7d/vmnet-only'
	Using 2.6.x kernel build system.
	make: Entering directory '/tmp/modconfig-j0gI7d/vmblock-only'
	/usr/bin/make -C /lib/modules/3.5.2-3.fc17.i686.PAE/build/include/.. SUBDIRS=$PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= modules
	make[1]: Entering directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/filesystem.o
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/stubs.o
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/file.o
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/block.o
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/super.o
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/module.o
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/control.o
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/inode.o
	CC [M] /tmp/modconfig-j0gI7d/vmblock-only/linux/dentry.o
	LD [M] /tmp/modconfig-j0gI7d/vmblock-only/vmblock.o
	Building modules, stage 2.
	MODPOST 1 modules
	CC /tmp/modconfig-j0gI7d/vmblock-only/vmblock.mod.o
	LD [M] /tmp/modconfig-j0gI7d/vmblock-only/vmblock.ko
	make[1]: Leaving directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	/usr/bin/make -C $PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= postbuild
	make[1]: Entering directory '/tmp/modconfig-j0gI7d/vmblock-only'
	make[1]: 'postbuild' is up to date.
	make[1]: Leaving directory '/tmp/modconfig-j0gI7d/vmblock-only'
	cp -f vmblock.ko ./../vmblock.o
	make: Leaving directory '/tmp/modconfig-j0gI7d/vmblock-only'
	Using 2.6.x kernel build system.
	make: Entering directory '/tmp/modconfig-j0gI7d/vmci-only'
	/usr/bin/make -C /lib/modules/3.5.2-3.fc17.i686.PAE/build/include/.. SUBDIRS=$PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= modules
	make[1]: Entering directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/linux/driver.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/linux/vmciKernelIf.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciDatagram.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciDriver.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciResource.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciRoute.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciContext.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciHashtable.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciEvent.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciQueuePair.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciQPair.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/common/vmciDoorbell.o
	CC [M] /tmp/modconfig-j0gI7d/vmci-only/driverLog.o
	LD [M] /tmp/modconfig-j0gI7d/vmci-only/vmci.o
	Building modules, stage 2.
	MODPOST 1 modules
	CC /tmp/modconfig-j0gI7d/vmci-only/vmci.mod.o
	LD [M] /tmp/modconfig-j0gI7d/vmci-only/vmci.ko
	make[1]: Leaving directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	/usr/bin/make -C $PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= postbuild
	make[1]: Entering directory '/tmp/modconfig-j0gI7d/vmci-only'
	make[1]: 'postbuild' is up to date.
	make[1]: Leaving directory '/tmp/modconfig-j0gI7d/vmci-only'
	cp -f vmci.ko ./../vmci.o
	make: Leaving directory '/tmp/modconfig-j0gI7d/vmci-only'
	Using 2.6.x kernel build system.
	make: Entering directory '/tmp/modconfig-j0gI7d/vsock-only'
	/usr/bin/make -C /lib/modules/3.5.2-3.fc17.i686.PAE/build/include/.. SUBDIRS=$PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= modules
	make[1]: Entering directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	CC [M] /tmp/modconfig-j0gI7d/vsock-only/linux/af_vsock.o
	CC [M] /tmp/modconfig-j0gI7d/vsock-only/linux/vsockAddr.o
	CC [M] /tmp/modconfig-j0gI7d/vsock-only/linux/notifyQState.o
	CC [M] /tmp/modconfig-j0gI7d/vsock-only/linux/util.o
	CC [M] /tmp/modconfig-j0gI7d/vsock-only/linux/stats.o
	CC [M] /tmp/modconfig-j0gI7d/vsock-only/linux/notify.o
	CC [M] /tmp/modconfig-j0gI7d/vsock-only/driverLog.o
	LD [M] /tmp/modconfig-j0gI7d/vsock-only/vsock.o
	Building modules, stage 2.
	MODPOST 1 modules
	CC /tmp/modconfig-j0gI7d/vsock-only/vsock.mod.o
	LD [M] /tmp/modconfig-j0gI7d/vsock-only/vsock.ko
	make[1]: Leaving directory '/usr/src/kernels/3.5.2-3.fc17.i686.PAE'
	/usr/bin/make -C $PWD SRCROOT=$PWD/. \
	MODULEBUILDDIR= postbuild
	make[1]: Entering directory '/tmp/modconfig-j0gI7d/vsock-only'
	make[1]: 'postbuild' is up to date.
	make[1]: Leaving directory '/tmp/modconfig-j0gI7d/vsock-only'
	cp -f vsock.ko ./../vsock.o
	make: Leaving directory '/tmp/modconfig-j0gI7d/vsock-only'
	Starting vmware (via systemctl):

	All done, you can now run VMWare WorkStation.
	Modules sources backup can be found in the '/usr/lib/vmware/modules/source-workstation9.0.0-2012-09-04-18:51:38-backup' directory


The shell script patched the "vmmon-only/linux/hostif.c" file and then recompiled the VMware Workstation kernel modules. After the modules re-compiled, I was successfully able to power on my VMs.

