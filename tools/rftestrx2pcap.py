#!/usr/bin/python
# (C) 2012, Mariano Alvira <mar@devl.org>

import sys,os,time
from termios import *
from struct import *
import re

if len(sys.argv) < 3:
	sys.stderr.write( "Usage: %s tty channel\n" %(sys.argv[0]))
        sys.stderr.write( "       channel = 11-26\n")
	sys.exit(2)

# change the channel
# rftest-rx increments it's channel everytime you send a character and returns "channel: num"
# send a character until we get to the right channel

try:
	serport = os.open(sys.argv[1], os.O_RDWR|os.O_NOCTTY)
except IOError:
	print "error opening port"
	sys.exit(2)

try:
	oldattrs = tcgetattr(serport)
	attrs = [IGNPAR, oldattrs[1], 0, 0, B115200, B115200, oldattrs[6]]
	attrs[2] = B115200 | CS8 | CLOCAL | CREAD
	attrs[6][VTIME] = 0
	attrs[6][VMIN] = 1
	tcflush(serport, TCIFLUSH)
	tcsetattr(serport, TCSANOW, attrs)
except IOError:
	print "IOError in termios"
	sys.exit(2)

chan = ''
while chan != int(sys.argv[2]) - 11:
	os.write(serport, ' ')
	chanstr = ''
	while 1:
		chanstr += os.read(serport, 1)
		m = re.match(".*channel: (\w+)\s+", chanstr)
		if m is not None:
			chan = int(m.group(1))
			break

os.close(serport)

sys.stderr.write("RX: 0")

try:
	serport = open(sys.argv[1], 'r+')
except IOError:
	print "error opening port"
	sys.exit(2)


### PCAP setup
MAGIC = 0xa1b2c3d4;
MAJOR = 2;
MINOR = 4;
ZONE = 0;
SIG = 0;
SNAPLEN = 0xffff;
NETWORK = 230; # 802.15.4 no FCS

# output overall PCAP header
sys.stdout.write(pack('<LHHLLLL', MAGIC, MAJOR, MINOR, ZONE, SIG, SNAPLEN, NETWORK))

count = 0
fileempty = 1
newpacket = 0

try:
	while 1:
		line = serport.readline().rstrip()

		m_rftestline = re.match(".*rftest-rx --- len 0x(\w\w).*", line)

		if m_rftestline is not None:
			newpacket = 1
			t = time.time()
			sec = int(t)
			usec = (t - sec) * 100000
			length = int(m_rftestline.group(1), 16)
#			sys.stderr.write(line + "\n")
#			sys.stderr.write("rftestline: %d %d %d\n" % (sec, usec, length))
			continue
			
			# if this is a new packet, add a packet header
		if newpacket == 1:
			newpacket = 0
			sys.stdout.write(pack('<LLLL',sec,usec,length,length))

			count += 1
			sys.stderr.write("\x1b[G") # Move the cursor up one
			sys.stderr.write("RX: %d" % count)

			# clear file empty flag
			if fileempty:
				fileempty = 0
		if fileempty == 0 :
			# write payload
			for d in line.split(' '):
				# do a match because their might be a \r floating around
				m = re.match('.*(\w\w).*', d)
				if m is not None:
					sys.stdout.write(pack('<B', int(m.group(1),16)))


#             cn.recv_block()
#             if cn.data != None:

# #                sys.stderr.write(cn.data)
# #                sys.stderr.write(cn.data)
# #                sys.stderr.write(str(time.time()))

#                 t = time.time()
#                 sec = int(t)
#                 usec = (t - sec) * 100000

# #                sys.stderr.write("new packet: %d %d %d" % (sec, usec, len(cn.data)))

#                 count += 1
#                 sys.stderr.write("\x1b[G") # Move the cursor up one
#                 sys.stderr.write("RX: %d" % count)

#                 sys.stdout.write(pack('<LLLL',sec,usec,len(cn.data),len(cn.data)))
#                 sys.stdout.write(cn.data)

except KeyboardInterrupt:
#		cn.close()
		sys.exit(2)


