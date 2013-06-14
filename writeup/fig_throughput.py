#!/usr/bin/python
# -*- coding: utf-8 -*-

import numpy as np
import matplotlib.pyplot as plt
import matplotlib
import sys
import datetime
import time
import signal
import string
import os.path
import json
import httplib2
import urllib
import os

def print_graph(filename):
    base = []
    fuse = []
    ajfs = []
    nodes = []
    with open(filename, 'r') as f:
        for line in f:
            line = line.rstrip()
	    parts = line.split()
	    if parts[0] == "Read":
		op = 'Read'
		continue
	    elif parts[0] == 'Write':
		op = 'Write'
		continue
	    else:
	        base.append(float(parts[1]))
	        fuse.append(float(parts[2]))
	        ajfs.append(float(parts[3]))
	        nodes.append(int(parts[0]))
    file.close(f)

    ax = plt.subplot(111)
    ax.plot(nodes, base, '--', marker = '^', markersize = 15, linewidth = 5, label = 'Local')
    ax.plot(nodes, fuse, '-.', marker = 'v', markersize = 15, linewidth = 5, label = 'Fuse')
    ax.plot(nodes, ajfs, '-', marker = 's', markersize = 15, linewidth = 5, label = 'AJFS')
    ylabel = op + ' ' + 'Throughput (MB/s)'
    ax.set_ylabel(ylabel, fontsize = '30')
    plt.xticks(fontsize = '30')
    plt.yticks(fontsize = '30')
    if op == 'Write':
	ax.set_ylim([0, 9])
	loc1 = 7
    else:
	ax.set_ylim([0, 1100])
	loc1 = 4
    ax.set_xlim([0.8, 8.2])
    ax.set_xlabel("Number of AJFS nodes #", fontsize = '30')
    ax.legend(loc = loc1, prop = {'size': 30})
    plt.grid()
    plt.show()

if len(sys.argv) < 2:
    print "Parameter Error"
    sys.exit(0)

filename = sys.argv[1]

print_graph(filename)

sys.exit(0)
