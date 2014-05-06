---
author: joe
comments: true
date: 2013-09-07 21:29:09+00:00
layout: post
slug: make-anonymous-amazon-s3-rest-request
title: How to Make an Anonymous Amazon S3 REST Request
wordpress_id: 9457
categories:
- AWS
ef_usergroup:
- JJK
following_users:
- joechan
- kelatov
tags:
- Amazon_S3
- aws
- REST
---

## How to make an anonymous S3 REST request





I recently had a problem where the bucket owner could not access or modify an object. It turns out that the object was created with an anonymous (unauthenticated) user and had the following ACL and properties:





**ExampleObject.txt**







  * Bucket owner: Joe


  * Object owner: Anonymous


  * Creator: Anonymous






  


    

      **Grantee**
    

    
    

      **Permission**
    

  
  
  


    

      Anonymous
    

    
    

      Write
    

  




### Solution





To make it so that the bucket owner could access the file again, we need to add ACL rules to the object that look like this:






  


    

      **Grantee**
    

    
    

      **Permission**
    

  
  
  


    

      Joe
    

    
    

      Read
    

  




**Joe**, however, currently can't modify the ACL because of the current ACL rules.





To make an anonymous request, we can use the `curl` tool.





From the [API docs for Put Acl](http://docs.aws.amazon.com/AmazonS3/latest/API/RESTObjectPUTacl.html), an example request looks like this:





**Example request**





> 

>     

```
>     PUT ExampleObject.txt?acl HTTP/1.1
>     Host: examplebucket.s3.amazonaws.com
>     x-amz-acl: public-read
>     Accept: */*
>     Authorization: AWS AKIAIOSFODNN7EXAMPLE:xQE0diMbLRepdf3YB+FIEXAMPLE=
>     Host: s3.amazonaws.com
>     Connection: Keep-Alive
>     
```

> 
> 






**Example curl command to grant Joe read access to the ExampleObject.txt**





> 

>     

```
>     curl -X PUT \
>     -H 'x-amz-grant-read: emailAddress="joe@amazon.com"' \
>     http://examplebucket.s3.amazonaws.com/ExampleObject.txt?acl
>     
```

> 
> 




