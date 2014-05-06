---
author: karim
comments: true
date: 2012-04-09 23:48:44+00:00
layout: post
slug: error-updating-chromium-on-freebsd-8-2-p6
title: Error updating Chromium on FreeBSD 8.2-p6
wordpress_id: 576
categories:
- OS
following_users:
- kelatov
tags:
- chromium
- freebsd
- gcc
- ports
- portupgrade
---

Recently I ran into an issue with updating chromium on my FreeBSD machine. I was running the following version:

    
    moxz:~>sudo cat /var/db/freebsd-update/tag | awk -F '|' '{print $3,"patch-"$4}'
    8.2-RELEASE patch-6
    moxz:>uname -a
    FreeBSD moxz 8.2-RELEASE-p3 FreeBSD 8.2-RELEASE-p3 #0: Tue Sep 27 18:45:57 UTC 2011 root@amd64-builder.daemonology.net:/usr/obj/usr/src/sys/GENERIC amd64


Whenever I would try to update chromium I would run into the following error message:


```

===> chromium-18.0.1025.142 does not compile with base gcc.
*** Error code 1

Stop in /usr/ports/www/chromium.

```


To get around the issue I had to install a newer version of gcc, by default FreeBSD comes with 4.2:


```

moxz:~>/usr/bin/gcc -v
Using built-in specs.
Target: amd64-undermydesk-freebsd
Configured with: FreeBSD/amd64 system compiler
Thread model: posix
gcc version 4.2.1 20070831 patched [FreeBSD]

```


Apparently to compile chromium you need 4.6 or above, so I went ahead and installed gcc4.6


```

moxz:~>cd /usr/ports/lang/gcc46/
moxz:/usr/ports/lang/gcc46>sudo make install clean

```


After that compiled (which was about 15 minutes), I ensured that was installed:


```

moxz:~>pkg_info | grep gcc
gcc-4.6.4.20120330  GNU Compiler Collection 4.6

```


I then updated the Makefile for the chromium port


```

moxz:/usr/ports/www/chromium>sudo make config

```


and made sure gcc4.6 is checked, it looks like something this:
[![](http://virtuallyhyper.com/wp-content/uploads/2012/04/make_config_chromium_fb8_2.png)](http://virtuallyhyper.com/wp-content/uploads/2012/04/make_config_chromium_fb8_2.png)

I was then able to run the following to update chromium with the following command:


```

moxz:~>sudo portupgrade -a

```


If you want to you can setup gcc 4.6 to be your default compiler, you can do that by following the instructions at [Using newer version of GCC and binutils with the FreeBSD Ports Collection](http://www.freebsd.org/doc/en_US.ISO8859-1/articles/custom-gcc/article.html)

You would then then recompile all the ports just in case. This can be done by running the following:


```

moxz:~>sudo portsnap fetch update

```


That would update all of your ports, and then run:


```

moxz:~>sudo portupgrade -af

```


And that would forcefully recompile all of your packages.
