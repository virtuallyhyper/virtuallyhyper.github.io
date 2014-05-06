---
author: joe
comments: true
date: 2012-09-27 19:30:31+00:00
layout: post
slug: selinux-and-httpd
title: Wordpress - Error establishing a database connection, but able to connect via
  command line - SELinux and httpd
wordpress_id: 3929
categories:
- OS
following_users:
- jarret
- joechan
- kelatov
tags:
- drupal
- httpd
- linux
- selinux
- wordpress
---

I was installing Drupal and Wordpress the other day and during the initial setup I came across the following messages:


```
In order for Drupal to work, and to continue with the installation process, you must resolve all issues reported below. For more help with configuring your database server, see the installation handbook. If you are unsure what any of this means you should probably contact your hosting provider.

Failed to connect to your database server. The server reports the following message: SQLSTATE[HY000] [2003] Can't connect to MySQL server on 'testdb.joechan.us' (13).

Is the database server running?
Does the database exist, and have you entered the correct database name?
Have you entered the correct username and password?
Have you entered the correct database hostname?

```


You will see something similar in Wordpress:

```

Error establishing a database connection
This either means that the username and password information in your wp-config.php file is incorrect or we can't contact the database server at testdb.joechan.us. This could mean your host's database server is down.

Are you sure you have the correct username and password?
Are you sure that you have typed the correct hostname?
Are you sure that the database server is running?
If you're unsure what these terms mean you should probably contact your host. If you still need help you can always visit the WordPress Support Forums.

```


So I try logging into the MySQL server directly:


```

798ip-10-252-128-9$ mysql -u root -h testdb.joechan.us -p
Enter password:
Welcome to the MySQL monitor.  Commands end with ; or \g.
Your MySQL connection id is 62
Server version: 5.5.27-log Source distribution

Copyright (c) 2000, 2011, Oracle and/or its affiliates. All rights reserved.

Oracle is a registered trademark of Oracle Corporation and/or its
affiliates. Other names may be trademarks of their respective
owners.

Type 'help;' or '\h' for help. Type '\c' to clear the current input statement.

mysql>

```


Strange... it succeeds.

A bit of Googling led me to this [StackOverflow page](http://stackoverflow.com/questions/4078205/php-cant-connect-to-mysql-with-error-13-but-command-line-can).

It turns out there is a special SELinux boolean that specifically blocks Apache (httpd) from talking to databases (more info about the different SELinux booleans [here](http://wiki.centos.org/TipsAndTricks/SelinuxBooleans)):


    
    
    httpd_can_network_connect_db (HTTPD Service)
    Allow HTTPD scripts and modules to network connect to databases.
    



So I just disabled SELinux per the instructions [here](http://www.centos.org/docs/5/html/5.2/Deployment_Guide/sec-sel-enable-disable.html).

To enable temporarily until you reboot, run the following as root:

```

# echo 0 >/selinux/enforce

```


If you want to leave SELinux on, but allow an exception for httpd to talk to your database, you can run the following:

```

setsebool -P httpd_can_network_connect_db on

```

