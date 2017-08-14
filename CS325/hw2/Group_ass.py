import os
import random
import time
import sys

DEBUG = False

def total_mid(left_key, right_key, Total_balls):
	left_idx = left_key
	max_gap = 0
	idx = None
	
	for x in xrange(left_key, right_key +1):
		if Total_balls[x]:
			gap = x - left_idx
			if gap > max_gap:
				max_gap = gap
				idx = (left_idx, x)
			left_idx = x 
	
	if not idx:
		return 0
		
def left_path(start, Total_balls):
	leftmost = start
	
	for x in xrange(start -1, -1, -1):
		if Total_balls[x]:
			leftmost = x 
	return start - leftmost
	
def right_path(start, Total_balls):
	rightmost = start
	
	for x in xrange(start +1,len(Total_balls)):
		if Total_balls[x]:
			rightmost = x 
	return rightmost - start
	
def power_set(_set):
	if not _set:
		return [[]]
	return (power_set(_set[1:]) + [[_set[0]] + x for x in power_set(_set[1:])])
	
def check_path(keys, Total_balls, Known_balls):
	path = len(keys) + left_path(keys[0], Total_balls) + right_path(keys[-1], Total_balls)
	
	if len(keys) == 1:
		return path
		
	for x in xrange(len(keys) - 1):
		path  += total_mid(keys[x], keys[x+1], Total_balls)
		
	return path
	
def ball_alg1(num_lockers, num_keys, num_balls, given_keys, desired_lockers):
	if not DEBUG:
		key_set = power_set(given_keys)
	else:
		from itertools import combinations
		
		key_set = combinations(given_keys)
	desired_lockers.sort()
	all_lockers = [x in desired_lockers for x in xrange(num_lockers + 1)]
	total_op = []
	
	for keys in key_set:
		keys.sort()
		if not keys:
			continue
		p = check_path(keys, all_lockers, desired_lockers)
		total_op.append(p)
	return min(total_op)

def ball_alg2():
	pass

def ball_alg3():
	pass
	
if __name__ == '__main__':
	arg = None
	if not len(sys.argv) > 1:
		print 'Usage: python Group_ass2.py [option]'
		print 'Options: '
		options = ['--test']
		print '\n'.join(map(lambda x: '\t' + x, options))
	elif sys.argv[1] == '--test':
		test_sets = open('dp.txt', 'r').read().strip()
		test_chunks = map(lambda x: '\n'.join(x.split('\n)')[1:4] + [x.split('\n')[5]]), test_sets.split('\n\n\n\n'))
		
		output = open("output.txt", "w")
		for i, test_sets in enumerate(test_chunks):
			if i != 4:
				continue
			lines = map(lambda x: x.strip(), test_sets.split('\n'))
			num_lockers, num_keys, num_balls = map(int, lines[0].split(' '))
			given_keys = map(int, lines[1].split(' '))
			desired_lockers = map(int, lines[2].split(' '))
			answer = int(lines[3])
			guess = ball_alg1(num_lockers, num_keys, num_balls, given_keys,desired_lockers)

			output.write('Correct Answer' + str(answer))
			sys.stdout.write('Tested answer: ' + str(guess))
			sys.stdout.flush()
