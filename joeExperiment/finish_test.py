#!/usr/bin/python

import os

for line in os.popen('ps -A |grep connected'):
	parts = line.strip().split()
	print 'kill %s' %(parts[0])
	os.system('kill -9 %s' % parts[0])

os.system('umount test')
os.system('umount test1')
