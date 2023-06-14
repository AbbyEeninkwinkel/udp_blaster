# UDP Blaster
Credits for the basic Inet engine idea go to 
Yves Chevallier https://github.com/nowox/udp-test<br>
<br>
<br>
This tool is suited for tuning and optimizing the performance of log collectors.<br>
But not limited to log collection, you may use it for any udp receiving device under test.<br>
It simulates sending a supplied log message to a log collector at a rate of a cozy 1 message per second to a flooding 100.000 messages per second if you system can handle that :)<br>
Currently it supports UPD only and runs native on Linux.<br>

# How to build
Clone this repo to your Linux machine<br>
Make the executable by executing:<br>

```
make 
```
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

<hr>
Abby Eeninkwinkel 2023
