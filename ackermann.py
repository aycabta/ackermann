#!/usr/bin/python

import cPickle

def memoized(func):
    memory = {}
    def memo(*args,**kwargs):
       hash = cPickle.dumps((args, sorted(kwargs.iteritems())))
       if hash not in memory:
           memory[hash] = func(*args,**kwargs)
       return memory[hash]
    return memo

@memoized
def a(m, n):
	if m == 0:
		return n + 1
	if n == 0:
		return a(m-1, 1)
	return a(m-1, a(m, n-1))

if __name__ == '__main__':
	import sys
	sys.setrecursionlimit(90000)
	print a(int(sys.argv[1]), int(sys.argv[2]))
