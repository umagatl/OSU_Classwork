import os
import random
import time
import sys
from itertools import chain, combinations

DEBUG = False

#create class for lockers
class Locker:

	def __init__(self):
		self._numLockers = 0
		self._numKeys = 0
		self._numTennisBalls = 0
		self._keys = []
		self._tennisBalls = []
		self._output = 0
		
	#shifted idea to class format
	# def total_mid(left_key, right_key, Total_balls):
		# left_idx = left_key
		# max_gap = 0
		# idx = None
		
		# for x in xrange(left_key, right_key +1):
			# if Total_balls[x]:
				# gap = x - left_idx
				# if gap > max_gap:
					# max_gap = gap
					# idx = (left_idx, x)
				# left_idx = x 
		
		# if not idx:
			# return 0
			
	# def left_path(start, Total_balls):
		# leftmost = start
		
		# for x in xrange(start -1, -1, -1):
			# if Total_balls[x]:
				# leftmost = x 
		# return start - leftmost
		
	# def right_path(start, Total_balls):
		# rightmost = start
		
		# for x in xrange(start +1,len(Total_balls)):
			# if Total_balls[x]:
				# rightmost = x 
		# return rightmost - start
		
	# def power_set(_set):
		# if not _set:
			# return [[]]
		# return (power_set(_set[1:]) + [[_set[0]] + x for x in power_set(_set[1:])])
		
	# def check_path(keys, Total_balls, Known_balls):
		# path = len(keys) + left_path(keys[0], Total_balls) + right_path(keys[-1], Total_balls)
		
		# if len(keys) == 1:
			# return path
			
		# for x in xrange(len(keys) - 1):
			# path  += total_mid(keys[x], keys[x+1], Total_balls)
			
		# return path
		
	def ball_alg1(self):
		least_lockersOpened = self._numLockers
		
		key_set = set(self._keys)
		
		for keys in chain.from_iterable(combinations(key_set, r) for r in range(len(key_set)+1)):
			not_opened = 0
			keys = sorted(keys)
			
			if (len(keys) < 1):
				continue
			if self._tennisBalls[0] < keys[0]:
				not_opened += self._tennisBalls[0] - 1
			else:
				not_opened += keys[0] -1
			
			if(len(keys) == 1):
				if((self._numLockers - not_opened) < least_lockersOpened):
					least_lockersOpened = self._numLockers - not_opened
					not_opened = 0
				continue
				
			ball_count = 0
			
			while(self._tennisBalls[ball_count] < keys[0]):
				if ball_count + 1 < self._numTennisBalls:
					ball_count += 1
				else:
					break
			
			temp = 0
			for temp in range(len(keys)-1):
				if ball_count >= self._numTennisBalls:
					break
				first = keys[temp] + 1
				best = 0
				
				for i in range(first, keys[temp + 1]):
					if ball_count >= self._numTennisBalls:
						break
					j = i
					if (i == self._tennisBalls[ball_count]):
						ball_count =+1
						continue
					else:
						while(j + 1 < self._tennisBalls[ball_count]):
							j += 1
						if(j - i) + 1 > best and (j-i) + 1 > 0:
							ball_count += 1
							best = (j - i) + 1
				not_opened += best
			if((self._numLockers - not_opened) < least_lockersOpened):
				least_lockersOpened = self._numLockers - not_opened
				not_opened = 0
			
		print least_lockersOpened
		return least_lockersOpened

	def ball_alg2(self):
		n = self._numLockers
		m = self._numKeys
		t = self._numTennisBalls
		k = self._keys
		b = self._tennisBalls
		
		k.sort()
		b.sort()
		
		arr = []
		
		for i in range(0, m):
			arr.append(float('inf'))
			
		#check first key
		#print(str(k))
		if k[0] <= b[0]:
			arr[0] = 0
		else:
			arr[0] = k[0] - b[0] + 1
		
		#check middle keys
		for i in range(1,m):
			for j in range(0,i):
				leastOpened = self.LEAST_OPENED(k[i], k[j])
				if arr[j] + leastOpened < arr [i]:
					arr[i] = arr[j] + leastOpened
		
		#second key
		if b[t-1] >= k[m-1]:
			arr[m - 1] += (b[t-1] - k[m-1]) + 1
		return arr[m-1]
		
	def LEAST_OPENED(self, mi, mj):
		bestUnopened = 0
		if mi - mj == 1:
			if mi in self._tennisBalls:
				if mj in self._tennisBalls:
					return 1
				else:
					return 0
			else:
				if mj in self._tennisBalls:
					return 1
				else:
					return 0
		else:
			for i in range(mj + 1, mi):
				j = i
				if i in self._tennisBalls:
					continue
				else:
					while(j+1) not in self._tennisBalls and j < mi - 1:
						j+= 1
					if (j-i) + 1 > bestUnopened:
						bestUnopened = (j-i) + 1
			return (mi - (mj+1) + 1) - bestUnopened
			
	def ball_alg3():
		pass
		
	def file_load(self, file, idx):
		f = open(file, 'r')
		if idx > 7:
			print "Index out of bounds"
			return
			
		for i in range(idx):
			for i in range(9):
				trash = f.readline()
		
		#trashes the new line and heading
		f.readline()
		f.readline()
		count_base = f.readline().rstrip(' \r\n').split(' ')
		keys = f.readline().rstrip(' \r\n')
		tennis_balls = f.readline().rstrip(' \r\n')
		f.readline()
		sample_out = f.readline().rstrip(' \r\n')
		f.readline()
		f.readline()
		
		self._numLockers = int(count_base[0])
		self._numKeys = int(count_base[1])
		self._numTennisBalls = int(count_base[2])
		self._keys = [int(num) for num in keys.split(' ')]
		self._tennisBalls = [int(num) for num in tennis_balls.split(' ')]
		self._output = int(sample_out)
		
		self._keys.sort()
		self._tennisBalls.sort()
	def format_print(self):
		print 'Number of Lockers = {} Number of keys = {} Number of Balls = {}\nKeys = {} Balls = {}\nExpexted: {}'.format(self._numLockers, self._numKeys, self._numTennisBalls, self._keys, self._tennisBalls, self._output)

		

def main():
	for i in range(8):
		lock = Locker()
		lock.file_load('dp.txt', i)
		lock.format_print()
		print lock.ball_alg1()
		print lock.ball_alg2()
		
		
if __name__ == '__main__':
	main()