import sys
import time
import random

fileTest = "validation.txt"
fileIn = "testing.txt"

#with open(fileIn) as f:
#	content = f.readlines()
#	A = content.split()
#	for line in f:
#		for word in line.split():
#			A.append(int(word))
#f.close

def MaxSubarr(A):
	res = A[0]
	mem = start = fin =0
	sum = A[0]
	for i in range(1,len(A)):
		#sum = 0
		#print"Mem: %d, Sum: %d, Res: %d" % (mem,sum,res)
		#sum = (mem + A[i])
		#mem = max(sum, mem)
		#res = max(res, mem)
		#print"Mem: %d, Sum: %d, Res: %d" % (mem,sum,res)
		
		if A[i] > (sum + A[i]):
			sum = A[i]
			mem = i
		else:
			sum += A[i]
		
		if sum > res:
			res = sum
			start = mem
			fin = i
	#print("Current value: %d" % res)
	return res

res = 0
A = [-11, 44, 40, -26, -5, 48, 19, -6, 25, 48, -28, 1, 23, -16, 24, 37, 12, -19, -3, -19, -13, 22, -11, 47, -16, -5, 0, 16, 29, -46, 1]

#for random array:
#for i in range (10):
#		A = [random.randint(-100,100) for _ in range(100)]
#		start = time.clock()
#		res = MaxSubarr(A)
start = time.clock()
res = MaxSubarr(A) 
print("Array: \n %s" % A)
print ("Max Sub Array: %d" % res)
print ("Total Time: %sms" % ((time.clock()-start)*1000))
print ("Time for set of 10: %sms" % (str((time.clock()-start)*100)))
	
