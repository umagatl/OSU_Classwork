import sys
import time

fileTest = "validation.txt"
fileIn = "testing.txt"

#with open(fileIn) as f:
#	content = f.read()
#	A = content.split()
#	for line in f:
#		for word in line.split():
#			A.append(int(word))
#f.close

def MaxSubarr(A, res):
	for i in range(0,len(A)):
		for j in range(i,len(A)):
			sum = 0
			for k in range(i,j):
				sum = sum + A[k]
			res = max(sum,res)
	print("Current value: %d" % res)
	return res

res = 0
A = [-11, 44, 40, -26, -5, 48, 19, -6, 25, 48, -28, 1, 23, -16, 24, 37, 12, -19, -3, -19, -13, 22, -11, 47, -16, -5, 0, 16, 29, -46, 1]
start = time.clock()
res = MaxSubarr(A, res)
print ("Max Sub Array: %d" % res)
print ("Time: %sms" % (time.clock()*1000))