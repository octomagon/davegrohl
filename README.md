# DaveGrohl
A ~~Distributed~~ Password Cracker for OS X

## History
DaveGrohl was initially created in early 2011 as a password hash extractor & companion tool to John the Ripper.  It's blossoming into a multithreaded distributed password cracker was a product of sheer curiosity/boredom.  Dave has always been aimed at brute-forcing OS X user passwords, although he's been known to get lost staring into a cloud of Linux machines.

Sadly, my day job as a ninja spy astronaut and my highly Scandinavian level of ethanol consumption has led to an old stanky code base.  In light of these facts, I've decided to unleash the stank upon the world in hopes that it brings joy and nerdgasms to whomever finds it.

## Current Status
Dave compiles fine on Yosemite and will happily (but very slowly) crack user passwords.  Out of the box, OS X uses PBKDF2 to encrypt its user passwords which is very slow to crack for one machine.  Turning on Windows (SMB) file sharing will disable this feature.

## Quick How-To
If you're pulling Dave from Github, make sure XCode is installed then clone the repository.
```bash
MacBook-Pro:~$ git clone https://github.com/octomagon/davegrohl.git
```
Compile away...
```bash
MacBook-Pro:~$ cd davegrohl
MacBook-Pro:~/davegrohl$ make

*** A bunch of stuff happens here ***

Make succeeded...
Created: dave
MacBook-Pro:~/davegrohl$ 
```
Then run Dave.  Press spacebar to see the progress of each thread.  Threads running a dictionary attack will be in `()` while incremental attacks will be in `[]`.  For dictionary attacks, Dave looks in a folder called 'wordlists' for plain text files and will dedicate a thread for each file it finds.
```bash
MacBook-Pro:~/davegrohl$ sudo ./dave -u someuser
-- Loaded PBKDF2 (Salted SHA512) hash...
-- Starting attack

      TIME             GUESSES
0000:00:08                 351  (aaru) (loveme) [x] [86n] [bpc] [2s5] [ojf] [wkea] [52la] [caha] 
0000:00:14                 613  (abaculus) (samantha) [9a] [38n] [t3c] [an5] [yjf] [k4ea] [dmla] [ieha] 
0000:00:20                 875  (abandoning) (spongebob) [pe] [n7n] [x3c] [8n5] [bvf] [25ea] [odla] [weha] 

-- Found password : 'shorty'
-- (dictionary attack)

Finished in 31.330 seconds / 1,318 guesses...
42 guesses per second.
```
In this example, I purposely set my password to one I knew Dave would guess early.  When using PBKDF2, OS X throttles the time it takes to hash a password.  This is great for password security, but bad for brute force attacks.

Out of the box, Dave will not check every possible password because it is insanely impractical.  With no special arguments, Dave will check every password between 0 and 16 digits long using the default character set of 'aeorisn1tl2md0cp3hbuk45g9687yfwjvzxq'.  With these settings on my 8-core Macbook Pro, it would take longer than the age of the universe to check every possibility.

You can greatly improve your time-to-crack if you have an idea of how long the password is and what characters may be in it.  The following example tells Dave the password is between 5 & 7 digits long and may contain the characters 'sbeio'.
```bash
MacBook-Pro:~/davegrohl$ sudo ./dave -c sbeio -m 5 -M 7 -u someuser
Password:
-- Loaded PBKDF2 (Salted SHA512) hash...
-- Starting attack

      TIME             GUESSES
0000:00:07                 245  (aardwolves) (chocolate) [oosss] [oossbe] [ssbsess] [oossiis] [ssbsobb]
0000:00:20                 682  (abaiser) (666666) [iiess] [eiesbe] [iiesess] [iiesiis] [iiesobb]
```
You can get a time estimate by pressing 't'.
```bash
0000:00:27                 894  1 Hour 26 Seconds left...
0000:00:34               1,152  (abashes) (sayang) [oeoss] [oeosbe] [siosess] [siosiis] [siosobb]

-- Found password : 'boobs'
-- (incremental attack)

Finished in 74.250 seconds / 2,477 guesses...
33 guesses per second.
```
The time estimate is how long Dave will take to check every possibility, not how long to find the password.



## Why ~~Distributed~~?
Distributed mode stopped working an OS or two ago.  I sadly haven't had the time to look into it, although it's probably something simple.

Also while distributed mode was cool, it was not built to scale massively.  This is partly due to it's being tied to Mac OS X.  Who the hell wants to virtualize a thousand instances of OS X anyways?  Real scale means linux.  If DaveGrohl is destined to live on, it should be stripped of it's Cocoa and start embracing platform independence.


Not Windows of course...  Fuck Windows.

## Disclaimer
Although Dave is mostly in C++11 & Objective-C, I have never coded professionally in those languages.  If while reading the code you start to get a nosebleed or an erection lasting more than 4 hours, please accept my sincere apologies for my mockery of good coding practices.
