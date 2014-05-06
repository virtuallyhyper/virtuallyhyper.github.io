---
author: karim
comments: true
date: 2013-01-07 14:06:24+00:00
layout: post
slug: vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance
title: 'VCAP5-DCA Objective 6.2 – Troubleshoot CPU and Memory Performance '
wordpress_id: 5281
categories:
- Certifications
- VCAP5-DCA
- VMware
following_users:
- jarret
- kelatov
tags:
- VCAP5-DCA
---

### Identify resxtop/esxtop metrics related to memory and CPU



From "[vSphere Monitoring and Performance](http://pubs.vmware.com/vsphere-50/topic/com.vmware.ICbase/PDF/vsphere-esxi-vcenter-server-501-monitoring-performance-guide.pdf)", here are the CPU counters:

[![esxtop_cpu_counters_p1](http://virtuallyhyper.com/wp-content/uploads/2012/12/esxtop_cpu_counters_p1.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/esxtop_cpu_counters_p1/)

Here are more of CPU counters:

[![esxtop_cpu_p2](http://virtuallyhyper.com/wp-content/uploads/2012/12/esxtop_cpu_p2.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/esxtop_cpu_p2/)

From the same pdf, here are the memory counters:

[![esxtop_mem_p1](http://virtuallyhyper.com/wp-content/uploads/2012/12/esxtop_mem_p1.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/esxtop_mem_p1/)

Here are more counters from the same page:

[![esxtop_mem_p2](http://virtuallyhyper.com/wp-content/uploads/2012/12/esxtop_mem_p2.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/esxtop_mem_p2/)

And here is the last page for the memory counters:

[![esxtop_mem_p3](http://virtuallyhyper.com/wp-content/uploads/2012/12/esxtop_mem_p3.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/esxtop_mem_p3/)



### Identify vCenter Server Performance Chart metrics related to memory and CPU


From [this](http://communities.vmware.com/docs/DOC-5600) Communities page:

[![perf_charts_cpu_counters](http://virtuallyhyper.com/wp-content/uploads/2012/12/perf_charts_cpu_counters.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/perf_charts_cpu_counters/)

and here are the memory counters:

[![perf_charts_mem_counters](http://virtuallyhyper.com/wp-content/uploads/2012/12/perf_charts_mem_counters.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/perf_charts_mem_counters/)

You can also check out the full list [here](http://pubs.vmware.com/vsphere-50/index.jsp?topic=/com.vmware.wssdk.apiref.doc_50/right-pane.html).



### Troubleshoot ESXi host and Virtual Machine CPU performance issues using appropriate metrics


From [this](http://www.vmworld.net/wp-content/uploads/2012/05/Esxtop_Troubleshooting_eng.pdf) pdf:

[![esxtop_cpu_performance](http://virtuallyhyper.com/wp-content/uploads/2012/12/esxtop_cpu_performance.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/esxtop_cpu_performance/)

Here is a good summary:



> 
%RDY: Percentage of time a VM was waiting to be scheduled. Possible reasons: too many vCPUs, too many vSMP VMs or a CPU limit setting (Trouble when >10)
%CSTP: Percentage of time a ready to run VM has spent in co-descheduling state (Trouble when > 3 to resolve, decrease number of vCPUs on the VM)
$MLMTD: Percentage of time a ready to vCPU was no schedules because of a CPU limit setting (Trouble when > 1, to resolve; remove the CPU limit)
$SWPWT: How Long a VM has to wait for swapped pages read from disk. (Trouble when > 5, possible reason; memory overcommitment




Also this is from a 4.x document but it applies to 5.0 as well. Here is the document: "[Performance Troubleshooting for VMware vSphere 4.1](http://communities.vmware.com/servlet/JiveServlet/previewBody/14905-102-2-17952/vsphere41-performance-troubleshooting.pdf)". From that document:

[![check_for_host_cpu_saturation](http://virtuallyhyper.com/wp-content/uploads/2012/12/check_for_host_cpu_saturation.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/check_for_host_cpu_saturation/)

Here is a similar diagram for Guest CPU saturation:

[![check_for_guest_cpu_saturation](http://virtuallyhyper.com/wp-content/uploads/2012/12/check_for_guest_cpu_saturation.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/check_for_guest_cpu_saturation/)



### Troubleshoot ESXi host and Virtual Machine memory performance issues using appropriate metrics


From [this](http://www.vmworld.net/wp-content/uploads/2012/05/Esxtop_Troubleshooting_eng.pdf) pdf:

[![esxtop_memory_issues](http://virtuallyhyper.com/wp-content/uploads/2012/12/esxtop_memory_issues.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/esxtop_memory_issues/)

Here is a good summary:



> 
MCTLSZ: Amount of guest physical memory (MB) the ESXi Host is reclaiming by balloon driver. (Trouble when > 1)
ZIP/s: Values larger than 0 indicates that the host is actively compressing memory (Trouble when > 1)
UNZIP/s: Values larger than 0 indicate that the host is accessing compressed memory (Reason for this behavior is memory overcommitment)
SWCUR: Memory (in MB) that has been swapped by VMkernel (Possible cause is memory overcommitment, Trouble when > 1)
CACHEUSD: Memory (in MB) compressed by ESXi Host (Trouble when > 1)
SWR/s SWW/s: Rate at which the ESXi host is writing to or reading from swapped memory (Trouble when > 1)




And from "[Performance Troubleshooting for VMware vSphere 4.1](http://communities.vmware.com/servlet/JiveServlet/previewBody/14905-102-2-17952/vsphere41-performance-troubleshooting.pdf)":

[![check_for_vm_swapping](http://virtuallyhyper.com/wp-content/uploads/2012/12/check_for_vm_swapping.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/check_for_vm_swapping/)

and here is another flow chart for VM memory issues:

[![check_vm_fo_compression](http://virtuallyhyper.com/wp-content/uploads/2012/12/check_vm_fo_compression.png)](http://virtuallyhyper.com/2013/01/vcap5-dca-objective-6-2-troubleshoot-cpu-and-memory-performance/check_vm_fo_compression/)



### Use Hot-Add functionality to resolve identified Virtual Machine CPU and memory performance issues


If you discover then the VM needs more resources, then follow the instruction laid out in [VCAP5-DCA Objective 3.2](http://virtuallyhyper.com/2012/11/vcap5-dca-objective-3-2-optimize-virtual-machine-resources/) to hot add memory or CPU

