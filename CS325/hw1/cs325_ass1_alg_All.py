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

def MaxSubarr1(A, res):
	#global curr_max
	curr_max =[]
	curr_arr = []
	for i in range(0,len(A)):
		for j in range(i,len(A)):
			c_sum = 0
			for k in range(i,j):
				c_sum = c_sum + A[k]
				curr_arr.append(A[k])
			#res = max(c_sum,res)
			if(c_sum > res):
				res = c_sum
				curr_max = curr_arr
			curr_arr = []
			#max_so_far = max(max_so_far, c_sum)
	return (res, curr_max)
	
def MaxSubarr2(A):
	#global curr_max
	#curr_max = []
	curr_arr = []
	res = 0
	for i in range(0,len(A)):
		c_sum = 0
		for j in range(i,len(A)):
			c_sum = c_sum + A[j]
			#res = max(res, c_sum)
			#print("curr:" + str(curr_arr))	
			if(c_sum > res):
				res = c_sum
				curr_max = A[i:j+1]
	return (res, curr_max)
	
def MaxSubarr3(A):
	curr_max = []
	curr_arr = []
	res = A[0]
	mem = start = fin =0
	c_sum = A[0]
	for i in range(1,len(A)):
		#c_sum = 0
		#print"Mem: %d, c_sum: %d, Res: %d" % (mem,c_sum,res)
		#c_sum = (mem + A[i])
		#mem = max(c_sum, mem)
		#res = max(res, mem)
		#print"Mem: %d, c_sum: %d, Res: %d" % (mem,c_sum,res)
		
		if A[i] > (c_sum + A[i]):
			c_sum = A[i]
			mem = i
		else:
			c_sum += A[i]
		
		if c_sum > res:
			res = c_sum
			start = mem
			curr_max = A[mem:i+1]
			fin = i
	#print("Current value: %d" % res)
	return (res, curr_max)

res = res2 = res3 = 0
#curr_max = []
A = [31, -41, 59, 26, -53, 58, 97, -93, -23, 84]
print type(A)
start = time.clock()
res = MaxSubarr1(A, res)
res2 = MaxSubarr2(A)
res3 = MaxSubarr3(A)

text_file = open("Output.txt", "w")
text_file.write("Algorithm 1 results: %s\n" % str(res))
text_file.write("Algorithm 2 results: %s\n" % str(res2))
text_file.write("Algorithm 3 results: %s\n" % str(res3))
text_file.close()
