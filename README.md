# UDP Blaster
Credits for the basic Inet engine idea go to
Yves Chevallier https://github.com/nowox/udp-test<br>
<br>
<br>
This tool is suited for tuning and optimizing the performance of log collectors.<br>
But not limited to log collection, you may use it for any UDP receiving device under test.<br>
It simulates sending a supplied log message to a log collector at a rate of a cozy 1 message per second to a flooding 100.000 messages per second if you system can handle that :)<br>
Currently it supports UPD only and runs native on Linux.<br>

# How to build
Clone this repo to your Linux machine<br>
Make the executable by executing:<br>

```
make
```

You need GCC installed and other C libraries depending on you distribution.

# How to run

Run the file and see the possible options.<br>

For example consider this short syslog message:
```
<34>Oct 11 22:14:15 mymachine su: 'su root' failed for Joe on /dev/pts/8
```
Say we want to send this message at an interval of 100 packets per second to a local logcollector listening on port 2450, the instruction would be:

```
./udp_blaster 127.0.0.1 2450 100 "<34>Oct 11 22:14:15 mymachine su: 'su root' failed for Joe on /dev/pts/8"
```

Simple as that! <br>

Success with the tuning!<br>

# Labeling messages

In the payload put the text [SEQUENCE] anywhere your like. UDP Blaster will inject a hexadecimal sequence number on the same spot.

# Getting performance

This little program is not mutitasking (yet) and opens only 1 socket at a time. <br>
You might get something like 10KPS to 20KPS on a normal machine. <br>
UDP Blaster will signal you when reaching the limit: <br>

```
Sorry. Unable to run faster on this one socket; try reducing packets per second or launch multiple instances.

```

<br>
If you need more KPS simply run multiple instances. <br>
<br>
Consider for example:


```
<instance 1> && <instance 2> && fg

```
# Advanced

Getting information from the interface your are blasting at:

```
ethtool --statistics [your interface] | grep tx

```

<hr>
Abby Eeninkwinkel 2023 & 2024
