# davegrohl
A ~~Distributed~~ Password Cracker for OS X

## History
DaveGrohl was initially created in early 2011 as a password hash extractor & companion tool to John the Ripper.  It's blossoming into a multithreaded distributed password cracker was a product of sheer curiosity/boredom.  Dave has always been aimed at brute-forcing OS X user passwords, although he's been known to get lost staring into a cloud of Linux machines.

Sadly, my day job as a ninja spy astronaut and my highly Scandinavian level of ethanol consumption has led to an old stanky code base.  In light of these facts, I've decided to unleash the stank upon the world in hopes that it brings joy and nerdgasms to whomever finds it.

## Current Status
Dave compiles fine on Yosemite and will happily (but very slowly) crack user passwords.  Out of the box, OS X uses PBKDF2 to encrypt its user passwords which is very slow to crack for one machine.  Turning on Windows (SMB) file sharing will disable this feature.

## Why ~~Distributed~~?
Distributed mode stopped working an OS or two ago.  I sadly haven't had the time to look into it, although it's probably something simple.

Also while distributed mode was cool, it was not built to scale massively.  This is partly due to it's being tied to Mac OS X.  Who the hell wants to virtualize a thousand instances of OS X anyways?  Real scale means linux.  If DaveGrohl is destined to live on, it should be stripped of it's Cocoa and start embracing platform independence.


Not Windows of course...  Fuck Windows.

## Disclaimer
Although Dave is mostly in C++11 & Objective-C, I have never coded professionally in those languages.  If while reading the code you start to get a nosebleed or an erection lasting more than 4 hours, please accept my sincere apologies for my mockery of good coding practices.
