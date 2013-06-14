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
    ax.plot(nodes, base, '--', color='y', marker = '^', markersize = 15, linewidth = 15, label = 'Local')
    ax.plot(nodes, fuse, '-.', color='0.4', marker = 'v', markersize = 15, linewidth = 15, label = 'Fuse')
    ax.plot(nodes, ajfs, '-', color='#fda566', marker = 's', markersize = 15, linewidth = 15, label = 'AJFS')
    ylabel = op + ' ' + 'Throughput (MB/s)'
    ax.set_ylabel(ylabel, fontsize = '60')
    plt.xticks(fontsize = '60')
    plt.yticks(fontsize = '60')
    if op == 'Write':
	ax.set_ylim([0, 9])
    else:
	ax.set_ylim([0, 1100])
    ax.set_xlim([0.8, 8.2])
    ax.set_xlabel("Number of AJFS nodes #", fontsize = '60')
    ax.legend(loc = 3, prop = {'size': 50}, ncol = 3)
#    plt.grid()
    plt.show()

if len(sys.argv) < 2:
    print "Format: $ python fig_throughput.py input_file"
    sys.exit(0)

filename = sys.argv[1]

print_graph(filename)

sys.exit(0)
