#!/usr/bin/env python

import sys
import socket
import select

def fail(reason):
    sys.stderr.write(reason + '\n')
    sys.exit(1)


left = None
right = None

try:
    left = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    left.bind(('', 60000))
except:
    fail('Failed to bind on port 60000')

try:
    right = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    right.bind(('', 60001))
except:
    fail('Failed to bind on port 60001')

knownLeftAddr = None
knowRightAddr = None

while True:
    input = [left, right]
    inputready,outputready,exceptready = select.select(input,[],[])

    if left in inputready:
        data, addr = left.recvfrom(32768)
        if knownLeftAddr is None:
            print 'new address for left port is '
            print addr
        knownLeftAddr = addr
        if knowRightAddr:
            right.sendto(data, knowRightAddr)

    if right in inputready:
        data, addr = right.recvfrom(32768)
        if knowRightAddr is None:
            print 'new address for right port is '
            print addr
        knowRightAddr = addr
        if knownLeftAddr:
            left.sendto(data, knownLeftAddr)

