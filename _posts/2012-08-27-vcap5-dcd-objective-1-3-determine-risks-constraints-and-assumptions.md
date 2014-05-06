---
author: karim
comments: true
date: 2012-08-27 14:06:29+00:00
layout: post
slug: vcap5-dcd-objective-1-3-determine-risks-constraints-and-assumptions
title: 'VCAP5-DCD Objective 1.3 – Determine Risks, Constraints, and Assumptions '
wordpress_id: 2718
categories:
- VCAP5-DCD
- VMware
following_users:
- jarret
- kelatov
tags:
- VCAP5-DCD
---

### Differentiate between the general concepts of a risk, a requirement, a constraint, and an assumption.





From the [US BrownBag Objective 1](http://www.slideshare.net/ProfessionalVMware/professionalvmware-brownbag-jason-boche-vcapdcd-objective-1) Slide Deck:





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/arch-vision.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/arch-vision.png)





and from the [APAC BrownBag Session 1 Slide Deck](http://portal.sliderocket.com/BLIHZ/VCAP5-DCD-BrownBag---Session-1):





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/rcar.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/rcar.png)





### Given a statement, determine whether it is a risk, requirement, a constraint, or an assumption





From this [PDF](http://virtuallyhyper.com/wp-content/uploads/2013/04/vcap-dcd_notes.pdf):





> 
  
> 
> **Architectural Vision: A high level vision for the project** includes the following aspects:
> 
> 
  
  
> 
> 
  
>   1. Scope - Identify Scope in detail 
  
  
  
>     * Defined scope prevents unintended expansion (ie this project includes the US productions servers only. No dev/test and no Canada servers)
> 
  
>     * prevents the need to renegotiate cost of project or work for fee
> 
  
> 
  
>   2. Goals - set specific goals for a project 
  
  
  
>     * Goals need to be specific, measurable, and actionable
> 
  
>     * without goals it is difficult to determine success and value of a project
> 
  
>     * i.e the organization wants to achieve a 50% reduction in production server equipment by the end of the year
> 
  
> 
  
>   3. Requirements - Identify key business and technical requirements for the project 
  
  
  
>     * ie, SOX compliance, physical separation from production and dev/test, up-time requirements, etc
> 
  
> 
  
>   4. Assumptions- Design components that are assumed valid without proof 
  
  
  
>     * i.e. the organization has sufficient bandwidth between sites for replication
> 
  
> 
  
>   5. Constrains - constraints limit the design choices, could be a policy, process, or technical constraint 
  
  
  
>     * i.e. due to existing relationships all hardware is dell
> 
  
> 
  
>   6. Risks - Identify risks that might prevent achieving project goals 
  
  
  
>     * i.e. lack of core redundancy introduces risk of %99.99 up-time
> 
  
>     * discussing risks eliminated surprise 
> 
  
> 
  






Also good examples can be found here:







  * http://www.virten.net/2012/06/vdcd510-objective-1-3-determine-risks-constraints-and-assumptions/


  * http://www.slideshare.net/ProfessionalVMware/professionalvmware-brownbag-jason-boche-vcapdcd-objective-1





### Analyze impact of VMware best practices to identified risks, constraints, and assumptions





From the [US BrownBag Objective 1](http://www.slideshare.net/ProfessionalVMware/professionalvmware-brownbag-jason-boche-vcapdcd-objective-1) Slide Deck:





[![](http://virtuallyhyper.com/wp-content/uploads/2012/08/best-practices-rules.png)](http://virtuallyhyper.com/wp-content/uploads/2012/08/best-practices-rules.png)





Basically try to be flexible with best practices, not everything falls under best practices or a certain criteria. For this section the Blue Print has a link to an excellent document from Glasshouse, I would definitely recommend reading it: [Developing Your Virtualization Strategy and Deployment Plan](http://communities.vmware.com/docs/DOC-17431)



