# LOLbject - a Objective-c like runtime for C

TL;DR -> experiments with message passing based object oriented framework in C.

NOTE: This is a toy project, not intended for production. The prefix LOL means that. Does not take is seriously.
The idea is that evetything will be done in runtime, including type checking, message passing, etc.
Yes, this is bad for performance and counter-intuitive, but people have been using similar for decades (ObjC)
and I am trying to understand how they did that, as I still do not think it scales well.

Features:

* dynamic loading of modules

* dynamic dispatching

* class diagram generation during runtime

* Mixins and Inheritance

* Properties

Planned Features:
* Hot update (deploy new modules and updates without stop the running application)

* Classes extensions (categories)

* Safe cast -> ```if (struct Number* n = lolbj_cast(Number, obj)) {do something with n}```

* Meta programming -> 

* Introspection and Reflection

TODO: document how to use it and how things look like.

WHY?
* Because I was curious to learn how the ObjC runtime works, since I have worked professionally with it for a few months now.

PS: I am still mostly a C++ programmer, but confess have not used it a lot recently.
