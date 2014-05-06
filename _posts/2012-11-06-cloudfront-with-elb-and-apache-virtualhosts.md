---
author: joe
comments: true
date: 2012-11-06 01:19:19+00:00
layout: post
slug: cloudfront-with-elb-and-apache-virtualhosts
title: CloudFront with ELB and Apache VirtualHosts
wordpress_id: 4655
categories:
- AWS
following_users:
- jarret
- joechan
tags:
- apache
- aws
- cloudfront
- ec2
- elb
- httpd
- vhosts
- virtualhosts
- web
- webserver
---

I was recently trying to set up multiple websites `joesite.joechan.us` and `evesite.joechan.us` with Apache VirtualHosts on a single EC2 instance behind an Elastic Load Balancer (ELB), but I also wanted that content to be cached by CloudFront similar to this diagram below.





![Amazon CloudFront for Dynamic Websites](http://media.amazonwebservices.com/blog/cloudfront_dynamic_web_sites_full_1.jpg)





In setting this up, I ran into an issue where the VirtualHosts weren't working as expected. I would always get sent to the same site, regardless of which subdomain I was pointing to. Going to evesite.joechan.us kept pointing me to the contents of joesite.joechan.us.





## My old configuration





In my old configuration, I had 2 subdomains, pointing to 2 CF distributions, with each CF distribution pointing to a single ELB.







  * Old DNS records




    

```
    joesite.joechan.us. 300 IN CNAME d1byny1djxkk2x.cloudfront.net.
    evesite.joechan.us. 300 IN CNAME d2ey6g4539mma3.cloudfront.net.
    
```




  * Old CF Distribution settings




    

```
    Distribution Domain Name: d1byny1djxkk2x.cloudfront.net
    Alternate Domain Names(CNAMEs): joesite.joechan.us
    Origin Domain Name:  elb-1603695720.us-west-2.elb.amazonaws.com (ELB)
    Distribution Domain Name: d2ey6g4539mma3.cloudfront.net
    Alternate Domain Names(CNAMEs): evesite.joechan.us
    Origin Domain Name:  elb-1603695720.us-west-2.elb.amazonaws.com (ELB)
    
```




  * Old Apache vHosts configuration




    

```
    <VirtualHost *:80>
    DocumentRoot /var/www/html/joesite
    ServerName joesite.joechan.us
    ErrorLog logs/test.joechan.us-error_log
    CustomLog logs/test.joechan.us-access_log common
    </VirtualHost>
    
    <VirtualHost *:80>
    DocumentRoot /var/www/html/evesite
    ServerName evesite.joechan.us
    ErrorLog logs/test.joechan.us-error_log
    CustomLog logs/test.joechan.us-access_log common
    </VirtualHost>
    
```







# Solution





It turns out that CloudFront will pass what is configured in the 'Origin Domain Name' field of the CloudFront settings (in my case, the ELB hostname) as the HTTP Host header to ELB, which will then get forwarded to Apache and Apache doesn't have a VirtualHost set up for my ELB hostname (elb-1603695720.us-west-2.elb.amazonaws.com).





Here is the fix:







  * Create 2 new CNAME records (one for each site), and use those as the CloudFront Origin Domain Names. 


  * Modify VirtualHost configuration to expect the proper CloudFront Origin Domain Names in the HTTP Host header. 





## My end configuration





In my new configuration, I have each original subdomain pointing to a CF distribution, which points to a new "vHosts" subdomain (like joestuff.joechan.us), which points to my single ELB. The ELB will forward the HTTP host header from the Origin Domain Name (<joe|eve>stuff.joechan.us), which is what I configure Apache to look for.







  * New DNS records




    

```
    joesite.joechan.us. 300 IN CNAME d1byny1djxkk2x.cloudfront.net.
    evesite.joechan.us. 300 IN CNAME d2ey6g4539mma3.cloudfront.net.
    joestuff.joechan.us. 300 IN CNAME elb-1603695720.us-west-2.elb.amazonaws.com.
    evestuff.joechan.us. 300 IN CNAME elb-1603695720.us-west-2.elb.amazonaws.com.
    New CF Distribution settings
    
```




  * New CF Distribution settings




    

```
    d1byny1djxkk2x.cloudfront.net
    Alternate Domain Names(CNAMEs): joesite.joechan.us
    Origin Domain Name:  joestuff.joechan.us
    d2ey6g4539mma3.cloudfront.net
    Alternate Domain Names(CNAMEs): evesite.joechan.us
    Origin Domain Name:  evestuff.joechan.us
    
```




  * New Apache vHosts configuration




    

```
    <VirtualHost *:80>
    DocumentRoot /var/www/html/joesite
    ServerName joestuff.joechan.us
    ErrorLog logs/test.joechan.us-error_log
    CustomLog logs/test.joechan.us-access_log common
    </VirtualHost>
    
    <VirtualHost *:80>
    DocumentRoot /var/www/html/evesite
    ServerName evestuff.joechan.us
    ErrorLog logs/test.joechan.us-error_log
    CustomLog logs/test.joechan.us-access_log common
    </VirtualHost>
    
```







I hope that this helps some of you out there trying to configure CloudFront with ELB and Apache VirtualHosts. If you guys have any thoughts or questions, please feel free to leave a comment!



