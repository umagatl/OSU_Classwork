import math
from random import randint
from operator import itemgetter
import sys
import os.path
import time
import string
sys.setrecursionlimit(1500)
#file = 'test_cases_without_solutions.txt'
file = 'test_hw3.1.txt'

##def Algo1(arr):    #not actually needed since check list only wants method 2 or 3
##	min_l = 0
##	min_r = 1
##	mid = (len(arr) -1)
##	min_sum = arr[0] + arr[1]	
##	
##	for i in range(0,mid +1):
##		sum = 0
##		for j in range(mid + 1,len(arr)):
##			sum = arr[i] + arr[j]
##			if(min_sum > sum):
##				min_sum = sum
##				min_l = i
##				min_r = j
##	return (arr[min_l],arr[min_r])


def Algo2(arr):
	min_l = 0
	min_r = 1
	mid = (len(arr) -1) // 2
	min_sum = int(arr[0]) + int(arr[1])	
	#print arr
	#print arr[0:mid+1]
	#print arr[mid+1:len(arr)]
	for i in range(0,mid +1):
		sum = 0
		for j in range(mid + 1,len(arr)):
			sum = int(arr[i]) + int(arr[j])
			if(abs(min_sum) > abs(sum)):
				min_sum = sum
				min_l = i
				min_r = j
	CtZ = int(arr[min_l]) + int(arr[min_r])
	return (int(min_l), int(min_r))
##	while(low < high):
##		sum = int(arr[low]) + int(arr[high])
##		
##		if(abs(sum) < abs(min_sum)):
##			min_sum = sum
##			min_l = low
##			min_r = high
##		if(sum < 0):
##			low += 1
##		else:
##			high -= 1
##	CtZ = int(arr[min_l]) + int(arr[min_r])		
##	return CtZ

def Algo3(arr, size):
	arr1_size = size // 2
	arr1 = []
	print "Size: ", arr1_size
	for i in range (0, arr1_size):
		arr1.append(arr[i])
	
	arr2_size = size - size//2
	arr2 = []
	for i in range(arr2_size, size):
		arr2.append(arr[i])
	print "Size2 : ", arr2_size	
	return check(enumerate(arr1, arr1_size, 0), enumerate(arr2, arr2_size, arr1_size), enumerate2(arr1, arr2, arr1_size, arr2_size))
		
def check(a, b, c):
	if abs(a[0]) < abs(b[0]) <= abs(c[0]):
		return a
	elif abs(b[0]) < abs(c[0]):
		return b
	else:
		return c
		
def enumerate(arr, size, idx):
	print "array in enumerate : ", arr
	arr1_size = size // 2
	arr1 = []
	for i in range (0, arr1_size):
		arr1.append(arr[i])
		
	arr2_size = size - size//2
	arr2 = []
	for i in range(0,arr2_size):
		arr2.append(arr[i])
		
	arr2rev = []
	for t in range(1, arr2_size):
		arr2rev.append(arr2[arr2_size-t])
	print "arr1 : ", arr1
	print "arr2 : ", arr2
	min_i = 0
	min_j = 1
	min_sum = int(arr1[0]) + int(arr2[1])
	temp = 0
	for i in range(1, arr1_size):
		for j in range(0, arr2_size):
			if i != j:
				temp = int(arr1[i]) + int(arr2[j])
				if abs(temp) <= abs(min_sum):
					min_i = i
					min_j = j
					min_sum = temp
	if idx == 0:
		results = [min_sum, min_i, (min_j + arr1_size)]
		print "i, j: ", min_i, " ", (min_j + arr1_size)
	else:
		results = [min_sum, (min_i + idx), (min_j + arr1_size + idx)]
		print "i, j: ", min_i + idx, " ", (min_j + arr1_size + idx)
	print "min_sum : ", min_sum	
	return results
	
def enumerate2(arr1, arr2, arr1_size, arr2_size):
	temp = 0
	min_sum = 1000
	for i in range(0, arr1_size):
		for j in range(0, arr2_size):
			temp = int(arr1[i]) + int(arr2[j])
			if abs(temp) <= abs(min_sum):
					min_i = i
					min_j = j
					min_sum = temp
	print "i, j : ", min_i, " ", (min_j + arr1_size)
	print "min_sum in second enumerate : ", min_sum
	results = [min_sum, min_i, (min_j + arr1_size)]
	return results
def main():
	f = open(file, "r")
	a = []
	total = 0
	count = 0
	arr = []

	
	for row in f:
		arr.append(row)
		
	for i in arr:
		i = i.replace("[", "")
		i = i.replace("]", "")
		i = i.replace("\n", "")
		i = i.replace(" ", "")
		a.append(i.split(","))
		
	# for i in range(0,len(a)):
		# #sort the array in order, time complexity is O(nlogn)
		# s = sorted(a[i], key = int)
		# start = time.clock()
		# sub_arr = Algo2(s)
		# stop = time.clock()
		# total += (stop - start)
		# print "Subarray value closest to zero: ", sub_arr
		# count += 1
	f = open("output.txt", 'w')
	
	for i in range(0,len(a)):
		#sort the array in order, time complexity is O(nlogn)
		s = sorted(a[i], key = int)
		start = time.clock()
		sub_arr = Algo3(s, len(s))
		stop = time.clock()
		total += (stop - start)
		f.write("%s  %s  %s\n"% (sub_arr[0],sub_arr[1],sub_arr[2]))
		count += 1
	Avg = (total / len(arr)) * 1000	
	print "Number of times ran: %s" % count
	print "Average time taken: %s ms" % Avg
			

if __name__ == '__main__':
	main()
